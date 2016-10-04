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

static int createBitmap( CAppPtr pThis, int16 w, int16 h, IBitmap **ppIBitmap )
{
	IBitmap *pIDeviceBitmap;
	IBitmap *pIBitmap;
	AEERect rect;

    if ( IDISPLAY_GetDeviceBitmap( GetDisplay( pThis ), &pIDeviceBitmap ) != SUCCESS )
	{
	    return ENOMEMORY;
	}
	
	if( IBITMAP_CreateCompatibleBitmap( pIDeviceBitmap, &pIBitmap, w, h ) != SUCCESS )
	{
		IBITMAP_Release( pIDeviceBitmap );
		return ENOMEMORY;
	}
	IBITMAP_Release( pIDeviceBitmap );
	SETAEERECT( &rect, 0, 0, w, h );
	IBITMAP_FillRect( pIBitmap, &rect, IBITMAP_RGBToNative( pIBitmap, RGB_WHITE ), AEE_RO_COPY );
	
	*ppIBitmap = pIBitmap;

	return SUCCESS;
}

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
	CtlAddItem addItemInfo = { 0 };
	int itemID = 1;
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );

	IMENUCTL_SetTitle( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ],
		APP_RES_FILE, 
		IDS_MAINMENUTITLE, 
		NULL );

	// Build a menu of choices for controls.
	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)AS_DisplayHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_DISP;
	addItemInfo.wImage = IDI_BMP1;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)AS_TransformHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_GRAPHICS;
	addItemInfo.wImage = IDI_BMP1;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&addItemInfo );

	ICONTROL_SetProperties( pThis->m_app.m_apControl[ Ctl_NavMenu ],
		MP_UNDERLINE_TITLE );

	// Size the menu.
	ICONTROL_SetRect( pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&pThis->m_rc );

	ICONTROL_SetActive( pThis->m_app.m_apControl[ Ctl_NavMenu ], TRUE );
	ICONTROL_Redraw( pThis->m_app.m_apControl[ Ctl_NavMenu ] );

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

	ICONTROL_Reset( pThis->m_app.m_apControl[ Ctl_NavMenu ] );

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

/** 
* @param void *p: this applicaton
* @param EStateChange change: why we entered this state
* @return nothing
*/
static boolean displayEntry( void *p, 
							EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	IBitmap *pIBitmap;

	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis )  );

	// Fetch the bitmap in question
	pIBitmap = ISHELL_LoadResBitmap( GetShell( pThis ), APP_RES_FILE, IDI_BMP2 );
	
	// Bitblit to someplace on the display
	if ( pIBitmap )
	{
		IBitmap *pIDeviceBitmap;
		
		IDISPLAY_GetDeviceBitmap( GetDisplay( pThis ), &pIDeviceBitmap );
		
		if ( pIDeviceBitmap )
		{
			IBITMAP_BltIn( pIDeviceBitmap, 0, 0, 98, 130, 
							pIBitmap, 0, 0, AEE_RO_COPY );
			IBITMAP_Release( pIDeviceBitmap );
		}
	
	}
	// Free the bitmap in question
	IBITMAP_Release( pIBitmap );
	
	IDISPLAY_Update( GetDisplay( pThis ) );
	return TRUE;   
}

/** 
* @param void *p: this applicaton
* @param EStateChange change: why we exited this state
* @return nothing
*/
static boolean displayExit( void *p, 
						   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;

	UNUSED( change );

	ASSERT( pThis );

	return TRUE;
}


/** 
* @param void *p: this applicaton
* @param AEEEvent eCode: event code
* @param uint16 wParam: event parameter
* @param uint32 dwParam: event parameter
* @return TRUE if application handled event
*/
boolean AS_DisplayHandleEvent( void *p, 
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
			result = displayEntry( p, wParam );
			break;

		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = displayExit( p, wParam );
			break;

		case EVT_COMMAND:
			result = TRUE;
			break;

		default:
			break;
	}

	return result;
}

/** 
* @param void *p: this applicaton
* @param EStateChange change: why we entered this state
* @return nothing
*/
static boolean transformEntry( void *p, 
					  EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	IBitmap *pIBitmap;
	IBitmap *pIDeviceBitmap;

	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis )  );

	// Fetch the bitmap in question
	pIBitmap = ISHELL_LoadResBitmap( GetShell( pThis ), APP_RES_FILE, IDI_BMP2 );
	IDISPLAY_GetDeviceBitmap( GetDisplay( pThis ), &pIDeviceBitmap );
	
	// Bitblit to someplace on the display
	if ( pIBitmap && pIDeviceBitmap  )
	{
		ITransform *pITransform;
		IBITMAP_QueryInterface( pIDeviceBitmap, 
								 AEECLSID_TRANSFORM, 
								 (void **)&pITransform );

		if ( pITransform )
		{
			ITRANSFORM_TransformBltSimple( pITransform,
											60, 60,
											pIBitmap,
											0, 0, 98, 130,
											TRANSFORM_SCALE_QUARTER , 
											COMPOSITE_OPAQUE );
			ITRANSFORM_Release( pITransform );
			
			DBGPRINTF("Transform done");				
		}
		else
		{
			IBITMAP_BltIn( pIDeviceBitmap, 0, 0, 98, 130, 
								pIBitmap, 0, 0, AEE_RO_COPY );
		}
		IBITMAP_Release( pIDeviceBitmap );
		IBITMAP_Release( pIBitmap );
	
	}	
		
	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );
	return TRUE;  
}

/** 
* @param void *p: this applicaton
* @param EStateChange change: why we exited this state
* @return nothing
*/
static boolean transformExit( void *p, 
					 EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;

	UNUSED( change );

	ASSERT( pThis );

	return TRUE;
}


/** 
* @param void *p: this applicaton
* @param AEEEvent eCode: event code
* @param uint16 wParam: event parameter
* @param uint32 dwParam: event parameter
* @return TRUE if application handled event
*/
boolean AS_TransformHandleEvent( void *p, 
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
			result = transformEntry( p, wParam );
			break;

		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = transformExit( p, wParam );
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
