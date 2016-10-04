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
 * @name APP_DATABASE_NAME
 * @memo Application Database name.
 * @doc This is the name of the database used by the application.
 */
#define APP_DATABASE_NAME "mileage"


/**
 * @name CAppPrefs
 * @memo Application preferences structure.
 * @doc This structure contains the application preferences loaded on launch and committed to EFS on exit.
 */
typedef struct 
{
  /// total miles driven
  uint32 m_nMiles;
  /// total fuel consumed
  uint32 m_nFuel;
} CAppPrefs, *CAppPrefsPtr;

/**
 * @name CAppData
 * @memo Application data structure.
 * @doc This structure contains the application preferences loaded on launch and committed to EFS on exit.
 */
typedef struct 
{
    IDatabase *m_pIDatabase;
    void *m_recordCache;
} CAppData, *CAppDataPtr;


/**
 * @name EAppRecordFieldName
 * @memo Application database field names.
 * @doc This enumerates the records in the application database
 */
typedef enum 
{
	/// Mileage event type
	Fld_EntryType = 1,
	/// Time
	Fld_Time,
	/// Odometer
	Fld_Miles,
	/// Other agument for event
	Fld_Other,
	/// Cost of event
	Fld_Cost,
	/// Time at which to do this service again
	Fld_DueTime,
	/// Mileage at which to do this service again
	Fld_DueMiles,
	/// The last item. Don't change or remove this.
	Fld_LastField
} EAppRecordFieldName;

/**
 * @name Mileage record entry types
 */
typedef enum _EMileageEntryType
{
 ME_Gas = 1,
 ME_Oil,
 ME_Tire,
 ME_Tuneup,
 ME_OtherService,
 ME_Parking,
 ME_Mileage,
 ME_Undefined
} EMileageEntryType; 


/**
 * @name CAppRecord
 * @memo Application database record structure.
 * @doc This structure contains the RAM-resident format for a single database record.
 */
typedef struct 
{
  /// Mileage event
  EMileageEntryType m_type; 
  /// When the event occurred
  uint32 m_nTime;  // time in seconds
  /// What the odometer read when the event occurred
  uint32 m_nMiles; 
  /// argument of the event (gallons purchased, &c)
  uint32 m_nOther; 
  /// What the event cost
  uint32  m_nCost; // in cents
  /// When this item will be due again (time)
  uint32 m_nDueTime; // time in seconds
  /// When this item will be due again (miles)
  uint32 m_nDueMiles;
} CAppRecord, *CAppRecordPtr;
