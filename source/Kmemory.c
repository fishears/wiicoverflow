///////////////////////////////////////////////////////////////////////////////
// Kernel Memory Library                                                     //
// Created by Hell Hibou (2008)                                              //
// Part of Wii Homebrew Launcher                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "Kmemory.h"
#include "T_error.h"

#ifndef PACKED
#	define PACKED __attribute__((__packed__))
#endif

/* Structure table d'allocation memoire */
typedef struct
{
    u8 Used : 1;	// = 0 -> La zone memoire est libre.
					// = 1 -> La zone memoire est utilisee.
    u8 Expand : 1;	// = 0 -> Fin du bloc.
					// = 1 -> Le bloc se prolonge sur le bloc suivant.
    u8 UnUsed : 6;	// Zone libre.
} PACKED T_KernelMemoryMap;


static T_KernelMemoryMap * BlocAllocate = NULL;  // Table d'allocation des blocs memoire.
static void  *             BaseDynAlloc = NULL;  // Pointeur sur la zone memoire allouable.
static u32                 MaxMemoryBlocs;


// Initialisation du gestionnaire de memoire.
void InitMemory (void * DataMemory, u32 TotalMemorySize)
{
    u32 Boucle;
    u32 TableBlocs;
    u32 TotalBlocs;

    // Alignement des bloc de memoire a un multiple de MEMORY_BLOC_SIZE
    if ( (u32)DataMemory % MEMORY_BLOC_SIZE != 0)
    {  DataMemory = ((void*) ((((u32)DataMemory) / MEMORY_BLOC_SIZE) * MEMORY_BLOC_SIZE) + MEMORY_BLOC_SIZE );  };

    TotalBlocs = TotalMemorySize / MEMORY_BLOC_SIZE;
    TableBlocs = TotalBlocs / MEMORY_BLOC_SIZE;
    MaxMemoryBlocs = TotalBlocs - TableBlocs;

//	Table de gestion memoire au debut
//	BlocAllocate = (T_KernelMemoryMap *)DataMemory;
//	BaseDynAlloc = DataMemory + (TableBlocs * MEMORY_BLOC_SIZE);

//	Table de gestion memoire a la fin 
	BaseDynAlloc = DataMemory;	
	BlocAllocate = DataMemory + (MaxMemoryBlocs * MEMORY_BLOC_SIZE);

	for (Boucle = 0; Boucle < MaxMemoryBlocs; Boucle++)
    { 
        BlocAllocate [Boucle].Used   = 0;
        BlocAllocate [Boucle].Expand = 0;
    }
}


// Alloue SIZE blocs de memoire
void * mballoc(u32 Size)
{
   u32 Counter = 0;
   u32 Base    = 0;
   u32 Boucle, Boucle2;

   if (Size == 0) return NULL;  
   for (Boucle = 0; Boucle < MaxMemoryBlocs; Boucle++)
   {
      if (BlocAllocate[Boucle].Used == 0) { Counter++; }
      else { Counter = 0; Base = Boucle + 1; }
		
      if (Counter >= Size)
      {
         if (Base+Size > MaxMemoryBlocs) return NULL;
         for (Boucle2=Base; Boucle2 < (Base+Size); Boucle2++)
         {
            BlocAllocate[Boucle2].Used   = 1;
            if (Boucle2+1 < (Base+Size) ) { BlocAllocate[Boucle2].Expand = 1; }
            else { BlocAllocate[Boucle2].Expand = 0; }
        }
        return ((void*) (BaseDynAlloc + (Base * MEMORY_BLOC_SIZE) ));
      }
   }
   return NULL;
}

// Libere un bloc memoire
int mbfree (void *Adresse)
{
   u32 Bloc; 
   
   if (Adresse == NULL) { return ERROR_NULL_VALUE; }
   if (Adresse < BaseDynAlloc) { return ERROR_BAD_PARAM; }
   if (((Adresse - BaseDynAlloc) % MEMORY_BLOC_SIZE) != 0) return ERROR_BAD_PARAM;
   if ((((u32)Adresse) % MEMORY_BLOC_SIZE) != 0) return ERROR_BAD_PARAM;

   Bloc = (Adresse - BaseDynAlloc) / MEMORY_BLOC_SIZE;

   for (;  BlocAllocate[Bloc].Expand == 1; Bloc++)
   {
       BlocAllocate[Bloc].Used   = 0;
       BlocAllocate[Bloc].Expand = 0;
   }

   BlocAllocate[Bloc].Used   = 0;
   BlocAllocate[Bloc].Expand = 0;

   return NO_ERROR;
};

void * mbrealloc (void *Adresse, u32 Size)
{
   u32 Bloc, Boucle, OldSize, CopySize;
   void * NewAdresse;

   if (((Adresse - BaseDynAlloc) % MEMORY_BLOC_SIZE) != 0) return NULL;
   Bloc = (Adresse - BaseDynAlloc) / MEMORY_BLOC_SIZE;
   if (BlocAllocate[Bloc].Used == 0) return NULL;

   for (Boucle = Bloc; BlocAllocate[Boucle].Expand == 1; Boucle++) 
   { if (Boucle >= MaxMemoryBlocs) break; };
   
   OldSize = Boucle - Bloc + 1;

   // Si pas de changement de taille de bloc, ...
   if ( Size == OldSize) return Adresse;

   // Si on reduit le nombre de bloc, ...
   if ( Size < OldSize)
   {
      for (Boucle = (Bloc + Size); Boucle < (Bloc + OldSize) ; Boucle++)
      { 
         BlocAllocate[Boucle].Used   = 0;
         BlocAllocate[Boucle].Expand = 0;
      }
      BlocAllocate[Size-1].Expand = 0;
      return Adresse;
   }
   
   // Si on augmente le nombre de bloc
   for (Boucle = (Bloc + OldSize); Boucle < (Bloc + Size) ; Boucle++)
   {
       // Si le bloc ne peut etre simplement agrandit, ...
       if ( BlocAllocate[Boucle].Used == 1 )
       {
           NewAdresse = mballoc (Size);
           if (NewAdresse == NULL) return NULL;
           CopySize = (OldSize * MEMORY_BLOC_SIZE) / sizeof(u32);
           for (Boucle = 0; Boucle < CopySize; Boucle++)
           { ((u32*) NewAdresse)[Boucle] = ((u32*) Adresse)[Boucle]; };
           mbfree (Adresse);
           return NewAdresse;
       }
   }
   
   // Le bloc est simplement agrandit
   for (Boucle = (Bloc + OldSize - 1); Boucle < (Bloc + Size) ; Boucle++)
   {
         BlocAllocate[Boucle].Used   = 1;
         BlocAllocate[Boucle].Expand = 1;
   }
   BlocAllocate[Size-1].Expand = 0;
   return Adresse;
}

///////////////////////////////////////////////////////////////////////////////  
