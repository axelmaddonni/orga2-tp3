/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */


/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

void mmu_inicializar_dir_kernel(){
	uint *directory = (uint*)(0x27000);
	directory[0] = 0x00028003;
	uint i;
	for(i = 1; i<1024; i++){
		directory[i] = 0x00000000;
	}
}

void mmu_inicializar_tabla_kernel(){
	
	uint *tabla_kernel = (uint*)(0x28000);
	
	uint i;
	for(i = 0; i<1024; i++){
		uint j = i;
		j = (j<<12) | 0x3;
		tabla_kernel[i] = j;
	}
}

void * siguiente_libre;

void * dar_siguiente(){
	siguiente_libre += 0x1000;
	return siguiente_libre - 0x1000;
}

void inicializar_mmu(){
  siguiente_libre = (void *) PAGE_COUNTER_INIT;
}

/******************************************************************/


void mmu_inicializar_tabla_kernel_para_pirata(pde * tabla){
	
	uint *tabla_kernel = (uint*)(0x28000);
	
	uint i;
	for(i = 0; i<1024; i++){
    tabla[i].base_address = i;
    tabla[i].present = 1;
    tabla[i].read_write = 0;
    //tabla[i].user_supervisor = 1;
    // ^^^^ PREGUNTAR ESTO
  	tabla_kernel[i] = i;
	}
}


pte * mmu_inicializar_dir_pirata(){
  //obtengo la siguiente libre
  pte * resultado = (pte *) dar_siguiente();

  // lo pongo todo vacio
	uint i;
	for(i = 0; i<1024; i++){
		resultado[i].present = 0;
	}

  // me armo la tabla del kernel
  pde * tabla_kernel = dar_siguiente();
  mmu_inicializar_tabla_kernel_para_pirata(tabla_kernel);
   
	
  resultado[0].base_address = ((uint) tabla_kernel)>>12;
  resultado[0].read_write = 0;
  resultado[0].present = 1;

  //FALTA TODO LO DEL CODIGO!!!!


  return resultado;
}

// direccion virtual
// | directorio  |    tabla    |    offset   |
// | 31       22 | 21       12 | 11        0 |
  

void mmu_mapear_pagina(uint virtual, pde * cr3, uint fisica, uchar rw, uchar user_supervisor){

  uint directorio = (virtual >> 22) & 0x3ff;
  uint tabla = (virtual >> 12) & 0x3ff;

  pte * page_table = (pte *) (cr3[directorio].base_address << 12);

  page_table[tabla].user_supervisor = user_supervisor;
  page_table[tabla].read_write = rw;
  page_table[tabla].base_address = fisica >> 12;
  page_table[tabla].present = 1;	
  
  tlbflush();
	
}


void mmu_unmapear_pagina(uint virtual, pde * cr3){
  uint directorio = (virtual >> 22) & 0x3ff;
  uint tabla = (virtual >> 12) & 0x3ff;

  pte * page_table = (pte *) (cr3[directorio].base_address << 12);
  
  page_table[tabla].present = 0;

  tlbflush();
}




