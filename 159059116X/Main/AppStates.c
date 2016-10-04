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
 *  This file provides the implementation for the states.
 */

/*
 * Includes
 */
#include "inc.h"


/*
 * Prototypes
 */

/*
 * Implementation
 */

/**
 * Initialize the application-specific data.
 * @param *pThis: application
 * @return EFAILED or ESUCCESS
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

/**
 * Frees the application-specific data.
 * @param *pThis: application
 * @return nothing
 */
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

/**
 * Sets the application's default preferences.
 * @param *pThis: 
 * @return nothing
 */
void AS_DefaultPrefs( CAppPrefsPtr pPrefs )
{
	ASSERT( pPrefs );
	MEMSET( pPrefs, 0, sizeof( CAppPrefs) );
}

/**
 * Handle incoming notifications.
 * @param void *pThis: application
 * @return TRUE if the notification was handled.
 */
boolean AS_HandleNotify( CAppPtr pThis, uint16 wParam, uint32 dwParam )
{
	DBGPRINTF( "Notification %x %x", wParam, dwParam );
	
	return TRUE;
}


/**
 * Sets a record field to the specific value from a database entry
 * @param pRecord: RAM-resident record in which to set value
 * @param iFieldName: name of field to set
 * @param iFieldType: data type of field
 * @param pData: pointer to data to set
 * @param piFieldLength: size of pointer to data allocated
 * @return: SUCCESS on success, or else BREW error code
 */
int AS_DBFieldFromDatabase( CAppRecordPtr pRecord, 
					        AEEDBFieldName iFieldName, 
						    AEEDBFieldType iFieldType,
						    byte *pData, uint16 iFieldLength )
{
	int result = SUCCESS;
	
	switch (iFieldName)
	{
		// For each field, set the field to the contents
		case Fld_unused:
			pRecord->unused = (uint16)*pData;
			break;
			
		// We probably shouldn't be here.
		default:
			result = EBADPARM;
			break;
	}
	return result;
}


/**
 * Sets a database record field to the specific value from a record 
 * @param pRecord: RAM-resident record from which to set value
 * @param iFieldName: name of field to set
 * @param piFieldType: pointer to where to store data type of field
 * @param ppData: pointer to pointer where data is to copy
 * @param piFieldLength: pointer to size of pointer to data allocated
 * @return: SUCCESS on success, or else BREW error code
 */
int AS_DBFieldToDatabase( CAppRecordPtr pRecord, 
			  	          AEEDBFieldName iFieldName, 
						  AEEDBFieldType *piFieldType,
						  byte **ppData, uint16 *piFieldLength )
{
	int result = SUCCESS;
	
	switch (iFieldName)
	{
		// For each field, set the field to the contents
		case Fld_unused:
			*ppData = (byte*)&(pRecord->unused);
			*piFieldType = AEEDB_FT_WORD;
			*piFieldLength = sizeof( int16 );
			break;
		// We probably shouldn't be here.
		default:
			result = EBADPARM;
			break;
	}
	return result;
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

  UNUSED( change );

  ASSERT( pThis );

  // Clear the display
  IDISPLAY_ClearScreen( GetDisplay( pThis ) );
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
boolean AS_MainHandleEvent( void *p, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam)
{
	CAppPtr pThis = (CAppPtr)p;
	boolean result = FALSE;
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
