psect lowpage

global header0,header1

header0:
        defw    0 ;; page
        defw    0a400h + 12 ;; width table
        defw    0a400h ;; font delta
        defb    0 ;; height
        defb    07fh ;; mask

header1:
        defw    0 ;; page
        defw    0b180h + 12 ;; width table
        defw    0b180h ;; font delta
        defb    0 ;; height
        defb    07fh ;; mask

