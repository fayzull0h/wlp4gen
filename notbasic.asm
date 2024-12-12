; begin prologue
.import init
.import new
.import delete
lis $4
.word 4
lis $10
.word println
lis $11
.word 1
sub $29, $30, $4 ; setup frame pointer
sw $1, 0($29) ; push variable a
sub $30, $30, $4 ; update stack pointer
sw $2, -4($29) ; push variable b
sub $30, $30, $4 ; update stack pointer
lis $5
.word 0
sw $5, -8($29) ; push variable i
sub $30, $30, $4 ; update stack pointer
lis $5
.word 0
sw $5, -12($29) ; push variable j
sub $30, $30, $4 ; update stack pointer
lis $5
.word 1
sw $5, -16($29) ; push variable reverse
sub $30, $30, $4 ; update stack pointer

sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word init
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)

lw $3, -4($29) ; $3 <- b
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word new
jalr $5
add $30, $30, $4
lw $31, -4($30)
bne $3, $0, 1
add $3, $11, $0
sw $3, -16($29)
lw $3, -4($29) ; $3 <- b
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -12($29)
FEloop81:
lw $3, -8($29) ; $3 <- i
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29) ; $3 <- b
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
beq $3, $0, FEendWhile81
lw $3, 0($29) ; $3 <- a
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29) ; $3 <- j
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -16($29) ; $3 <- reverse
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29) ; $3 <- i
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
lw $3, -8($29) ; $3 <- i
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -8($29)
lw $3, -12($29) ; $3 <- j
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -12($29)
beq $0, $0, FEloop81
FEendWhile81:
lw $3, -12($29) ; $3 <- j
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -12($29)
FEloop176:
lw $3, -12($29) ; $3 <- j
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29) ; $3 <- b
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
beq $3, $0, FEendWhile176
lw $3, -16($29) ; $3 <- reverse
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29) ; $3 <- j
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3)
add $6, $1, $0
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
jalr $10
add $30, $30, $4
lw $31, -4($30)
add $1, $6, $0
lw $3, -12($29) ; $3 <- j
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -12($29)
beq $0, $0, FEloop176
FEendWhile176:
lw $3, -16($29) ; $3 <- reverse
beq $3, $11, FEskipDelete229
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word delete
jalr $5
add $30, $30, $4
lw $31, -4($30)
FEskipDelete229:
lis $3
.word 0

; begin epilogue
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
jr $31

; $1 = the twos complement of the integer
; $2 = to test if negative
; $3 = output
; $4 = 4
; $5 = num of numbers pushed to stack
; $6 = 48 (ascii of 0)

println:
; store away all the registers that I want to use
sw $1, -4($30)
sw $2, -8($30)
sw $3, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $6, -24($30)

lis $4
.word 24
sub $30, $30, $4

; set the special registers that i wanna use
lis $3
.word 0xffff000c
lis $4
.word 4
add $5, $0, $0
lis $6
.word 48

slt $2, $1, $0      ; if $1 is negative, set $2 = 1
beq $2, $0, 4       ; $2 is 0, skip 4 lines
lis $2
.word 0x2d
sw $2, 0($3)        ; if $2 is 1, print '-'
sub $1, $0, $1      ; and make $1 the absolute value of $1

FEstoreloop:
lis $2
.word 10
divu $1, $2         ; divide $1 by 10
mfhi $2             ; move the remainder into $2
mflo $1             ; move the quotient into $1
sw $2, -4($30)      ; store away the 'remainder'
add $5, $5, $4      ; add 4 to the numbe of bytes stored away
sub $30, $30, $4    ; move the stack pointer up by 4
bne $1, $0, FEstoreloop    ; if $1 is not 0, loop

FEpoploop:
beq $5, $0, FEend     ; if all bytes not printed yet, go back and print more
lw $2, 0($30)       ; take the top off the stack into $2
add $2, $2, $6      ; convert to ascii number
sw $2, 0($3)        ; print the content of $2
add $30, $30, $4    ; move stack pointer down
sub $5, $5, $4      ; subtract 4 from bytes stored
beq $0, $0, FEpoploop

FEend:
lis $2
.word 10
sw $2, 0($3)
lis $3
.word 24
add $30, $30, $3
lw $6, -24($30)
lw $5, -20($30)
lw $4, -16($30)
lw $3, -12($30)
lw $2, -8($30)
lw $1, -4($30)

jr $31
