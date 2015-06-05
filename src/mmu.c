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
	uint i;
	
	for(i = 0; i<1024; i++){
		((pde *) siguiente_libre)[i].present = 0;
	}
	
	siguiente_libre += 0x1000;
	return siguiente_libre - 0x1000;
}

void inicializar_mmu(){
  siguiente_libre = (void *) PAGE_COUNTER_INIT;
}

/******************************************************************/


void mmu_inicializar_tabla_kernel_para_pirata(pte * tabla){
	uint i;
	for(i = 0; i<1024; i++){
      tabla[i].base_address = i;
      tabla[i].present = 1;
      tabla[i].read_write = 0;  
      tabla[i].user_supervisor = 1; 
	}
}


pde * mmu_inicializar_dir_pirata(){
  //obtengo la siguiente libre
  pde * resultado = (pde *) dar_siguiente();
  // dar_siguiente lo devuelve en 0
  
  // me armo la tabla del kernel
  pte * tabla_kernel = dar_siguiente();
  mmu_inicializar_tabla_kernel_para_pirata(tabla_kernel);
   
	
  resultado[0].base_address = ((uint) tabla_kernel)>>12;
  resultado[0].read_write = 0;
  resultado[0].present = 1;
  resultado[0].user_supervisor = 0; //ok
  
  
  //FALTA TODO LO DEL CODIGO!!!!
  //Falta mapear posicion inicial y mapa descubierto


  return resultado;
}

// direccion virtual
// | directorio  |    tabla    |    offset   |
// | 31       22 | 21       12 | 11        0 |
  

void mmu_mapear_pagina(uint virtual, pde * cr3, uint fisica, uchar rw, uchar user_supervisor){

  uint directorio = (virtual >> 22) & 0x3ff;
  uint tabla = (virtual >> 12) & 0x3ff;
  
  pte * page_table;

  if(!cr3[directorio].present){
	  page_table = dar_siguiente();
	  
	  cr3[directorio].present = 1;
	  cr3[directorio].read_write = rw;
	  cr3[directorio].read_write = user_supervisor;
	  cr3[directorio].base_address = ((uint) page_table) >> 12;
  } else{
      page_table = (pte *) (cr3[directorio].base_address << 12);
  }

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




