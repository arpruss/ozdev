;       run time start off for Hi-Tech C
;               allows custom ISRs via ozsetisr()

        global _memo1

        psect lowpage

        DEFB 00h, 64h, 09h, 0FEh, 31h, 26h, 48h, 41h
        DEFB 30h, 31h, 32h,  0Dh, 75h, 31h, 03h, 0FEh, 5Ah, 0Fh

start:
global _memo1
        call   _memo1
        ret
