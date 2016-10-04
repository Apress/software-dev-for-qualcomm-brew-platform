/*
* @name Extension_Ext.c
*
* @author Ray Rischpater
* Copyright (c) 2001 - 2003 Ray Rischpater.
* Portions copyright (c) 2001 - 2002 QUALCOMM, Inc.
* @memo 
* @doc
* 
*/

#include "ext_inc.h"

/// @name Functions implementing <tt>IBase</tt>
//@{

/// See {@link IEXTENSION_AddRef IEXTENSION_AddRef}
static uint32 AddRef(IExtensionCls *p)
{
	ExtensionPtr pThis = (ExtensionPtr)p;
	// validate parameter(s)
	if (!pThis)
	{
		return 0;
	}
	return (++(pThis->m_nRefs));
}



/// See {@link IEXTENSION_Release IEXTENSION_Release}
static uint32 Release(IExtensionCls *p)
{
	ExtensionPtr pThis = (ExtensionPtr)p;

	// validate parameter(s)
	if (!pThis)	
	{ 
		return 0;	
	}

	// Manage our reference count.
	pThis->m_nRefs--;

	if (pThis->m_nRefs != 0)	
	{ 
		return pThis->m_nRefs; 
	}

	// Ref count is zero. So, release memory associated with this object.
	Extension_Free(pThis);

	// Release interfaces
	if (pThis->m_pIShell)		
	{ 
		ISHELL_Release(pThis->m_pIShell); 
	}

	if (pThis->m_pIModule)	
	{ 
		IMODULE_Release(pThis->m_pIModule); 
	}

	FREE_VTBL(pThis, IModule);

	// Free the object itself
	FREE( pThis );

	return 0;
}

/**
*	@name ExtensionCls_New
*	Called from AEEClsCreateInstance, performs data initialization.
*
*	@param nSize	Specifies the size (in bytes) of the structure to be allocated for the module
*	@param pIShell	Pointer to IShell interface
*	@param pIModule	Pointer to the IModule interface
*	@param ppMod	On return, *ppMod contains a valid pointer to the class structure (output parameter)
*	@return AEE_SUCCESS, Success
*/
int ExtensionCls_New(int16 nSize, IShell *pIShell, IModule* pIModule, IModule **ppMod)
{
	ExtensionPtr pThis = NULL;
	VTBL(IExtensionCls) *modFuncs;

	// validate parameter(s)
	if(!ppMod || !pIShell || !pIModule)
	{
		return EFAILED;
	}

	*ppMod = NULL;

	// Allocate memory for the object
	if(nSize < sizeof(Extension)) nSize += sizeof(Extension);

	pThis = (ExtensionPtr) MALLOC(nSize + sizeof(IExtensionClsVtbl));

	if (NULL == pThis)
	{
		return ENOMEMORY;
	}

	// Allocate the vtbl and initialize it. Note that the modules and apps must not
	// have any static data. Hence, we need to allocate the vtbl as well.
	modFuncs = (IExtensionClsVtbl *)((byte *)pThis + nSize);

	//Initialize individual entries in the VTBL
	modFuncs->AddRef			= AddRef;
	modFuncs->Release			= Release;
	// ... Add your vtable here.

	// initialize the vtable
	INIT_VTBL(pThis, IModule, *modFuncs);

	// initialize the data members
	pThis->m_nRefs      = 1;
	pThis->m_pIShell    = pIShell;
	pThis->m_pIModule   = pIModule;

	// Add References
	ISHELL_AddRef(pIShell);
	IMODULE_AddRef(pIModule);

	// Set the pointer in the parameter
	*ppMod = (IModule*)pThis;

	return AEE_SUCCESS;
}

/**
*	@name AEEClsCreateInstance
*
*	Called to create an instance of a class within this extension.
*
*	@param ClsId	class ID passed in
*	@param IShell	IShell pointer
*	@param po	IModule pointer
*	@param ppObj	pointer to a class object pointer to fill (output parameter)
*	@return AEE_SUCCESS, Success
*	@return EFAILED, Failure
*/
int AEEClsCreateInstance(AEECLSID clsId, IShell *pIShell, IModule *po, void **ppObj)
{
	int result = EFAILED;
	*ppObj = NULL;

	if (AEECLSID_THEEXTENSION == clsId)
	{
		result = ExtensionCls_New(sizeof(Extension), pIShell, po, (IModule **)ppObj);
		if ( result == SUCCESS )
		{
			result = Extension_Init( (void *) *ppObj );
			if ( result != SUCCESS )
			{
				Release( (IExtensionCls *)*ppObj );		
			}
			return result;
		}
	}
	// Wrong class, _New or _Init failed
	return result;
}	// End Function AEEClsCreateInstance

//@}
// End "Internal functions" group

