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
#include "hello.bid" // Applet ClassID


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
		result= AEEApplet_New( sizeof(AEEApplet),
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
	AEEApplet * pMe = (AEEApplet*)pi;
	boolean handled = FALSE;

  // mark these as unused for the sake of warnings.
  wParam;
  dwParam;

	// Decide what to do with the incoming event.
	switch (eCode)
	{
		// The application is launching.
		case EVT_APP_START: 
			// Clear the display.
			IDISPLAY_ClearScreen( pMe->m_pIDisplay ); 
			// Display string on the screen
			IDISPLAY_DrawText( pMe->m_pIDisplay, 
							   AEE_FONT_BOLD, 
							   szBuf,
							   -1, 0, 0, 0, 
							   IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE );
			// Redraw the display to show the drawn text
			IDISPLAY_Update (pMe->m_pIDisplay); 
			handled = TRUE;

		// Application is closing
		case EVT_APP_STOP:
			handled = TRUE;

		default:
			break;
	}
	return handled;
}
