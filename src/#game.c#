/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "tss.h"
#include "mmu.h"
#include "screen.h"
#include "i386.h"

#include <stdarg.h>


#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999

#define BOTINES_CANTIDAD 8

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {10, 2, 50}, {30, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {49, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;


void* error()
{
    __asm__ ("int3");
    return 0;
}

uint game_xy2lineal (uint x, uint y) 
{
    return (y * MAPA_ANCHO + x);
}

uint game_posicion_valida(int x, int y) 
{
    return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

pirata_t* id_pirata2pirata(uint id_pirata)
{
    uint i;
    for(i = 0; i < MAX_CANT_PIRATAS_VIVOS; i++)
    {
        if(jugadorA.piratas[i].id_pirata == id_pirata) return &(jugadorA.piratas[i]);
        if(jugadorB.piratas[i].id_pirata == id_pirata) return &(jugadorB.piratas[i]);
    }
    return NULL;
}

uint game_dir2xy(direccion dir, int *x, int *y)
{
    switch (dir)
    {
        case IZQ: *x = -1; *y =  0; break;
        case DER: *x =  1; *y =  0; break;
        case ABA: *x =  0; *y =  1; break;
        case ARR: *x =  0; *y = -1; break;
        default: return -1;
    }
    return 0;
}

uint game_valor_tesoro(uint x, uint y)
{
    int i;
    for (i = 0; i < BOTINES_CANTIDAD; i++)
    {
        if (botines[i][0] == x && botines[i][1] == y)
            return botines[i][2];
    }
    return 0;
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y)
{
    int next = 0;
    int i, j;
    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            vistas_x[next] = x + j;
            vistas_y[next] = y + i;
            next++;
        }
    }
}


void game_inicializar()
{
    game_jugador_inicializar(&jugadorA);
    game_jugador_inicializar(&jugadorB);
    id_del_pirata_actual = -1;
    modo_debug_activado = 0;
    pantalla_debug_activada = 0;
}

void game_jugador_inicializar_mapa(jugador_t *jug)
{
}

void game_jugador_inicializar(jugador_t *j)
{
    static int index = 0;

    j->jug = (cual_t) index++;
    uint i;  
    for(i = 0; i<MAX_CANT_PIRATAS_VIVOS; i++)
    {
        j->vivos[i] =  0;
    }

    uint h;
    for (i = 0; i<MAPA_ALTO; i++)
    {
        for(h = 0; h<MAPA_ANCHO; h++)
        {
            j->posiciones_exploradas[i][h] = 0;
        }
    }

    if(j->jug == A)
    {
        j->puerto[0] = POS_INIT_A_X;
        j->puerto[1] = POS_INIT_A_Y;
        j->color = (0x2 << 4); //verde

    }
    else
    {
        j->puerto[0] = POS_INIT_B_X;
        j->puerto[1] = POS_INIT_B_Y;
        j->color = (0x3 << 4); //cyan
    }

    j->posiciones_exploradas[j->puerto[1]-1][j->puerto[0]-1] = 1;
    j->posiciones_exploradas[j->puerto[1]  ][j->puerto[0]-1] = 1;
    j->posiciones_exploradas[j->puerto[1]+1][j->puerto[0]-1] = 1;
    j->posiciones_exploradas[j->puerto[1]-1][j->puerto[0]  ] = 1;
    j->posiciones_exploradas[j->puerto[1]  ][j->puerto[0]  ] = 1;
    j->posiciones_exploradas[j->puerto[1]+1][j->puerto[0]  ] = 1;
    j->posiciones_exploradas[j->puerto[1]-1][j->puerto[0]+1] = 1;
    j->posiciones_exploradas[j->puerto[1]  ][j->puerto[0]+1] = 1;
    j->posiciones_exploradas[j->puerto[1]+1][j->puerto[0]+1] = 1;

    j->monedas = 0;

    j->mineros_pendientes.proximo_a_ejecutar = 0;
    j->mineros_pendientes.proximo_libre = 0;


}

void game_pirata_inicializar(jugador_t *jugador, tipo_t tipo, uint xparam, uint yparam)
{ 
    pirata_t * pir = game_jugador_erigir_pirata(jugador, tipo);
    if(pir != NULL)
    {
        pde * cr3 = mmu_inicializar_dir_pirata(jugador, pir, xparam, yparam); 
        pir->cr3 = (uint) cr3;

        tss_inicializar_tarea(pir->index, jugador->jug, cr3);
        screen_actualizar_reloj_pirata(jugador, pir);
        screen_pintar_puerto(jugador);
        //si es minero, encolar
        //ESTO ESTA HACIENDO ALGO TURBIO AUNQUE NO DEBERIA...
        /*if(tipo == MINERO)
        {
            jugador->mineros_pendientes.ms[jugador->mineros_pendientes.proximo_libre] = (minero_obj) {xparam, yparam};
            jugador->mineros_pendientes.proximo_libre++;
        }*/
    }
}

void game_tick(uint id_pirata)
{
    screen_actualizar_reloj_global();
    //actualizar tambien el reloj del pirata
}


void game_pirata_relanzar(pirata_t *pirata, jugador_t *j, uint tipo)
{
}

pirata_t* game_jugador_erigir_pirata(jugador_t *j, tipo_t tipo)
{
    static uint id = 1;
    uint i=0;
    while(j->vivos[i] != 0 && i<MAX_CANT_PIRATAS_VIVOS)
    {
        i++;
    }
    if(i == MAX_CANT_PIRATAS_VIVOS) return NULL;
    else 
    {
        pirata_t * nuevo = &(j->piratas[i]);

        j->vivos[i] = 1;

        nuevo->index = i;

        nuevo->id_pirata = id++;
        nuevo->jugador = j;
        nuevo->tipo = tipo;


        nuevo->posicion[0] = j->puerto[0];
        nuevo->posicion[1] = j->puerto[1];

        nuevo->estado_reloj = 0; 

        return nuevo;
    }
}


void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y)
{
}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{
}


void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
}

void game_lanzar_minero(jugador_t *j, int x, int y)
{
  j->mineros_pendientes.ms[j->mineros_pendientes.proximo_libre] = (minero_obj) {x, y};
  j->mineros_pendientes.proximo_libre++;
}

uint en_rango(int x, int y)
{
    return ((x>=0) && (y>=0) && (x<MAPA_ANCHO) && (y<MAPA_ALTO));
}


uint game_syscall_pirata_mover(uint id, direccion dir)
{ 
    pirata_t * pir = id_pirata2pirata(id);
    if (!pir) return ~0;
    //si id_pirata2pirata devuelve NULL, return ~0 para indicar error
    uint x_viejo = pir->posicion[0]; 
    uint y_viejo = pir->posicion[1]; 
    
    //si un pirata trata de irse del mapa, return ~0 para indicar error
    switch(dir){
        case ARR:
            if(pir->posicion[1] == 0) return ~0;
            else pir->posicion[1]--;
            break;
        case ABA:
            if(pir->posicion[1] == MAPA_ALTO-1) return ~0;
            else pir->posicion[1]++;
            break;
        case DER:
            if(pir->posicion[0] == MAPA_ANCHO-1) return ~0;
            else pir->posicion[0]++;
            break;
        case IZQ:
            if(pir->posicion[0] == 0) return ~0;
            else pir->posicion[0]--;
            break;
    }


    int explorado_x[3];
    int explorado_y[3];

    explorado_x[0] = pir->posicion[0]-1;
    explorado_x[1] = pir->posicion[0];
    explorado_x[2] = pir->posicion[0]+1;

    explorado_y[0] = pir->posicion[1]-1;
    explorado_y[1] = pir->posicion[1];
    explorado_y[2] = pir->posicion[1]+1;


    jugador_t * jug = pir->jugador;

    if(pir->tipo == MINERO && jug->posiciones_exploradas[pir->posicion[1]][pir->posicion[0]] == 0)
    {
        return ~0; //si un minero cae a una posicion no explorada 
    }

    if(pir->tipo == EXPLORADOR)
    {
        uint i, h;

        // pintar pantalla
        for(i = 0; i<3; i++)
        {
            for(h = 0; h<3; h++)
            {
                if(en_rango(explorado_x[i], explorado_y[h]) && jug->posiciones_exploradas[explorado_y[h]][explorado_x[i]] == 0)
                {
                    screen_pintar_rect(0, jug->color, explorado_y[h]+1, explorado_x[i], 1, 1);
                }
            }
        }

       
        for(i = 0; i<3; i++)
        {
            for(h = 0; h<3; h++)
            {
                if(en_rango(explorado_x[h], explorado_y[i]))
                {
                    //si hay un botin con monedas
                    if(game_valor_tesoro(explorado_x[h], explorado_y[i]) && jug->posiciones_exploradas[explorado_y[i]][explorado_x[h]] == 0)
                    {    
                        screen_pintar_rect('o', jug->color, explorado_y[i]+1, explorado_x[h], 1, 1); //no se porque va el +1
                        //lanzo minero
                        game_lanzar_minero(jug, explorado_x[h], explorado_y[i]); 
                    }
                    jug->posiciones_exploradas[explorado_y[i]][explorado_x[h]] = 1;
                }
            }
        }


        uint p;
        for(p = 0; p<MAX_CANT_PIRATAS_VIVOS; p++)
        {
            if(jug->vivos[p])
            {
                for(i = 0; i<3; i++)
                {
                    for(h = 0; h<3; h++)
                    {			
                        if(en_rango(explorado_x[i], explorado_y[h]))
                        {
                            uint ind = (explorado_y[h]*MAPA_ANCHO+explorado_x[i]) * 0x1000;
                            mmu_mapear_pagina(0x800000+ind, (pde *) jug->piratas[p].cr3, 0x500000+ind, 0, 1);
                        }
                    }
                }
            }
        }

    }


    if(pir->tipo == EXPLORADOR)
    {
        screen_pintar_rect('E', jug->jug == A? 0x4f : 0x1f, pir->posicion[1]+1, pir->posicion[0], 1, 1);
        screen_pintar_rect('E', jug->color, y_viejo+1, x_viejo, 1, 1);
    }
    else
    {
        screen_pintar_rect('M', jug->jug == A? 0x4f : 0x0f , pir->posicion[1]+1, pir->posicion[0], 1, 1);
        screen_pintar_rect('M', jug->color, y_viejo+1, x_viejo, 1, 1);
    }

    uint indice_viejo = (y_viejo*MAPA_ANCHO+x_viejo) * 0x1000;
    uint indice_nuevo = (pir->posicion[1]*MAPA_ANCHO+pir->posicion[0]) * 0x1000;

    mmu_mapear_pagina(0x400000, (pde *) pir->cr3, 0x500000+indice_nuevo, 1, 1);
    //copiamos el codigo la pagina actual
    copiar_pagina(0x800000+indice_viejo, 0x400000);	

    return 1;
}

uint game_syscall_cavar(uint id)
{

    pirata_t * pir = id_pirata2pirata(id);
    if (!pir) return ~0;
    if (pir->tipo != MINERO) return ~0;

    uint x = pir->posicion[0]; 
    uint y = pir->posicion[1]; 

    uint i;
    for (i = 0; i < BOTINES_CANTIDAD; i++)
    {
        if (x == botines[i][0] && y == botines[i][1])
        {
            if (botines[i][2] > 0) 
            {
                (pir->jugador)->monedas++;
                screen_pintar_puntajes();
                return --botines[i][2];
            }
            else return ~0;
        }
    }

    return ~0;
}

uint game_syscall_pirata_posicion(uint id, int idx)
{
    if (idx < -1 || idx > 7) return ~0;
    //si el indice no es -1 o un indice valido del arreglo de piratas, dar error (return ~0)
    pirata_t * pir = id_pirata2pirata(id);
    if (!pir) return ~0;
    //si la id no corresponde a ningun pirata, dar error
    jugador_t * jug = pir->jugador;
    uint x, y;

    if (idx == -1)
    {
        x = pir->posicion[0]; 
        y = pir->posicion[1]; 
    }
    else
    {
        if (!jug->vivos[idx]) return ~0;
        x = (jug->piratas[idx]).posicion[0]; 
        y = (jug->piratas[idx]).posicion[1];
    }
    return (y << 8 | x);
}

uint game_syscall_manejar(uint syscall, uint param1)
{
    uint res; //error = ~0
    switch (syscall)
    {
        case 1:
            res = game_syscall_pirata_mover(id_del_pirata_actual, (direccion) param1);
            break;

        case 2:
            res = game_syscall_cavar(id_del_pirata_actual);
            break;

        case 3:
            res = game_syscall_pirata_posicion(id_del_pirata_actual, param1);
            break;
        
        default:
            //si el codigo del syscall es incorrecto
            res = ~0;
            break;
    }
    if (res == ~0) error();
    return res;
}

void game_pirata_exploto(uint id)
{
    pirata_t * pir = id_pirata2pirata(id);

    uint x = pir->posicion[0];
    uint y = pir->posicion[1];

    screen_pintar('X', (pir->jugador)->color, y + 1, x);
    //void screen_pintar(unsigned char c, unsigned char color, uint fila, uint columna);

    pir->jugador->vivos[pir->index] = 0;

    if(modo_debug_activado)
    {
        pantalla_debug_activada = 1;
        screen_debug(); 
        pantalla_debug_activada = 0;
    }

    if(pir->jugador->jug == A) screen_pintar('X', 0x04, 48, 4+2*pir->index);
    else screen_pintar('X', 0x01, 48, 59+2*pir->index);
  
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
    return NULL;
}


void game_jugador_anotar_punto(jugador_t *j)
{
}



void game_terminar_si_es_hora()
{
}


#define KB_w_Aup    0x11 // 0x91
#define KB_s_Ado    0x1f // 0x9f
#define KB_a_Al     0x1e // 0x9e
#define KB_d_Ar     0x20 // 0xa0
#define KB_e_Achg   0x12 // 0x92
#define KB_q_Adir   0x10 // 0x90
#define KB_i_Bup    0x17 // 0x97
#define KB_k_Bdo    0x25 // 0xa5
#define KB_j_Bl     0x24 // 0xa4
#define KB_l_Br     0x26 // 0xa6
#define KB_shiftA   0x2a // 0xaa
#define KB_shiftB   0x36 // 0xb6


void game_atender_teclado(unsigned char tecla)
{ 
    //jugadorA
    if(tecla == '<' && !pantalla_debug_activada)
    { 
        game_pirata_inicializar(&jugadorA, EXPLORADOR, jugadorA.puerto[0], jugadorA.puerto[1]);
    } 
    //jugadorB
    else if(tecla == '>' && !pantalla_debug_activada)
    { 
        game_pirata_inicializar(&jugadorB, EXPLORADOR, jugadorB.puerto[0], jugadorB.puerto[1]);
    }
    //modo debug
    else if(tecla == 'y')
    { 
        modo_debug_activado = modo_debug_activado? 0: 1;
    }
}

void testear_crear_tarea()
{
    pirata_t * pir = game_jugador_erigir_pirata(&jugadorA, EXPLORADOR);
    if(pir != NULL)
    {
        pde * cr3 = mmu_inicializar_dir_pirata(&jugadorA, pir, 1, 1);
        pir->cr3 = (uint) cr3;

        tss_inicializar_tarea(pir->index, A, cr3);
        id_del_pirata_actual = pir->id_pirata;

    }
}


