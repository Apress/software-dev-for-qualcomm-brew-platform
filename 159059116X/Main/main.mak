#============================================================================
#  Name:
#    Makefile
#
#  Description:
#    Makefile to build the expense app downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     nmake [target]
#
# Assumptions:
#   1. The ARM ADS 1.0.1 tools are installed in the c:\ADS\ directory.
#
# Copyright (c) 2001 by QUALCOMM Incorported.  All Rights Reserved.
# Changes Copyright (c) 2003-2003 by Ray Rischpater. All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

TARGET_DIR	= .
SUPPORT_DIR    	= ..\..\BREW\BREW_AEE_11
SUPPORT_INCDIR 	= ..\..\BREW\BREW_inc_11

TARGET     = $(TARGET_DIR)\Main  # Target name for output files and object dir
EXETYPE    = elf            # Target image file format
MODULE     = mod            # Downloadable module extension

#-------------------------------------------------------------------------------
# Target compile time symbol definitions
#-------------------------------------------------------------------------------

DYNAPP          = -DDYNAMIC_APP 

#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------
ADSROOT = c:\ADS
ARMBIN =  $(ADSROOT)\bin       # ARM ADS application directory
ARMINC =  $(ADSROOT)\include    # ARM ADS include file directory
ARMLIB =  $(ADSROOT)\lib        # ARM ADS library directory

ARMCC   = $(ARMBIN)\armcc      # armcpp ARM ADS ARM 32-bit inst. set ANSI C compiler
LD      = $(ARMBIN)\armlink     # ARM ADS linker
HEXTOOL = $(ARMBIN)\fromelf     # ARM ADS utility to create hex file from image

OBJ_CMD    = -o                 # Command line option to specify output filename

#-------------------------------------------------------------------------------
# ARM Procedure Call Standard (APCS) options
#-------------------------------------------------------------------------------

ROPI     = ropi                 # Read-Only(code) Position independence
INTERWRK = interwork            # Allow ARM-Thumb interworking

APCS = -apcs /$(ROPI)/$(INTERWRK)

#-------------------------------------------------------------------------------
# Additional compile time error checking options
#-------------------------------------------------------------------------------

CHK = -fa                       # Check for data flow anomolies

#-------------------------------------------------------------------------------
# Compiler output options
#-------------------------------------------------------------------------------

OUT = -c                        # Object file output only

#-------------------------------------------------------------------------------
# Compiler/assembler debug options
#-------------------------------------------------------------------------------

# DBG = -g                        # Enable debug

#-------------------------------------------------------------------------------
# Compiler optimization options
#-------------------------------------------------------------------------------

OPT = -Ospace -O2               # Full compiler optimization for space


#-------------------------------------------------------------------------------
# Build options
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Compiler code generation options
#-------------------------------------------------------------------------------

END = -littleend                # Compile for little endian memory architecture
ZA  = -zo #-za1                 # LDR may only access 32-bit aligned addresses
ZAS = -zas4                     # Min byte alignment for structures

CODE = $(END) $(ZA) $(ZAS)


#-------------------------------------------------------------------------------
# Include file search path options
#-------------------------------------------------------------------------------

#INC = -I. -I$(TARGET_DIR) -I$(MSM_DIR) -I$(SUPPORT_INCDIR)
INC = -I. -I$(TARGET_DIR) -I$(SUPPORT_INCDIR) 


#-------------------------------------------------------------------------------
# Compiler pragma emulation options
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Linker options
#-------------------------------------------------------------------------------

LINK_CMD = -o                    #Command line option to specify output file
                                 #on linking

ROPILINK = -ropi                 #Link image as Read-Only Position Independent

LINK_ORDER = -first AEEMod_Load

#-------------------------------------------------------------------------------
# HEXTOOL options
#-------------------------------------------------------------------------------

BINFORMAT = -bin


#-------------------------------------------------------------------------------
# Compiler flag definitions
#-------------------------------------------------------------------------------

CFLAGS = $(OUT) $(DYNAPP) $(BETA) $(INC) $(APCS) $(CODE) $(CHK) $(DBG) $(OPT)

#-------------------------------------------------------------------------------
# Linker flag definitions
#-------------------------------------------------------------------------------

LFLAGS = $(ROPILINK) -nodebug -symdefs $(TARGET).sym -entry AEEMod_Load


#----------------------------------------------------------------------------
# Default target
#----------------------------------------------------------------------------

all :  $(TARGET).$(MODULE)

#----------------------------------------------------------------------------
# Clean target
#----------------------------------------------------------------------------

# The object subdirectory, target image file, and target hex file are deleted.

clean :
        @echo ---------------------------------------------------------------
        @echo CLEAN
        -del /f *.o
        -del /f $(TARGET).$(EXETYPE)
        -del /f $(TARGET).$(MODULE)
        @echo ---------------------------------------------------------------

#============================================================================
#                           DEFAULT SUFFIX RULES
#============================================================================

# The following are the default suffix rules used to compile all objects that
# are not specifically included in one of the module specific rules defined
# in the next section.
# The following macros are used to specify the output object file, MSG_FILE
# symbol definition and input source file on the compile line in the rules
# defined below.

SRC_FILE = $(@F:.o=.c)        # Input source file specification
OBJ_FILE = $(OBJ_CMD) $(@F)   # Output object file specification

.SUFFIXES :
.SUFFIXES : .o .dep .c

#--------------------------------------------------------------------------
# C code inference rules
#----------------------------------------------------------------------------

.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(SRC_FILE)
        @echo ---------------------------------------------------------------

{$(SUPPORT_DIR)}.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(SUPPORT_DIR)\$(SRC_FILE)
        @echo ---------------------------------------------------------------

#===============================================================================
#                           MODULE SPECIFIC RULES
#===============================================================================

APP_OBJS = AEEAppGen.o \
	       AEEModGen.o \
           AppStates.o \
           Main.o \
           Controls.o \
           State.o 

#----------------------------------------------------------------------------
# Lib file targets
#----------------------------------------------------------------------------

$(TARGET).$(MODULE) : $(TARGET).$(EXETYPE)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(HEXTOOL)  $(TARGET).$(EXETYPE) $(BINFORMAT) $(TARGET).$(MODULE)

$(TARGET).$(EXETYPE) : $(APP_OBJS)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(LD) $(LINK_CMD) $(TARGET).$(EXETYPE) $(LFLAGS) $(APP_OBJS) $(LINK_ORDER)


# ------------------------------
# Files to compile



