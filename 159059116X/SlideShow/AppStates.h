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
 *  This file provides the interface for the states of SlideShow.
 */



/*
 * A slide show indicates the 
 *  bar file name
 *  current slide to display
 *  timer (when nonzero, time to next slide
 */

typedef struct _CSlideShow
{
  char    *m_szFile;
  uint16  m_next;
  uint16  m_timer;
  int8    m_dir;
  boolean m_bAnimate;
} CSlideShow, *CSlideShowPtr;


/*
 * Function Prototypes
 */
void AS_MenuEntry( void *pApp, 
                   EStateChange change );
void AS_MenuExit( void *pApp, 
                  EStateChange change );
boolean AS_MenuHandleEvent( void *pApp, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam);

void AS_SlideEntry( void *p, 
                      EStateChange change );
void AS_SlideExit( void *p, 
                     EStateChange change );
boolean AS_SlideHandleEvent( void *p, 
                            AEEEvent eCode, 
                            uint16 wParam, uint32 dwParam);
