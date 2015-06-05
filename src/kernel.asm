; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "imprimir.mac"

global start
extern GDT_DESC
extern print

extern screen_inicializar
extern idt_inicializar
extern IDT_DESC

;; PAGINACION (EJERCICIO 3)
extern mmu_inicializar_dir_kernel
extern mmu_inicializar_tabla_kernel
%define PAGE_DIRECTORY_CR3 0x00027000

;; MAS PAGINACION (EJECICIO 4)
extern mmu_inicializar_dir_pirata
extern inicializar_mmu

;; INTERRUPCIONES (EJERCICIO 5)
extern resetear_pic
extern habilitar_pic


;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0

    ; Habilitar A20
    call habilitar_A20  
   
    ; Cargar la GDT
	lgdt [GDT_DESC]      ; cargo la estructura que esta en gdt.c

    ; Setear el bit PE del registro CR0
    mov eax, cr0
	or eax, 1
	mov cr0, eax
    ; Saltar a modo protegido

	jmp 0x40:modoprotegido
    ; Establecer selectores de segmentos


BITS 32
modoprotegido:
    xor eax, eax
	mov ax,  1001000b

	mov ds, ax
	mov es, ax
	mov gs, ax
	mov ss, ax
    
	mov ax, 1100000b
    mov fs, ax
    
    ; Establecer la base de la pila
	mov ebp, 0x27000
	mov esp, 0x27000
	
		
    ; Imprimir mensaje de bienvenida
    push dword 0x0f0f   ; fruta
    push dword 2
    push dword 0
    push MENSAJE_MODO_PROTEGIDO
    call print
    add esp, 4*4
    
    ;mov fs, 1001000b chequear despues

    ; Inicializar el juego

    ; Inicializar pantalla
    call screen_inicializar

    ; Inicializar el manejador de memoria

    ; Inicializar el directorio de paginas
    call mmu_inicializar_dir_kernel
    call mmu_inicializar_tabla_kernel
     
    ; Cargar directorio de paginas
    mov eax, PAGE_DIRECTORY_CR3
    mov cr3, eax
   
    ; Habilitar paginacion
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    call inicializar_mmu
    call mmu_inicializar_dir_pirata
    ;xchg bx, bx
    mov cr3, eax
   

    ;;;; ESTO SE DEBE BORRAR DE LA  VERSION FINAL
    ;push dword 0x0e0e   ; notar que cambia el color
    ;push dword 2
    ;push dword 0
    ;push MENSAJE_MODO_PROTEGIDO
    ;call print
    ;;;;; ES SOLO UN  TEST QUE SUGIEREN
    

    ; Inicializar tss

    ; Inicializar tss de la tarea Idle

    ; Inicializar el scheduler

    ; Inicializar la IDT
    call idt_inicializar

    ; Cargar IDT
    
    lidt [IDT_DESC]
     
     mov ebx, 0
     
     or eax, 0xffffffff
     sub eax, 0x0000000
     
    ; Configurar controlador de interrupciones
    call resetear_pic
    call habilitar_pic
    
    ; Cargar tarea inicial

    ; Habilitar interrupciones
    sti 
  
    
    
    ; Saltar a la primera tarea: Idle
    
    
    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $
	
;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
MENSAJE_MODO_PROTEGIDO: db 'Corriendo en modo protegido...', 0 
