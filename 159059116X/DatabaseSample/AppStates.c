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

typedef struct _NumericNode DBRecordCache;
typedef struct _NumericNode *DBRecordCachePtr;

#define GetRecordCache( p ) (GetAppData( pThis )->m_recordCache)

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
		SetAppData(pThis, pAppData );
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
		if ( pAppData->m_recordCache )
		{
			DBRecordCachePtr pTemp, pNode;
			pNode = pAppData->m_recordCache;
			while( pNode )
			{
				pTemp = pNode;
				pNode = NodeNext( pTemp );
				FREE( pTemp );
			}
		}
		if ( pAppData->m_recordCache ) FREE( pAppData->m_recordCache );
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
	case Fld_EntryType:
		pRecord->m_type = *((uint16 *)pData);
		break;			
	case Fld_Time:
		pRecord->m_nTime = *((uint32 *)pData);
		break;
	case Fld_Miles:
		pRecord->m_nMiles = *((uint32 *)pData);
		break;
	case Fld_Other:
		pRecord->m_nOther = *((uint32 *)pData);
		break;
	case Fld_Cost:
		pRecord->m_nCost = *((uint32 *)pData);
		break;
	case Fld_DueTime:
		pRecord->m_nDueTime = *((uint32 *)pData);
		break;
	case Fld_DueMiles:
		pRecord->m_nMiles = *((uint32 *)pData);
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
	case Fld_EntryType:
		*ppData = (byte *)&(pRecord->m_type);
		*piFieldType = AEEDB_FT_WORD;
		*piFieldLength = sizeof( uint16 );
		break;
	case Fld_Time:
		*ppData = (byte *)&(pRecord->m_nTime);
		*piFieldType = AEEDB_FT_DWORD;
		*piFieldLength = sizeof( uint32 );
		break;			
	case Fld_Miles:
		*ppData = (byte *)&(pRecord->m_nMiles);
		*piFieldType = AEEDB_FT_DWORD;
		*piFieldLength = sizeof( uint32 );
		break;			
	case Fld_Other:
		*ppData = (byte *)&(pRecord->m_nOther);
		*piFieldType = AEEDB_FT_DWORD;
		*piFieldLength = sizeof( uint32 );
		break;			
	case Fld_Cost:
		*ppData = (byte *)&(pRecord->m_nCost);
		*piFieldType = AEEDB_FT_DWORD;
		*piFieldLength = sizeof( uint32 );
		break;						
	case Fld_DueTime:
		*ppData = (byte *)&(pRecord->m_nDueTime);
		*piFieldType = AEEDB_FT_DWORD;
		*piFieldLength = sizeof( uint32 );
		break;			
	case Fld_DueMiles:
		*ppData = (byte *)&(pRecord->m_nDueMiles);
		*piFieldType = AEEDB_FT_DWORD;
		*piFieldLength = sizeof( uint32 );
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
* Adds some records to the database.
* @param void *p: this applicaton
* @return nothing
*/
static void add( void *p ) 
{
	CAppPtr pThis = (CAppPtr)p;
	CAppRecord record;
	AEEDBField *arFields;
	IDBMgr *pIDBMgr;
	IDatabase *pIDatabase;
	IDBRecord *pIDBRecord;
	int i;
	int miles = 20000;
	uint32 time = ISHELL_GetSeconds( GetShell( pThis ) );

	// Open the database
	if ( ISHELL_CreateInstance( GetShell( pThis ), 
		AEECLSID_DBMGR, (void **)&pIDBMgr ) != SUCCESS ) 
		return;
	pIDatabase = IDBMGR_OpenDatabase( pIDBMgr, 
		APP_DATABASE_NAME, TRUE );
	IDBMGR_Release( pIDBMgr );
	if ( !pIDatabase ) return;

#define EIGHTWEEKSASSECONDS (4838400)
	for ( i = ME_Gas; i < ME_Undefined; i++ )
	{
		record.m_type = i;
		record.m_nMiles = miles;
		miles += 500;
		record.m_nDueMiles = miles;
		record.m_nTime = time;
		record.m_nDueTime = record.m_nTime + EIGHTWEEKSASSECONDS;
		record.m_nCost = 2495;
		record.m_nOther = 31415;
		if ( DBFreezeRecord( &record, &arFields ) == SUCCESS )
		{
			pIDBRecord = IDATABASE_CreateRecord( pIDatabase, 
				arFields, 
				Fld_LastField );		
			IDBRECORD_Release( pIDBRecord );
		}
		if ( arFields ) 
			FREE( arFields );
		time += ( 24 * 60 * 60 );
	}
	IDATABASE_Release( pIDatabase );
}

static int32 orderRecordsByTime( void *p, 
								DBRecordCachePtr n1, 
								DBRecordCachePtr n2 )
{
	int32 result = 0;
	UNUSED( p );	
	// Compare the two
	return n2->m_nMetaData - n1->m_nMetaData;
}

/** 
* Sorts the records in the database; outputs 
* records in order to debug console
* @param void *p: this applicaton
* @return nothing
*/
static void sort( void *p ) 
{
	CAppPtr pThis = (CAppPtr)p;
	IDBMgr *pIDBMgr;
	IDatabase *pIDatabase;
	IDBRecord *pIDBRecord;
	DBRecordCachePtr pNode, pHead;
	AEEDBFieldType iFieldType;
	uint16 iFieldLength;
	uint32 *pt;
	CAppRecordPtr pRecord;

	pHead = MALLOC( sizeof( DBRecordCache ) );
	GetAppData(pThis)->m_recordCache = pHead;
	if ( pHead )
	{
		MEMSET( pHead, 0, sizeof( DBRecordCache ) );
	}
	else return;

	// Open the database
	if ( ISHELL_CreateInstance( GetShell( pThis ), 
		AEECLSID_DBMGR, (void **)&pIDBMgr ) != SUCCESS ) 
		return;
	GetAppData( pThis )->m_pIDatabase = pIDatabase = 
		IDBMGR_OpenDatabase( pIDBMgr, APP_DATABASE_NAME, TRUE );

	IDBMGR_Release( pIDBMgr );
	if ( !pIDatabase ) return;

	// Iterate across each record of the database, adding
	// each to the linked list in turn.
	IDATABASE_Reset( pIDatabase );
	pIDBRecord = IDATABASE_GetNextRecord( pIDatabase );

	while( pIDBRecord != NULL )
	{
		pNode = (DBRecordCachePtr)MALLOC( sizeof( DBRecordCache ) );
		if ( !pNode )
		{
			DBRecordCachePtr pTemp;
			pNode = NodeNext( pHead );
			while( pNode )
			{
				pTemp = pNode;
				pNode = NodeNext( pTemp );
				FREE( pTemp );
			}
			return;
		}
		pNode->m_nData = IDBRECORD_GetID( pIDBRecord );	

		DBRecordField( pIDBRecord, Fld_Time, &iFieldType, &pt, &iFieldLength );	
		pNode->m_nMetaData = *pt;
		FREE( pt );

		NodeLinkSorted( pThis, pHead, pNode, orderRecordsByTime, DBRecordCache );
		IDBRECORD_Release( pIDBRecord );
		pIDBRecord = IDATABASE_GetNextRecord( pIDatabase );
	}
	// Iterate across all the nodes.
	// Dump their contents to the debug log so we can see each record
	pNode = NodeNext( pHead );

	while( pNode )
	{
		pRecord = NULL;
		pIDBRecord = IDATABASE_GetRecordByID( pIDatabase, (uint16)pNode->m_nData );
		if ( pIDBRecord ) DBThawRecord( pIDBRecord, &pRecord );
		if ( pRecord )
		{
			DBGPRINTF("Type : %d", pRecord->m_type );
			DBGPRINTF("Time : %d", pRecord->m_nTime );
			DBGPRINTF("Miles: %d", pRecord->m_nMiles );
			DBGPRINTF("Cost : %d", pRecord->m_nCost );
			DBGPRINTF("Other: %d", pRecord->m_nOther );
			DBGPRINTF("Next (time) : %d", pRecord->m_nDueTime );
			DBGPRINTF("Next (miles): %d", pRecord->m_nDueMiles );
		}
		if ( pIDBRecord ) IDBRECORD_Release( pIDBRecord );
		if ( pRecord ) FREE( pRecord );
		pNode = NodeNext( pNode );
	}

	IDATABASE_Release( pIDatabase );
	GetAppData( pThis )->m_pIDatabase  = NULL;

	if ( pHead )
	{
		DBRecordCachePtr pTemp, pNode;
		pNode = pHead;
		while( pNode )
		{
			pTemp = pNode;
			pNode = NodeNext( pTemp );
			FREE( pTemp );
		}
	}
}

/** 
* Removes all records in the database.
* @param void *p: this applicaton
* @return nothing
*/
static void del( void *p ) 
{
	CAppPtr pThis = (CAppPtr)p;
	IDBMgr *pIDBMgr;
	IDatabase *pIDatabase;
	IDBRecord *pIDBRecord;

	// Open the database
	if ( ISHELL_CreateInstance( GetShell( pThis ), 
		AEECLSID_DBMGR, (void **)&pIDBMgr ) != SUCCESS ) 
		return;
	GetAppData( pThis )->m_pIDatabase = pIDatabase = 
		IDBMGR_OpenDatabase( pIDBMgr, APP_DATABASE_NAME, TRUE );
	IDBMGR_Release( pIDBMgr );
	if ( !pIDatabase ) return;


	IDATABASE_Reset( pIDatabase );
	pIDBRecord = IDATABASE_GetNextRecord( pIDatabase );

	while( pIDBRecord != NULL )
	{
	    IDBRECORD_Remove(  pIDBRecord );
		pIDBRecord = IDATABASE_GetNextRecord( pIDatabase );
	}
	IDATABASE_Release( pIDatabase );
}


/** 
* Performs actions associated with a menu selection.
* @param void *p: this applicaton
* @param choice: menu selection
* @return nothing
*/
static boolean handleMenuChoice( void *p, 
								uint16 choice)
{
	switch( choice )
	{
	case 1:
		add( p );
		break;
	case 2:
		sort( p );
		break;
	case 3:
		del( p );
		break;
	}
	return TRUE;

}



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
	CtlAddItem addItemInfo = { 0 };
	int itemID = 1;
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );

	// Build a menu of choices for controls.
	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)NULL;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = NULL; // APP_RES_FILE;
	addItemInfo.wText = IDS_M_ADD;
	addItemInfo.wImage = 0;
	IMENUCTL_AddItemEx( GetMenu( pThis ), &addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)NULL;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = NULL; // APP_RES_FILE;
	addItemInfo.wText = IDS_M_SORT;
	addItemInfo.wImage = 0;
	IMENUCTL_AddItemEx( GetMenu( pThis ), &addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)NULL;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = NULL; // APP_RES_FILE;
	addItemInfo.wText = IDS_M_DELETE;
	addItemInfo.wImage = 0;
	IMENUCTL_AddItemEx( GetMenu( pThis ), &addItemInfo );

	Control_ShowControl( GetMenu( pThis ), MP_UNDERLINE_TITLE, pThis->m_rc );

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

		case EVT_COMMAND:
			result = handleMenuChoice( p, wParam );
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