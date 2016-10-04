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
