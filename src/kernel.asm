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

extern testear_paginacion


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

;; TAREAS (EJERCICIO 6)
extern tss_inicializar
extern tss_inicializar_tarea

;; SCHEDULER (EJERCICIO 7)
extern game_inicializar
extern testear_crear_tarea
extern sched_inicializar

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

	mov ax,  1001000b
    mov fs, ax

    ; Inicializar pantalla
    call screen_inicializar

    ; Inicializar el manejador de memoria
    call inicializar_mmu

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

    
    
    
    ;;;; ESTO SE DEBE BORRAR DE LA  VERSION FINAL
    ;push dword 0x0e0e   ; notar que cambia el color
    ;push dword 2
    ;push dword 0
    ;push MENSAJE_MODO_PROTEGIDO
    ;call print
    ;;;;; ES SOLO UN  TEST QUE SUGIEREN
    

    ; Inicializar tss
    call tss_inicializar
    
    
    ; Inicializar tss de la tarea Idle
    ; call tss_inicializar_idle
    ; no hace falta, se hace en tss_inicializar


    ; Inicializar el scheduler
    call sched_inicializar
    
    ; Inicializar la IDT
    call idt_inicializar
    ; Cargar IDT
    
    lidt [IDT_DESC]
    
    
    ; Configurar controlador de interrupciones
    call resetear_pic
    call habilitar_pic
     ; Inicializar el juego
        call game_inicializar

    ; Cargar tarea inicial
    mov ax, 0x68  ; 13 << 3
    ltr ax

    ; Habilitar interrupciones
    sti 
 

    
    ;call testear_crear_tarea 
    ; Saltar a la primera tarea: Idle
    ;xchg bx, bx 
    jmp 0x70:0 ; 14 << 3   111|0000
  
    ;jmp 0x78:0
    
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
