.text
.align 2
main:
 # Initializing the first node with Blue Devil
  li $v0, 9
  li $a0, 24
  syscall
  move $s1, $v0
# Initializing values of BlueDevil
  move $a0, $s1
  la $a1, blue_devil
  jal strcpy
  move $s1, $v0
  li $t0, 0
  sw $t0, 16($s1)
  li $t0, 0
  sw $t0, 20($s1)
loop:
# Asking for patient name
  li $v0, 4
  la $a0, patient_prompt
  syscall
# Retrieving patient name and putting int $s5
  li $v0, 8
  la $a0, name
  li $a1, 16
  move $s5, $a0
  syscall
# Passing the name and DONE into strcmp
  move $a0, $s5
  la $a1, done
  jal strcmp
 # Checking if should end program
  bnez $v0, next
  j exit_loop
next:
# Calling function to create a node
  move $a0, $s5
  jal create_node
  move $s4, $v0
# Prompting for infector and saving it
  li $v0, 4
  la $a0, infecter_prompt
  syscall
  li $v0, 8
  la $a0, name
  li $a1, 16
  move $t2, $a0
  syscall
# Search for node with the infector name
  la $a0, 0($s1)
  move $a1, $t2
  jal search_node
  move $s3, $v0
  lw $a0, 16($s3)
  move $a1, $s4
  jal left_right
# Putting node on left or right
  beqz $v0, else
# Putting the node on the left
# and moving the other node to the right
 
  lw $t1, 16($s3)
  move $t0, $s4
  sw $t0, 16($s3)
  move $t0, $t1
  sw $t0, 20($s3)
 
  j skip_else
# Putting node on right
else:
  move $t0, $s4
  sw $t0, 20($s3)
 
skip_else:
  j loop
exit_loop:
# Print entire tree
  la $a0, 0($s1)
  jal print_nodes
  li $v0, 10
  syscall
.text
# Checks if node should be put on the left or the right
# $v0 = 0 if should be on right
# $v0 = 1 if should be on left
left_right:
  sub $sp, $sp, 8
  sw $s0, 4($sp)
  sw $ra, 0($sp)
  bnez $a0, not_NULL
  li $v0, 1
  jal end_lr
not_NULL:
 
# li $v0, 4
# syscall
 
 # move $t7, $a0
# li $v0, 4
# move $a0, $a1
# syscall
 
  jal strcmp
  bltz $v0, go_right
  li $v0, 1
  jal end_lr
go_right:
  li $v0, 0
end_lr:
  lw $s0, 4($sp)
  lw $ra, 0($sp)
  add $sp, $sp, 8
  jr $ra
 
# Creates a node and returns it in $v0
create_node:
  addi $sp, $sp, -8
  sw $ra, 0($sp)
  sw $s0, 4($sp)
  move $t2, $a0
  li $v0, 9
  li $a0, 24
  syscall
  move $s0, $v0
  la $a0, 0($s0)
  la $a1, 0($t2)
  jal strcpy
  li $t0, 0
  sw $t0, 16($s0)
  li $t0, 0
  sw $t0, 20($s0)
  move $v0, $s0
  lw $ra, 0($sp)
  lw $s0, 4($sp)
  addi $sp, $sp, 8
  jr $ra
# Returns node of infector
search_node:
  addi $sp, $sp, -16
  sw $ra, 0($sp)
 # skipping if null
  beqz $a0, return_search_null
  beq $v0, $a1, return_search_null
 
 # Saving the node and the name to be
 # searched for to be used later
  move $t5, $a1
  move $t4, $a0
 # Calling strcmp on the node and the parent name
 # Storing the inputs to be used later
  sw $t4, 4($sp)
  sw $t5, 8($sp)
  move $a0, $t4
  move $a1, $t5
  jal strcmp
  lw $t4, 4($sp)
  lw $t5, 8($sp)
 # If they are equal, return
  beqz $v0, return_search
 # Calling search_node on the left node
  lw $a0, 16($t4)
  move $a1, $t5
  sw $t4, 4($sp)
  sw $t5, 8($sp)
  jal search_node
  lw $t4, 4($sp)
  lw $t5, 8($sp)
 
 # skipping the right side if find the node
  beq $v0, $t5, return_search
 # Calling search_node on the right node
  lw $a0, 20($t4)
  sw $t4, 4($sp)
  sw $t5, 8($sp)
  jal search_node
  lw $t4, 4($sp)
  lw $t5, 8($sp)
  bne $v0, $t5, return_search_null
return_search:
 # storing the found node in $v0
  la $v0, 0($t4)
  move $t6, $v0
return_search_null:
  beqz $t6, next_return
  move $v0, $t6
next_return:
  lw $ra, 0($sp)
  add $sp, $sp, 16
  jr $ra
print_nodes:
  addi $sp, $sp, -8
  sw $ra, 0($sp)
 
  move $t5, $a0
  beqz $a0, exit_print
  li $v0, 4
  move $a0, $t5
  syscall
  lw $a0, 16($a0)
  sw $t5, 4($sp)
  jal print_nodes
  lw $t5, 4($sp)
  lw $a0, 20($t5)
  jal print_nodes
exit_print:
  lw $ra, 0($sp)
  addi $sp, $sp, 8
  jr $ra
# $a0 = dest, $a1 = src
strcpy:
  lb $t0, 0($a1)
  beq $t0, $zero, done_copying
  sb $t0, 0($a0)
  addi $a0, $a0, 1
  addi $a1, $a1, 1
  j strcpy
  done_copying:
  jr $ra
# $a0 = string buffer to be zeroed out
strclr:
  lb $t0, 0($a0)
  beq $t0, $zero, done_clearing
  sb $zero, 0($a0)
  addi $a0, $a0, 1
  j strclr
  done_clearing:
  jr $ra
# $a0, $a1 = strings to compare
# $v0 = result of strcmp($a0, $a1)
strcmp:
  lb $t0, 0($a0)
  lb $t1, 0($a1)
  bne $t0, $t1, done_with_strcmp_loop
  addi $a0, $a0, 1
  addi $a1, $a1, 1
  bnez $t0, strcmp
  li $v0, 0
  jr $ra
 
  done_with_strcmp_loop:
  sub $v0, $t0, $t1
  jr $ra
.data
patient_prompt:    .asciiz "Enter Patient:"
infecter_prompt:    .asciiz "Enter infecter:"
blue_devil:        .asciiz "BlueDevil\n"
name:       .space 16
done:       .asciiz "DONE\n"
 
 
 

