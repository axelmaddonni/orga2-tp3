/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";
#define reloj_size 4


void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_actualizar_reloj_pirata (jugador_t *j, pirata_t *pirata){
  
    pirata->estado_reloj = (pirata->estado_reloj+ 1) % reloj_size;

    if(j->jug == A){
      screen_pintar(reloj[pirata->estado_reloj], C_BW, 48, 4+2*pirata->index);
    }else {
      screen_pintar(reloj[pirata->estado_reloj], C_BW, 48, 59+2*pirata->index);
    }
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

void screen_pintar_rect(unsigned char c, unsigned char color, int fila, int columna, int alto, int ancho)
{
	int i;
	int j;
	for (i = 0; i < ancho; i++){
		for (j = 0; j < alto; j++){
		    p[fila + j][columna + i].c = c;
		    p[fila + j][columna + i].a = color;
		}
	}
}

void screen_inicializar()
{
	//limpio la pantalla
	screen_pintar_rect(0x00, 0x77, 0, 0, 50, 80);
	//barra negra abajo
	screen_pintar_rect(0x00, 0x00, 45, 0, 5, 80);
    //barra negra arriba
	screen_pintar_rect(0x00, 0x00, 0, 0, 1, 80);
	//barra roja
	screen_pintar_rect(0x00, 0xCC, 45, 33 , 5, 7);
	//barra azul
	screen_pintar_rect(0x00, 0x99, 45, 40 , 5, 7);
    
    //puntajes
    print_dec(jugadorA.monedas, 3, 35, 47, 0x4F);
    print_dec(jugadorB.monedas, 3, 42, 47, 0x1F);

    //slots
    int i;
    for (i = 1; i < 9; i++){
        print_dec(i, 1, 2+2*i , 46, C_BW);
        screen_pintar('X', 0x04 , 48, 2+2*i);
        print_dec(i, 1, 57+2*i , 46, C_BW);
        screen_pintar('X', 0x01 , 48, 57+2*i);
    }

}

void screen_pintar_puntajes()
{
    print_dec(jugadorA.monedas, 3, 35, 47, 0x4F);
    print_dec(jugadorB.monedas, 3, 42, 47, 0x1F);

}

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}


