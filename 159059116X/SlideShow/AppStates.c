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

#define TIMER_DEFAULT ( 2048 )
#define TIMER_MINIMUM (  128 )

/*
 * Function Prototypes
 */
static void ShowNextSlide( void *p );
static void FillMenu( CAppPtr pThis );


/*
 * Implementation
 */

/* 
 * Menu state
 * This is the first state of the application;
 * it shows the list of bar files in the current 
 * directory.
 */

/**
 * Populates the menu with the list of slide shows.
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

  // Setup the file manager instance
  result = ISHELL_CreateInstance( pThis->a.m_pIShell, 
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
        // We're interested in files that end in .bar
        if ( STRENDS( ".bar", info.szName ) )
        {
          // None was loaded, so create our own
          // Drop the .bar --- it looks hokey.
          info.szName[ STRLEN( info.szName ) - 4 ] = '\000';

          // convert to a wide string
          STRTOWSTR( info.szName, wszBuff, MAX_FILE_NAME + 1 );

          // Add it to the menu
          IMENUCTL_AddItem( pThis->m_pIMenu, 
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

  if ( nItem == 1 )
  {
    // Menu addition failed.
    // But we always have our .bar, so add it manually.

    // convert to a wide string
    STRTOWSTR( "SlideShow", wszBuff, MAX_FILE_NAME + 1 );
  
    // Add it to the menu
    IMENUCTL_AddItem( pThis->m_pIMenu, 
                      NULL, // Resource file for item
                      0,    // Don't use the resource file
                      nItem++,
                      wszBuff,
                      (uint32)0 );  // Item data

  } // Add what is always there
}


/** 
 * Prepares user interface for menu state.
 * Initializes menu, sets menu rectangle to screen, 
 * @param void *p: this applicaton
 * @param EStateChange change: why we entered this state
 * @return nothing
 */
void AS_MenuEntry( void *p, 
                   EStateChange change )
{
  CAppPtr pThis = (CAppPtr)p;

  UNUSED( change );

  ASSERT( pThis && pThis->m_pIMenu );

  // Clear the display
  IDISPLAY_ClearScreen( pThis->a.m_pIDisplay );

  // Reset the menu
  IMENUCTL_Reset( pThis->m_pIMenu );
 
  // Set the menu's bounds
  IMENUCTL_SetRect( pThis->m_pIMenu, &pThis->m_rc );

  // Populate the menu
  FillMenu( pThis );

  // Activate the menu & update screen
  IMENUCTL_SetActive( pThis->m_pIMenu, TRUE );
  IDISPLAY_Update( pThis->a.m_pIDisplay );
}

/** 
 * Exits the user interface for menu state.
 * Removes all menu entries and deactivates the menu
 * @param void *p: this applicaton
 * @param EStateChange change: why we exited this state
 * @return nothing
 */
void AS_MenuExit( void *p, 
                  EStateChange change )
{
  CAppPtr pThis = (CAppPtr)p;

  UNUSED( change );

  ASSERT( pThis && pThis->m_pIMenu );

  // Reset the menu
  IMENUCTL_Reset( pThis->m_pIMenu );

  // Deactivate this menu 
  IMENUCTL_SetActive( pThis->m_pIMenu, FALSE );
}


/** 
 * Handles events for the menu state
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
 */
boolean AS_MenuHandleEvent( void *p, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam)
{
  CAppPtr pThis = (CAppPtr)p;
  boolean result = FALSE;
  CtlAddItem menuItem;  

  ASSERT( pThis && pThis->m_pIMenu );

  // Give the control the first crack at it
  result = IMENUCTL_HandleEvent( pThis->m_pIMenu,
                                 eCode,
                                 wParam,
                                 dwParam );

  if ( !result ) switch ( eCode )
  {
    case EVT_COMMAND:
      // Find the filename of the selected resource
      IMENUCTL_GetItem( pThis->m_pIMenu, 
                        wParam, 
                        &menuItem );
      WSTRTOSTR( menuItem.pText, pThis->m_szFile, MAX_FILE_NAME + 1);
      STRCAT( pThis->m_szFile, ".bar" );
      
      // Start the slide show
      result = State_Push( p, 
                           AS_SlideEntry, 
                           AS_SlideExit, 
                           AS_SlideHandleEvent );
      break;

    default:
      break;
  }

  return result;
}


/* 
 * Slide state
 * This is the workhorse of the application
 * it shows each slide, managing keystrokes and an
 * optional animation timer.
 */

/** 
 * Shows the next slide.
 * Sets a timer for animation if necessary.
 * @param void *p: this applicaton
 * @return nothing
 */
static void ShowNextSlide( void *p )
{
  CAppPtr pThis = (CAppPtr)p;
  CStatePtr pState;
  CSlideShowPtr pInfo;

  ASSERT( pThis );
  pState = State_GetCurrentState( pThis );
  ASSERT( pState );

  // Clear the display
  IDISPLAY_ClearScreen( pThis->a.m_pIDisplay );

  if ( pState->m_pData  )
  {
    pInfo = (CSlideShowPtr)pState->m_pData;
    ASSERT( pInfo );

    if ( pInfo )
    {
      IImage *pImage;

      // Point to the current image
      pInfo->m_next += pInfo->m_dir;
      if ( pInfo->m_next == 0 )
      {
        pInfo->m_next = 1;
        pInfo->m_dir = 1;
      }

      // Load the bitmap
      pImage = ISHELL_LoadResImage( pThis->a.m_pIShell, 
                                    pInfo->m_szFile, 
                                    pInfo->m_next );

      // Wrap back around to first image if nothing is shown
      if ( !pImage )
      {
        pInfo->m_next = 1;
        pImage = ISHELL_LoadResImage( pThis->a.m_pIShell, 
                                      pInfo->m_szFile, 
                                      pInfo->m_next );
      } // Do we have an image? no? then wrap around

      // Display the bitmap
	    if (pImage)
	    {
		    IIMAGE_Draw( pImage, 0, 0 ) ;

        // Free the image
		    IIMAGE_Release( pImage );
		    pImage = NULL;

	    } // Do we have an image

      // Update the screen
	    IDISPLAY_Update( pThis->a.m_pIDisplay );

      // Set the timer to do it again if necessary
      if ( pInfo->m_bAnimate != 0 )
      	ISHELL_SetTimer( pThis->a.m_pIShell, 
                         pInfo->m_timer, 
                         (PFNNOTIFY) ShowNextSlide, 
                         pThis );

    } // Do we have valid slide info?
  } // Do we have valid state data?
}

/** 
 * Prepares user interface for slide show state
 * Shows the first slide.
 * @param void *p: this applicaton
 * @param EStateChange change: why we entered this state
 * @return nothing
 */
void AS_SlideEntry( void *p, 
                      EStateChange change )
{
  CAppPtr pThis = (CAppPtr)p;
  CStatePtr pState;
  UNUSED( change );

  pState = State_GetCurrentState( pThis );
  ASSERT( pThis && pState);

  // Allocate memory for the state info
  pState->m_pData = MALLOC( sizeof( CSlideShow ) );
  if ( pState->m_pData )
  {
    CSlideShowPtr pInfo = (CSlideShowPtr)pState->m_pData;
    pInfo->m_szFile = pThis->m_szFile;
    pInfo->m_next = 0;
    pInfo->m_timer = pThis->m_frameDelay ? 
      pThis->m_frameDelay : TIMER_DEFAULT;
    pInfo->m_dir = 1;
    pInfo->m_bAnimate = FALSE;
    // Show this slide
    ShowNextSlide( p );
  }  
  // Screen update wsa done in ShowNextSlide
}
  

/** 
 * Tears down the UI for the slide show.
 * Cancels the animation timer.
 * @param void *p: this applicaton
 * @param EStateChange change: why we exited this state
 * @return nothing
 */
void AS_SlideExit( void *p, 
                     EStateChange change )
{
  CAppPtr pThis = (CAppPtr)p;
  CStatePtr pState;
  UNUSED( change );

  pState = State_GetCurrentState( pThis );
  ASSERT( pThis && pState);

  // Free the state data
  if ( pState->m_pData )
  {
    // Save the current playback frame rate
    pThis->m_frameDelay = ((CSlideShowPtr)(pState->m_pData))->m_timer;

    FREE( pState->m_pData );
    pState->m_pData = NULL;
  }
  
  // Cancel the animation timer
  ISHELL_CancelTimer( pThis->a.m_pIShell, 
                      (PFNNOTIFY) ShowNextSlide, 
                      pThis );

}

/** 
 * Handles events for the menu state
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
 */
boolean AS_SlideHandleEvent( void *p, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam)
{
  CAppPtr pThis = (CAppPtr)p;
  CStatePtr pState;
  CSlideShowPtr pInfo;
  boolean result = FALSE;
  
  ASSERT( pThis );

  pState = State_GetCurrentState( pThis );
  ASSERT( pState);
  
  pInfo = (CSlideShowPtr)pState->m_pData;

  if ( pInfo ) switch ( eCode )
  {
    case EVT_KEY:
      switch( wParam )
      {
        case AVK_SELECT:
          pInfo->m_bAnimate = !pInfo->m_bAnimate;
          ShowNextSlide( p );
          result = TRUE;
          break;

        case AVK_UP:
          if ( pInfo->m_timer > TIMER_MINIMUM )
          {
            pInfo->m_timer /= 2;
            ISHELL_CancelTimer( pThis->a.m_pIShell, 
                               (PFNNOTIFY) ShowNextSlide, 
                               pThis );
            ShowNextSlide( p );
            result = TRUE;
          }
          break;

        case AVK_DOWN:
          if ( pInfo->m_timer && pInfo->m_timer < TIMER_DEFAULT )
          {
            pInfo->m_timer *= 2;
            ISHELL_CancelTimer( pThis->a.m_pIShell, 
                               (PFNNOTIFY) ShowNextSlide, 
                               pThis );
            ShowNextSlide( p );
            result = TRUE;
          }
          break;

        case AVK_LEFT:
          ISHELL_CancelTimer( pThis->a.m_pIShell, 
                             (PFNNOTIFY) ShowNextSlide, 
                             pThis );
          // Point to the currently displayed frame
          pInfo->m_dir = -1;
          ShowNextSlide( p );
          result = TRUE;
          break;

        case AVK_RIGHT:
          ISHELL_CancelTimer( pThis->a.m_pIShell, 
                             (PFNNOTIFY) ShowNextSlide, 
                             pThis );
          pInfo->m_dir = 1;
          ShowNextSlide( p );
          result = TRUE;
          break;
      }
      break;

    default:
      break;
  }

  return result;
}
