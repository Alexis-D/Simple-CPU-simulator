        TEXT 30

main    EQU *
        ADD R00, #12345
        CALL NC, 6[R00]
        NOP
        NOP

        END

        DATA 30

        WORD 0
        WORD 0
        WORD 0
        WORD 0

        END
