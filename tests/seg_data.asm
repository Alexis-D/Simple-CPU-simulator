        TEXT 30

main    EQU *
        LOAD R00, #1234
        LOAD R01, 3[R00]
        END
        
        DATA 30
        
        WORD 0
        WORD 0
        WORD 0
        WORD 0
        
        END
