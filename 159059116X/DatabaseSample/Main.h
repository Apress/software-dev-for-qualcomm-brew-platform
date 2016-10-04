/*
 *  @name Main.h
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
 *  This file provides the definition for the application.
 */

/**
 * @name APP_COPYRIGHT_CLOSE
 * @memo How long the splash screen is shown.
 * @doc How long (in milliseconds) the splash screen will be shown.
 */
#ifdef _WINDOWS
#define APP_COPYRIGHT_CLOSE 500
#else
#define APP_COPYRIGHT_CLOSE 2000
#endif

/**
 * @name CApp
 * @memo Application context.
 * @doc This stores the application's current context and state information.
 */
typedef struct _CApp
{
	/// Stores the application framework information.
	CStateApp   m_app;
 
	/// Screen width
	int			m_cx;
	/// Screen height
	int			m_cy;
	/// Color depth
	int			m_colorDepth;
	/// Font height in pixels
	int			m_nFontHeight;
	///	Does the screen support color?
	boolean				m_bColor;
	///	A rectangle with the drawable bounds
	AEERect				m_rc;
	///	True if the splash screen is showing.
	boolean		m_bSplashing;
} CApp, *CAppPtr;

