
/*
 * Assembler1.S
 *
 * Created: 05/04/2024 02:50:57
 *  Author: patth
 */ 

.equ DDRB, 0x24
.equ DDRD, 0x2A
.global setup

setup:
    ; Set Port D as output
    ldi r16, 0xFF   ; Load 0xFF into register r16 (all bits set to 1)
    out DDRD, r16   ; Set all bits of DDRD register to 1 (output)

    ; Set Port B as input
    ldi r16, 0x00   ; Load 0x00 into register r16 (all bits set to 0)
    out DDRB, r16   ; Set all bits of DDRB register to 0 (input)

loop:
    rjmp loop       ; Loop indefinitely