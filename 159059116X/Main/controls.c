/*
 *  @name controls.c
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
 *  This file provides shortcut functions and function prototypes
 *  for use with BREW controls within the application framework.
 */
#include "inc.h"

/**
 * @name Control_TextSetDefaultSoftKeyInfo
 * @memo Sets the default soft key info for a text control
 * @doc Sets the default menu items for a soft key control.
 * @param ITextCtl *pITextCtl: target text control
 * @param IMenuCtl *pIMenuCtl: target soft menu control
 * @param uint16 okIcon: resource id of ok icon in menu
 * @param uint16 okString: resource id of ok string in menu
 * @param uint16 okState: state to enter when OK is pressed
 * @param uint16 cancelIcon: resource id of cancel icon in menu
 * @param uint16 cancelString: resource id of ok string in menu 
 * @param uint16 cancelState: state to enter when cancel is pressed
 * @return nothing
 */
void Control_TextSetDefaultSoftKeyInfo( ITextCtl *pITextCtl, 
										IMenuCtl *pIMenuCtl, 
										uint16 okIcon, 
										uint16 okString, 
										PFNSTATEEVENT okState, 
										uint16 cancelIcon, 
										uint16 cancelString, 
										PFNSTATEEVENT cancelState )
{
	CtlAddItem addItemInfo = { 0 };
	AEERect rc;
	CAppPtr pThis = (CAppPtr)GETAPPINSTANCE();
	
	// Set default text control properties
	ITEXTCTL_SetProperties( pITextCtl, 
		TP_MULTILINE | TP_FRAME | TP_T9_MODE ); 
	
	// Configure SELECT item
	addItemInfo.wItemID = StateChangeInfo_SELECT; 
	addItemInfo.dwData = (uint32)okState; 
	addItemInfo.pszResText = okString ? APP_RES_FILE : NULL; 
	addItemInfo.pszResImage = okIcon ? APP_RES_FILE : NULL; 
	addItemInfo.wText = okString; 
	addItemInfo.wImage = okIcon; 
	IMENUCTL_AddItemEx( pIMenuCtl, &addItemInfo ); 
	
	// Configure CANCEL item
	addItemInfo.wItemID = StateChangeInfo_CLR; 
	addItemInfo.dwData = (uint32)cancelState; 
	addItemInfo.pszResText = cancelString ? APP_RES_FILE : NULL; 
	addItemInfo.pszResImage = cancelIcon ? APP_RES_FILE : NULL; 
	addItemInfo.wText = cancelString; 
	addItemInfo.wImage = cancelIcon; 
	IMENUCTL_AddItemEx( pIMenuCtl, &addItemInfo );	
	
	// Resize the text control to leave space for the menu
	IMENUCTL_GetRect( pIMenuCtl, &rc );
	SETAEERECT( &rc, 0, 0, pThis->m_cx, pThis->m_cy - rc.dy );
	ITEXTCTL_SetRect( pITextCtl, &rc );
	
	// Link the soft key menu to the control
	ITEXTCTL_SetSoftKeyMenu( pITextCtl,  pIMenuCtl );
}