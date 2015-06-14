/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#ifndef __ISR_H__
#define __ISR_H__

#include "screen.h"
#include "game.h"

void _isr0();
void _isr1();
void _isr2();
void _isr3();
void _isr4();
void _isr5();
void _isr6();
void _isr7();
void _isr8();
void _isr9();
void _isr10();
void _isr11();
void _isr12();
void _isr13();
void _isr14();
void _isr15();
void _isr16();
void _isr17();
void _isr18();
void _isr19();



void _isr32();
void _isr33();
void _isr70();



void handler_teclado(uchar scan_code){
	if(scan_code == 0x1e){
    screen_pintar('a', 0x0f, 0, 79);
  }
  else if(scan_code == 0x2a){
		screen_pintar('<', 0x0f, 0, 79);
    game_atender_teclado('<'); 
	}
  else if(scan_code == 0x36){
		screen_pintar('>', 0x0f, 0, 79);
    game_atender_teclado('>');
	}	
}

void matar_tarea(){
  game_pirata_exploto(id_del_pirata_actual); 
}


#endif  /* !__ISR_H__ */
