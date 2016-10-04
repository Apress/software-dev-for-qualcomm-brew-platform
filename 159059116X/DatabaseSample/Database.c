/*
 *  @name Database.c
 *  @author Ray Rischpater, KF6GPE
 * 
 *  Created by Ray Rischpater on 22 March 2003
 *  Copyright (c) 2003 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  @doc
 *  This file provides shortcut functions and function prototypes
 *  for use with BREW databases
 */

#include "inc.h"

/**
 * Takes a record from the database and creates a RAM-resident 
 * copy of the record.
 * @param pRecord: database record to thaw
 * @param ppResult: pointer to pointer to store result (caller must free)
 * @return SUCCESS on success with record at *ppResult
 * On failure, check *ppResult and free any fields!
 */
int DBThawRecord( IDBRecord *pRecord, 
				  CAppRecordPtr *ppResult )
{
	AEEDBFieldName iFieldName;
	AEEDBFieldType iFieldType;
	uint16 iFieldLength;
	uint16 iRecordID;
	CAppRecordPtr pResult;
	byte *pData;
	int result = SUCCESS;	
	
	ASSERT( pRecord && ppResult );

	// Allocate our result
	pResult = MALLOC( sizeof( CAppRecord ) );
	if ( !pResult ) return ENOMEMORY;
	*ppResult = pResult;
	MEMSET( pResult, 0, sizeof( CAppRecord ) );
	
	// Begin at the beginning
	iRecordID = IDBRECORD_GetID( pRecord );
	IDBRECORD_Reset( pRecord );

	// Get the first field
	iFieldType = IDBRECORD_NextField( pRecord, 
									  &iFieldName, &iFieldLength);

	// While there are fields to examine...
	while( result == SUCCESS && 
		   iFieldType != AEEDB_FT_NONE )
	{
		// Get the contents of this field
		pData = IDBRECORD_GetField( pRecord, &iFieldName, 
									&iFieldType, &iFieldLength );					
		if (!pData) break;

		// Copy variable-length fields for storage
		if ( iFieldType == AEEDB_FT_BITMAP ||
			 iFieldType == AEEDB_FT_BINARY ||
			 iFieldType == AEEDB_FT_STRING )
		{
			byte *pTemp = (byte *)MALLOC( iFieldLength );
			
			if ( !pTemp ) return ENOMEMORY;
			MEMCPY( pTemp, pData, iFieldLength );
			pData = pTemp;
		}
		result = AS_DBFieldFromDatabase( pResult, 
										 iFieldName, iFieldType,
									     pData, iFieldLength );
		iFieldType = IDBRECORD_NextField( pRecord, 
										  &iFieldName, &iFieldLength);
	}
	
	return result;
}

/**
 * Takes a record from RAM and creates a serialized
 * copy of the record for storage in the database
 * @param pRecord: database record to freeze
 * @param **ppResult: pointer to where to store address of field array (caller must free) 
 * @return SUCCESS on success with record at *ppResult
 * On failure, check *ppResult and free any fields!
 */
int DBFreezeRecord( CAppRecordPtr pRecord, 
				  AEEDBField **ppResult )
{
	AEEDBField *arFields;	
	AEEDBFieldName iFieldName;
	AEEDBFieldType iFieldType;
	uint16 iFieldLength;
	byte *pData;
	int result = SUCCESS;
	
	ASSERT( pRecord && ppResult );
	
	// Allocate our result buffer
	arFields = (AEEDBField *)MALLOC( Fld_LastField * sizeof( AEEDBField ) );
	if ( !arFields ) return ENOMEMORY;
	*ppResult = arFields;

	// For each field, get the database representation
	for ( iFieldName = 1; iFieldName < Fld_LastField; iFieldName++ )
	{
		result = AS_DBFieldToDatabase( pRecord, 
									   iFieldName, 
									   &iFieldType,
									   &pData, 
									   &iFieldLength );
		if ( result != SUCCESS ) break;		
		arFields[iFieldName-1].fType = iFieldType;
		arFields[iFieldName-1].fName = iFieldName;
		arFields[iFieldName-1].wDataLen = iFieldLength; 
		arFields[iFieldName-1].pBuffer  = (void*)(pData);
	}
	
	if ( result != SUCCESS )
	{
		// Free the field array before exiting
		FREE( arFields );
		*ppResult = NULL;
	}
	return result;
}

/**
 * Fetches the indicated field from a database record
 * @param pRecord: database record 
 * @param ppResult: pointer to pointer to store result (caller must free)
 * @return SUCCESS on success with record at *ppResult
 * On failure, check *ppResult and free any fields!
 */
int DBRecordField( IDBRecord *pRecord, 
				   AEEDBFieldName iFieldName,
				   AEEDBFieldType *piFieldType,
				   void **ppResult, uint16 *piFieldLength )
{
	AEEDBFieldType iFieldType;
	AEEDBFieldName iThisFieldName;
	byte *pData;	
	
	ASSERT( pRecord && piFieldType && piFieldType && 
	        ppResult && piFieldLength );
	
	// Begin at the beginning
	IDBRECORD_Reset( pRecord );

	// Get the first field
	iFieldType = IDBRECORD_NextField( pRecord, 
									  &iThisFieldName, 
									  piFieldLength);

	// While there are fields to examine...
	while( iFieldType != AEEDB_FT_NONE )
	{
		// Is this the field we want?
		if ( iThisFieldName == iFieldName )
		{
			// Get the contents of this field
			pData = IDBRECORD_GetField( pRecord, &iFieldName, 
										piFieldType, piFieldLength );					
			if (!pData) break;
			
			// Copy the contents to a new buffer
			*ppResult = (byte *)MALLOC( *piFieldLength );		
			if ( !*ppResult ) return ENOMEMORY;
			MEMCPY( *ppResult, pData, *piFieldLength );
			return SUCCESS;
		}
		iFieldType = IDBRECORD_NextField( pRecord, 
										  &iThisFieldName, 
										  piFieldLength);
	}
	
	return EBADPARM;
}
