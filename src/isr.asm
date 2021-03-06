; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

extern print
extern game_syscall_manejar

;; DEBUGGING
extern debug_info

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

;; Reloj
extern game_tick
extern esta_pantalla_debug_activada

;;Teclado
extern handler_teclado
extern matar_tarea

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1
global _isr32
global _isr33
global _isr70

	
_isr%1:
    mov dword [debug_info + 00], eax
    mov dword [debug_info + 04], ebx
    mov dword [debug_info + 08], ecx
    mov dword [debug_info + 12], edx
    mov dword [debug_info + 16], esi
    mov dword [debug_info + 20], edi
    mov dword [debug_info + 24], ebp
    mov dword [debug_info + 28], esp
 
	mov eax, [esp+12] ; eip
	mov dword [debug_info + 32], eax ;eip  \
		
	mov ax, cs
	mov word [debug_info + 36], ax
	mov ax, ds
	mov word [debug_info + 38], ax
	mov ax, es
	mov word [debug_info + 40], ax
	mov ax, fs
	mov word [debug_info + 42], ax
	mov ax, gs
	mov word [debug_info + 44], ax
	mov ax, ss
	mov word [debug_info + 46], ax
    
    xor eax, eax
    pushf    ; obtenemos el registro
    pop ax   ; eflags
    mov dword [debug_info + 48], eax
    
    mov eax, cr0
  	mov dword [debug_info + 52], eax
  	mov eax, cr2
  	mov dword [debug_info + 56], eax
  	mov eax, cr3
  	mov dword [debug_info + 60], eax
  	mov eax, cr4
  	mov dword [debug_info + 64], eax
  	
  	mov eax, [esp]
  	mov dword [debug_info + 68], eax
  	mov eax, [esp+4]
  	mov dword [debug_info + 72], eax
  	mov eax, [esp+8]
  	mov dword [debug_info + 76], eax
  	mov eax, [esp+12]
  	mov dword [debug_info + 80], eax
    mov eax, [esp+16]
  	mov dword [debug_info + 84], eax


    sti
    call matar_tarea
    jmp 0x70:0 ;voy a idle

   
    ;mov eax, %1
    ;push dword 0x0f0f   ; fruta
    ;push dword 0
    ;push dword 0
    ;push MENSAJE_ERROR_%1
    ;call print
    ;xchg bx, bx 
    ;jmp $

%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
;ISR 33





;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;

offset: dd 0
selector: dw 0
_isr32:
  pushad
  pushfd
  

  call fin_intr_pic1
  

  call esta_pantalla_debug_activada
  cmp eax, 1
  je .fin
  xor eax, eax

  call sched_tick
 
  
  str cx
  cmp ax, cx
  je .fin
    mov [selector], ax
   jmp far [offset]
  
  .fin
  popfd
  popad
  
  iret
;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
  
_isr33:
  pushad
  pushfd

  xor eax, eax
  in al, 0x60
  push eax
  call handler_teclado
  
  add esp, 4
  
  
  call fin_intr_pic1
  
  popfd
  popad
  
  iret
  
;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;


_isr70:
  push ecx
  push edx
  push ebx
  push esp
  push ebp
  push esi
  push edi
  pushfd
  
  push ecx
  push eax
  call game_syscall_manejar
  add esp, 8

  jmp 0x70:0 ;voy a idle
 
  popfd
  pop edi
  pop esi
  pop ebp
  pop esp
  pop ebx
  pop edx
  pop ecx
  
  iret

MENSAJE_ERROR_0: db 'Interrupcion de division por 0. (Error 0)', 0
MENSAJE_ERROR_1: db 'Interrupcion reservada. (Error 1)', 0
MENSAJE_ERROR_2: db 'Interrupcion no enmascarable. (Error 2)', 0
MENSAJE_ERROR_3: db 'Interrupcion BreakPoint. (Error 3)', 0
MENSAJE_ERROR_4: db 'Interrupcion de overflow. (Error 4)', 0
MENSAJE_ERROR_5: db 'Interrupcion BOUND Range Exceeded. (Error 5)', 0
MENSAJE_ERROR_6: db 'Interrupcion Invalid Opcode. (Error 6)' , 0
MENSAJE_ERROR_7: db 'Interrupcion Coprocesador No Disponible. (Error 7)', 0
MENSAJE_ERROR_8: db 'Interrupcion Doble Fault. (Error 8)', 0
MENSAJE_ERROR_9: db 'Interrupcion Coprocessor Segment Overrun (reservada). (Error 9)', 0
MENSAJE_ERROR_10: db 'Interrupcion TSS Invalido. (Error 10)', 0
MENSAJE_ERROR_11: db 'Interrupcion Segmento no Presente. (Error 11)', 0
MENSAJE_ERROR_12: db 'Interrupcion de Falta en el Stack Segment. (Error 12)', 0
MENSAJE_ERROR_13: db 'Interrupcion de General Protection. (Error 13)', 0
MENSAJE_ERROR_14: db 'Interrupcion de Page Fault. (Error 14)', 0
MENSAJE_ERROR_15: db 'Interrupcion Reservada por Intel. (Error 15)', 0
MENSAJE_ERROR_16: db 'Interrupcion X-87 FPU Error de punto flotante. (Error 16)', 0
MENSAJE_ERROR_17: db 'Interrupcion de Alignment Check Fault. (Error 17)', 0
MENSAJE_ERROR_18: db 'Interrupcion de Machine Check abort. (Error 18)', 0
MENSAJE_ERROR_19: db 'Interrupcion SIMD Floating Point. (Error 19)', 0


