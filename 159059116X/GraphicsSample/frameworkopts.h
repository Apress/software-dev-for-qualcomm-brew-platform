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
 * @name APP_PREFS_VERSION 
 * @memo Application preferences structure version.
 * @doc This is the version number of the CAppPrefs structure. You should increment this any time the preferences structure is changed.
 */
#define APP_PREFS_VERSION ( 1 )

/**
 * @name CAppPrefs
 * @memo Application preferences structure.
 * @doc This structure contains the application preferences loaded on launch and committed to EFS on exit.
 */
typedef struct 
{
	/// Replace this with your preferences fields
	int unused;
} CAppPrefs, *CAppPrefsPtr;

#define NUMSHAPES ( 16 )
// Three coordinates (x, y), r, g, b, shape
#define POINTSPERSHAPE ( 10 )
/**
 * @name CAppData
 * @memo Application data structure.
 * @doc This structure contains the application preferences loaded on launch and committed to EFS on exit.
 */
typedef struct 
{
	IGraphics	*pIGraphics;
	uint16	cxCanvas, cyCanvas;
	uint16 x, y;
	uint16 arRandom[ NUMSHAPES * POINTSPERSHAPE ];
} CAppData, *CAppDataPtr;

/**
 * @name EAppRecordFieldName
 * @memo Application database field names.
 * @doc This enumerates the records in the application database
 */
typedef enum 
{
	/// Place constants that name your database fields here.
	Fld_unused = 1,
	
	/// The last item. Don't change or remove this.
	Fld_LastField
} EAppRecordFieldName;

/**
 * @name CAppRecord
 * @memo Application database record structure.
 * @doc This structure contains the RAM-resident format for a single database record.
 */
typedef struct 
{
	/// Replace this with your application database record fields
	int16 unused;
} CAppRecord, *CAppRecordPtr;
