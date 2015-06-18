global $ozsetlcdstate,$ozlcdstatus,restore_a000
psect lowpage
$ozsetlcdstate:
        pop     de
        pop     hl        ;; status to set
        push    hl
        push    de

        xor     a
        out     (3),a
        ld      a,3
        out     (4),a      ;; page in page 300h
        ld      (0a000h),hl
        ld      ($ozlcdstatus),hl
        jp      restore_a000

