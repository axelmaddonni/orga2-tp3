/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"

extern void *dar_siguiente();

tss tss_inicial;
tss tss_idle;

tss tss_jugadorA[MAX_CANT_PIRATAS_VIVOS];
tss tss_jugadorB[MAX_CANT_PIRATAS_VIVOS];


void tss_inicializar_tarea_inicial(){
  gdt[GDT_TAREA_INICIAL].base_31_24 =  (unsigned int) (&tss_inicial) >> 24;
  gdt[GDT_TAREA_INICIAL].base_23_16 = ((unsigned int) (&tss_inicial) >> 16) & 0xff;
  gdt[GDT_TAREA_INICIAL].base_0_15  =  (unsigned int) (&tss_inicial)        & 0xffff;
}


void tss_inicializar_idle(){
	tss_idle.eip = 0x00016000;
    tss_idle.ebp = 0x27000;
    tss_idle.esp = 0x27000;
    
    tss_idle.cs = 0x40;
    tss_idle.ds = 0x48; 
    tss_idle.es = 0x48; 
    tss_idle.gs = 0x48; 
    tss_idle.ss = 0x48; 
   
    tss_idle.fs = 0x48;
    
    tss_idle.eflags = 0x00000002;
 
    tss_idle.iomap = 0xFFFF;
    tss_idle.cr3 = 0x27000;

    tss_idle.ptl      = 0x0;
    tss_idle.unused0  = 0x0;
    tss_idle.esp0     = 0x27000;
    tss_idle.ss0      = 0x48;
    tss_idle.unused1  = 0x0;
    tss_idle.esp1     = 0x0;
    tss_idle.ss1      = 0x0;
    tss_idle.unused2  = 0x0;
    tss_idle.esp2     = 0x0;
    tss_idle.ss2      = 0x0;
    tss_idle.unused3  = 0x0;

    tss_idle.unused9  = 0x0;
    tss_idle.ldt      = 0x0;
    tss_idle.unused10 = 0x0;
    tss_idle.dtrap    = 0x0;
  
    tss_idle.esi      = 0x0;
    tss_idle.edi      = 0x0;
  
    tss_idle.eax      = 0x0;
    tss_idle.ecx      = 0x0;
    tss_idle.edx      = 0x0;
    tss_idle.ebx      = 0x0;
 
    
    gdt[GDT_TAREA_IDLE] = (gdt_entry) { // falta
        (unsigned short)    0x0068,         /* limit[0:15]  */
        (unsigned short)    ((unsigned int) (&tss_idle) & 0xffff),    /* base[0:15]   */
        (unsigned char)     ((((unsigned int) (&tss_idle)) >>16) & 0xff),/* base[23:16]  */
        (unsigned char)     0x09,           /* type = r/w   */
        (unsigned char)     0x00,           /* system       */
        (unsigned char)     0x00,           /* dpl          */
        (unsigned char)     0x01,           /* p            */
        (unsigned char)     0x00,           /* limit[16:19] */
        (unsigned char)     0x01,           /* avl          */
        (unsigned char)     0x00,           /* l            */
        (unsigned char)     0x00,           /* db           */
        (unsigned char)     0x00,           /* g            */
        (unsigned char)     (((unsigned int) (&tss_idle)) >>24), /* base[31:24]  */
    };
    
}



void tss_inicializar(tss * tss_tarea,  pde * cr3_nuevo) {
	
    tss_idle.eip = 0x400000; // 0x00010000 + 0x1000 * indice_codigo;
	
	//pde * nuevo_cr3 = mmu_inicializar_dir_pirata(fisica en el mapa, que codigo de tarea correr);
	
    tss_idle.cr3 = (unsigned int) cr3_nuevo;
	
	
    tss_idle.ebp = tss_idle.eip + 0x1000; 
    tss_idle.esp = tss_idle.ebp;
    
    /*fisica = dame_pagina_libre();
    virtual = 0x335000;
    mapear(cr3, fisica, virtual);
    
    */
    tss_idle.esp0 = (unsigned int) dar_siguiente() + 0x1000;
    tss_idle.ss0 = 0x48;  //ver kernel.asm
    
    tss_idle.ds = 0x5b; 
    tss_idle.es = 0x5b; 
    tss_idle.gs = 0x5b; 
    tss_idle.ss = 0x5b; 
   
    tss_idle.fs = 0x5b; 
    
    tss_idle.eflags = 0x00000002;
 
    tss_idle.iomap = 0xFFFF;
    
}






