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
