/*
*  @name State.c
*  @author Ray Rischpater, KF6GPE
* 
*  Created by Ray Rischpater on 1 October 2002
*  Copyright (c) 2002 by the author. All rights reserved.
*  This file is provided without any warranty, express or implied,
*  including but not limited to fitness of purpose.
*  You may use this file so long as the copyright attribution
*  remains.
*  @memo Application framework
*  @doc
*  This file provides the implementation for a state
*  machine manager based on a single stack to track application
*  state transitions.
*/


/*
* Includes.
*/
#include "inc.h"

/*
 * Static function prototypes
 */
 
static boolean State_PopEx( void *p,
                            unsigned char info );
static boolean State_PushEx( void *p,
	   			             PFNSTATEEVENT *pfEvent,
				             unsigned char info );
                            
/*
 * Implementation
 */

/**
 * Pushes indicated state on stack with add'l info;
 * transitions to that state.
 * @param void *p: pointer to app
 * @param PFNSTATEEVENT pfnEvent: call on each event
 * @return TRUE if transition succeeds, FALSE if not
*/
static boolean State_PushEx( void *p,
	   			             PFNSTATEEVENT *pfEvent,
				             unsigned char info )
{
	CStateAppPtr pThis = ( CStateAppPtr )p;
	CStatePtr pCurState, pNewState;
	boolean result = TRUE;
	PFNSTATEEVENT *pfn;
	PFNSTATEEVENT *pfn_OnError = NULL;
	uint16 reason = StateChange_push | info;
	
	ASSERT( pfEvent );
	ASSERT( p );
	
	if ( pfEvent == State_PopState )
		return State_PopEx( p, info );

	pCurState = State_GetCurrentState( pThis );
	pNewState = (CStatePtr)MALLOC( sizeof( CState ) );
	State_SetStateEventHandler( pNewState, pfEvent );
	
	if ( !pNewState || !pThis ) return FALSE;
	
	if ( pCurState && ( pfn = State_GetStateEventHandler( pCurState ) ) )
		result = (pfn)( p, EVT_APP_STOP, reason, (uint32)&pfn_OnError );

	// If the current state function said it's OK, start the new state.
	if ( result && ( pfn = State_GetStateEventHandler( pNewState ) ) )
		result = (pfn)( p, EVT_APP_START, reason, (uint32)&pfn_OnError );

	// If result is TRUE, the current state is ready
	if ( result )
	{
		// Push the new state onto the stack..
		NodeLinkNext( pThis->m_pState, pNewState);		
	}
	else
	{	
		// Free the current state.
		FREE( pNewState );
		// if pfn_OnError is set, this is our new state...
		if ( pfn_OnError )
		{
			if ( pfn_OnError == State_PopState )
			{
				// Pop this state instead
				result = State_PopEx( pThis, info );
			}
			else
			{
				// Push the replacement state
				result =  State_PushEx( pThis, pfn_OnError, info );
			}
		}
		// If pfn_OnError is NULL, we stayed in the same state.
	}

	return result;
}

/**
 * Pushes indicated state on stack.
 * Transitions to that state.
 * @param void *p: pointer to app
 * @param PFNSTATEEVENT pfnEvent: call on each event
 * @return TRUE if transition succeeds, FALSE if not
*/
boolean State_Push( void *p,
	    	        PFNSTATEEVENT *pfEvent )
{
	return State_PushEx( p, pfEvent, 0 );
}


/**
 * Pops current state from the stack with additional info.
 * Transitions to that state.
 * @param void *p: pointer to app
 * return TRUE if transition succeeds, FALSE if not
 */
static boolean State_PopEx( void *p,
                            unsigned char info )
{
	CStateAppPtr pThis = ( CStateAppPtr )p;
	CStatePtr pCurState, pOldState;
	boolean result = TRUE;
	PFNSTATEEVENT *pfn;
	PFNSTATEEVENT *pfn_OnError = NULL;
	uint16 reason = StateChange_pop | info;
		
	ASSERT( p );
	if ( !p ) return FALSE;

	pCurState = State_GetCurrentState( pThis );
	ASSERT( pCurState );
	if ( !pCurState ) return FALSE;
	
	pOldState = NodeNext( pCurState );

	// Call the present state's exit function
	if ( ( pfn = State_GetStateEventHandler( pCurState ) ) )
		result = (pfn)( p, EVT_APP_STOP, reason, (uint32)&pfn_OnError );
		
	// If the current state function said it's OK, 
	// restart the previous state.
	if ( result && pOldState &&
		 ( pfn = State_GetStateEventHandler( pOldState ) ) )
		result = (pfn)( p, EVT_APP_START, reason, (uint32)&pfn_OnError );
		
	// If result is TRUE, we can pop the current state
	if ( result )
	{
		// Pop the state from the stack.
		NodeUnlinkNext( pThis->m_pState );		
	}
	else
	{	
		// if pfn_OnError is set, this is our new state...
		if ( pfn_OnError )
		{
			if ( pfn_OnError == State_PopState )
			{
				// Pop the current state...
				NodeUnlinkNext( pThis->m_pState );
				// ... and the previous one as well
				result = State_PopEx( pThis, info );
			}
			else
			{
				// Pop the state from the stack.
				NodeUnlinkNext( pThis->m_pState );		
				// Push the replacement state				
				result =  State_PushEx( pThis, pfn_OnError, info );
			}
		}
		// If pfn_OnError is NULL, we stayed in the same state.
	}

	return result;
}


/**
 * Pops current state from the stack with additional info.
 * Transitions to that state.
 * @param void *p: pointer to app
 * return TRUE if transition succeeds, FALSE if not
*/
boolean State_Pop( void *p )
{
	return State_PopEx( p, 0 );
}



/** 
 * Handles incoming events sent to framework controls.
 * @param void *p: pointer to app
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParan: event parameter
 * @return boolean FALSE if app framework should continue handling event
*/

static boolean 
_controlHandleEvent( CStateAppPtr pThis, 
					 AEEEvent eCode, 
					 uint16 wParam, 
					 uint32 dwParam )
{
	boolean result = FALSE;
	boolean bTextControlOnscreen = 
		ITEXTCTL_GetTextPtr( (ITextCtl *)pThis->m_apControl[ Ctl_TextInput ] ) != NULL ? TRUE : FALSE;
	boolean bSoftMenuOnscreen = 
		IMENUCTL_GetItemCount( (IMenuCtl *)pThis->m_apControl[ Ctl_SoftKeyMenu ] ) != 0 ? TRUE : FALSE;
	int wCurrCtl;
	
	// Dispatch the event to any of the active controls
	for ( wCurrCtl = 0; 
		  wCurrCtl < pThis->m_nControl; 
		  wCurrCtl++ )
	{
		if ( pThis->m_apControl[ wCurrCtl ] &&
			ICONTROL_IsActive( pThis->m_apControl[ wCurrCtl ] ) )
		{

			result = 
				ICONTROL_HandleEvent( 
					pThis->m_apControl[ wCurrCtl ], 
					eCode, wParam, dwParam );
			if ( result ) break;
		}
	}

	// Select on a text field with no soft key pops state
	if ( eCode == EVT_KEY && wParam == AVK_SELECT &&
		 ICONTROL_IsActive( pThis->m_apControl[ Ctl_TextInput ] ) &&
		 !ICONTROL_IsActive( pThis->m_apControl[ Ctl_SoftKeyMenu ] ) )
	{
		State_PopEx( pThis, StateChangeInfo_SELECT );
		result = TRUE;
	}

	// Key up in text control with soft key menu 
	// activates text control
	if ( bSoftMenuOnscreen && bTextControlOnscreen &&
		 eCode == EVT_KEY && wParam == AVK_UP &&
		 !ICONTROL_IsActive( pThis->m_apControl[ Ctl_TextInput ] ) &&
		 ICONTROL_IsActive( pThis->m_apControl[ Ctl_SoftKeyMenu ] ) )
	{
		ICONTROL_SetActive( pThis->m_apControl[ Ctl_SoftKeyMenu ], FALSE );
		ICONTROL_SetActive( pThis->m_apControl[ Ctl_TextInput ], TRUE );
		result = TRUE;
	}

	// Key down in text control with soft key menu 
	// activates menu control
	if ( bSoftMenuOnscreen && bTextControlOnscreen &&
		 eCode == EVT_KEY && wParam == AVK_DOWN &&
		 ICONTROL_IsActive( pThis->m_apControl[ Ctl_TextInput ] ) &&
		 !ICONTROL_IsActive( pThis->m_apControl[ Ctl_SoftKeyMenu ] ) )
	{
		ICONTROL_SetActive( pThis->m_apControl[ Ctl_SoftKeyMenu ], TRUE );
		ICONTROL_SetActive( pThis->m_apControl[ Ctl_TextInput ], FALSE );
		result = TRUE;
	}
	// Menu selection navigates to new state
	if ( !result && eCode == EVT_COMMAND )
	{
		if ( ICONTROL_IsActive( pThis->m_apControl[ Ctl_NavMenu ] ) ||
	  		 ICONTROL_IsActive( pThis->m_apControl[ Ctl_SoftKeyMenu ] ) )
		{
			PFNSTATEEVENT *pState;
			IMenuCtl *pIMenu;
			pIMenu = ICONTROL_IsActive( pThis->m_apControl[ Ctl_NavMenu ] ) ?
				(IMenuCtl *)( pThis->m_apControl[ Ctl_NavMenu ] ) : 
				(IMenuCtl *)( pThis->m_apControl[ Ctl_SoftKeyMenu ] );

			IMENUCTL_GetItemData( pIMenu, 
				wParam, (uint32 *)(&pState) );
				
			if ( pState != NULL )
			{
				State_PushEx( pThis, pState, 
					(unsigned char)(wParam & 0xFF));				
				result = TRUE;
			}
			else
			{
				DBGPRINTF("Error? Menu state null; state should handle");
			}
		}
	}
	return result;
}

/** 
 * Handles incoming events.
 * Dispatches suspend and resume to current state.
 * Pops state on AVK_CLR
 * Pops all states on EVT_APP_STOP
 * @param void *p: pointer to app
 * @param AEEEvent eCode: event code
 * @param uint16 wParam: event parameter
 * @param uint32 dwParan: event parameter
 * @return boolean FALSE if app framework should continue handling event
 */
boolean State_HandleEvent( void *p, 
						  AEEEvent eCode, 
						  uint16 wParam, 
						  uint32 dwParam )
{
	boolean result = FALSE;
	CStatePtr pCurState;
	CStateAppPtr pThis = ( CStateAppPtr )p;
	PFNSTATEEVENT *pfn;

	ASSERT( pThis );
	if (!pThis) return FALSE;
	
	// Offer the event to the framework controls first
	result = _controlHandleEvent( pThis, eCode, wParam, dwParam );
	if ( result ) 
		return TRUE;	
	
	// Get the current state. If there's not one, that's OK.
	pCurState = State_GetCurrentState( pThis );
	if ( !pCurState ) return FALSE;
	
	pfn = State_GetStateEventHandler( pCurState );

	// The state comes next.
	if ( !result && pfn )
		result = (*pfn)( p, eCode, wParam, dwParam );

	// If the state didn't do anything with it, we will.
	if ( !result ) switch( eCode )
	{
		// Pop ALL states from the stack -- we're exiting
		case EVT_APP_STOP:
			while( State_GetCurrentState( pThis ) )
				State_Pop( p );
			result = TRUE;
			break;

		// If the user hit CLR, pop
		case EVT_KEY:
			if ( wParam == AVK_CLR &&
				State_GetCurrentState( pThis ) )
			{
				State_PopEx( p, StateChangeInfo_CLR );
				// If there are no more states, we exit by saying the event
				// isn't completely handled. The shell will send us an
				// EVT_APP_STOP after this.
				result = State_GetCurrentState( pThis ) != NULL ? 
					TRUE : FALSE;
				break;
			}      
	}

	return result;
}

