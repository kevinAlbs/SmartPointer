# This can be compiled by itself with `as --32`
# This is GNU assembler syntax (aka AT-T syntax) (src, dest)
.section .text
.globl test_asm
.type test_asm, @function
test_asm:
pushl %ebp
movl %esp, %ebp
movl $1, %eax # integers are returned through eax
movl %ebp, %esp
popl %ebp
ret  # pop the return address and jmp to it
