/*===========================================================================

FILE: AEEModGen.c

SERVICES: Sample IModule implementation file

DESCRIPTION
  This file contains sample implementation of the IModule interface
  
  This file can be used for developing dynamic apps as well as static apps. Static
  apps are the apps that will be statically linked with the phone image by the OEM.
  To develop a static app, the following steps need to be done:
  1. Provide a GetModInfo() function
  2. Provide a Load() function
  3. Provide a CreateInstance() function (which is identical to the AEEClsCreateInstance()
  function that must be provided by a dynamic module.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


          Copyright � 2000-2001 QUALCOMM Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
  ========================================================================== */


/*==========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
  ========================================================================== */
#include "AEEHeap.h"    // Memory allocation Routines
#include "AEEStdlib.h"  // AEE Standard C library functions
#include "AEEModGen.h"  // Contains Sample AEEModule declarations
#include "AEEAppGen.h"

/*==========================================================================

                      PUBLIC DATA DECLARATIONS

  ========================================================================== */

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

/*
  || Functions to be provided by the module developers.
*/

static uint32     AEEMod_AddRef(IModule *po);
static uint32     AEEMod_Release(IModule *po);
static int        AEEMod_CreateInstance(IModule *po,IShell *pIShell,
                                        AEECLSID ClsId, void **ppObj);
static void       AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);
static AEEModObj *AEEMod_FindObj(AEEMod *pme, AEECLSID cls, AEEModObj **ppPrev);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/
#ifdef AEE_SIMULATOR
   // IMPORTANT NOTE: g_pvtAEEStdLibEntry global variable is defined for 
   //   SDK ONLY! This variable should NOT BE:
   //
   //      (1) overwritten 
   //      (2) USED DIRECTLY by BREW SDK users. 
   //
   //  g_pvtAEEStdLibEntry is used as an entry point to AEEStdLib,
AEEHelperFuncs *g_pvtAEEStdLibEntry;
#endif

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/


/* =========================================================================
                     FUNCTION DEFINITIONS
   ========================================================================= */

/* =========================================================================
Function:  AEEMod_Load()

Description:
   This is the module load function. It is invoked when the module is being
   loaded. It creates an instance of the AEEMod structure and initializes
   the data members appropriately.

   If running under the AEE Simulator, the function is prefixed with the tag
   __declspec(dllexport) so that this function is exported by the App DLL.
   This function shall be invoked from within the AEE.
   
Prototype:
   boolean AEEMod_Load(IShell *pIShell, char *psz, IModule **ppMod)

Parameters:
   piShell: Pointer to IShell interface
   psz:     String containing name of the resource file for module
   ppMod: [ou]: On return, *ppMod contains a valid pointer to the AEEMod 
               structure

Return Value:
   SUCCESS: If successful
   Error code: IF failed

Comments:
   This function must be the first function in the first module for 
   it to work on the handset!

Side Effects: 
   None

============================================================================*/
#ifdef AEE_SIMULATOR
__declspec(dllexport) int AEEMod_Load(IShell *pIShell, void *ph, IModule **ppMod)
#else
#if defined(BREW_MODULE) || defined(FLAT_BREW)
extern int module_main(IShell *pIShell, void *ph, IModule **ppMod);
int module_main(IShell *pIShell, void *ph, IModule **ppMod)
#else
int AEEMod_Load(IShell *pIShell, void *ph, IModule **ppMod)
#endif
#endif
{
   // For dynamic apps, verify the version of the AEE STD library to ensure
   //  that it matches with the version that this module was built with.
   // No need to do this check under AEE_SIMULATOR
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
   if (GET_HELPER_VER() != AEESTDLIB_VER) {
      return EVERSIONNOTSUPPORT;
   }
#endif

   // Invoke helper function to do the actual loading.
   return AEEStaticMod_New(sizeof(AEEMod),pIShell,ph,ppMod,NULL,NULL);
}

/*===========================================================================

Function:  AEEStaticMod_New()

Description:
   This is the module load function for static modules. It is directly 
   invoked by static modules when the static module is being loaded. 
   It also serves as a helper function for dynamic modules. 
   It creates an instance of the AEEMod structure and initializes the data
   members appropriately. The only difference between this function and the
   one used for dynamic mods is that this function takes an additional 
   parameter that specifies the CreateInstance function of the static module.

Prototype:
   boolean AEEStaticMod_New(int nSize, IShell *pIShell, void *ph, 
                            IModule **ppMod,PFNMODCREATEINST pfn)

Parameters:
   nSize: Specifies the size (in bytes) of the structure to be allocated for 
           the module.  Typically, this is the size of the AEEMod structure
   piShell: Pointer to IShell interface
   ph:  Not used on phone. In SDK, this is a pointer to the AEEHelperFuncs 
           structure. 
   ppMod: [ou]: On return, *ppMod contains a valid pointer to the AEEMod 
           structure
   pfn: Pointer to the modules' CreateInstance() function

Return Value:
   SUCCESS: If successful
   Error code: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
int AEEStaticMod_New(int16 nSize, IShell *pIShell, void *ph, IModule **ppMod,
                     PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF)
{
   AEEMod *pMe = NULL;
   VTBL(IModule) *modFuncs;

   if (!ppMod || !pIShell) {
      return EFAILED;
   }

   *ppMod = NULL;
  
#ifdef AEE_SIMULATOR
   // IMPORTANT NOTE: g_pvtAEEStdLibEntry global variable is defined for 
   //   SDK ONLY! This variable should NOT BE:
   //
   //      (1) overwritten 
   //      (2) USED DIRECTLY by BREW SDK users. 
   //
   //  g_pvtAEEStdLibEntry is used as an entry point to AEEStdLib,
   //   DO NOT REMOVE the next five lines.
   if (!ph) {
      return EFAILED;
   } else {
      g_pvtAEEStdLibEntry = (AEEHelperFuncs *)ph;
   }
#endif

   //Allocate memory for the AEEMod object

   if (nSize < sizeof(AEEMod)) {
      nSize += sizeof(AEEMod);
   }

   if (NULL == (pMe = (AEEMod *)MALLOC(nSize + sizeof(IModuleVtbl)))) {
      return ENOMEMORY;
   }
  
   // Allocate the vtbl and initialize it. Note that the modules and apps 
   // must not have any static data. Hence, we need to allocate the vtbl as 
   // well.

   modFuncs = (IModuleVtbl *)((byte *)pMe + nSize);

   // Initialize individual entries in the VTBL
   modFuncs->AddRef         = AEEMod_AddRef;
   modFuncs->Release        = AEEMod_Release;
   modFuncs->CreateInstance = AEEMod_CreateInstance;
   modFuncs->FreeResources  = AEEMod_FreeResources;


   // initialize the vtable
   INIT_VTBL(pMe, IModule, *modFuncs);

   // initialize the data members

   // Store address of Module's CreateInstance function
   pMe->pfnModCrInst = pfnMC;

   // Store Address of Module's FreeData function
   pMe->pfnModFreeData = pfnMF;

   pMe->m_nRefs = 1;
   pMe->m_pIShell = pIShell;

   // Set the pointer in the parameter
   *ppMod = (IModule*)pMe;

   return SUCCESS;
}

/*===========================================================================

Function:  AEEMod_ListAdd()

Description:
   This function adds an object into the list tracked by the module.The module
   tracks all the classes created in the module so far.

Prototype:void AEEMod_ListAdd(IModule *po, AEEModObj *poAdd)

Parameters:
   po: Pointer to the module
   poAdd: Pointer to the new class that needs to be tracked.

Return Value:
   None

Comments:  None

Side Effects: None

==============================================================================*/
void AEEMod_ListAdd(IModule *po, AEEModObj *poAdd)
{
   AEEMod *pme = (AEEMod *)po;

   if (poAdd && !AEEMod_FindObj(pme, poAdd->cls, NULL)) {
      poAdd->pNext = pme->m_pObjList;
      pme->m_pObjList = poAdd;
   }
}

/*===========================================================================

Function:  AEEMod_ListRemove()

Description:
   Removes an object from the tracking list.

Prototype:
     void AEEMod_ListRemove(IModule *po, AEEModObj *pObj)

Parameters:
   po: Pointer to the module
   poAdd: Pointer to the class that needs to be removed from the tracking list.

Return Value:

Comments:  None

Side Effects: None

==============================================================================*/
void AEEMod_ListRemove(IModule *po, AEEModObj *pObj)
{
   AEEMod    *pme = (AEEMod *)po;
   AEEModObj *pPrev;
   
   if (pObj && AEEMod_FindObj(pme, pObj->cls, &pPrev)) {
      if (pPrev) {
         pPrev->pNext = pObj->pNext;
      } else {
         pme->m_pObjList = pObj->pNext;
      }
   }
}


/*===========================================================================

Function:  AEEMod_AddRef()

Description:
   This function increases the referecne count for the AEEMod object

Prototype:
   uint32 AEEMod_AddRef(IModule *po)

Parameters:
   po: Pointer to IModule interface whos reference count is to be incremented

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

==============================================================================*/
static uint32 AEEMod_AddRef(IModule *po)
{
   return (++((AEEMod *)po)->m_nRefs);
}


/*===========================================================================

Function:  AEEMod_Release()

Description:
   This function decreases the referecne count for the IModule object. If
   the reference count reaches zero, it does the required cleanup

Prototype:
   uint32 AEEMod_Release(IModule *po)

Parameters:
   po: Pointer to the IModule interface whose reference count needs to be
     decremented.

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

==============================================================================*/
static uint32 AEEMod_Release(IModule *po)
{
   AEEMod *pMe = (AEEMod *)po;

   if (--pMe->m_nRefs != 0) {
      return pMe->m_nRefs;
   }

   // Ref count is zero. So, release memory associated with this object.


   // First, release user-specific data if any
   // Invoke User's FreeData function if they have registered
   if (pMe->pfnModFreeData) {
      pMe->pfnModFreeData(po);
   }

   //Free the object itself
   FREE_VTBL(pMe, IModule);
   FREE(pMe);
  
   return 0;
}


/*===========================================================================

Function:  AEEMod_CreateInstance()

Description:
   This function is invoked so as to create the individual classes belonging
   to this module. It is invoked once for each class this module contains. 
   This function invokes the AEEClsCreateInstance() function that the module
   implementer must provide.

Prototype:
   int AEEMod_CreateInstance(IModule *pIModule,IShell *pIShell,AEECLSID ClsId,void **ppObj)

Parameters:
   pIModule: [in]: Pointer to this module
   pIShell: [in]: Pointer to the AEE Shell interface
   ClsID:   [in]: ClassId of the class that needs to be created. 
   ppobj:   [out]: If ClsID points to an applet, then *ppobj must point to a 
            valid structure that implemetns the IApplet interface. If ClsID 
            points to a class that is not an applet, *ppobj must point to a 
            valid IBase*.

Return Value:
   SUCCESS: If successful
   Error code: If failed

Comments:  None

Side Effects: None

==============================================================================*/
static int AEEMod_CreateInstance(IModule *pIModule,IShell *pIShell,
                                 AEECLSID ClsId,void **ppObj)
{
   AEEMod    *pme = (AEEMod *)pIModule;
   AEEModObj *poObj;
   int        nErr = 0;

   // See if we already have the module in memory...If yes, just increment
   // the reference count and return.
   poObj = AEEMod_FindObj(pme, ClsId, NULL);
   if (poObj) {
      IBASE_AddRef((IBase *)poObj);
      *ppObj = poObj;
      return SUCCESS;
   }

   // For a dynamic module, they must supply the AEEClsCreateInstance() 
   //   function. Hence, invoke it. For a static app, they will have 
   //   registered the create Instance function. Invoke it.
   if (pme->pfnModCrInst) {
      nErr = pme->pfnModCrInst(ClsId, pIShell, pIModule, ppObj);
#if !defined(BREW_STATIC_APP)
   } else {
      nErr = AEEClsCreateInstance(ClsId, pIShell, pIModule, ppObj);
#endif
   }

   return nErr;
}

/*===========================================================================

Function:  AEEMod_FreeResources()

Description:
   This function is invoked so that the module can free any of the resources
   that it has loaded.

Prototype:
   void AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)

Parameters:
   po: [in]: Pointer to this module
   ph: [in]: Pointer to the AEE IHeap interface
   pfm: Pointer to the resource file

Return Value:
   None

Comments:  None

Side Effects: None

==============================================================================*/
static void AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)
{
   (void)po,(void)ph,(void)pfm; /* all unused */
}

/*===========================================================================

FUNCTION AEEMod_FindObj

DESCRIPTION: This is a utility function that helps to find a class within
a module's tracking list.

Prototype:
  static AEEModObj *AEEMod_FindObj(AEEMod *pme, AEECLSID cls, 
                                   AEEModObj **ppPrev)

Parameters:
   pme: [in]: Pointer to this module
   Cls: Clsss that needs to be searched in the tracking list
   pprev: [out]: if the object was found, this contains a pointer to the 
                 previous node in the list on return. This is useful in
                  removing the element from the list.

Return Value:
   None

Comments:  None

Side Effects: None
===========================================================================*/
static AEEModObj *AEEMod_FindObj(AEEMod *pme, AEECLSID cls, AEEModObj **ppPrev)
{
   AEEModObj *po;
   AEEModObj *poPrev;

   for (poPrev = NULL, po = pme->m_pObjList; po != NULL; po = po->pNext) {
      if (po->cls == cls) {
         break;
      }
      poPrev = po;
   }
   if (ppPrev) {
      *ppPrev = poPrev;
   }

   return po;
}

/*========================End of File ================================================*/
