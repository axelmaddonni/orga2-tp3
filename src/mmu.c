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

void mmu_inicializar_dir_kernel()
{
    uint *directory = (uint*)(0x27000);
    directory[0] = 0x00028003;
    uint i;
    for(i = 1; i<1024; i++)
    {
        directory[i] = 0x00000000;
    }
}

void mmu_inicializar_tabla_kernel()
{
    uint *tabla_kernel = (uint*)(0x28000);

    uint i;
    for(i = 0; i<1024; i++)
    {
        uint j = i;
        j = (j<<12) | 0x3;
        tabla_kernel[i] = j;
    }
}

void * siguiente_libre;

void * dar_siguiente()
{
    uint i;

    for(i = 0; i<1024; i++)
    {
        ((pde *) siguiente_libre)[i].present = 0;
    }

    siguiente_libre += 0x1000;
    return siguiente_libre - 0x1000;
}

void inicializar_mmu()
{
  siguiente_libre = (void *) PAGE_COUNTER_INIT;
}

/******************************************************************/


void mmu_inicializar_tabla_kernel_para_pirata(pte * tabla)
{
    uint i;
    for(i = 0; i<1024; i++)
    {
        tabla[i].base_address = i;
        tabla[i].present = 1;
        tabla[i].read_write = 0;  
        tabla[i].user_supervisor = 1; 
    }
}


pde * mmu_inicializar_dir_pirata(jugador_t * jugador, pirata_t * pirata, uint xparam, uint yparam)
{
    //obtengo la siguiente libre
    pde * resultado = (pde *) dar_siguiente();
    // dar_siguiente lo devuelve en 0

    // me armo la tabla del kernel
    pte * tabla_kernel = dar_siguiente();
    mmu_inicializar_tabla_kernel_para_pirata(tabla_kernel);


    resultado[0].base_address = ((uint) tabla_kernel)>>12;
    resultado[0].read_write = 0;
    resultado[0].present = 1;
    resultado[0].user_supervisor = 0; 
    resultado[0].reserved = 0;


    uint codigo_tarea;
    if(jugador->jug == A)
    {
        if(pirata->tipo == EXPLORADOR) codigo_tarea =  0x10000;
        else codigo_tarea = 0x11000;
    }
    else
    {
        if(pirata->tipo == EXPLORADOR) codigo_tarea = 0x12000;
        else codigo_tarea = 0x13000;
    }

    uint p[2];
    p[0] = jugador->puerto[0];
    p[1] = jugador->puerto[1];
    //mapeo paginas
    mmu_mapear_pagina(0x400000, resultado, game_xy2lineal(p[0],p[1])*0x1000+0x500000, 1, 1);
    uint i, j;
    for(i = 0; i<MAPA_ALTO; i++)
    {
        for(j = 0; j<MAPA_ANCHO; j++)
        {
            if(jugador->posiciones_exploradas[i][j])
            {
                uint ind = (i*MAPA_ANCHO+j) * 0x1000;
                mmu_mapear_pagina(0x800000+ind, resultado, 0x500000+ind, 0, 1);
            }
        }
    }



    //copio el codigo
    mmu_mapear_pagina(0x400000, (pde *) 0x27000, game_xy2lineal(p[0],p[1])*0x1000+0x500000, 1, 0);
    copiar_pagina(codigo_tarea, 0x400000);
    *((uint *) 0x400ffc) = yparam;  // parametros que toma
    *((uint *) 0x400ff8) = xparam;  // la tarea
    *((uint *) 0x400ff4) = 0;       //direccion de retorno, es fruta
    mmu_unmapear_pagina(0x400000, (pde *) 0x27000);


    return resultado;
}

void copiar_pagina(uint src, uint dest)
{
    uint i;
    for(i = 0; i<4096; i+=4)
    {
        *((uint *) (dest + i)) =  *((uint *) (src + i));
    }
}
	
void mmu_mapear_pagina(uint virtual, pde * cr3, uint fisica, uchar rw, uchar user_supervisor)
{

    uint directorio = (virtual >> 22) & 0x3ff;
    uint tabla = (virtual >> 12) & 0x3ff;

    pte * page_table;

    if(!cr3[directorio].present)
    {
        page_table = dar_siguiente();

        cr3[directorio].present = 1;
        cr3[directorio].read_write = rw;
        cr3[directorio].user_supervisor = user_supervisor;
        cr3[directorio].base_address = ((uint) page_table) >> 12;
        cr3[directorio].reserved = 0;
    }
    else
    {
        page_table = (pte *) (cr3[directorio].base_address << 12);
    }

    page_table[tabla].user_supervisor = user_supervisor;
    page_table[tabla].read_write = rw;
    page_table[tabla].base_address = fisica >> 12;
    page_table[tabla].present = 1;	

    tlbflush();
	
}


void mmu_unmapear_pagina(uint virtual, pde * cr3)
{
    uint directorio = (virtual >> 22) & 0x3ff;
    uint tabla = (virtual >> 12) & 0x3ff;

    pte * page_table = (pte *) (cr3[directorio].base_address << 12);

    page_table[tabla].present = 0;

    tlbflush();
}




void testear_paginacion()
{
    int cr3 = 0x27000;

    uint m1 = 0x400000;
    uint m2 = 0x800000;

    mmu_mapear_pagina(m1, (pde *) cr3, 0x500000, 0, 0);
    mmu_mapear_pagina(m2, (pde *) cr3, 0x500000, 0, 0); 

    *(uint *) m1 = 0x55AA;

    if( *(uint *) m2 != 0x55AA)
    {
        *((uint *) 0xf00000) = 40;
    }

    mmu_unmapear_pagina(m1, (pde *) cr3);
    mmu_unmapear_pagina(m2, (pde *) cr3);

}


