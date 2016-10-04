/*
 *  @name controls.h
 *  @author Ray Rischpater, KF6GPE
 * 
 *  Created by Ray Rischpater on 19 January 2003
 *  Copyright (c) 2003 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  @doc
 *  This file provides shortcut macros and function prototypes
 *  for use with BREW controls within the application framework.
 */
 
/**
 * @name Control_ShowControl
 * @memo Activates a control.
 * @doc Sets the specified control's options and bounds and shows the control.
 */

#define Control_ShowControl( c, o, r ) \
	ICONTROL_SetProperties( c, o ); \
	ICONTROL_SetRect( c, &r ); \
	ICONTROL_SetActive( c, TRUE ); \
	ICONTROL_Redraw( c );

/**
 * @name Control_HideControl
 * @memo Hides a control.
 * @doc Hides a control by deactivating and resetting the control. You must still refresh the screen region occupied by the control.
 */
#define Control_HideControl( control ) \
	ICONTROL_Reset( control ); 


/**
 * @name Control_MenuDefBegin
 * @memo Begins initializing the a menu.
 * @doc Resets the specific menu control, sets its title using a string from the resource, and initializes the structures necessary to add items to the menu.
 * @see Control_MenuDefItem
 * @see Control_MenuDefEnd
 */
#define Control_MenuDefBegin( menu, titleID, rect ) \
{\
	CtlAddItem _addItemInfo = { 0 }; \
	int _nItemId = 0; \
	IMenuCtl *_pMenu = (IMenuCtl *)menu; \
	ICONTROL_Reset( menu ); \
	IMENUCTL_SetRect( _pMenu, &rect ); \
	IMENUCTL_SetTitle( _pMenu, APP_RES_FILE, titleID, NULL );

/**
 * @name Control_MenuDefItem
 * @memo Adds an item to the specific menu control from the resource.
 * @see Control_MenuDefStart
 * @see Control_MenuDefEnd
 */
#define Control_MenuDefItem( textId, imageId, state ) \
  _addItemInfo.wItemID = _nItemId++;\
  _addItemInfo.dwData = (uint32)state;\
  _addItemInfo.pszResText = APP_RES_FILE;\
  _addItemInfo.pszResImage = APP_RES_FILE;\
  _addItemInfo.wText = textId;\
  _addItemInfo.wImage = imageId;\
  IMENUCTL_AddItemEx( _pMenu, &_addItemInfo );

/**
 * @name Control_MenuDefEnd
 * @memo Finishes initializing a menu.
 * @doc Sets the menu to include an underlined title and activates the menu. You must redraw the screen once you've finished defining the menu.
 * @see Control_MenuDefStart
 * @see Control_MenuDefItem
 */
#define Control_MenuDefEnd( ) \
	IMENUCTL_SetProperties( _pMenu, MP_UNDERLINE_TITLE );\
	IMENUCTL_SetActive( _pMenu, TRUE ); \
	IMENUCTL_Redraw( _pMenu );\
}


/*
 * Prototypes
 */
void Control_TextSetDefaultSoftKeyInfo( ITextCtl *pITextCtl, 
										IMenuCtl *pIMenuCtl, 
										uint16 okIcon, 
										uint16 okString, 
										PFNSTATEEVENT okState, 
										uint16 cancelIcon, 
										uint16 cancelString, 
										PFNSTATEEVENT cancelState );
