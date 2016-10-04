/**
 *  @name utils.h
 *  @author Ray Rischpater, KF6GPE
 *  
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.  
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  @doc
 *  This file provides the interface for miscellaneous utilities
 *  we miss when developing for BREW.
 */

/** 
 * @name UNUSED
 * @memo Flags unused variables.
 * @doc Marks a variable as unused to prevent workings in Visual Studio .NET
 */
#define UNUSED( x ) (x);

/** 
 * @name GetShell
 * @memo Returns IShell instance.
 * @doc Obtains an application's IShell instance from the AEEApplet structure.
 */
#define GetShell( pThis ) ( ((CStateAppPtr)pThis)->a.m_pIShell )
/** 
 * @name GetDisplay
 * @memo Returns IDisplay instance.
 * @doc Obtains an application's IDisplay instance from the AEEApplet structure.
 */
#define GetDisplay( pThis ) ( ((CStateAppPtr)pThis)->a.m_pIDisplay )

/** 
 * @name ASSERT
 * @memo Causes application to halt if assertion fails.
 * @doc Causes application to halt if assertion fails. Prints information pertaining to the assertion in the debug log.
 */
#ifdef _DEBUG
#define ASSERT( x ) if ( !( x ) ) { \
    int *p = NULL; \
    DBGPRINTF("%s(%d)[%s]\r\n Halting.", \
              __FILE__, __LINE__, #x); \
    *p=1; \
  } 
#else
#define ASSERT( x ) ;
#endif


/**
 * @name _Node
 * @memo Singly linked list.
 * @doc Provides the implementation for a simple singly-linked list with weak typing.
 * The head of the list is denoted with the data fields set to NULL.
 */
typedef struct _Node
{
	/// Next node
	struct _Node *m_pNext;
	/// Pointer to data for this node
	void *m_pData;
	/// Pointer to any additional data for this node.
	void *m_pMetaData;
};

/** 
 * @name NodeLinkNext
 * @memo Link two nodes.
 * @doc  Links two nodes by placing the second node at the head of the list. Returns the second node. 
 */
#define NodeLinkNext( n1, n2 ) \
	{ n2->m_pNext = n1->m_pNext; n1->m_pNext = n2; }
	
/** 
 * @name NodeUnlinkNext
 * @memo Unlinks the next node from the list.
 * @doc  Unlinks the next node after the indicated node. Frees the unlinked node.
 */
#define NodeUnlinkNext( n1 ) \
	{ struct _Node *_p; _p = n1->m_pNext; n1->m_pNext = _p->m_pNext; FREE( _p ); }

/** 
 * @name NodeNext
 * @memo Returns the next node after the current node.
 * @doc  Returns the next node after the current node.
 */
#define NodeNext( n ) \
	( n->m_pNext )
