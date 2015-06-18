global _ozclick,__ozclick_setting,enable_click
psect text

_ozclick:
        ld hl,2
        add hl,sp
        ld a,(hl)
        ld (__ozclick_setting),a
        or      a
        ret     z

enable_click:
        ld      a,1
        out     (19h),a
        ld      a,10
        out     (17h),a
        xor     a
        out     (18h),a
        ld      a,2
        out     (16h),a         ;; enable key click
        ret


