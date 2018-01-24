# This can be compiled by itself with `as --32`
# This is GNU assembler syntax (aka AT-T syntax) (src, dest)
.section .text
.globl thunk
.type thunk, @function
thunk:
# Note this is hijacked. The stack does not have a return address pushed.
# So instead, call our do_free, then jump to the address it returns.

# Call do_free. It will set our return %eip correctly.
# Note, the call below implicilty does pushl %eip
# According to https://stackoverflow.com/questions/12830253/gas-does-not-recognise-register-ip
# it is actually not possible in GAS to explicitly read %eip.

call do_free # how to call a C function from assembly
jmp %eax