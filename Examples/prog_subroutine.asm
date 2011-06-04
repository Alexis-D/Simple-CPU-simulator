//-----------------
// Instructions
//-----------------
        TEXT 30

        // Programme principal
main    EQU *
        PUSH @op1
        PUSH @op2
        CALL NC, @subprog
        ADD R15, #2
        STORE R00, @result
        HALT 
        NOP 
        NOP 
        NOP 
        NOP

        // Sous-programme
subprog EQU *
        LOAD R00, 3[R15]
        LOAD R01, 2[R15]
        SUB R01, #1
loop    BRANCH LE, @return
        ADD R00, 3[R15]
        SUB R01, #1
        BRANCH NC, @loop
return  RET
        
        END
        
//-----------------
// Données et pile
//-----------------
        DATA 30
        
        WORD 0
result  WORD 0
op1     WORD 20
op2     WORD 5
        
        END
