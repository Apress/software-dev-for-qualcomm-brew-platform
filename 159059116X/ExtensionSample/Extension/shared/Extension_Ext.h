/**
 *  @name ext_ext.h
 *
 *  @author Ray Rischpater
 *  Copyright (c) 2001 - 2003 Ray Rischpater.
 *  Portions copyright (c) 2001 - 2002 QUALCOMM, Inc.
 *  @memo Defines the interface for the extension.
 */
 
#ifndef __EXT_H__
#define __EXT_H__

//**************************************************************************
//		Class IDs
//**************************************************************************
//from Extension.bid
#ifndef AEECLSID_THEEXTENSION
#define AEECLSID_THEEXTENSION 0x01009FF8
#endif

//**************************************************************************

/// @name Public Interfaces
//@{

/**
 *	@memo See the {@link Extension Extension} Interface
 *	@doc This class provides ...
	@type struct
 */
typedef struct _IExtensionCls IExtensionCls;
typedef struct _IExtensionCls IExtension;

//@}
// End group

/// @name Enumerations
//@{

//@}
// End "enums" group

/// @name Callback Function Types
//@{

//@}
// End "callback function types" group


/// @name Interfaces
//@{
/**
	@name Extension
	@memo Interface for ...
	@doc .
*/
//@{

struct _IExtensionCls
{
	struct _IExtensionClsVtbl *pvt;
};
typedef struct _IExtensionClsVtbl IExtensionClsVtbl;
struct _IExtensionClsVtbl
{
	uint32  (*AddRef)(IExtensionCls *);
	uint32  (*Release)(IExtensionCls *);

	// Put your methods here.
};

/**
	@name Extension_AddRef

	@memo Increments the reference count
	@doc Increments the reference count of this object.  

	@args (ExtensionCls *pViewer)
	@type uint32

	@param p Pointer to ExtensionCls interface whose reference count is to be incremented.
	@return New reference count (after it has been incremented)
	
	@see ExtensionCls
*/
#define IEXTENSION_AddRef(p)	\
	GET_PVTBL(p, IExtension)->AddRef(p)

/**
	@name Extension_Release

	@memo Releases this object.
	@doc If the reference count is zero, performs memory clean-up otherwise decrements the ref count.

	@args (ExtensionCls *pViewer)
	@type uint32

	@param pViewer	Pointer to ExtensionCls interface whose reference count is to be decremented.

	@return New reference count (after it has been decremented)
	
	@see ExtensionCls
*/
#define IEXTENSION_Release(p)	\
	GET_PVTBL(p, IExtensionCls)->Release(p)

//@}
// End Extension Interface group

//@}
// End "interfaces" group


#endif  
