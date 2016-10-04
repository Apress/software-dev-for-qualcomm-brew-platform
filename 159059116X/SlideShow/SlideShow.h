/*
 *  SlideShow.h
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  This file provides the definition for SlideShow.
 */

#define APP_RES_FILE				SLIDESHOW_RES_FILE
#ifdef _WINDOWS
#define APP_COPYRIGHT_CLOSE 500
#else
#define APP_COPYRIGHT_CLOSE 10000
#endif

typedef struct _CApp
{
  AEEApplet a;
  CStatePtr m_state;

  // screen and font dimensions
  int       m_cx;
  int       m_cy;
  int       m_nFontHeight;
  boolean   m_bColor;
  boolean   m_bCopyright;
  AEERect   m_rc;

  // BREW controls
  IMenuCtl  *m_pIMenu;

  // Application globals
  char      m_szFile[ MAX_FILE_NAME + 1 ];
  int       m_frameDelay;
} CApp, *CAppPtr;
