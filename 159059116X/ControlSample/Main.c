/**
*  @name Main.c
*
*  @author Ray Rischpater
*  Copyright (c) 2001 - 2002 Ray Rischpater.
*  Portions copyright (c) 2001 - 2002 QUALCOMM, Inc.
*  @memo Application instantiation and teardown
*  @doc
*  This file contains the application's constructor, 
*  destructor, and event handler, along with ancillary
*  functions to manage application startup.
*/

#include "inc.h"

/*
* Private function prototypes
*/
static int Main_Init( CAppPtr p );
static void Main_Free( IApplet *p );

static void ShowCopyright( CAppPtr p );
static void HideCopyright( CAppPtr p );

static boolean Main_HandleEvent( IApplet * pi, 
								AEEEvent eCode, 
								uint16 wParam, 
								uint32 dwParam);

/**
* Create an instance of this class. This constructor is
* invoked by the BREW shell with the applet is launched.
*
* @param AEECLSID clsID: class ID of the class being requested
* @param IShell *pIShell: a pointer to the BREW shell
* @param IModule *po: a pointer to the current module
* @param void **ppObj: a pointer to the created applet
* @return AEE_SUCCESS on success, with the applet in *pobj.
*/
int AEEClsCreateInstance( AEECLSID clsID,
						 IShell * pIShell,
						 IModule * pIModule,
						 void ** ppObj )
{
	boolean result = EFAILED;
	*ppObj = NULL;

	// If it's this class being requested...
	if( clsID == AEECLSID_OURS )
	{
		// Use the BREW helper function to 
		// create an instance of this class
		if( AEEApplet_New( sizeof(CApp), 
			clsID, 
			pIShell,
			pIModule,
			(IApplet**)ppObj,
			(AEEHANDLER)Main_HandleEvent,
			(PFNFREEAPPDATA)Main_Free ) == TRUE)
		{
			DBGPRINTF("Initing...");
			result = Main_Init( (CAppPtr)(*ppObj) );
			DBGPRINTF("Init finished");
		}
	}
	return result;
}

/** 
* Initializes application structure, creating
* necessary UI components.
* @param CAppPtr pThis: pointer to application structure
* @return AEE_SUCCESS on success, or else EFAILED
*/
static int Main_Init( CAppPtr pThis )
{ 
	int result = SUCCESS;
	int i;
	AEEDeviceInfo  dm;

	// get device/display information
	if ( GetDisplay( pThis ) && GetShell( pThis ) ) 
	{
		ISHELL_GetDeviceInfo( GetShell( pThis ), &dm);
		pThis->m_cx = dm.cxScreen;
		pThis->m_cy = dm.cyScreen;
		pThis->m_colorDepth = dm.nColorDepth;
		if(dm.nColorDepth > 2)
			pThis->m_bColor = TRUE;
		else
			pThis->m_bColor = FALSE;

		// Construct a rectangle that will contain 
		// the static text controls
		pThis->m_rc.x = 0;
		pThis->m_rc.y = 0;
		pThis->m_rc.dx = (short)(dm.cxScreen);
		pThis->m_rc.dy = (short)(dm.cyScreen);

		// get the font metrics
		pThis->m_nFontHeight = IDISPLAY_GetFontMetrics( 
			GetDisplay( pThis ), AEE_FONT_BOLD, NULL, NULL);
	}
	else
		result = EFAILED;

	if ( result == EFAILED )
		return result;
		
	pThis->m_app.m_pState = MALLOC( sizeof( CState ) );
	
	result = pThis->m_app.m_pState ? SUCCESS : EFAILED;
	
	if ( result == EFAILED )
		return result;
		
	// Create the UI elements for the framework.
	pThis->m_app.m_nControl = MAX_NUM_CONTROLS;
	for ( i = 0; i < MAX_NUM_CONTROLS; i++ )
		pThis->m_app.m_apControl[ i ] = NULL; 

		ISHELL_CreateInstance( GetShell( pThis ), 
			AEECLSID_MENUCTL, 
			(void **)&pThis->m_app.m_apControl[ Ctl_NavMenu ]);

		ISHELL_CreateInstance( GetShell( pThis ), 
			AEECLSID_SOFTKEYCTL, 
			(void **)&pThis->m_app.m_apControl[ Ctl_SoftKeyMenu ]);
		ISHELL_CreateInstance( GetShell( pThis ), 
			AEECLSID_TEXTCTL, 
			(void **)&pThis->m_app.m_apControl[ Ctl_TextInput ]);

		result = pThis->m_app.m_apControl[ Ctl_NavMenu ] && 
			pThis->m_app.m_apControl[ Ctl_SoftKeyMenu ] &&
			pThis->m_app.m_apControl[ Ctl_TextInput ] ? SUCCESS : EFAILED;

	if ( result == EFAILED )
	{
		for ( i = 0; i < MAX_NUM_CONTROLS; i++ )
		{
			if ( pThis->m_app.m_apControl[ i ] )
			{
				ICONTROL_Release( pThis->m_app.m_apControl[ i ] );
				pThis->m_app.m_apControl[ i ] = NULL; 
			}
		}
		return result;
	}

	/*
	 * Reset all framework controls to begin with.
	 */
	for ( i = 0; i < Ctl_LastFrameworkControl; i++ )
		ICONTROL_Reset( pThis->m_app.m_apControl[ i ] );

	result = AS_Init( pThis );

	return result;
}


/** 
* Frees application structure, releasing
* necessary UI components.
* @param IApplet *p: pointer to application structure
* @return nothing
*/
static void Main_Free( IApplet *p )
{
	CAppPtr pThis = (CAppPtr)p;
	int i;

	ASSERT( pThis );

	// Release the head of the state list.
	while( State_GetCurrentState( pThis ) )
		State_Pop( p );
	FREE( pThis->m_app.m_pState );

	// Release all controls
	for ( i = 0; i < MAX_NUM_CONTROLS; i++ )
	{
		if ( pThis->m_app.m_apControl[ i ] )
		{
			ICONTROL_Release( pThis->m_app.m_apControl[ i ] );
			pThis->m_app.m_apControl[ i ] = NULL; 
		}
	}


	// Release any application state stuff
	AS_Free( pThis );
}


/**
* Handles incoming events from the shell.
*
* @param IApplet *pi: pointer to this applet.
* @param AEEEvent eCode: event to handle
* @param int wParam: word argument associated with event
* @param uint32 dwParam: double word arg associated with event
* @return TRUE if the event was handled, FALSE otherwise.
*/
static boolean Main_HandleEvent( IApplet *p,
								AEEEvent eCode,
								uint16 wParam, 
								uint32 dwParam )
{
	boolean result = FALSE;
	CAppPtr pThis = (CAppPtr)p;

	// App Startup --- dismiss the copyright screen on
	// any keystroke
	if ( pThis->m_bSplashing && eCode == EVT_KEY )
	{
		HideCopyright( pThis );
		pThis->m_bSplashing = FALSE;
	}

	result = State_HandleEvent( (void *)p, eCode, wParam, dwParam );

	if ( !result ) switch (eCode) 
	{
		case EVT_APP_START:                        
			// Show the splash screen
			ShowCopyright( pThis );
			result = TRUE;
			break;

		case EVT_COPYRIGHT_END:
			// set initial state to display the dialog
			State_Push( pThis, AS_FIRSTSTATE );
			break;

		case EVT_APP_STOP:
			break;

		default:
			break;
	}
	return result;
}

/** 
* Shows the copyright screen.
* @param IApplet *p: pointer to application structure
*/
static void ShowCopyright(CAppPtr pThis )
{
	IImage *pImage;

	// Fetch the copyright image
	pImage = ISHELL_LoadResImage( GetShell( pThis ), 
		APP_RES_FILE, 
		IDI_COPYRIGHT );
	if (pImage)
	{
		IIMAGE_Draw( pImage, 0, 0 ) ;

		// Free the image
		IIMAGE_Release( pImage );
		pImage = NULL;
	}

	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );

	// Dismiss the splash screen automatically after a while
	ISHELL_SetTimer( GetShell( pThis ), 
		APP_COPYRIGHT_CLOSE, 
		(PFNNOTIFY)HideCopyright, 
		pThis );

	// Let everybody know we're showing this.
	pThis->m_bSplashing = TRUE;
}


/** 
* Hides the copyright screen.
* @param IApplet *p: pointer to application structure
*/
static void HideCopyright(CAppPtr pThis)
{
	// We're done with the copyright screen.
	pThis->m_bSplashing = FALSE;

	// Cancel the timer, in case we got here 
	// on a keypress.
	ISHELL_CancelTimer( GetShell( pThis ), 
		(PFNNOTIFY)HideCopyright, 
		pThis );

	// Tell the app that the copyright screen was over.
	ISHELL_PostEvent( GetShell( pThis ), 
		AEECLSID_OURS, 
		EVT_COPYRIGHT_END, 0, 0 );
}


