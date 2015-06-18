;; routines common to all AUX area access routines ;;

global $aux_pagein,$aux_pagein0,$aux_pageout,__ozauxstart

psect beforebss
$aux_pagein:
$aux_pagein0:
;        ld      a,4    ;; need to send 400 (=404-4) to (1-2) and 405 to (3-4)
;        out     (2),a
;        out     (4),a
        ld      a,5
        out     (3),a
        xor     a
        out     (1),a
        ret

$aux_pageout:
        ld      a,7
        out     (3),a
        ld      a,6-4
        out     (1),a
        ret

