/*
 *  State.c
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  This file provides the implementation for a state
 *  machine manager based on a single stack to track application
 *  state transitions.
 */

 
/*
 * Includes.
 */
#include "inc.h"

/*
 * Function Prototypes
 */
static CStatePtr State_Create( 
                    void *p,
                    PFNSTATEENTRY pfEntry, 
                    PFNSTATEEXIT pfExit, 
                    PFNSTATEEVENT pfEvent );

static void State_Release( void *p,
                    CStatePtr pState );

/*
 * Implementation
 */

/** 
 * Allocates memory for new state and state data.
 * Sets fields of state with given arguments
 * @param void *p: pointer to app (unused)
 * @param PFNSTATEENTRY pfnEntry: call on function entry
 * @param PFNSTATEEXIT pfnExit: call on function exit
 * @param PFNSTATEEVENT pfnEvent: call on each event
 * @param uint32 nData: size of state data
 * @param byte *pData: state data or NULL to zero
 * @return pointer to new state or NULL if either allocation fails
 */
static CStatePtr State_Create( 
                         void *p,
                         PFNSTATEENTRY pfEntry, 
                         PFNSTATEEXIT pfExit, 
                         PFNSTATEEVENT pfEvent )
{
  CStatePtr pResult = NULL;

  UNUSED( p ); 

  // Gotta have at least one function
  ASSERT( pfEntry || pfExit || pfEvent );

  // Allocate memory
  pResult = MALLOC( sizeof( CState ) );

  // Initialize
  if ( pResult )
  {
    pResult->m_pPrevState = NULL;
    pResult->m_pfEntry = pfEntry;
    pResult->m_pfExit = pfExit;
    pResult->m_pfEvent = pfEvent;
    pResult->m_pData = NULL;
  }

  return pResult;
}

/** 
 * Frees memory for new state and state data.
 * Sets fields of state with given arguments
 * @param void *p: pointer to app (unused)
 * @param CStatePtr *pSate: state to free
 * @return nothing
 */
static void State_Release( void *p,
                           CStatePtr pState )
{
  UNUSED( p ); 

  ASSERT( pState );
  ASSERT( pState->m_pData == NULL );

  if ( pState ) FREE( pState );
}

/**
 * Pushes indicated state on stack;
 * Transitions to that state.
 * @param void *p: pointer to app
 * @param PFNSTATEENTRY pfnEntry: call on function entry
 * @param PFNSTATEEXIT pfnExit: call on function exit
 * @param PFNSTATEEVENT pfnEvent: call on each event
 * return TRUE if transition succeeds, FALSE if not
 */
boolean State_Push( void *p,
                    PFNSTATEENTRY pfEntry, 
                    PFNSTATEEXIT pfExit, 
                    PFNSTATEEVENT pfEvent )
{
  CStateAppPtr pThis = ( CStateAppPtr )p;
  CStatePtr pCurState, pNewState;
  
  ASSERT( pfEntry || pfExit || pfEvent );

  pCurState = State_GetCurrentState( pThis );
  pNewState = State_Create( p, pfEntry, pfExit, pfEvent );

  ASSERT( pNewState );
  if ( !pNewState ) return FALSE;

  // Call the present state's exit function
  if ( pCurState && pCurState->m_pfExit )
    pCurState->m_pfExit( p,
                         StateChange_push );

  // Push this state on to the stack.
  pNewState->m_pPrevState = pCurState;
  pThis->m_pState = pNewState;
  
  // Call new state's entry function  
  if ( pNewState->m_pfEntry )
    pNewState->m_pfEntry( p, 
                         StateChange_push );

  // No errors!
  return TRUE;
}

/**
 * Pushes indicated state on stack;
 * Transitions to that state.
 * @param void *p: pointer to app
 * @param PFNSTATEENTRY pfnEntry: call on function entry
 * @param PFNSTATEEXIT pfnExit: call on function exit
 * @param PFNSTATEEVENT pfnEvent: call on each event
 * return TRUE if transition succeeds, FALSE if not
 */
boolean State_Pop( void *p )
{
  CStateAppPtr pThis = ( CStateAppPtr )p;
  CStatePtr pCurState, pOldState;

  pCurState = State_GetCurrentState( pThis );
  pOldState = pCurState->m_pPrevState;

  ASSERT( pCurState );
  if ( !pCurState ) return FALSE;

  // Call the present state's exit function
  if ( pCurState->m_pfExit )
    pCurState->m_pfExit( p,
                         StateChange_pop );

  // Pop the old state off of the stack.
  pThis->m_pState = pOldState;
  pCurState->m_pPrevState = NULL;

  // Call new state's entry function  
  if ( pOldState && pOldState->m_pfEntry )
    pOldState->m_pfEntry( p, 
                         StateChange_pop );

  // Release the popped state
  State_Release( p, pCurState );

  // No errors!
  return TRUE;

}

/**
 * Issues suspend to current state.
 * @param void *p: pointer to app
 * @return nothing
 */
void State_Suspend( void *p )
{
  CStateAppPtr pThis = ( CStateAppPtr )p;
  CStatePtr pCurState;

  ASSERT( pThis );

  pCurState = State_GetCurrentState( pThis );

  if ( pCurState && pCurState->m_pfExit )
    pCurState->m_pfExit( p,
                         StateChange_suspend );

}

/**
 * Isses resume to current state.
 * @param void *p: pointer to app
 * @return nothing
 */
void State_Resume( void *p )
{
  CStateAppPtr pThis = ( CStateAppPtr )p;
  CStatePtr pCurState;

  ASSERT( pThis );

  pCurState = State_GetCurrentState( pThis );

  if ( pCurState && pCurState->m_pfEntry )
    pCurState->m_pfEntry( p,
                          StateChange_resume );

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

  ASSERT( pThis );
  pCurState = State_GetCurrentState( pThis );

  if ( !pCurState ) return FALSE;

  // We always give the state first dibs.
  if ( pCurState->m_pfEvent )
    result = pCurState->m_pfEvent( p, eCode, wParam, dwParam );

  // If the state didn't do anything with it, we will.
  if ( !result ) switch( eCode )
  {
    // Tell the current state to suspend operations
    case EVT_APP_SUSPEND:
      State_Suspend( p );
      result = TRUE;
      break;

    // Tell the current state to resume operations
    case EVT_APP_RESUME:
      State_Resume( p );
      result = TRUE;
      break;

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
        State_Pop( p );
        // If there are no more states, we exit by saying the event
        // isn't completely handled. The shell will send us an
        // EVT_APP_STOP after this.
        result = State_GetCurrentState( pThis ) ? TRUE : FALSE;
        break;
      }
  }

  return result;
}

