/*
 *  AppStates.c
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  This file provides the implementation for the states of SlideShow.
 */

/*
 * Includes
 */
#include "inc.h"


/*
 * Prototypes
 */
static void FillMenu( CAppPtr pThis );
static void DrawCentered( CAppPtr pThis, IImage *pIImage );
static IAStream *GetStreamFromFile( CAppPtr pThis, char *szName );
static IAStream *GetUnzipStreamFromStream( CAppPtr pThis, 
										   IAStream *pIAStream );

/*
 * Implementation
 */

/**
 * Populates the menu with the list of bitmaps.
 * @param CAppPtr pThis: the application
 * @return nothing
 */
static void FillMenu( CAppPtr pThis )
{
  IFileMgr *pIFileMgr;
  FileInfo info;
  AECHAR wszBuff[ MAX_FILE_NAME + 1 ];
  uint16 nItem = 1;
  int result;
  IMenuCtl *pIMenu = 
	(IMenuCtl *)pThis->m_app.m_apControl[ Ctl_NavMenu ];

  // Setup the file manager instance
  result = ISHELL_CreateInstance( GetShell( pThis ), 
                                  AEECLSID_FILEMGR, 
                                  (void **)&pIFileMgr );

  // Enumerate the list of .bar files
  if ( pIFileMgr )
  {
    // Begin enumeration
    if ( SUCCESS == IFILEMGR_EnumInit( pIFileMgr, "", FALSE ) )
    {
      while ( IFILEMGR_EnumNext( pIFileMgr, &info ) )
      {
        // We're interested in files that end in .bmp
        if ( STRENDS( ".bmp", info.szName ) ||
			 STRENDS( ".gz", info.szName ) )
        {

          // convert to a wide string
          STRTOWSTR( info.szName, wszBuff, MAX_FILE_NAME + 1 );

          // Add it to the menu
          IMENUCTL_AddItem( 
			pIMenu, 
            NULL, // Resource file for item
            0,    // Don't use the resource file
            nItem++,
            wszBuff,
            (uint32)0 );  // Item data
        } // add file name to menu
      } // file name enumeration
    } // enumeration guard

    // Clean up
    IFILEMGR_Release( pIFileMgr ); 
  } // pIFileMgr guard
}



/**
 * Initialize the application-specific data.
 * @param void *pThis: application
 * @return: EFAILED or ESUCCESS
 */
int AS_Init( CAppPtr pThis )
{
  CAppDataPtr pAppData;
  int result = EFAILED;
  
  ASSERT( pThis );
  // Create the application controls here.
  
  // Create the application's global data here.
  pAppData = MALLOC( sizeof( CAppData ) );
  
  if ( pAppData )
  {
	MEMSET( pAppData, 0, sizeof( CAppData ) );
	SetAppData( pThis, pAppData );
	result = SUCCESS;
  }
  
  return result;
}

void AS_Free( CAppPtr pThis )
{
  CAppDataPtr pAppData;
  
  ASSERT( pThis );
  
  pAppData = GetAppData( pThis );
  if ( pAppData )
  {
	FREE( pAppData );
	pAppData = NULL;
  }
}


/* 
 * This is the first state of the application
 */


/** 
 * Enters the first state.
 * @param void *p: this applicaton
 * @param EStateChange change: why we entered this state
 * @return nothing
 */
static boolean mainEntry( void *p, 
                          EStateChangeCause change )
{
  CAppPtr pThis = (CAppPtr)p;
  IMenuCtl *pIMenu = 
	(IMenuCtl *)pThis->m_app.m_apControl[ Ctl_NavMenu ];

  UNUSED( change );

  ASSERT( pThis && pIMenu );

  // Clear the display
  IDISPLAY_ClearScreen( GetDisplay( pThis ) );

  // Reset the menu
  IMENUCTL_Reset( pIMenu );
 
  // Set the menu's bounds
  IMENUCTL_SetRect( pIMenu, &pThis->m_rc );

  // Populate the menu
  FillMenu( pThis );

  // Activate the menu & update screen
  IMENUCTL_SetActive( pIMenu, TRUE );
  IDISPLAY_Update( GetDisplay( pThis ) );
  return TRUE;  
}

/** 
 * Exits the first state.
 * @param void *p: this applicaton
 * @param EStateChange change: why we exited this state
 * @return nothing
 */
static boolean mainExit( void *p, 
                         EStateChangeCause change )
{
  CAppPtr pThis = (CAppPtr)p;
  IMenuCtl *pIMenu = 
	(IMenuCtl *)pThis->m_app.m_apControl[ Ctl_NavMenu ];

  UNUSED( change );

  ASSERT( pThis && pIMenu );

  // Reset the menu
  IMENUCTL_Reset( pIMenu );

  return TRUE;
}


/** 
 * First state function for the application.
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
 */
boolean AS_MainHandleEvent( void *p, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam)
{
	CAppPtr pThis = (CAppPtr)p;
	boolean result = FALSE;
    CtlAddItem menuItem;
    CAppDataPtr pAppData;
	IMenuCtl *pIMenu = 
	  (IMenuCtl *)pThis->m_app.m_apControl[ Ctl_NavMenu ];

	ASSERT( pThis );
	pAppData = GetAppData(pThis);
	
	if ( !result ) switch ( eCode )
	{	
		case EVT_APP_START:
		case EVT_APP_RESUME:
			result = mainEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = mainExit( p, wParam );
			break;

		case EVT_COMMAND:
			// wParam contains the item selected.
			// Find the filename of the selected resource
			IMENUCTL_GetItem( pIMenu, 
							  wParam, 
							  &menuItem );
			WSTRTOSTR( menuItem.pText, 
				pAppData->szName, 
				MAX_FILE_NAME + 1);
			      
			// Show the bitmap
			result = State_Push( p, AS_ShowBitmapHandleEvent );
			
		default:
			break;
	}

	return result;
}


static void DrawCentered( CAppPtr pThis, IImage *pIImage )
{
	AEEImageInfo imageInfo;

	// offset into the image
	int xOffset = 0;
	int yOffset = 0;

	// offset into rect
	int xStart = 0;
	int yStart = 0;

	ASSERT( pIImage );

	IIMAGE_GetInfo( pIImage, &imageInfo );

	// determine offset into image, if the result is positive
	// determine offset into rect, if the result is negative
	xOffset = (imageInfo.cx - pThis->m_cx ) / 2;
	yOffset = (imageInfo.cy - pThis->m_cy ) / 2;

	// set the max drawing width
	IIMAGE_SetDrawSize(pIImage, pThis->m_cx, pThis->m_cy );

	if (xOffset < 0)
	{
		xStart = (0-xOffset);
		xOffset = 0;
	}

	if (yOffset < 0)
	{
		yStart = (0-yOffset);
		yOffset = 0;
	}

	IIMAGE_SetOffset( pIImage, xOffset, yOffset );

	IIMAGE_Draw( pIImage, xStart, yStart );
}

/** 
 * Gets a stream for a file.
 * @param CAppPtr *p: this applicaton
 * @param char *szName: file name
 * @return pointer to IAStream instance or NULL on failure.
 */
static IAStream *GetStreamFromFile( CAppPtr pThis, char *szName )
{
  IFileMgr *pIFileMgr = NULL;
  IFile *pIFile = NULL;
  int result;
 
  // Setup the file manager instance
  result = ISHELL_CreateInstance( GetShell( pThis ), 
                                  AEECLSID_FILEMGR, 
                                  (void **)&pIFileMgr );

  if ( result != SUCCESS || !pIFileMgr ) return NULL;

  // Open the specific file to get a stream to the image.  
  pIFile = IFILEMGR_OpenFile( pIFileMgr, 
		szName,
		_OFM_READ );
  IFILEMGR_Release( pIFileMgr );

  return (IAStream *)pIFile;
}

/** 
 * Replaces a stream with a IUnzipAStream to the same stream.
 * @param CAppPtr *pThis: this applicaton
 * @param IAStream *pStream
 * @return pointer to IAStream instance or NULL on failure.
 */
static IAStream *GetUnzipStreamFromStream( CAppPtr pThis, 
										   IAStream *pIAStream )
{
  IUnzipAStream *pIUnzipAStream = NULL;
  int result;

  // Setup the file manager instance
  result = ISHELL_CreateInstance( GetShell( pThis ), 
	                              AEECLSID_UNZIPSTREAM, 
                                  (void **)&pIUnzipAStream );
  if ( result != SUCCESS || pIUnzipAStream == NULL )
  {
    return NULL;
  }
  // Set the zip's source stream
  IUNZIPASTREAM_SetStream( pIUnzipAStream, pIAStream );

  // Release our reference to the original stream
  IASTREAM_Release( pIAStream );
    
  return (IAStream *)pIUnzipAStream;
}

/** 
 * Enters the ShowBitmap state.
 * @param void *p: this applicaton
 * @param EStateChange change: why we entered this state
 * @return nothing/
 */
static boolean showBitmapEntry( void *p, 
                    EStateChangeCause change )
{
  CAppPtr pThis = (CAppPtr)p;
  IImage *pIImage = NULL;
  CAppDataPtr pAppData = NULL;
  IAStream *pIAStream = NULL;
  int result;

  UNUSED( change );
  ASSERT( pThis );

  pAppData = GetAppData(pThis);

  // Clear the display
  IDISPLAY_ClearScreen( GetDisplay( pThis )  );

  // Note how much heap we start with...
  DBGPRINT_HeapUsed( pThis );

  // Get the stream for the file.
  pIAStream = GetStreamFromFile( pThis, pAppData->szName );
  
  if ( pIAStream == NULL ) return NULL;
  
  // If the steam is compressed, 
  if ( STRENDS( ".gz", pAppData->szName ) )
  {  
    // we need to get a stream to uncompress it
	pIAStream = GetUnzipStreamFromStream( pThis, pIAStream );
  }

  // Create an IImage control to display the bitmap.
  result = ISHELL_CreateInstance( GetShell( pThis ), 
	AEECLSID_WINBMP,
    (void **)&pIImage );
  pThis->m_app.m_apControl[ Ctl_Image ] = (IControl *)pIImage;

  if ( result != SUCCESS || !pIImage ) 
  {
	IASTREAM_Release( pIAStream );
	return FALSE;
  }
  
  // Set the image to the stream
  IIMAGE_SetStream( pIImage, pIAStream );

  // Draw the image
  DrawCentered( pThis, pIImage );

  DBGPRINT_HeapUsed( pThis );
  
  // We no longer need these interfaces.
  if ( pIAStream ) IASTREAM_Release( pIAStream );
  
  // Update the display
  IDISPLAY_Update( GetDisplay( pThis ) );
  return TRUE;  
}

/** 
 * Exits the ShowBitmap state.
 * @param void *p: this applicaton
 * @param EStateChange change: why we exited this state
 * @return nothing
 */
static boolean showBitmapExit( void *p, 
                   EStateChangeCause change )
{
  CAppPtr pThis = (CAppPtr)p;
  IImage *pIImage;
  
  UNUSED( change );

  ASSERT( pThis );

  pIImage = (IImage *)( pThis->m_app.m_apControl[ Ctl_Image ] );
  
  if ( pIImage )
  {
	IIMAGE_Release( pIImage );
	 pThis->m_app.m_apControl[ Ctl_Image ] = NULL;
  }
  	
  return TRUE;
}


/** 
 * State functiont to show the selected bitmap
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
 */
boolean AS_ShowBitmapHandleEvent( void *p, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam)
{
	CAppPtr pThis = (CAppPtr)p;
	boolean result = FALSE;

	ASSERT( pThis );

	if ( !result ) switch ( eCode )
	{	
		case EVT_APP_START:
		case EVT_APP_RESUME:
			result = showBitmapEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = showBitmapExit( p, wParam );
			break;

		case EVT_COMMAND:
			result = TRUE;
			break;

		default:
			break;
	}

	return result;
}



#if 0
/** 
 * Enters the first state.
 * @param void *p: this applicaton
 * @param EStateChange change: why we entered this state
 * @return nothing
 */
static boolean _entry( void *p, 
                    EStateChangeCause change )
{
  CAppPtr pThis = (CAppPtr)p;

  UNUSED( change );

  ASSERT( pThis );

  // Clear the display
  IDISPLAY_ClearScreen( GetDisplay( pThis )  );

  IDISPLAY_Update( GetDisplay( pThis ) );
  return TRUE;  
}

/** 
 * Exits the first state.
 * @param void *p: this applicaton
 * @param EStateChange change: why we exited this state
 * @return nothing
 */
static boolean _exit( void *p, 
                   EStateChangeCause change )
{
  CAppPtr pThis = (CAppPtr)p;

  UNUSED( change );

  ASSERT( pThis );

  return TRUE;
}


/** 
 * First state function for the application.
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
 */
boolean _HandleEvent( void *p, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam)
{
	CAppPtr pThis = (CAppPtr)p;
	boolean result = FALSE;

	ASSERT( pThis );

	if ( !result ) switch ( eCode )
	{	
		case EVT_APP_START:
		case EVT_APP_RESUME:
			result = _entry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = _exit( p, wParam );
			break;

		case EVT_COMMAND:
			result = TRUE;
			break;

		default:
			break;
	}

	return result;
}



#endif