/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "screen.h"

void sched_inicializar(){
  sched_struct.indiceA = 0;
  sched_struct.indiceB = 0;
  sched_struct.proximo = A;
}

void sched_setear_id_actual(uint ind, cual_t jugador){
  if(jugador == A)
    id_del_pirata_actual = jugadorA.piratas[ind].id_pirata;
  else
    id_del_pirata_actual = jugadorB.piratas[ind].id_pirata;
}

uchar sched_proxima_a_ejecutar(){
  if(sched_struct.proximo == A){
 
    uint viejo_iA = sched_struct.indiceA;
    do{ 
      uint iA = sched_struct.indiceA;
      sched_struct.indiceA = (sched_struct.indiceA+1)%8; //++
      if(jugadorA.vivos[iA]){
        sched_struct.proximo = B;
        sched_setear_id_actual(iA, A);
        return (iA+15)<<3; //las tareas de A empiezan en 15
      }
    } while(sched_struct.indiceA != viejo_iA);   
 
    uint viejo_iB = sched_struct.indiceB;
    do{ 
      uint iB = sched_struct.indiceB;
      sched_struct.indiceB = (sched_struct.indiceB+1)%8; //++
      if(jugadorB.vivos[iB]){
        sched_struct.proximo = A;
        sched_setear_id_actual(iB, B);
        return (iB+23)<<3; //las tareas de B empiezan en 23
      }
    } while(sched_struct.indiceB != viejo_iB);    
  } else {
    uint viejo_iB = sched_struct.indiceB;
    do{ 
      uint iB = sched_struct.indiceB;
      sched_struct.indiceB = (sched_struct.indiceB+1)%8; //++
      if(jugadorB.vivos[iB]){
        sched_struct.proximo = A;
        sched_setear_id_actual(iB, B);
        return (iB+23)<<3; //las tareas de B empiezan en 23
      }
    } while(sched_struct.indiceB != viejo_iB);    

    uint viejo_iA = sched_struct.indiceA;
    do{ 
      uint iA = sched_struct.indiceA;
      sched_struct.indiceA = (sched_struct.indiceA+1)%8; //++
      if(jugadorA.vivos[iA]){
        sched_struct.proximo = B;
        sched_setear_id_actual(iA, A);
        return (iA+15)<<3; //las tareas de A empiezan en 15
      }
    } while(sched_struct.indiceA != viejo_iA); 
  }

 
  return 0x70;//idle
}

void to_bin(char * a, uint n){
  uint i = 15;
  while(i>=0){
    a[i] = (n%2)+'0';
    n = n/2;
    i--;
  } 
}

uchar sched_tick(){
  game_tick(0xff); //fruta
  
  //char a[16];
  //uchar proxima = sched_proxima_a_ejecutar();
  //to_bin(a, proxima);
  //print(a, 0,0,0x0f0f);


  return sched_proxima_a_ejecutar();
}

