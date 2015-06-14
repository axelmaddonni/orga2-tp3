/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"

#define CODIGO_BASE       0X400000

#define MAPA_BASE_FISICA  0x500000
#define MAPA_BASE_VIRTUAL 0x800000

#define PAGE_COUNTER_INIT 0x00100000


// esto no lo dio la catedra
typedef struct pde_t {
    unsigned char present:1;
    unsigned char read_write:1;
    unsigned char user_supervisor:1;
    unsigned char page_level_write_through:1;
    unsigned char page_level_cache_disable:1;
    unsigned char accessed:1;
    unsigned char reserved:1;
    unsigned char page_size:1;
    unsigned char global:1;
    unsigned char available_9_11:3;
    unsigned int  base_address:20;
} __attribute__((__packed__, aligned (4))) pde;

typedef struct pte_t {
    unsigned char present:1;
    unsigned char read_write:1;
    unsigned char user_supervisor:1;
    unsigned char page_level_write_through:1;
    unsigned char page_level_cache_disable:1;
    unsigned char accessed:1;
    unsigned char dirty:1;
    unsigned char page_table_attribute_index:1;
    unsigned char global:1;
    unsigned char available_9_11:3;
    unsigned int  base_address:20;
} __attribute__((__packed__, aligned (4))) pte;

/* Como la estructura "page table entry" y "page directory entry" cada una
 * miden 4 bytes de tamaño, y van a estar en estructuras alineadas a 4K, 
 * las alineamos a 4 bytes. En realidad por como hardcodeamos donde empiezan
 * las tablas no hay problema, pero agregamos redundancia.
 */

void mmu_inicializar_dir_kernel();

void mmu_inicializar();

pde * mmu_inicializar_dir_pirata(jugador_t * jugador, pirata_t * pirata, uint xparam, uint yparam);

void mmu_mapear_pagina(uint virtual, pde * cr3, uint fisica, uchar rw, uchar user_supervisor);
void mmu_unmapear_pagina(uint virtual, pde * cr3);

void copiar_pagina(uint src, uint dest);

void testear_paginacion();

#endif	/* !__MMU_H__ */
