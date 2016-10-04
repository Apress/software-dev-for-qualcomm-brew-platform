/*
 *  @name Database.h
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

int DBThawRecord( IDBRecord *pRecord, CAppRecordPtr *ppResult );
int DBFreezeRecord( CAppRecordPtr pRecord, AEEDBField **ppResult );
int DBRecordField( IDBRecord *pRecord, 
				   AEEDBFieldName iFieldName,
				   AEEDBFieldType *piFieldType,
		   void **ppResult, uint16 *piFieldLength );
/**
 * @name DBStringField
 * @memo Initializes a database field as a string with the current data.
 * @param pf: pointer to database field (AEEDBField *)
 * @param p: multibyte string to store in field (AECHAR *)
 * @param dfb: name of the database field (AEEDBFieldName)
 */
#define DBStringField( pf, p, f ) \
	pf->fType = AEEDB_FT_STRING; \
	pf->fName = f; \
	pf->wDataLen = (uint16)WSTRSIZE(wszString); \
	pf->pBuffer  = (void*)(p);


/**
 * @name DBBinaryField
 * @memo Initializes a database field as a binary blob with the current data.
 * @param pf: pointer to database field (AEEDBField *)
 * @param p: binary blob to store in field (byte *)
 * @param l: length of binary blob
 * @param f: name of the database field (AEEDBFieldName)
 */
#define DBBlobField( pf, p, l, f ) \
	pf->fType = AEEDB_FT_BINARY; \
	pf->fName = f; \
	pf->wDataLen = (uint16)l; \
	pf->pBuffer  = (void*)(p);

/**
 * @name DBWordField
 * @memo Initializes a database field as a word with the current data.
 * @param pf: pointer to database field (AEEDBField *)
 * @param p: word (uint16 *)
 * @param dfb: name of the database field (AEEDBFieldName)
 */
#define DBWordField( pf, p, f ) \
	pf->fType = AEEDB_FT_WORD; \
	pf->fName = f; \
	pf->wDataLen = sizeof(uint16); \
	pf->pBuffer  = (void*)(p);

/**
 * @name DBDWordField
 * @memo Initializes a database field as a double word with the current data.
 * @param pf: pointer to database field (AEEDBField *)
 * @param p: dword (uint32 *)
 * @param dfb: name of the database field (AEEDBFieldName)
 */
#define DBDWordField( pf, p, f ) \
	pf->fType = AEEDB_FT_BINARY /* AEEDB_FT_DWORD */; \
	pf->fName = f; \
	pf->wDataLen = sizeof(uint32); \
	pf->pBuffer  = (void*)(p);

/**
 * @name DBByteField
 * @memo Initializes a database field as a byte with the current data.
 * @param pf: pointer to database field (AEEDBField *)
 * @param p: byte (byte *)
 * @param dfb: name of the database field (AEEDBFieldName)
 */
#define DBByteField( pf, p, f ) \
	pf->fType = AEEDB_FT_BYTE; \
	pf->fName = f; \
	pf->wDataLen = sizeof(byte); \
	pf->pBuffer  = (void*)(p);


