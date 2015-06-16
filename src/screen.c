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

ca backup[VIDEO_FILS][VIDEO_COLS];

const char reloj[] = "|/-\\";
#define reloj_size 4

struct {
  uint eax;
  uint ebx;
  uint ecx;
  uint edx;
  uint esi;
  uint edi;
  uint ebp;
  uint esp;
  uint eip;
  ushort cs;
  ushort ds;
  ushort es;
  ushort fs;
  ushort gs;
  ushort ss;
  uint eflags;
  uint cr0;
  uint cr2;
  uint cr3;
  uint cr4;
  uint stack0;
  uint stack1;
  uint stack2;
  uint stack3;
  uint stack4;
} __attribute__((__packed__)) debug_info;

void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_actualizar_reloj_pirata (jugador_t *j, pirata_t *pirata)
{

    pirata->estado_reloj = (pirata->estado_reloj+ 1) % reloj_size;

    if(j->jug == A)
    {
        screen_pintar(reloj[pirata->estado_reloj], C_BW, 48, 4+2*pirata->index);
    }
    else
    {
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
    for (i = 0; i < ancho; i++)
    {
        for (j = 0; j < alto; j++)
        {
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
    for (i = 1; i < 9; i++)
    {
        print_dec(i, 1, 2+2*i , 46, C_BW);
        screen_pintar('X', 0x04 , 48, 2+2*i);
        print_dec(i, 1, 57+2*i , 46, C_BW);
        screen_pintar('X', 0x01 , 48, 57+2*i);
    }

	print("Diablo II - Ciruelos / Maddonni / Thibeault", 0,0, 0x0e);

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

void print(const char * text, uint x, uint y, unsigned short attr) 
{
    int i;
    for (i = 0; text[i] != 0; i++)
    {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) 
        {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) 
{
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
    for(i = 0; i < size; i++) 
    {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) 
{
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) 
    {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}

void screen_pintar_puerto(jugador_t * j)
{
    uint x = j->puerto[0], y = j->puerto[1];

    uchar c = j->color;
    if(p[y  ][x-1].a != c) screen_pintar_rect(0, c, y  , x-1, 1, 1);
    if(p[y+1][x-1].a != c) screen_pintar_rect(0, c, y+1, x-1, 1, 1);
    if(p[y+2][x-1].a != c) screen_pintar_rect(0, c, y+2, x-1, 1, 1);
    if(p[y  ][x  ].a != c) screen_pintar_rect(0, c, y  , x  , 1, 1);
    if(p[y+1][x  ].a != c) screen_pintar_rect(0, c, y+1, x  , 1, 1);
    if(p[y+2][x  ].a != c) screen_pintar_rect(0, c, y+2, x  , 1, 1);
    if(p[y  ][x+1].a != c) screen_pintar_rect(0, c, y  , x+1, 1, 1);
    if(p[y+2][x+1].a != c) screen_pintar_rect(0, c, y+1, x+1, 1, 1);
    if(p[y+2][x+1].a != c) screen_pintar_rect(0, c, y+2, x+1, 1, 1);
}


void screen_make_backup()
{
  uint i, j;
  for(i = 0; i<VIDEO_FILS; i++){
    for(j = 0; j<VIDEO_COLS; j++){
      backup[i][j] = p[i][j];
    }
  }
}

void screen_restore_backup()
{
  uint i, j;
  for(i = 0; i<VIDEO_FILS; i++){
    for(j = 0; j<VIDEO_COLS; j++){
      p[i][j] = backup[i][j];
    }
  }
}

void screen_pantalla_debug()
{
  uint x = 25, y = 7;

  screen_pintar_rect(0, 0x00, y, x, 36, 32);
  screen_pintar_rect(0, 0x77, y+1, x+1, 34, 30);
  screen_pintar_rect(0, 0x44, y+1, x+1, 1, 30);


  print("eax", x+2, y+3 , 0x70); print_hex(debug_info.eax, 8, x+6, y+3 , 0x7f);
  print("ebx", x+2, y+5 , 0x70); print_hex(debug_info.ebx, 8, x+6, y+5 , 0x7f);
  print("ecx", x+2, y+7 , 0x70); print_hex(debug_info.ecx, 8, x+6, y+7 , 0x7f);
  print("edx", x+2, y+9 , 0x70); print_hex(debug_info.edx, 8, x+6, y+9 , 0x7f);
  print("esi", x+2, y+11, 0x70); print_hex(debug_info.esi, 8, x+6, y+11, 0x7f);
  print("edi", x+2, y+13, 0x70); print_hex(debug_info.edi, 8, x+6, y+13, 0x7f);
  print("ebp", x+2, y+15, 0x70); print_hex(debug_info.ebp, 8, x+6, y+15, 0x7f);
  print("esp", x+2, y+17, 0x70); print_hex(debug_info.esp, 8, x+6, y+17, 0x7f);
  print("eip", x+2, y+19, 0x70); print_hex(debug_info.eip, 8, x+6, y+19, 0x7f);


  print("cs", x+3, y+21, 0x70); print_hex(debug_info.cs, 4, x+6, y+21, 0x7f);
  print("ds", x+3, y+23, 0x70); print_hex(debug_info.ds, 4, x+6, y+23, 0x7f);
  print("es", x+3, y+25, 0x70); print_hex(debug_info.es, 4, x+6, y+25, 0x7f);
  print("fs", x+3, y+27, 0x70); print_hex(debug_info.fs, 4, x+6, y+27, 0x7f);
  print("gs", x+3, y+29, 0x70); print_hex(debug_info.gs, 4, x+6, y+29, 0x7f);
  print("ss", x+3, y+31, 0x70); print_hex(debug_info.ss, 4, x+6, y+31, 0x7f);
  
  print("eflags", x+3, y+33, 0x70); print_hex(debug_info.eflags, 8, x+9, y+33, 0x7f);
  
  print("cr0", x+16, y+3, 0x70); print_hex(debug_info.cr0, 8, x+20, y+3, 0x7f);
  print("cr2", x+16, y+5, 0x70); print_hex(debug_info.cr2, 8, x+20, y+5, 0x7f);
  print("cr3", x+16, y+7, 0x70); print_hex(debug_info.cr3, 8, x+20, y+7, 0x7f);
  print("cr4", x+16, y+9, 0x70); print_hex(debug_info.cr4, 8, x+20, y+9, 0x7f);

  print("stack", x+16, y+20, 0x70); 
  print_hex(debug_info.stack0, 8, x+16, y+22, 0x7f);
  print_hex(debug_info.stack1, 8, x+16, y+23, 0x7f);
  print_hex(debug_info.stack2, 8, x+16, y+24, 0x7f);
  print_hex(debug_info.stack3, 8, x+16, y+25, 0x7f);
  print_hex(debug_info.stack4, 8, x+16, y+26, 0x7f);

}


void screen_debug()
{
  screen_make_backup();
  screen_pantalla_debug();
  while(modo_debug_activado)
  {
    ;
  }
  screen_restore_backup();
}


