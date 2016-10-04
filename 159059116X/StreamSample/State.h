/*
 *  @name State.h
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
 * -- an event handler function
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

/**
 * @memo State function type definition.
 * @doc A state function is just an event handler by another name. The event handler
 * must handle at least four events. 
 * The currently active state will receive all unhandled application events, as
 * well as framework-generated events.
 * The state function will receive the
 * EVT_APP_START event whevever the state is entered. If the state is being
 * entered because it is being pushed on the stack, wParam will indicate 
 * this with an EStateChangeStateInfo of StateChangeInfo_push. If the
 * state is being entered because a following state was popped from the
 * state, the wParam will indicate this with an EStateChangeInfo of 
 * StateChangeInfo_pop. Similarly, the state function will receive the EVT_APP_STOP
 * event when the state should terminate, with the wParam indicating that the state
 * was either popped (wParam indicating this with StateChangeInfo_pop) or pushed deeper
 * on the stack (wParam indicating this with StateChangeInfo_push). 
 * Finally, the state function must handle the EVT_APP_SUSPEND and EVT_APP_RESUME events.
 */
typedef boolean (PFNSTATEEVENT)( void *pApp, 
								AEEEvent eCode, 
								uint16 wParam, uint32 dwParam);

/// Synonymous with POP when used as a state function.
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
	CAppPrefsPtr	m_pAppPrefs;
	
	/// The application global data
	CAppDataPtr		m_pAppData;

	/// The pool of controls that the framework will manage.
	IControl		*m_apControl[ MAX_NUM_CONTROLS ];
	uint8			m_nControl;
} CStateApp, *CStateAppPtr;

// Handy-dandy accessors and such.

/** 
 * @name GetAppData
 * @memo Returns application's global data.
 * @doc Returns a pointer to the application's global data.
 */
#define GetAppData( pThis ) (((CStateAppPtr)pThis)->m_pAppData)

/** 
 * @name SetAppData
 * @memo Returns application's global data.
 * @doc Sets the pointer to the application's global data.
 */
#define SetAppData( pThis, p )\
	( ((CStateAppPtr)pThis)->m_pAppData = p )

/** 
 * @name GetAppPrefs
 * @memo Returns application's preferences.
 * @doc Returns a pointer to the application's preferences.
 */
#define GetAppPrefs( pThis ) (((CStateAppPtr)pThis)->m_pAppPrefs)



/**
 * @name State_IsEqual
 * @memo Compares two states for equality.
 * @doc Tracks what the framework needs to know about the current state of the application: what state it's in, what controls it has, and so forth.
 */
#define State_IsEqual( s1, s2 ) \
	( s1->m_pData == s2->m_pData ) )
	
/**
 * @name State_GetStateData
 * @memo Returns the indicated state's private data.
 * @doc Returns the state's private data pointer for the indicated state. You may define
 * a structure and use MALLOC() to allocate space for the state's private data upon
 * entry to the state. You should use FREE() to free the state's private data when the state
 * is exited and popped from the state stack.
 */
#define State_GetStateData( s ) ( s->m_pData )

/**
 * @name State_SetStateData
 * @memo Sets the indicated state's private data.
 * @doc Assigns the state's private data pointer for the indicated state. You may define
 * a structure and use MALLOC() to allocate space for the state's private data upon
 * entry to the state. You should use FREE() to free the state's private data when the state
 * is exited and popped from the state stack.
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
#define State_SetStateEventHandler( s, d ) ( s->m_pMetaData = (void *)d )

/**
 * @name State_GetCurrentState
 * @memo Returns the current state.
 * @doc Returns the CStatePtr for the currently active state.
 */
#define State_GetCurrentState( app ) \
	( ((CStateAppPtr)(app))->m_pState->m_pNext ? \
      ((CStateAppPtr)(app))->m_pState->m_pNext : NULL )


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

