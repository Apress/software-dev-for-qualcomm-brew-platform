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

// #define DEBUG_SPRITES
/*
* Prototypes
*/
static void initSprites( AEESpriteCmd *pSprites );
static int initTiles( CAppPtr pThis, 
	AEETileMap *pTileMap, 
	int width, int height,
	uint16 *pRandom );
static int createBitmap( CAppPtr pThis, int16 w, int16 h, IBitmap **ppIBitmap );
static int loadSprites( CAppPtr pThis, IBitmap **ppBitmap );
static int loadTiles( CAppPtr pThis, IBitmap **ppBitmap );
static void mainDrawUpdate( CAppPtr pThis );
static void moveButterflies( CAppPtr pThis, uint16 *pRandom );
static void moveMouse( CAppPtr pThis, uint16 *pRandom );
static void mainDraw( void *p );
static void mainSoundNotifyCallback( void *p, 
									 AEESoundPlayerCmd  eType,
									 AEESoundPlayerStatus eStatus,
									 uint32 dwParam);
static void mainMusicPlay( CAppPtr pThis );
static void mainMusicStart( CAppPtr pThis );
static void mainMusicStop( CAppPtr pThis );
static boolean mainEntry( void *p, 
						 EStateChangeCause change );
static boolean mainExit( void *p, 
						EStateChangeCause change );						 		 
/*
* Implementation
*/


#define PIN_X_COORD( pThis, x ) \
	if ( x < 8 ) x = 8; else if ( x > 100 ) x = 100;
#define PIN_Y_COORD( pThis, y ) \
	if ( y < 8 ) y = 8; else if ( y > 100 ) y = 100;


static void initSprites( AEESpriteCmd *pSprites )
{	
	pSprites[Sprite_Mouse].x = 32;
	pSprites[Sprite_Mouse].y = 32;
	pSprites[Sprite_Mouse].unTransform = 0;
	pSprites[Sprite_Mouse].unSpriteSize = SPRITE_SIZE_16X16;
	pSprites[Sprite_Mouse].unSpriteIndex = Sprite_Mouse;
	pSprites[Sprite_Mouse].unLayer = 1;
	pSprites[Sprite_Mouse].unComposite = COMPOSITE_KEYCOLOR;

	pSprites[Sprite_Cat].x = 128 - 32;
	pSprites[Sprite_Cat].y = 128 - 32;
	pSprites[Sprite_Cat].unTransform = TRANSFORM_SCALE_2;
	pSprites[Sprite_Cat].unSpriteSize = SPRITE_SIZE_16X16;
	pSprites[Sprite_Cat].unSpriteIndex = Sprite_Cat;
	pSprites[Sprite_Cat].unLayer = 1;
	pSprites[Sprite_Cat].unComposite = COMPOSITE_KEYCOLOR;
	
	pSprites[Sprite_Butterfly_Red].x = 32;
	pSprites[Sprite_Butterfly_Red].y = 128 - 32;
	pSprites[Sprite_Butterfly_Red].unTransform = 0;
	pSprites[Sprite_Butterfly_Red].unSpriteSize = SPRITE_SIZE_16X16;
	pSprites[Sprite_Butterfly_Red].unSpriteIndex = Sprite_Butterfly_Red;
	pSprites[Sprite_Butterfly_Red].unLayer = 3;
	pSprites[Sprite_Butterfly_Red].unComposite = COMPOSITE_KEYCOLOR;

	pSprites[Sprite_Butterfly_Blue].x = 128 - 32;
	pSprites[Sprite_Butterfly_Blue].y = 32;
	pSprites[Sprite_Butterfly_Blue].unTransform = 0;
	pSprites[Sprite_Butterfly_Blue].unSpriteSize = SPRITE_SIZE_16X16;
	pSprites[Sprite_Butterfly_Blue].unSpriteIndex = Sprite_Butterfly_Blue;
	pSprites[Sprite_Butterfly_Blue].unLayer = 3;
	pSprites[Sprite_Butterfly_Blue].unComposite = COMPOSITE_KEYCOLOR;
	
	pSprites[Sprite_Last].x = 0;
	pSprites[Sprite_Last].y = 0;
	pSprites[Sprite_Last].unTransform = 0;
	pSprites[Sprite_Last].unSpriteSize = SPRITE_SIZE_END;
	pSprites[Sprite_Last].unSpriteIndex = 0;
	pSprites[Sprite_Last].unLayer = 0;
	pSprites[Sprite_Butterfly_Blue].unComposite = COMPOSITE_KEYCOLOR;
}

static int initTiles( CAppPtr pThis, 
	AEETileMap *pTileMap, 
	int width, int height,
	uint16 *pRandom )
{
	int i;
	uint16 *pTile;
	int flower = 0;
	
	// Create the tile map array.
	pTileMap->pMapArray = (uint16 *)MALLOC( width * height * sizeof( uint16 ) );

	if ( !pTileMap->pMapArray ) return ENOMEMORY;
	// Now fill it
	pTile = pTileMap->pMapArray;
	for ( i = 0; 
		  i < ( width * height ); 
		  i++ )
	{
		*pTile = *pRandom % 24;
		// Only 1 in 24 tiles has a flower (statistically speaking)
		if ( *pTile == 0 )
		{
			// Get another random bit to choose the tile
			if ( flower % 2 )
			{
				*pTile = Tile_Pink;
			}
			else
			{
				*pTile = Tile_Purple;
			}
			flower++;
		}
		else
		{
			*pTile = Tile_Grass;
		}
		pRandom++;
		pTile++;
	}
	
	// Center the tile map on the display.
	pTileMap->x = 0;
	pTileMap->y = 0;
	pTileMap->w = MAP_SIZE_8;
	pTileMap->h = MAP_SIZE_8;
	
	// The tiles are 16x16 pixels
	pTileMap->unTileSize = TILE_SIZE_16X16;

	// No flags
	pTileMap->unFlags = 0;
	
	// You MUST set these to zero, or bad things happen.
	pTileMap->reserved[ 0 ] = 
	pTileMap->reserved[ 1 ] = 
	pTileMap->reserved[ 2 ] = 
	pTileMap->reserved[ 3 ] = 0;
	
	pTileMap->reserved2[ 0 ] = 
	pTileMap->reserved2[ 1 ] = 
	pTileMap->reserved2[ 2 ] = 0;
	
	// Initialize the end-of-tile-map structure
	pTileMap++;
	
	pTileMap->pMapArray = NULL;
	
	return SUCCESS;
}

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

static int loadSprites( CAppPtr pThis, IBitmap **ppBitmap )
{
	IBitmap *pISpriteBitmap;
	int16 x, y, i;
	IBitmap *pBitmap;
	
	x = y = i = 0;
	
	if ( createBitmap( pThis, 16, 16 * Sprite_Last, &pISpriteBitmap ) != SUCCESS )
		return ENOMEMORY;
	
	// Blit each of the sprites on to the bitmap buffer.
	for ( i = Sprite_Mouse; i < Sprite_Last; i++ )
	{
		pBitmap = ISHELL_LoadResBitmap( GetShell( pThis ), APP_RES_FILE, RESID_SPRITE_BASE + i );
		if ( !pBitmap )
		{
			DBGPRINTF("Honk!");
			IBITMAP_Release( pISpriteBitmap );
			return ENOMEMORY;
		}
	
		IBITMAP_BltIn( pISpriteBitmap, x, y, 16, 16, 
					   pBitmap, 0, 0, AEE_RO_COPY );
		y+=16;
		IBITMAP_Release( pBitmap );
	}
#ifdef DEBUG_SPRITES
	{
		IBitmap *pIDeviceBitmap;
		IDISPLAY_ClearScreen( GetDisplay( pThis ) );
		IDISPLAY_Update( GetDisplay( pThis ) );
		
		IDISPLAY_GetDeviceBitmap( GetDisplay( pThis ), &pIDeviceBitmap );
		
		
		
		
		if ( pIDeviceBitmap )
		{
			IBITMAP_BltIn( pIDeviceBitmap, 0, 0, 16, 16 * Sprite_Last, 
							pISpriteBitmap, 0, 0, AEE_RO_COPY );
			IBITMAP_Release( pIDeviceBitmap );
		}

		IDISPLAY_Update( GetDisplay( pThis ) );
	}
#endif


	*ppBitmap = pISpriteBitmap;
	return SUCCESS;

}

static int loadTiles( CAppPtr pThis, IBitmap **ppBitmap )
{
	IBitmap *pITileBitmap;
	int16 x, y, i;
	IBitmap *pBitmap;
	
	x = y = i = 0;

	if ( createBitmap( pThis, 16, 16 * Tile_Last, &pITileBitmap ) != SUCCESS )
		return ENOMEMORY;
	

	// Blit each of the sprites on to the bitmap buffer.
	for ( i = Tile_Grass; i < Tile_Last; y+=16, i++ )
	{
		pBitmap = ISHELL_LoadResBitmap( GetShell( pThis ), APP_RES_FILE, RESID_TILE_BASE + i );
		if ( !pBitmap )
		{
			DBGPRINTF("Honk!");
			IBITMAP_Release( pITileBitmap );
			return ENOMEMORY;
		}
	
		IBITMAP_BltIn( pITileBitmap, x, y, 16, 16, 
					   pBitmap, 0, 0, AEE_RO_COPY );
		
		IBITMAP_Release( pBitmap );
	
	}
	
#ifdef DEBUG_SPRITES
	{
		IBitmap *pIDeviceBitmap;
		IDISPLAY_GetDeviceBitmap( GetDisplay( pThis ), &pIDeviceBitmap );
		
		if ( pIDeviceBitmap )
		{
			IBITMAP_BltIn( pIDeviceBitmap, 32, 0, 16, 16 * Tile_Last, 
							pITileBitmap, 0, 0, AEE_RO_COPY );
			IBITMAP_Release( pIDeviceBitmap );
		}

		IDISPLAY_Update( GetDisplay( pThis ) );
	}
#endif
	
	*ppBitmap = pITileBitmap;	
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
	uint16 arRandom[ 256  ];
	int width, height;
	IBitmap *pIBitmap;
	ISprite *pISprite;
	
	width = 8;
	height = 8;

	ASSERT( pThis );

	// Create the application's global data here.
	pAppData = MALLOC( sizeof( CAppData ) );

	if ( pAppData )
	{
		MEMSET( pAppData, 0, sizeof( CAppData ) );

		// Get our sprite interface
		result = ISHELL_CreateInstance( GetShell( pThis ),
			AEECLSID_SPRITE,
			(void **) &(pAppData->pISprite) );
		pISprite = pAppData->pISprite;

		if ( result == SUCCESS )
		{
			// Get a bag of random numbers
			GETRAND( (byte *)arRandom, 
			( 2 * Sprite_Last + width * height ) 
				* sizeof( uint16 ) );

			// Initialize our sprites
			initSprites( pAppData->arSprites );
			result = loadSprites( pThis, &pIBitmap );
			if ( result != SUCCESS )
			{
				if ( pIBitmap ) IBITMAP_Release( pIBitmap );
				ISPRITE_Release( pISprite );
				return result;
			}
			ISPRITE_SetSpriteBuffer( pISprite, TILE_SIZE_16X16, pIBitmap );
			IBITMAP_Release( pIBitmap );
			
			pIBitmap = NULL;
			
			// Initialize our tile map
			result = initTiles( pThis, 
				pAppData->arTileMap,
				width, height,
				arRandom );
			if ( result != SUCCESS )
			{
				if ( pIBitmap ) IBITMAP_Release( pIBitmap );
				ISPRITE_Release( pISprite );
				return result;
			}
			result = loadTiles( pThis, &pIBitmap );
			if ( result != SUCCESS )
			{
				IBITMAP_Release( pIBitmap );
				ISPRITE_Release( pISprite );
				return result;
			}
			ISPRITE_SetTileBuffer( pISprite, TILE_SIZE_16X16, pIBitmap );
			IBITMAP_Release( pIBitmap );

			// Set the destination
			result = createBitmap( pThis, pThis->m_cx, pThis->m_cy, &pIBitmap );
			if ( result != SUCCESS )
			{
				ISPRITE_Release( pISprite );
				return result;
			}
			ISPRITE_SetDestination( pAppData->pISprite, pIBitmap );
			pAppData->pIBitmap = pIBitmap;
			
			// Stash aside our application globals
			SetAppData( pThis, pAppData );
		}
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
		if ( pAppData->pISprite ) ISPRITE_Release( pAppData->pISprite );
		if ( pAppData->pIBitmap ) IBITMAP_Release( pAppData->pIBitmap );
		if ( pAppData->arTileMap[0].pMapArray )
			FREE( pAppData->arTileMap[0].pMapArray );
			
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

static void mainDrawUpdate( CAppPtr pThis )
{
	CAppDataPtr pData = GetAppData( pThis );
	ISprite *pISprite = pData->pISprite;
	IBitmap *pIDisplayBitmap = NULL;
	int result;
	

	// Draw the tiles
	ISPRITE_DrawTiles( pISprite, pData->arTileMap);

	// Draw the sprites
	ISPRITE_DrawSprites( pISprite, pData->arSprites );
	
	// Update the display
	result = IDISPLAY_GetDeviceBitmap( GetDisplay( pThis ), &pIDisplayBitmap );
	if ( result == SUCCESS )
	{
		IBITMAP_BltIn( pIDisplayBitmap,
			0, 0, // destination origin
			pThis->m_cx, pThis->m_cy,
			pData->pIBitmap, 0, 0, AEE_RO_COPY );

		IDISPLAY_Update( GetDisplay( pThis ) );
		IBITMAP_Release( pIDisplayBitmap );
	}
}

static void moveButterflies( CAppPtr pThis, uint16 *pRandom )
{
	CAppDataPtr pData = GetAppData( pThis );
	int16 newX, newY;
	int rotate;
	int dx, dy;
	int i;
	
	// For each butterfly...
	for ( i = Sprite_Butterfly_Red; 
		  i <= Sprite_Butterfly_Blue; 
		  pRandom++, i++ )
	{
		rotate = *pRandom & 0x3;
		dx = ( ( *pRandom & 0x0f0 ) >> 4 ) - 8 ;
		dy = ( ( *pRandom & 0xf00 ) >> 8 ) - 8 ;
		dx /= 2;
		dy /= 2;
		
		newX = pData->arSprites[ i ].x + dx;
		newY = pData->arSprites[ i ].y + dy;
		PIN_X_COORD( pThis, newX );
		PIN_Y_COORD( pThis, newY );
		pData->arSprites[ i ].x = (uint16)newX;
		pData->arSprites[ i ].y = (uint16)newY;
		pData->arSprites[ i ].unTransform = rotate;
	}
}

static void moveMouse( CAppPtr pThis, uint16 *pRandom )
{
	CAppDataPtr pData = GetAppData( pThis );
	int transform = 0;
	int16 newX, newY;
	int dx, dy, dx1, dy1;
	int xS, yS;
	
	// if the cat gets close to the mouse, we move.
	dx = pData->arSprites[ Sprite_Cat ].x - pData->arSprites[ Sprite_Mouse ].x;
	dy = pData->arSprites[ Sprite_Cat ].y - pData->arSprites[ Sprite_Mouse ].y;
	dx1 = dx > 0 ? dx : -dx;
	dy1 = dy > 0 ? dy : -dy;
	
	if ( dx1 < MOUSE_TERRITORY ||
		 dy1 < MOUSE_TERRITORY )
	{
		// If we're really close, play a squeaky sound.
		if ( pData->pISound &&
			 dx1 < MOUSE_TOO_CLOSE && dy1 < MOUSE_TOO_CLOSE )
		{
			AEESoundToneData tone = { 0 };
			
			tone.eTone = AEE_TONE_REORDER_TONE;
			tone.wDuration = 500;
			
			ISOUND_PlayTone( pData->pISound, tone );
			ISOUND_Vibrate( pData->pISound, tone.wDuration );
			
		}
	
		// dx, dy will now hold how far to move.
		xS = dx < 0 ? 1 : -1;
		yS = dy < 0 ? 1 : -1;
		
		if ( dx1 > 0 )
		{
			dx = xS * ( *pRandom & 0x3 );
		}
		if ( dy1 > 0 )
		{
			dy = yS * ( ( *pRandom >> 2 )  & 0x3 );
		}
		
		
		// rotate the mouse to face the direction it ran.
		if ( dx > 0 )
		{
			// Face mouse to the right.
			transform = TRANSFORM_ROTATE_90;
		}
		else if ( dx < 0 )
		{
			transform = TRANSFORM_ROTATE_270;		
		}
		if ( dy > 0 )
		{
			transform = 0;
		}
		else if (dy < 0 )
		{
			transform = TRANSFORM_ROTATE_180;		
		}
		
		newX = pData->arSprites[ Sprite_Mouse ].x + dx;
		newY = pData->arSprites[ Sprite_Mouse ].y + dy;
		PIN_X_COORD( pThis, newX );
		PIN_Y_COORD( pThis, newY );
		pData->arSprites[ Sprite_Mouse ].x = (uint16)newX;
		pData->arSprites[ Sprite_Mouse ].y = (uint16)newY;
		pData->arSprites[ Sprite_Mouse ].unTransform = transform;	
	}
}

static void mainDraw( void *p )
{
	CAppPtr pThis = (CAppPtr)p;
	CAppDataPtr pData = GetAppData( pThis );
	ISprite *pISprite = pData->pISprite;
	uint16 arRandom[ 3 ];
	
	// Get a bag of random numbers
	GETRAND( (byte *)arRandom, 
			 ( 2 + 1 ) * sizeof( uint16 ) );
		
	// Update the display
	mainDrawUpdate( pThis );	
	
	/* 
	   Figure out where things are going to move.
	   Butterflies move every turn
	   The mouse moves every two turns
	 */
	
	// Move the butterflies
	moveButterflies( pThis, arRandom );
	
	// Move the mouse
	if ( pData->nTurn % 2 )
	{
		moveMouse( pThis, arRandom + 2 );
	}

	// And do it again!
	pData->nTurn++;
	
	ISHELL_SetTimer( GetShell( pThis ), 
					FRAME_DELAY_MSECS, 
					mainDraw, pThis );
}

static void mainSoundNotifyCallback( void *p, 
									 AEESoundPlayerCmd  eType,
									 AEESoundPlayerStatus eStatus,
									 uint32 dwParam)
{
	CAppPtr pThis = (CAppPtr)p;
	CAppDataPtr pData;

	if ( !pThis ) return;
	pData = GetAppData( pThis );
	if ( !pData ) return;
	if ( !pData->pISound && !pData->pISoundPlayer ) return;

	if ( eStatus == AEE_SOUNDPLAYER_DONE )
	{
		mainMusicPlay( pThis );
	}
	
}


static void mainMusicPlay( CAppPtr pThis )
{
	CAppDataPtr pData = GetAppData( pThis );
	
	// It's OK if there's no sound player
	if ( pData->pISoundPlayer )
	{
		ISoundPlayer *pISoundPlayer = pData->pISoundPlayer;
		AEESoundPlayerInfo info = { 0 };
		int result;

		// Set our data source
		info.eInput = SDT_FILE;
		info.pData = MUSIC_FILE;
		
		// SetInfo may not be available on older platforms.
		// But Set is deprecated, so you need to use the right one
		// based on your includes.
		result = ISOUNDPLAYER_SetInfo( pISoundPlayer, &info );
		
		if ( result == SUCCESS )
		{
			// Set the notification callback
			ISOUNDPLAYER_RegisterNotify( pISoundPlayer,
				mainSoundNotifyCallback, pThis );
				
			// Start the playback
			ISOUNDPLAYER_Play( pISoundPlayer );
		}
	}
}


static void mainMusicStart( CAppPtr pThis )
{
	CAppDataPtr pData = GetAppData( pThis );
	
	// If these fail, it's OK.
	ISHELL_CreateInstance( GetShell( pThis ), 
		AEECLSID_SOUND, (void **)&pData->pISound );
		
	ISHELL_CreateInstance( GetShell( pThis ),
		AEECLSID_SOUNDPLAYER, (void **)(&pData->pISoundPlayer ));
	
	// Start the music playback	
	mainMusicPlay( pThis );
}


static void mainMusicStop( CAppPtr pThis )
{
	CAppDataPtr pData = GetAppData( pThis );

	if ( pData->pISoundPlayer )
		ISOUNDPLAYER_Release( pData->pISoundPlayer );

	if ( pData->pISound )
		ISOUND_Release( pData->pISound );
		
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
	CAppDataPtr pData;

	UNUSED( change );

	ASSERT( pThis );

	pData = GetAppData( pThis );

#ifndef DEBUG_SPRITES
	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );

	// Draw the board and start the animation.
	mainDraw( pThis );
#endif

	// Start the background music
	mainMusicStart( pThis );

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

	mainMusicStop( pThis );

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
	int dx = 0, dy = 0;
	int32 newX, newY;
	CAppDataPtr pData = GetAppData( pThis );
	uint32 now = GETTIMEMS();
	
	/// The user can only move every so often.
	if ( now > ( pData->nTime + PLAYER_DELAY_MSECS ) ) switch( wParam )
	{
		case AVK_UP: 
			dy = -CAT_DELTA;
			result = TRUE;
			break;
		case AVK_DOWN:
			dy = CAT_DELTA;
			result = TRUE;
			break;
		case AVK_LEFT:
			dx = -CAT_DELTA;
			result = TRUE;
			break;
		case AVK_RIGHT:
			dx = CAT_DELTA;
			result = TRUE;
			break;
	}

	if ( result )
	{
		// First determine the new position.
		newX = pData->arSprites[ Sprite_Cat ].x + dx;
		newY = pData->arSprites[ Sprite_Cat ].y + dy;
		
		PIN_X_COORD( pThis, newX );
		PIN_Y_COORD( pThis, newY );

		pData->arSprites[ Sprite_Cat ].x = (uint16)newX;
		pData->arSprites[ Sprite_Cat ].y = (uint16)newY;

		// Update the display --- don't wait until the next pass.
		pData->nTime = now;
		mainDrawUpdate( pThis );
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
