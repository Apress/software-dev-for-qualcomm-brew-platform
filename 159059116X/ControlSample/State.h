/**
 *  @name AppStates.h
 *  @author Ray Rischpater, KF6GPE
 * 
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  @doc
 *  This file provides the structures and interface for a state
 *  machine manager based on a single stack to track application
 *  state transitions.
 *
 * To the user, a state is a screen.
 * Programmatically, a state is an ordered tuple containing
 * 
 * -- a memory region to hold internal scratch data
 *
 * -- event handler
 *
 * The currently active state --- that at the top of the stack -- 
 * will have its functions called. Other states on the stack do 
 * not receive function calls.
 */


/**
 * @name EStateControl
 * @memo Delineates framework controls.
 * @doc This enum is used by the framework to refer to framework-maintained controls in the control pool. Your states can use any of these controls, too.
 */
typedef enum 
{
	/**
	 * @memo Navigation menu.
	 * @doc This menu item expects a new state in its dwData slot. The framework will dispatch to the new state unless dwData is NULL. When entering a state from a menu selection, the cause code will contain the menu's wItemID.
	 */
	Ctl_NavMenu = 0,
	/**
	 * @memo Soft Key menu.
	 * @doc For use with text input lines or other soft key assignments. This menu item expects a new state in its dwData slot. The framework will dispatch to the new state unless dwData is NULL. The OK item should have a wItemID of StateChangeInfo_SELECT, while the Cancel item must have an itemID of StateChangeInfo_CLR.
	 */
	Ctl_SoftKeyMenu,
	/**
	 * @memo Text input screen.
	 * @doc Provides transparent handling of key presses for text input. When user presses SELECT to finish entry, the state using this control will be popped. The cause code for the pop will contain either StateChangeInfo_SELECT (if SELECT closed the input field) or StateChangeInfo_CLR (if StateChangeInfo_CLR closed the input field).
	 */
	Ctl_TextInput,
	  
	// End of the list... don't mess with this one!
	Ctl_LastFrameworkControl
} EStateControl;

typedef struct _Node CState;
typedef struct _Node *CStatePtr;

/**
 * @name EStateChangeCause
 * @memo State transitions.
 * @doc Each state function is given one of these enums in a bit field in the wParam during a state transition to indicate the reason for the state change.
 */
typedef enum 
{
	/// A new state is being pushed
	StateChange_push = 512,    
	/// The current state is being popped.
	StateChange_pop = 1024,     
	/// The system has sent an EVT_SUSPEND
	StateChange_suspend = 2048, 
	/// The system has sent an EVT_RESUME
	StateChange_resume = 4096, 
	/// The state change cause mask
	StateChange_causemask = 0xFF00,
	StateChange_infomask = 0x00FF
} EStateChangeCause;

/**
 * @name EStateChangeInfo
 * @memo State transition extra information.
 * @doc Each state function is given one of these values in a bit field in the wParam during a state transition to indicate the reason for the state change.
 */
typedef enum 
{
	/// Nothing particular is known
	StateChangeInfo_push = 0,
	/// User pressed CLR key
	StateChangeInfo_CLR = 254,
	/// User pressed SELECT key
	StateChangeInfo_SELECT = 255
} EStateChangeInfo;

/**
 * @name StateChangeCause
 * @memo Returns reason for state change.
 * @doc Manipulates given bitfield to mask off reason for state change
 */
#define StateChangeCause( x ) ( ( x & StateChange_causemask ) )

/**
 * @name StateChangeInfo
 * @memo Returns information for state change from cause info.
 * @doc Manipulates given bitfield to mask off info regarding state change
 */
#define StateChangeInfo( x ) ( ( x & StateChange_infomask ) )



/*
 * Suspend forces an exit of the current state, while resume
 * forces a reentry of the current state.
 */

// Returning true indicates event was handled.
typedef boolean (PFNSTATEEVENT)( void *pApp, 
								AEEEvent eCode, 
								uint16 wParam, uint32 dwParam);

#define State_PopState ((PFNSTATEEVENT *)(-1))

/**
 * @name CStateApp
 * @memo Application state data.
 * @doc Tracks what the framework needs to know about the current state of the application: what state it's in, what controls it has, and so forth.
 */
typedef struct _CStateApp
{
	/// The application context. This must always be the first field.
	AEEApplet	a;

	/// The state stack
	CStatePtr      m_pState;
	
	/// The application preferences
	void		*m_pAppPrefs;
	
	/// The application global data
	void		*m_pAppData;

	/// The pool of controls that the framework will manage.
	IControl      *m_apControl[ MAX_NUM_CONTROLS ];
	uint8         m_nControl;
} CStateApp, *CStateAppPtr;

// Handy-dandy accessors and such.

/**
 * @name State_IsEqual
 * @memo Compares two states for equality.
 * @doc Tracks what the framework needs to know about the current state of the application: what state it's in, what controls it has, and so forth.
 */
#define State_IsEqual( s1, s2 ) \
	( s1->m_pData == s2->m_pData ) )
	
/**
 * @name State_GetStateData
 * @memo Returns the indicated state's data.
 * @doc Returns the state data for the indicated state.
 */
#define State_GetStateData( s ) ( s->m_pData )

/**
 * @name State_SetStateData
 * @memo Sets the indicated state's data.
 * @doc Assigns the state data for the indicated state 
 * to the given pointer.
 */
#define State_SetStateData( s, d ) ( s->m_pData = ( d ) )

/**
 * @name State_GetStateEventHandler
 * @memo Returns the indicated state's event handler.
 * @doc Returns the state event handler for the indicated state.
 */
#define State_GetStateEventHandler( s ) ( (PFNSTATEEVENT *)(s->m_pMetaData) )

/**
 * @name State_SetStateEventHandler
 * @memo Sets the indicated state's event handler
 * @doc Assigns the state event handler for the indicated state 
 * to the given pointer.
 */
#define State_SetStateEventHandler( s, d ) ( s->m_pMetaData = d )

/**
 * @name State_GetCurrentState
 * @memo Returns the current state.
 * @doc Returns the CStatePtr for the currently active state.
 */
#define State_GetCurrentState( app ) \
	( ((CStateAppPtr)(app))->m_pState->m_pNext ? \
      ((CStateAppPtr)(app))->m_pState->m_pNext : NULL )

/**
 * @name State_GetStateData
 * @memo Returns the current state's data
 * @doc Returns the state data for the current state.
 */
#define State_GetCurrentStateData( app ) \
	( (State_GetCurrentState(app))->m_pData )
/**
 * @name State_SetStateData
 * @memo Sets the current state's data.
 * @doc Assigns the state data for the currently active state to the given pointer.
 */
#define State_SetCurrentStateData( app, data ) \
	( (State_GetCurrentState(app))->m_pData = ( data ) )

/** 
 * @name STATE_DEFERREDACTIONDELAY
 * @memo Delay for deferred action
 * @doc How long the framework should wait before performing a deferred action.
 */
#define STATE_DEFERREDACTIONDELAY ( 10 )
   
/**
 * @name State_DeferAction
 * @memo Defer a function call.
 * @doc Instructs the framework to make the specified function call with the application context when next idle.
 */     
#define State_DeferAction( p, action ) ISHELL_SetTimer( \
	((CStateAppPtr)(p))->a.m_pIShell, \
	STATE_DEFERREDACTIONDELAY, \
	(PFNNOTIFY)(action), \
	p );


/*
 * Prototypes
 */
boolean State_Push( void *p,
				   PFNSTATEEVENT *pfEvent );

boolean State_Pop( void *p );
boolean State_HandleEvent( void *pApp, 
						  AEEEvent eCode, 
						  uint16 wParam, 
						  uint32 dwParam );

