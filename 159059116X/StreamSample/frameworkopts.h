/*
 *  @name frameworkopts.h
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
 *  This file provides application-specific options for the
 *  application framework.
 */
/**
 * @name AEECLSID_OURS
 * @memo application class ID.
 * @doc Your application's QUALCOMM-assigned class id. You can set this using the constant defined in the BID file.
 */
#define AEECLSID_OURS AEECLSID_MAIN

/**
 * @name APP_RES_FILE
 * @memo application resource file.
 * @doc Your application's primary resource file. You can set this using the constant defined in the _res.h file.
 */
#define APP_RES_FILE MAIN_RES_FILE

/**
 * @name MAX_NUM_CONTROLS
 * @memo number of controls.
 * @doc This tells the framework how many controls it must manage when handling events.
 */
#define MAX_NUM_CONTROLS ( 10 )

/**
 * @name AS_FIRSTSTATE
 * @memo Application first state.
 * @doc This is the state the application will enter once the copyright display is dismissed.
 */
#define AS_FIRSTSTATE ( AS_MainHandleEvent ) 

/**
 * @name CAppData
 * @memo Application global data.
 * @doc This is the structure that will hold your application's global data, that is data that persists across all states.
 */
typedef struct _CAppData
{
	char szName[ MAX_FILE_NAME + 1];
} CAppData, *CAppDataPtr;

/**
 * @name APP_PREFS_VERSION
 * @memo Application preferences version.
 * @doc This is version number for the CAppPrefs structure. You should increment this any time you change the preferences structure CAppPrefs.
 */
#define APP_PREFS_VERSION ( 1 )

/**
 * @name CAppPrefs
 * @memo Application preferences.
 * @doc This is the structure that will hold your application's preferences, that is the data that persists across invocations for storing preferences.
 */
typedef struct _CAppPrefs
{
	uint32	version;
} CAppPrefs, *CAppPrefsPtr;
