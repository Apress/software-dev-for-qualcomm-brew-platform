/*
 *  @name Extension.h
 *
 *  @author Ray Rischpater
 *  Copyright (c) 2001 - 2003 Ray Rischpater.
 *  Portions copyright (c) 2001 - 2002 QUALCOMM, Inc.
 *  @memo Defines the internal structures, enumerations, and functions used and implemente by the extension.
 *  @doc
 *  
 */

#ifndef	__EXTENSION_H__
/*
 * Structures
 */
 
/**
	@name IExtension

	@memo Main data structure for the extension
	@doc .
*/
struct _Extension
{
	// note: this needs to be the first item in this structure
	/// Virtual function table
	IExtensionCls	vtExtensionCls;

	/// @name Member variables
	//@{
	/// References to us
	uint32			m_nRefs;
	/// copy of Shell pointer
	IShell			*m_pIShell;
	/// IModule interface pointer
	IModule			*m_pIModule;
	//@}
};
typedef struct _Extension Extension;
typedef struct _Extension *ExtensionPtr;


/*
 * Function prototypes
 */
extern int Extension_Init( void *p );
extern void Extension_Free( void *p );
#endif
