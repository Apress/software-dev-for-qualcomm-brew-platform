/*
 *  utils.h
 *
 *  Created by Ray Rischpater on 1 October 2002
 *  Copyright (c) 2002 by the author. All rights reserved.
 *  This file is provided without any warranty, express or implied,
 *  including but not limited to fitness of purpose.
 *  You may use this file so long as the copyright attribution
 *  remains.
 * 
 *  This file provides the interface for miscellaneous utilities
 *  we miss when developing for BREW.
 */

// ARM tools don't grok #pragma unused
#define UNUSED( x ) (x);

// An ASSERT for all seasons
#ifdef _DEBUG
#define ASSERT( x ) if ( !( x ) ) { \
    int *p = NULL; \
    DBGPRINTF("Assert failed: [%s]\r\n Halting.", #x); \
    *p=1; \
  } 
#else
#define ASSERT( x ) ;
#endif


