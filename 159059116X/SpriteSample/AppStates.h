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
 *  This file provides the interface for the states.
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
  
  /// A new control.
  Ctl_ = Ctl_LastFrameworkControl + 1,

  /// Used to mark 
  Ctl_LastControl
} EStateCtlPoolIdx;

/*
 * Function Prototypes
 */
int AS_Init( CAppPtr pApp );
void AS_Free( CAppPtr pApp );
void AS_DefaultPrefs( CAppPrefsPtr pPrefs );
boolean AS_HandleNotify( CAppPtr pThis, uint16 wParam, uint32 dwParam );
int AS_DBFieldFromDatabase( CAppRecordPtr pRecord, 
				  		    AEEDBFieldName iFieldName, 
							AEEDBFieldType iFieldType,
							byte *pData, uint16 iFieldLength );
int AS_DBFieldToDatabase( CAppRecordPtr pRecord, 
			  	          AEEDBFieldName iFieldName, 
						  AEEDBFieldType *piFieldType,
						  byte **ppData, uint16 *piFieldLength );


/*
 * State Function Prototypes
 */
boolean AS_MainHandleEvent( void *pApp, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam );
