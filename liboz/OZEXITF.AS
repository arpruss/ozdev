global __ozcallexitfunctions

psect text

macro triple_nop
        nop
        nop
        nop
endm

__ozcallexitfunctions:
        triple_nop
        triple_nop
        triple_nop
        triple_nop
        triple_nop

        triple_nop
        triple_nop
        triple_nop
        triple_nop
        triple_nop

        ret

