/*
 *  SlideShow.c
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  This file provides the entry point, exit point, and main
 *  event handler for the SlideShow application.
 */

/*
 * Includes
 */
#include "inc.h"

/*
 * Function Prototypes
 */
static int SlideShow_Init( CAppPtr p );
static void SlideShow_Free( IApplet *p );

static void ShowCopyright( CAppPtr p );
static void HideCopyright( CAppPtr p );

static boolean SlideShow_HandleEvent( IApplet * pi, 
                                      AEEEvent eCode, 
                                      uint16 wParam, 
                                      uint32 dwParam);

/*
 * Implementation
 */

/** 
 * Creates application class using AEEApplet_New
 * Initializes application structure, creating
 * necessary UI components.
 * @param AEECLSID clsid: class ID to create
 * @param IShell *pIShell: pointer to shell
 * @param IModule *pModule: pointer to module in
 *   which this applet was found
 * @param void **ppObj: pointer with which to hold
 *   created applet instance
 * @return AEE_SUCCESS on success, or else EFAILED
 */
int AEEClsCreateInstance( AEECLSID clsid,
                          IShell *pIShell,
                          IModule *pModule,
                          void **ppObj )
{
  int result = EFAILED;
  *ppObj = NULL;
		
  if( clsid == AEECLSID_SLIDESHOW )
  {
    if( AEEApplet_New( sizeof(CApp), 
                       clsid, 
                       pIShell,
                       pModule,
                       (IApplet**)ppObj,
        (AEEHANDLER)SlideShow_HandleEvent,
        (PFNFREEAPPDATA)SlideShow_Free ) == TRUE)
    {
      result = SlideShow_Init( (CAppPtr)(*ppObj) );
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
static int  SlideShow_Init( CAppPtr pThis )
{ 
  int result = AEE_SUCCESS;
  AEEDeviceInfo  dm;

  if (pThis->a.m_pIDisplay && pThis->a.m_pIShell) 
  {
	ISHELL_GetDeviceInfo( pThis->a.m_pIShell, &dm );
	pThis->m_cx = dm.cxScreen;
	pThis->m_cy = dm.cyScreen;
		
	pThis->m_bColor = dm.nColorDepth > 2;

	pThis->m_rc.x = 0;
	pThis->m_rc.y = 0;
	pThis->m_rc.dx = (short)(dm.cxScreen);
	pThis->m_rc.dy = (short)(dm.cyScreen);

	pThis->m_nFontHeight = 
    IDISPLAY_GetFontMetrics( pThis->a.m_pIDisplay, 
                             AEE_FONT_BOLD, NULL, NULL);
  }
  else
  {
    result = EFAILED;
  }

  if ( result == AEE_SUCCESS )
    result = ISHELL_CreateInstance( pThis->a.m_pIShell, 
                                    AEECLSID_MENUCTL, 
                                    (void **)&pThis->m_pIMenu );
  return result;
}


/** 
 * Frees application structure, releasing
 * necessary UI components.
 * @param IApplet *p: pointer to application structure
 * @return nothing
 */
static void SlideShow_Free( IApplet *p )
{
  CAppPtr pThis = (CAppPtr)p;

  if ( pThis->m_pIMenu ) IMENUCTL_Release( pThis->m_pIMenu );
}

/** 
 * Handles all events for the application
 * @param IApplet *p: pointer to application structure
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
 */
static boolean SlideShow_HandleEvent( IApplet *p, 
                                      AEEEvent eCode, 
                                      uint16 wParam, 
                                      uint32 dwParam )
{  
  boolean result = FALSE;
  CAppPtr pThis = (CAppPtr)p;

  // App Startup --- dismiss the copyright screen on
  // any keystroke
  if ( pThis->m_bCopyright && eCode == EVT_KEY )
  {
    HideCopyright( pThis );
    pThis->m_bCopyright = FALSE;
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
      result = State_Push( p, 
                            AS_MenuEntry, 
                            AS_MenuExit, 
                            AS_MenuHandleEvent );


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
  pImage = ISHELL_LoadResImage( pThis->a.m_pIShell, 
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
  IDISPLAY_Update( pThis->a.m_pIDisplay );

  // Dismiss the splash screen automatically after a while
  ISHELL_SetTimer( pThis->a.m_pIShell, 
                   APP_COPYRIGHT_CLOSE, 
                   (PFNNOTIFY)HideCopyright, 
                   pThis );

  // Let everybody know we're showing this.
  pThis->m_bCopyright = TRUE;
}


/** 
 * Hides the copyright screen.
 * @param IApplet *p: pointer to application structure
 */
static void HideCopyright(CAppPtr pThis)
{
  // We're done with the copyright screen.
  pThis->m_bCopyright = FALSE;
  
  // Cancel the timer, in case we got here 
  // on a keypress.
	ISHELL_CancelTimer( pThis->a.m_pIShell, 
                      (PFNNOTIFY)HideCopyright, 
                      pThis );

  // Tell the app that the copyright screen was over.
	ISHELL_PostEvent( pThis->a.m_pIShell, 
                    AEECLSID_SLIDESHOW, 
                    EVT_COPYRIGHT_END, 0, 0 );
}




