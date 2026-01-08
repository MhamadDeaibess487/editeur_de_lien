    .extern ext_func

    .section .text
    .global global_func

local_label:
    nop

global_func:
    bl ext_func
    b local_label
