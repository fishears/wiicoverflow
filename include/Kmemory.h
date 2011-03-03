///////////////////////////////////////////////////////////////////////////////
// Kernel Memory Library                                                     //
// Created by Hell Hibou (2008)                                              //
// Part of Wii Homebrew Launcher                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef _LIB_UOBIHLLEH_KERNEL_MEMORY
#define _LIB_UOBIHLLEH_KERNEL_MEMORY

#define MEMORY_BLOC_SIZE			(512)		/* Taille des blocs memoire */


///////////////////////////////////////////////////////////////////////////////
// Macro pour Wii                                                            //
///////////////////////////////////////////////////////////////////////////////

#include <ogcsys.h>
#include <malloc.h>
#define MEM2_MEMORY_ADRESS_BEGIN	0x90100000	/* Adresse debut memoire a utiliser */

#define MEM2_MEMORY_ADRESS_END		0x93300000	/* Adresse fin memoire a utiliser   */

#define MEM2_Init()        InitMemory ((void*) MEM2_MEMORY_ADRESS_BEGIN, (MEM2_MEMORY_ADRESS_END - MEM2_MEMORY_ADRESS_BEGIN) )

#define MEM2_malloc(size)  mballoc((size/MEMORY_BLOC_SIZE)+1)

#define MEM2_free(Adresse) mbfree(Adresse)

#define MEM2_realloc(Adresse,size) mbrealloc(Adresse,(size/MEMORY_BLOC_SIZE)+1)

///////////////////////////////////////////////////////////////////////////////
// Fonctions 'tout system'                                                   //
///////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C" 
{
#endif
												// Initialise les gestionnaire de memoire.
void   InitMemory (void *DataMemory,			// DataMemory pointe vers la zone de memoire disponible.
                       u32 TotalMemorySize);	// TotalMemorySize est la quantite total de memoire disponible.

void * mballoc(u32 Size);      	// Alloue SIZE blocs de memoire et retourne un pointeur sur la zone.
								// Retourne un pointeur nul si l'allocation est impossible.

int    mbfree (void *Adresse);		// Libere le bloc memoire alloue a l'adresse ADRESSE.
									// Retourne -1 si adresse incorrecte et 0 si tout s'est bien deroule.

void * mbrealloc (void *Adresse, u32 Size); 	// Redimensionne un bloc, retourne NULL si le bloc
                                                // n'est pas redimensionne.

#ifdef __cplusplus
}
#endif

#endif

///////////////////////////////////////////////////////////////////////////////
