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

#define VIEW_EXTENTS ( 256 )

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

		// Set our canvas size
		pAppData->cxCanvas = VIEW_EXTENTS;
		pAppData->cyCanvas = VIEW_EXTENTS;

		// Set the default viewport position to the center of the extents
		pAppData->x = pAppData->cxCanvas / 2;
		pAppData->y = pAppData->cyCanvas / 2;

		SetAppData( pThis, pAppData );
		result = ISHELL_CreateInstance( GetShell( pThis ),
			AEECLSID_GRAPHICS,
			(void **) &(pAppData->pIGraphics) );
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
	IGRAPHICS_Release( pAppData->pIGraphics );
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
* Draws a bunch of random stuff to the viewport
* @param pThis: Application pointer
*/
static void mainDraw( CAppPtr pThis )
{
	CAppDataPtr pData = GetAppData( pThis );
	uint16 iShape, i;
	IGraphics *pIGraphics = pData->pIGraphics;
	AEEClip clip = { 0 };	
	AEETriangle triangle;
	AEERect rectangle;
	AEEEllipse ellipse;
	byte r, g, b, shape;
	boolean fill;
	uint16 *arRandom = pData->arRandom;
	
	i = 0;

	// Clear our canvas
	IGRAPHICS_SetBackground( pIGraphics, 255, 255, 255 );
	IGRAPHICS_ClearViewport( pIGraphics );

	// Set our clipping region
	clip.type = CLIPPING_RECT;
	clip.shape.rect.x = 0;
	clip.shape.rect.y = 0;
	clip.shape.rect.dx = pData->cxCanvas;
	clip.shape.rect.dy = pData->cyCanvas;
	IGRAPHICS_SetClip( pIGraphics, &clip, 0 );	

	for ( iShape = 0; iShape < NUMSHAPES; iShape++ )
	{
		shape = (byte)(arRandom[ i++ ] % 4);
		r = (byte)(arRandom[ i++ ] % 255 );
		g = (byte)(arRandom[ i++ ] % 255 );
		fill = (boolean)( ( arRandom[ i ] >> 8 ) && 0x1 );
		b = (byte)(arRandom[ i++ ] % 255 );

		// Set the fill color and mode for this shape
		IGRAPHICS_SetFillColor( pIGraphics, r, g, b, 0 );
		IGRAPHICS_SetFillMode( pIGraphics, fill );

		// Add a random shape
		switch( arRandom[i-1] % 3 )
		{
			case 0:
				// Draw a triangle
				triangle.x0 = arRandom[ i++ ] % VIEW_EXTENTS;
				triangle.y0 = arRandom[ i++ ] % VIEW_EXTENTS;
				triangle.x1 = arRandom[ i++ ] % VIEW_EXTENTS;
				triangle.y1 = arRandom[ i++ ] % VIEW_EXTENTS;
				triangle.x2 = arRandom[ i++ ] % VIEW_EXTENTS;
				triangle.y2 = arRandom[ i++ ] % VIEW_EXTENTS;
				IGRAPHICS_DrawTriangle( pIGraphics, &triangle );
				break;

			case 1:
				// Draw an square
				rectangle.x = arRandom[ i++ ] % VIEW_EXTENTS;
				rectangle.y = arRandom[ i++ ] % VIEW_EXTENTS;
				rectangle.dx = arRandom[ i++ ] % VIEW_EXTENTS / 4;
				rectangle.dy = arRandom[ i++ ] % VIEW_EXTENTS / 4;
				IGRAPHICS_DrawRect( pIGraphics, &rectangle );
				break;	

			case 2: 
				// Draw an ellipse
				ellipse.cx = arRandom[ i++ ] % VIEW_EXTENTS;
				ellipse.cy = arRandom[ i++ ] % VIEW_EXTENTS;
				ellipse.wx = arRandom[ i++ ] % VIEW_EXTENTS / 4;
				ellipse.wy = arRandom[ i++ ] % VIEW_EXTENTS / 4;
				IGRAPHICS_DrawEllipse( pIGraphics, &ellipse );
				break;
		}
	}

	// Update the display
	IGRAPHICS_Update( pIGraphics );		
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
	CAppDataPtr pData;

	UNUSED( change );

	ASSERT( pThis );

	pData = GetAppData( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );

	// Decide what to draw.
	// We're going to get a shape, colors, and three coordinates 
	// for each shape

	// Get a buffer filled with random numbers
	GETRAND( (byte *)pData->arRandom, 
		NUMSHAPES * 5 * sizeof( uint16 ) );
	
	IGRAPHICS_Pan( pData->pIGraphics, 
		VIEW_EXTENTS / 2, VIEW_EXTENTS/2 );
	
		
	// Do some drawing
	mainDraw( pThis );
	
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
* Handles arrow key presses by moving the view port.
* @param pThis: pointer to app instance
* @param wParam: key code
* @result TRUE if function handled the event
*/
static boolean mainHandleKey( CAppPtr pThis, uint16 wParam )
{
	boolean result = FALSE;
	int dx, dy;
	int32 newX, newY;
	CAppDataPtr pAppData = GetAppData( pThis );

	dx = dy = 0;

	switch( wParam )
	{
		case AVK_UP: 
			dy = -( pThis->m_cy / 4 );
			result = TRUE;
			break;
		case AVK_DOWN:
			dy = ( pThis->m_cy / 4 );
			result = TRUE;
			break;
		case AVK_LEFT:
			dx = -( pThis->m_cx / 4 );
			result = TRUE;
			break;
		case AVK_RIGHT:
			dx = ( pThis->m_cx / 4 );
			result = TRUE;
			break;
	}

	if ( result )
	{
		// Adjust the view port in the correct direction
		newX = pAppData->x + dx;
		newY = pAppData->y + dy;

		// Pin scrolling within our canvas
		if ( newX < 0 ) newX = 0;
		if ( newY < 0 ) newY = 0;
		if ( newX > VIEW_EXTENTS ) newX = VIEW_EXTENTS;
		if ( newY > VIEW_EXTENTS ) newY = VIEW_EXTENTS;

		pAppData->x = (uint16)newX;
		pAppData->y = (uint16)newY;
		IGRAPHICS_Pan( pAppData->pIGraphics, pAppData->x, pAppData->y );
		mainDraw( pThis );
	}

	return result;
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

		case EVT_KEY_PRESS:
		case EVT_KEY_HELD:
			result = mainHandleKey( p, wParam );
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