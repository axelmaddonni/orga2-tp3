
/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"

#include "tss.h"

idt_entry idt[255] = { };

idt_descriptor IDT_DESC = {
    sizeof(idt) - 1,
    (unsigned int) &idt
};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0);
        ...
        IDT_ENTRY(19);

        ...
    }
*/

/*
#define IDT_ENTRY(numero)                                                                                        \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) 0x00;                                                                  \
    idt[numero].attr = (unsigned short) 0x0000;                                                                  \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);
*/

#define IDT_ENTRY(numero)                                                                                        \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) 0x0040;                                                                  \
    idt[numero].attr = (unsigned short) 0x8e00;                                                                  \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);


void idt_inicializar() {
	IDT_ENTRY(0);
	IDT_ENTRY(1);
	IDT_ENTRY(2);
	IDT_ENTRY(3);
	IDT_ENTRY(4);
	IDT_ENTRY(5);
	IDT_ENTRY(6);
	IDT_ENTRY(7);
	IDT_ENTRY(8);
	IDT_ENTRY(9);
	IDT_ENTRY(10);
	IDT_ENTRY(11);
	IDT_ENTRY(12);
	IDT_ENTRY(13);
	IDT_ENTRY(14);
	IDT_ENTRY(15);
	IDT_ENTRY(16);
	IDT_ENTRY(17);
	IDT_ENTRY(18);
	IDT_ENTRY(19);
	
	//RELOJ
	IDT_ENTRY(32);
	//TECLADO
	IDT_ENTRY(33);
	//INTERRUPCION
	//IDT_ENTRY(70); 
	
	idt[70].offset_0_15 = (unsigned short) ((unsigned int)(&_isr70) & (unsigned int) 0xFFFF);        \
    idt[70].segsel = (unsigned short) 0x0040;                                                                  \
    idt[70].attr = (unsigned short) 0xee00;                                                                  \
    idt[70].offset_16_31 = (unsigned short) ((unsigned int)(&_isr70) >> 16 & (unsigned int) 0xFFFF);

	/* por las dudas, 0x46 = 70, cualquier cosa hay que cambiarlo de aca
	 * de isr.h, isr.asm.
	 */	 
	
}
