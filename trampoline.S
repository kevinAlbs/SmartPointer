# This can be compiled by itself with `as --32`
# This is GNU assembler syntax (aka AT-T syntax) (src, dest)
.section .text
.globl trampoline 
.type trampoline, @function
trampoline:
call do_free
jmp %eax # jump directly back to the old eip.
