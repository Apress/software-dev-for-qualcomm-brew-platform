/*
 *  AppStates.h
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  This file provides the interface for the states of the application.
 */

 
/**
 * @name EStateCtlPoolIdx
 * @memo The control pool index.
 * @doc The fields in this enumeration identify specific controls in your application.
 */
typedef enum 
{
  // Defined and allocated by the framework. 
  // Do not change these.
  // Ctl_NavMenu = 0,
  // Ctl_SoftKeyMenu,
  // Ctl_TextInput,
  // Add your other controls here
  
  /// IStatic used for demonstrating IStatic.
  Ctl_Static = Ctl_LastFrameworkControl + 1,
  Ctl_Time,
  Ctl_List,

  /// Used to mark 
  Ctl_LastControl
} EStateCtlPoolIdx;



/*
 * Function Prototypes
 */
int AS_Init( CAppPtr pApp );
void AS_Free( CAppPtr pApp );


/*
 * State functions
 */
boolean AS_MainHandleEvent( void *pApp, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam );
boolean AS_StaticHandleEvent( void *pApp, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam );
boolean AS_TextHandleEvent( void *p, 
					  AEEEvent eCode, 
					  uint16 wParam, uint32 dwParam );
boolean AS_TextWithMenuHandleEvent( void *p, 
					  AEEEvent eCode, 
					  uint16 wParam, uint32 dwParam );
boolean AS_TimeHandleEvent( void *p, 
					  AEEEvent eCode, 
					  uint16 wParam, uint32 dwParam );
boolean AS_MenuHandleEvent( void *p,
					  AEEEvent eCode, 
					  uint16 wParam, uint32 dwParam );
boolean AS_ListHandleEvent( void *p, 
					  AEEEvent eCode, 
					  uint16 wParam, uint32 dwParam);

