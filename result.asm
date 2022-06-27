.data
string_0: .asciiz "Add: "
string_1: .asciiz "\n"
string_2: .asciiz "Sub: "
string_3: .asciiz "\n"
string_4: .asciiz "Mult: "
string_5: .asciiz "\n"
string_6: .asciiz "Div: "
string_7: .asciiz "\n"
string_8: .asciiz "Mod: "
string_9: .asciiz "\n"

.globl main

.text
addFunc: 
addiu $sp, $sp, -12

sw $ra, 0($sp)

sw $a0, 4($sp)
sw $a1, 8($sp)
lw $t0, 4($sp)
lw $t1, 8($sp)
add $t0, $t0, $t1


move $v0, $t0
lw $ra, 0($sp)

addiu $sp, $sp, 12
jr $ra

subFunc: 
addiu $sp, $sp, -12

sw $ra, 0($sp)

sw $a0, 4($sp)
sw $a1, 8($sp)
lw $t1, 4($sp)
lw $t2, 8($sp)
sub $t1, $t1, $t2


move $v0, $t1
lw $ra, 0($sp)

addiu $sp, $sp, 12
jr $ra

multFunc: 
addiu $sp, $sp, -12

sw $ra, 0($sp)

sw $a0, 4($sp)
sw $a1, 8($sp)
lw $t2, 4($sp)
lw $t3, 8($sp)
mult $t2, $t3
mflo $t2

move $v0, $t2
lw $ra, 0($sp)

addiu $sp, $sp, 12
jr $ra

divFunc: 
addiu $sp, $sp, -12

sw $ra, 0($sp)

sw $a0, 4($sp)
sw $a1, 8($sp)
lw $t3, 4($sp)
lw $t4, 8($sp)
div $t3, $t4
mflo $t3

move $v0, $t3
lw $ra, 0($sp)

addiu $sp, $sp, 12
jr $ra

modFunc: 
addiu $sp, $sp, -12

sw $ra, 0($sp)

sw $a0, 4($sp)
sw $a1, 8($sp)
lw $t4, 4($sp)
lw $t5, 8($sp)
div $t4, $t5
mfhi $t4

move $v0, $t4
lw $ra, 0($sp)

addiu $sp, $sp, 12
jr $ra

main: 
addiu $sp, $sp, -20

sw $ra, 0($sp)


la $a0, string_0
li $v0, 4
syscall
li $t5, 5

li $t6, 15

move $a0, $t5
move $a1, $t6
jal addFunc
move $t5, $v0

move $a0, $t5
li $v0, 1
syscall

la $a0, string_1
li $v0, 4
syscall

la $a0, string_2
li $v0, 4
syscall
li $t5, 100

li $t6, 75

move $a0, $t5
move $a1, $t6
jal subFunc
move $t5, $v0

move $a0, $t5
li $v0, 1
syscall

la $a0, string_3
li $v0, 4
syscall

la $a0, string_4
li $v0, 4
syscall
li $t5, 5

li $t6, 10

move $a0, $t5
move $a1, $t6
jal multFunc
move $t5, $v0

move $a0, $t5
li $v0, 1
syscall

la $a0, string_5
li $v0, 4
syscall

la $a0, string_6
li $v0, 4
syscall
li $t5, 50

li $t6, 25

move $a0, $t5
move $a1, $t6
jal divFunc
move $t5, $v0

move $a0, $t5
li $v0, 1
syscall

la $a0, string_7
li $v0, 4
syscall

la $a0, string_8
li $v0, 4
syscall
li $t5, 5

li $t6, 50

move $a0, $t5
move $a1, $t6
jal modFunc
move $t5, $v0

move $a0, $t5
li $v0, 1
syscall

la $a0, string_9
li $v0, 4
syscall
li $t5, 10

sw $t5 4($sp)
li $t5, 5

sw $t5 8($sp)
li $t5, 2

sw $t5 12($sp)
lw $t5, 8($sp)

sw $t5 12($sp)
lw $t5, 12($sp)

move $a0, $t5
li $v0, 1
syscall
lw $ra, 0($sp)

addiu $sp, $sp, 20
jr $ra


li $v0, 10
syscall
