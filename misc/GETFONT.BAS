SCREEN 2
DEFINT I-K, N
OPEN "8x8.as" FOR OUTPUT AS #1
PRINT #1, "ibm8x8:"
FOR i = 0 TO 255
  LOCATE 1, 1
  PRINT CHR$(i)
  PRINT #1, "defb ";
  FOR j = 0 TO 7
    n = 0
    FOR k = 0 TO 7
        n = n + POINT(k, j) * 2 ^ k
    NEXT
    PRINT #1, n;
    IF (j <> 7) THEN PRINT #1, ",";
  NEXT
  PRINT #1, " ;"; i
NEXT i

