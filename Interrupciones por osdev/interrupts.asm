; interrupts.asm
; contains all system exception interrupts
; and other system interface interrupts
; note: exception 15 and 20-31 are Intel Reserved

[extern _int_00]
[global _int00]
_int00:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_00    ; Divide by Zero #DE
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_01]
[global _int01]
_int01:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_01    ; Debug #DB
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_02]
[global _int02]
_int02:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_02    ; NMI interrupt #--
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_03]
[global _int03]
_int03:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_03    ; Breakpoint #BP
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_04]
[global _int04]
_int04:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_04    ; Overflow #OF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_05]
[global _int05]
_int05:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_05    ; Bound Range Exception #BR
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_06]
[global _int06]
_int06:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_06    ; Invalid Opcode #UD
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_07]
[global _int07]
_int07:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_07    ; Device Not Available #NM
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_08]
[global _int08]
_int08:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_08    ; Double Fault #DF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret


[extern _int_09]
[global _int09]
_int09:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_09    ; Coprocessor Segment Overrun
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_10]
[global _int10]
_int10:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_10    ; Invalid Task State Segment (TSS) #TS
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_11]
[global _int11]
_int11:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_11    ;  Segment Not Present #NP
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_12]
[global _int12]
_int12:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_12    ; Stack Segment Fault #SS
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_13]
[global _int13]
_int13:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_13    ; General Protection Fault (GPF) #GP
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_14]
[global _int14]
_int14:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_14    ; Page Fault #PF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

;; Exception 15 is an Intel Reserved Interrupt

[extern _int_16]
[global _int16]
_int16:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_16    ; x87 Floating-Point Error #MF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_17]
[global _int17]
_int17:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_17    ; Alignment Check #AC
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_18]
[global _int18]
_int18:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_18    ; Machine Check #MC
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern _int_19]
[global _int19]
_int19:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _int_19    ; SIMD Floating-Point Exception #XF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

;; Exceptions 20-31 are Intel Reserved Interrupts

;; E n d   o f   E x c e p t i o n s   I n t e r r u p t s

[extern _testint]
[global _int32]
_int32:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call _testint   ; Test Interrupt Handler
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret
