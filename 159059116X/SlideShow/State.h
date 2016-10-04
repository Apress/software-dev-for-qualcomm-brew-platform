/*
 *  AppStates.h
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  This file provides the structures and interface for a state
 *  machine manager based on a single stack to track application
 *  state transitions.
 */

/*
 * What's a state?
 * To the user, a state is a screen.
 * Programmatically, a state is a
 * -- memory region to hold internal scratch data
 * -- entry function (draws the screen and what have you)
 * -- event handler
 * -- exit function (serializes state info and what have you)
 * The currently active state --- that at the top of the stack --- will have
 * its functions called. Other states on the stack do not receive function
 * calls.
 */

// State Changes
typedef enum {
  StateChange_push,    // occurs when app pushes a new state
  StateChange_pop,     // occurs when app forces a pop
  StateChange_suspend, // occurs when pump sends EVT_SUSPEND
  StateChange_resume   // occurs when pump sends EVT_RESUME
} EStateChange;

/*
 * Suspend forces an exit of the current state, while resume
 * forces a reentry of the current state.
 */

// State function type declarations

typedef void (PFNSTATEENTRY)( void *pApp, EStateChange change );
typedef void  (PFNSTATEEXIT)( void *pApp, EStateChange change );

// Returning true indicates event was handled.
typedef boolean (PFNSTATEEVENT)( void *pApp, 
                 AEEEvent eCode, 
                 uint16 wParam, uint32 dwParam);

typedef struct _CState
{
  struct _CState *m_pPrevState;
  PFNSTATEENTRY  *m_pfEntry;
  PFNSTATEEXIT   *m_pfExit;
  PFNSTATEEVENT  *m_pfEvent;
  void           *m_pData;
} CState, *CStatePtr;

// This is all the state machine stuff 
// knows about the app structure.
typedef struct _CStateApp
{
	AEEApplet			a;  // Must always be first 
  CStatePtr     m_pState; // The current state.
} CStateApp, *CStateAppPtr;

// Handy-dandy accessors and such.
#define STATE_ISEQUAL( s1, s2 ) \
  ( ( s1->m_pfEntry == s2->m_pfEntry ) && \
    ( s1->m_pfExit == s2->m_pfExit ) && \
    ( s1->m_pfHandleEvent == s2->m_pfHandleEvent ) )

#define STATE_GETSTATE( p ) ( ((CStateAppPtr)(p))->m_pState )

// Macros that could be functions...
#define State_GetStateData( s ) ( s->m_pData )
#define State_SetStateData( s, d ) ( s->m_pData = d );

// There is no SetCurrentState to keep the user from
// honking the stack by mistake...
#define State_GetCurrentState( app ) \
  ( ((CStateAppPtr)(app))->m_pState )
#define State_GetCurrentStateData( app ) \
  ( (((CStateAppPtr)(app))->m_pState)->m_pData );
#define State_SetCurrentStateData( app, data ) \
  ( (((CStateAppPtr)(app))->m_pState)->m_pData = ( data ) );

/*
 * Prototypes
 */
boolean State_Push( void *p,
                    PFNSTATEENTRY pfEntry, 
                    PFNSTATEEXIT pfExit, 
                    PFNSTATEEVENT pfEvent );
boolean State_Pop( void *p );
void State_Suspend( void *p );
void State_Resume( void *p );
boolean State_HandleEvent( void *pApp, 
                           AEEEvent eCode, 
                           uint16 wParam, 
                           uint32 dwParam );
                           
#define DEFER_STATE_ACTION( p, action ) ISHELL_SetTimer( \
                    p->a.m_pIShell, \
                    100, \
                    (PFNNOTIFY)(action), \
                    p );
