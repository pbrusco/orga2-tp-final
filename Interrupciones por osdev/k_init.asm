[bits 32]
[global start]
[extern _k_main]

start:     


call _k_main     ; jump to k_main() in kernel.c


hlt              ; halt the cpu