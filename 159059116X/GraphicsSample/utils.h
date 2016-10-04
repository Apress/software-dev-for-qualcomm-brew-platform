/*
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
#define GetDisplay( pThis ) ( ((CAppPtr)pThis)->m_app.a.m_pIDisplay )

/**
 * @name GetMenu
 * @memo Returns IMenuCtl instance
 * @doc Obtains an application's IMenuCtl instance from the framework's control index.
 */
#define GetMenu( pThis ) ( (IMenuCtl *)( ((CAppPtr)pThis)->m_app.m_apControl[ Ctl_NavMenu ] ) )

/**
 * @name GetSoftMenu
 * @memo Returns IMenuCtl instance
 * @doc Obtains an application's IMenuCtl instance (for the softmenu) from the framework's control index.
 */
#define GetSoftMenu( pThis ) ( (IMenuCtl *)( ((CAppPtr)pThis)->m_app.m_apControl[ Ctl_SoftKeyMenu ] ) )

/**
 * @name GetText
 * @memo Returns ITextCtl instance 
 * @doc Obtains an application's ITextCtl instance from the framework's control index
 */
#define GetText( pThis) ( (ITextCtl *)( ((CAppPtr)pThis)->m_app.m_apControl[ Ctl_TextInput ] ) )

typedef uint32 RandSeed, *RandSeedPtr;

/**
 * @name srand32
 * @memo Seeds linear congruent pseudorandom number generator
 * @doc Seeds linear congruent psuedorandom number generator.
 * @param pSeed: RandSeedPtr with seed. 
 * @return Returns RandSeedPtr key to pass to rand.
 */
#define srand32( pSeed ) ( pSeed )


/**
 * @name rand
 * @memo Linear congruent psuedorandom number generator
 * @doc Returns a psuedorandom number generated using the linear congruence algorithm.
 * @param pSeed: RandSeedPtr seed for next random number
 * @return Returns psuedorandom number
 */
 #define rand32( pNext ) \\
  ( (*pNext = *pNext * 1103515245 + 12345), \\
    ( (unsigned int)( *pNext/65536 ) % 32768 ) )
    
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
 * @param n1: head node in list
 * @param n2: node to link to the head of the list
 */
#define NodeLinkNext( n1, n2 ) \
	{ n2->m_pNext = n1->m_pNext; n1->m_pNext = n2; }
	
/** 
 * @name NodeUnlinkNext
 * @memo Unlinks the last node from the list.
 * @doc  Unlinks the last node after the indicated node. Frees the unlinked node. 
 * Leaks any nodes after the indicated node, and any meta data associated with the node.
 * @param n1: Node in list after which to free.
 */
#define NodeUnlinkNext( n1 ) \
	{ struct _Node *_p; _p = n1->m_pNext; n1->m_pNext = _p->m_pNext; FREE( _p ); _p = NULL; }

/** 
 * @name NodeNext
 * @memo Returns the next node after the current node.
 * @doc  Returns the next node after the current node.
 * @param n: node to return next node
 * @return next node after n or NULL if n is the last node on the list.
 */
#define NodeNext( n ) \
	( n->m_pNext )


/** 
 * @name PFNNODEORDER
 * @memo Comparison function for sorting linked lists
 * @doc  Compares the contents of two linked list entries for equality.
 * @param pThis: pointer to app context passed to order function 
 * @param head: Head node of sorted list.
 * @param n: node to insert
 * @return negative if n1<n2, 0 if n1==n2, positive if n1>n2
 */
typedef int32 (PFNNODEORDER)( void *pThis, 
							  struct _Node *n1, 
							  struct _Node *n2 );

/** 
 * @name NodeLinkSorted
 * @memo Links a node into a sorted list.
 * @doc  Links a node into a list sorted by the given comparison function.
 * @param pThis: pointer to app context passed to order function 
 * @param head: Head node of sorted list.
 * @param n: node to insert
 * @param fnSort: comparison function (takes two nodes and application data)
 * @return nothing
 */
#define NodeLinkSorted( pThis, head, n, fnSort ) \
{ \
	struct _Node *_p = head;\
	while( _p->pNext && fnSort( pThis, head, n ) ) _p = _p->m_pNext; \
	n->pNext = _p->pNext; \
	_p->pNext = n; \
}



