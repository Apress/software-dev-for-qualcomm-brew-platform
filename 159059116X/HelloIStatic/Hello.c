/**
 *  @name Hello.c
 *
 *  @author Ray Rischpater
 *  Copyright (c) 2001 - 2002 Ray Rischpater.
 *  Portions copyright (c) 2001 - 2002 QUALCOMM, Inc.
 *  @doc
 *  A sample application that draws hello world.
 */
#include "AEEModGen.h" // Module interface definitions
#include "AEEAppGen.h" // Applet interface definitions
#include "AEEDisp.h"   // Display Interface definitions
#include "AEEShell.h"
#include "hello.bid" // Applet ClassID


typedef struct
{
	AEEApplet a;	// Must always come first
	// Our application variables follow
	IStatic		*pIStatic;
} App, *AppPtr;


/*
 * Private function prototypes
 */
static boolean HelloWorld_HandleEvent( IApplet * pi, 
									   AEEEvent eCode,
									   uint16 wParam, 
									   uint32 dwParam );

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
						  IModule * po,
						  void ** ppObj )
{
	boolean result = FALSE;
	*ppObj = NULL;

	// If it's this class being requested...
	if( clsID == AEECLSID_HELLOWORLD )
	{
		// Use the BREW helper function to 
		// create an instance of this class
		result= AEEApplet_New( sizeof(App),
			                   clsID, 
                               pIShell,
							   po,
							   (IApplet**)ppObj, 
							   (AEEHANDLER)HelloWorld_HandleEvent,
							   NULL);
	}
	return result ? AEE_SUCCESS : EFAILED;
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
static boolean HelloWorld_HandleEvent( IApplet *pi,
									   AEEEvent eCode,
									   uint16 wParam, 
									   uint32 dwParam )
{
	AECHAR szBuf[] = {'H','e','l','l','o',' ',
			          'W','o','r','l','d','\0'};
	AppPtr pThis = (AppPtr)pi;
	boolean handled = FALSE;
	AEERect rc;

  // mark these as unused for the sake of warnings.
  wParam;
  dwParam;

	// Decide what to do with the incoming event.
	switch (eCode)
	{
		// The application is launching.
		case EVT_APP_START: 
			// Clear the display.
			IDISPLAY_ClearScreen( pThis->a.m_pIDisplay ); 
			// Create a static text control.
			ISHELL_CreateInstance( pThis->a.m_pIShell, 
								   AEECLSID_STATIC, 
								   (void *)&pThis->pIStatic );
			if ( pThis->pIStatic )
			{
				// Set the content
				ISTATIC_SetText( pThis->pIStatic, 
								 NULL, 
								 szBuf, 
								 AEE_FONT_NORMAL, 
								 AEE_FONT_NORMAL );
				// Set the bounds
				rc.x = 0;
				rc.y = 0;
				rc.dx = 60;
				rc.dy = 60;
				ISTATIC_SetRect( pThis->pIStatic, &rc );
				// Activate the control
				ISTATIC_SetActive( pThis->pIStatic, TRUE );
				ISTATIC_Redraw( pThis->pIStatic );
			}					   
			// Populate it with a message

			// Redraw the display to show the drawn text
			IDISPLAY_Update(pThis->a.m_pIDisplay ); 
			handled = TRUE;

		// Application is closing
		case EVT_APP_STOP:
			ISTATIC_Release( pThis->pIStatic );
			handled = TRUE;

		default:
			break;
	}
	return handled;
}
