/*
 *  @name Extension.c
 *
 *  @author Ray Rischpater
 *  Copyright (c) 2001 - 2003 Ray Rischpater.
 *  Portions copyright (c) 2001 - 2002 QUALCOMM, Inc.
 *  @memo Implementation of the extension (Internals)
 *  @doc
 *  
 */

#include "ext_inc.h"

/**
 * @name ExtensionCls_Init
 *
 * Called to initalize an instance of a class within this extension.
 *
 *	@param p pointer to an uninitalized instance.
 *	@return AEE_SUCCESS Success
 */
int Extension_Init( void * p )
{
	ExtensionPtr pThis = (ExtensionPtr)p;
	int result = EFAILED;

	if ( !p ) return result;
	
	result = SUCCESS;
	return result;
}


/**
 * @name ExtensionCls_Free
 *
 * Called to free the private data in an instance of a class within this extension.
 *
 *	@param p pointer to an instance.
 *	@return AEE_SUCCESS Success
 */
void Extension_Free( void *p )
{
	ExtensionPtr pThis = (ExtensionPtr)p;
	
	if ( !p ) return;

	return;
}