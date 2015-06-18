global _ozenableinterrupts,_ozdisableinterrupts

psect text

_ozenableinterrupts:
        ei
        ret

_ozdisableinterrupts:
        di
        ret
