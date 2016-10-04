/*
 *  @name AppStates.c
 *  @author Ray Rischpater, KF6GPE
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 *  @memo Application states
 *  @doc
 *  This file provides the implementation for the states of SlideShow.
 */

/*
 * Includes
 */
#include "inc.h"

#if 0
/* 
 * Stub state. You can copy/paste this as a starting point.
 */

/** 
 * Constructs the user interface for the _ state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
 * @return nothing
 */
static boolean _Entry( void *p, 
 					   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );
	
	// Do something here.
	
	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );

	return TRUE;
}

/** 
 * Exits the user interface for the _ state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we exited this state
 * @return nothing
*/
static boolean _Exit( void *p, 
				      EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	UNUSED( change );

	ASSERT( pThis );

	// Do something here
	return TRUE;
}


/** 
 * Handles events for the _ state
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
			result = _Entry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = _Exit( p, wParam );
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

/*
 * Implementation
 */

/**
 * Initialize the application-specific data.
 * @param void *pThis: application
 * @return: EFAILED or ESUCCESS
 */
int AS_Init( CAppPtr pThis )
{
	int result = SUCCESS;

	ASSERT( pThis );
	// Create the application controls here.
	result = ISHELL_CreateInstance( GetShell( pThis ), 
		AEECLSID_STATIC, 
		(void **)&pThis->m_app.m_apControl[ Ctl_Static ]);

	if ( result == SUCCESS )
		result = ISHELL_CreateInstance( GetShell( pThis ),
			AEECLSID_CLOCKCTL,
			(void **)&pThis->m_app.m_apControl[ Ctl_Time ]);

	if ( result == SUCCESS )
		result = ISHELL_CreateInstance( GetShell( pThis ),
			AEECLSID_LISTCTL,
			(void **)&pThis->m_app.m_apControl[ Ctl_List ]);

	return result;
}

/**
 * Free allocated resources.
 * We don't have to release the controls in the
 * control pool --- the framework does this for 
 * us.
 * @param void *pThis: application
 * @return nothing
 */
void AS_Free( CAppPtr pThis )
{
	UNUSED( pThis );
}

/* 
 * This is the first state of the application
 */

/** 
 * Prepares user interface for the first state
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
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
	addItemInfo.dwData = (uint32)AS_StaticHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_STATIC;
	addItemInfo.wImage = IDI_STATIC;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)AS_TextHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_TEXT;
	addItemInfo.wImage = IDI_TEXT;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&addItemInfo );


	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)AS_TextWithMenuHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_TEXTWITHMENU;
	addItemInfo.wImage = IDI_OK ; //IDI_TEXT;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)AS_TimeHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_TIME;
	addItemInfo.wImage = IDI_TIME;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)AS_MenuHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_MENU;
	addItemInfo.wImage = IDI_MENU;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_NavMenu ], 
		&addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = (uint32)AS_ListHandleEvent;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_LIST;
	addItemInfo.wImage = IDI_MENU;
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
 * Exits the user interface for the first state.
 * @param void *p: this application
 * @param EStateChangeCause change: why we exited this state
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
 * Handles events for the first state
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

	ASSERT( pThis );

	// Give the control the first crack at it

	result = /* HandleEvent */ FALSE;

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
			result = TRUE;
			break;

		default:
			break;
	}

	return result;
}


/* 
 * Shows a static text.
 */

/** 
 * Prepares user interface for the first state
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
 * @return nothing
 */
static boolean staticEntry( void *p, 
							  EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	AECHAR pszTitle[64], pszContent[256];
	UNUSED( change );
	ASSERT( pThis );
	
	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );

	// Size the menu.
	ISTATIC_SetRect( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ], 
		&pThis->m_rc );

	// Put something fun here.
	ISHELL_LoadResString( GetShell( pThis ), 
		APP_RES_FILE, 
		IDS_ISTATICTITLE,
		pszTitle, 
		sizeof(pszTitle));
	ISHELL_LoadResString( GetShell( pThis ), 
		APP_RES_FILE, 
		IDS_ISTATICCONTENTS,
		pszContent, 
		sizeof(pszContent));


	ISTATIC_SetText( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ],
		pszTitle, pszContent, AEE_FONT_BOLD, AEE_FONT_NORMAL );

	ISTATIC_SetActive( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ], TRUE );
	ISTATIC_Redraw( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ] );

	IDISPLAY_Update( GetDisplay( pThis ) );

	return TRUE;
}

/** 
 * Exits the user interface for the first state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we exited this state
 * @return nothing
 */
static boolean staticExit( void *p, 
				   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;

	UNUSED( change );
	ASSERT( pThis );

	ISTATIC_Reset( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ] );

	return TRUE;
}


/** 
 * Handles events for the static text state
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
 */
boolean AS_StaticHandleEvent( void *p, 
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
			result = staticEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = staticExit( p, wParam );
			break;

		case EVT_COMMAND:
			result = TRUE;
			break;

		default:
			break;
	}

	return result;
}

/*
 * TextCtl example
 */

/** 
 * Constructs the user interface for the text state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
 * @return nothing
 */
static boolean textEntry( void *p, 
 					   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	AECHAR pszTitle[32];
	AECHAR pszText[256];
	ITextCtl *pITextCtl = (ITextCtl *)pThis->m_app.m_apControl[ Ctl_TextInput ];
	
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );
	// Set the control's options
	ICONTROL_SetProperties( pThis->m_app.m_apControl[ Ctl_TextInput ], 
		TP_MULTILINE | TP_FRAME | TP_T9_MODE );
	
	
	STR_TO_WSTR( "Message?", pszTitle, sizeof( pszTitle ) );
	STR_TO_WSTR( "Hello world.", pszText, sizeof( pszText ) );
	
	ITEXTCTL_SetTitle( pITextCtl, NULL, 0, pszTitle );
	ITEXTCTL_SetText( pITextCtl, pszText, -1 );
	
	// Size the control.
	ICONTROL_SetRect( 
		pThis->m_app.m_apControl[ Ctl_TextInput ], 
		&pThis->m_rc );
	ICONTROL_SetActive( pThis->m_app.m_apControl[ Ctl_TextInput ], TRUE ); 
	ICONTROL_Redraw( 
		pThis->m_app.m_apControl[ Ctl_TextInput ] );
	
	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );

	return TRUE;
}

/** 
 * Exits the user interface for the Text state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we exited this state
 * @return nothing
*/
static boolean textExit( void *p, 
				      EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	char pszText[512];
	AECHAR *psz;
	ITextCtl *pITextCtl = (ITextCtl *)pThis->m_app.m_apControl[ Ctl_TextInput ];

	ASSERT( pThis );

	// Note user input for posterity.
	psz = ITEXTCTL_GetTextPtr( pITextCtl );
	if ( psz )
	{
		WSTR_TO_STR( psz, pszText, sizeof( pszText ) );
		if ( StateChangeInfo( change ) == StateChangeInfo_SELECT )
			DBGPRINTF("entered %s", pszText );
		else
			DBGPRINTF("hit CLR" );
	}

	ICONTROL_Reset( 
		pThis->m_app.m_apControl[ Ctl_TextInput ] );
	return TRUE;
}


/** 
 * Handles events for the text state
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
*/
boolean AS_TextHandleEvent( void *p, 
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
			result = textEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = textExit( p, wParam );
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
 * Constructs the user interface for the text with menu state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
 * @return nothing
 */
static boolean textWithMenuEntry( void *p, 
 					   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	AECHAR pszTitle[32];
	AECHAR pszText[256];
	AEERect rc;
	CtlAddItem addItemInfo = { 0 };
	ITextCtl *pITextCtl = (ITextCtl *)pThis->m_app.m_apControl[ Ctl_TextInput ];
	IMenuCtl *pIMenuCtl = (IMenuCtl *)pThis->m_app.m_apControl[ Ctl_SoftKeyMenu ];
	
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );

	// Set the text control's options
	ITEXTCTL_SetProperties( pITextCtl, 
		TP_MULTILINE | TP_FRAME | TP_T9_MODE );
	
	// Set the text control's contents
	STR_TO_WSTR( "Message?", pszTitle, sizeof( pszTitle ) );
	STR_TO_WSTR( "Hello world.", pszText, sizeof( pszText ) );
	
	ITEXTCTL_SetTitle( pITextCtl, NULL, 0, pszTitle );
	ITEXTCTL_SetText( pITextCtl, pszText, -1 );
	
	// Set the menu control's contents
	addItemInfo.wItemID = StateChangeInfo_SELECT;
	addItemInfo.dwData = (uint32)State_PopState;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = APP_RES_FILE;
	addItemInfo.wText = IDS_OK;
	addItemInfo.wImage = IDI_OK;
	IMENUCTL_AddItemEx( pIMenuCtl, &addItemInfo );

	addItemInfo.wItemID = StateChangeInfo_CLR;
	addItemInfo.dwData = (uint32)State_PopState;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = NULL;
	addItemInfo.wText = IDS_CANCEL;
	addItemInfo.wImage = IDI_CANCEL; 
	IMENUCTL_AddItemEx( pIMenuCtl, &addItemInfo );
	
	// Size the control.
	IMENUCTL_GetRect( pIMenuCtl, &rc );
	SETAEERECT( &rc, 0, 0, pThis->m_cx, pThis->m_cy - rc.dy );
	ITEXTCTL_SetRect( pITextCtl, &rc );
	
	// Attach the soft key menu to the control
	ITEXTCTL_SetSoftKeyMenu( pITextCtl,  pIMenuCtl );

	ITEXTCTL_SetActive( pITextCtl, TRUE ); 

	ITEXTCTL_Redraw( pITextCtl );
	
	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );

	return TRUE;
}

/** 
 * Exits the user interface for the text with menu state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we exited this state
 * @return nothing
*/
static boolean textWithMenuExit( void *p, 
				      EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	char pszText[512];
	AECHAR *psz;
	ITextCtl *pITextCtl = (ITextCtl *)pThis->m_app.m_apControl[ Ctl_TextInput ];

	UNUSED( change );

	ASSERT( pThis );

	// Note user input for posterity.
	psz = ITEXTCTL_GetTextPtr( pITextCtl );
	if ( psz )
	{
		WSTR_TO_STR( psz, pszText, sizeof( pszText ) );
		if ( StateChangeInfo( change ) == StateChangeInfo_SELECT )
			DBGPRINTF("entered %s", pszText );
		else
			DBGPRINTF("hit CLR" );
	}
	
	ICONTROL_Reset( 
		pThis->m_app.m_apControl[ Ctl_TextInput ] );

	ICONTROL_Reset( 
		pThis->m_app.m_apControl[ Ctl_SoftKeyMenu ] );

	return TRUE;
}


/** 
 * Handles events for the text with menu state
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
*/
boolean AS_TextWithMenuHandleEvent( void *p, 
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
			result = textWithMenuEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = textWithMenuExit( p, wParam );
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
 * Constructs the user interface for the time state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
 * @return nothing
 */
static boolean timeEntry( void *p, 
 					   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );
	
	// Set the control's options
	ICONTROL_SetProperties( pThis->m_app.m_apControl[ Ctl_Time ], 
		0 );
	
	// Size the control.
	ICONTROL_SetRect( 
		pThis->m_app.m_apControl[ Ctl_Time ], 
		&pThis->m_rc );
	ICONTROL_SetActive( pThis->m_app.m_apControl[ Ctl_Time ], TRUE ); 
	ICONTROL_Redraw( 
		pThis->m_app.m_apControl[ Ctl_Time ] );
	
	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );

	return TRUE;
}

/** 
 * Exits the user interface for the _ state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we exited this state
 * @return nothing
*/
static boolean timeExit( void *p, 
				      EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	UNUSED( change );

	ASSERT( pThis );

	ICONTROL_Reset( 
		pThis->m_app.m_apControl[ Ctl_Time ] );
	return TRUE;
}


/** 
 * Handles events for the time state
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
*/
boolean AS_TimeHandleEvent( void *p, 
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
			result = timeEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = timeExit( p, wParam );
			break;

		case EVT_COMMAND:
			result = TRUE;
			break;

		default:
			break;
	}

	return result;
}


/*
 * Menu state
 */
 
/** 
 * Constructs the user interface for the menu state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
 * @return nothing
 */
static boolean menuEntry( void *p, 
 					   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );
	pThis->m_app.m_apControl[ Ctl_NavMenu ];

	Control_MenuDefBegin( pThis->m_app.m_apControl[ Ctl_NavMenu ], IDS_MENU, pThis->m_rc );
	Control_MenuDefItem( IDS_STATIC, IDI_STATIC, State_PopState );	 
	Control_MenuDefItem( IDS_TIME, IDI_TIME, AS_TimeHandleEvent );	 
	Control_MenuDefEnd( );
	
	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );

	return TRUE;
}

/** 
 * Exits the user interface for the menu state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we exited this state
 * @return nothing
*/
static boolean menuExit( void *p, 
				      EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	UNUSED( change );

	ASSERT( pThis );

	Control_HideControl( pThis->m_app.m_apControl[ Ctl_NavMenu ] );
	return TRUE;
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
	int32 *pResult = (int32 *)dwParam;

	ASSERT( pThis );

	if ( !result ) switch ( eCode )
	{	
		case EVT_APP_START:
		case EVT_APP_RESUME:
			result = menuEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = menuExit( p, wParam );
			break;

		case EVT_COMMAND:
			*pResult = -1;
			result = FALSE;
			break;

		default:
			break;
	}

	return result;
}

/** 
 * Constructs the user interface for the list state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we entered this state
 * @return nothing
 */
static boolean listEntry( void *p, 
 					   EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	CtlAddItem addItemInfo = { 0 };
	int itemID = 1;
	AECHAR pszTitle[64], pszContent[256];

	AEERect	rc;
	UNUSED( change );

	ASSERT( pThis );

	// Clear the display
	IDISPLAY_ClearScreen( GetDisplay( pThis ) );
	
	
	// Do something here.
	
		// Size the static item.
	rc.x = 0;
	rc.y = 0;
	rc.dx = pThis->m_cx;
	rc.dy = pThis->m_cy / 2;

	ISTATIC_SetRect( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ], 
		&pThis->m_rc );

	// Put something fun here.
	ISHELL_LoadResString( GetShell( pThis ), 
		APP_RES_FILE, 
		IDS_LISTTITLE,
		pszTitle, 
		sizeof(pszTitle));
	ISHELL_LoadResString( GetShell( pThis ), 
		APP_RES_FILE, 
		IDS_LISTHELP,
		pszContent, 
		sizeof(pszContent));

	ISTATIC_SetText( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ],
		pszTitle, pszContent, AEE_FONT_BOLD, AEE_FONT_NORMAL );

	ISTATIC_SetActive( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ], TRUE );
	ISTATIC_Redraw( (IStatic *)pThis->m_app.m_apControl[ Ctl_Static ] );

	
	// Build a menu of choices for controls.
	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = 0;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = NULL;
	addItemInfo.wText = IDS_RED;
	addItemInfo.wImage = 0;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_List ], 
		&addItemInfo );

	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = 0;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = NULL;
	addItemInfo.wText = IDS_GREEN;
	addItemInfo.wImage = 0;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_List ], 
		&addItemInfo );


	addItemInfo.wItemID = itemID++;
	addItemInfo.dwData = 0;
	addItemInfo.pszResText = APP_RES_FILE;
	addItemInfo.pszResImage = NULL;
	addItemInfo.wText = IDS_BLUE;
	addItemInfo.wImage = 0;
	IMENUCTL_AddItemEx( (IMenuCtl *)
		pThis->m_app.m_apControl[ Ctl_List ], 
		&addItemInfo );
	
	// Size the control.
	rc.x = 0;
	rc.y = pThis->m_cy / 2 + pThis->m_nFontHeight + 4;
	rc.dx = pThis->m_cx;
	rc.dy = pThis->m_nFontHeight + 4;
	
	ICONTROL_SetRect( 
		pThis->m_app.m_apControl[ Ctl_List ], 
		&rc );
	ICONTROL_SetActive( pThis->m_app.m_apControl[ Ctl_List ], TRUE ); 
	ICONTROL_Redraw( 
		pThis->m_app.m_apControl[ Ctl_List ] );
	
	// Update the display
	IDISPLAY_Update( GetDisplay( pThis ) );

	return TRUE;
}

/** 
 * Exits the user interface for the list state.
 * @param void *p: this applicaton
 * @param EStateChangeCause change: why we exited this state
 * @return nothing
*/
static boolean listExit( void *p, 
				      EStateChangeCause change )
{
	CAppPtr pThis = (CAppPtr)p;
	UNUSED( change );

	ASSERT( pThis );

	ICONTROL_Reset( 
		pThis->m_app.m_apControl[ Ctl_List ] );
	return TRUE;
}


/** 
 * Handles events for the _ state
 * @param void *p: this applicaton
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParam: event parameter
 * @return TRUE if application handled event
*/
boolean AS_ListHandleEvent( void *p, 
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
			result = listEntry( p, wParam );
			break;
			
		case EVT_APP_STOP:
		case EVT_APP_SUSPEND:
			result = listExit( p, wParam );
			break;

		case EVT_COMMAND:
			result = TRUE;
			break;

		default:
			break;
	}

	return result;
}
