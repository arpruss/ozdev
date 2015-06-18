
SCREEN 1
OPEN "8x8_X.txt" FOR OUTPUT AS #1
PRINT #1, "0"
PRINT #1, 255
PRINT #1, 8
PRINT #1, 8
FOR i = 0 TO 255
        PRINT #1, i, i
        LOCATE 1, 1
        PRINT CHR$(i)
        FOR j = 0 TO 7
                FOR k = 0 TO 7
                        IF (POINT(k, j)) THEN PRINT #1, "X";  ELSE PRINT #1, ".";
                NEXT k
                PRINT #1,
        NEXT j
NEXT i
SCREEN 0

