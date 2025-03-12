STAKSG SEGMENT PARA STACK 'STACK'
    DW 64 DUP(?)
STAKSG ENDS

DATASG SEGMENT PARA 'DATA'
    DIZI  DB 500 DUP(-1)
    SONUC DW ?
    N     DW 500
    
DATASG ENDS


CODESG SEGMENT PARA 'CODE'
    ASSUME SS:STAKSG , DS:DATASG , CS:CODESG
    DNUM PROC FAR
         PUSH BP
         PUSH DI
         PUSH SI
         
         MOV BP , SP
         MOV AX , [BP+10]
         MOV DI , AX
         
         CMP DIZI[DI] , -1 
         JNE L1
         
         CMP AX , 2
         JE J12
         
         CMP AX , 1
         JE J12
         
         CMP AX , 0
         JE J0
         
         
         SUB AX , 2
         PUSH AX 
         CALL FAR PTR DNUM
         POP DX 
           
         
         
         MOV SI , [BP+10]
         SUB SI , 1
         SUB SI , DX
         PUSH SI
         CALL FAR PTR DNUM
         POP SI   ;SONUC1
         
         MOV CX , [BP+10]
         SUB CX , 1        
         PUSH CX
         CALL FAR PTR DNUM
         CALL FAR PTR DNUM
         POP BX   ;SONUC2
         
         ADD BX , SI
         MOV AX , BX
         MOV DIZI[DI] , AL         
         JMP EXIT
     L1: 
         XOR AX , AX
         MOV AL , DIZI[DI]
         JMP EXIT           
         
    J12:
         MOV AX , 1
         MOV DIZI[DI] , AL
         JMP EXIT
    J0:  
         MOV AX , 0 
         MOV DIZI[DI] , AL
    
   EXIT: 
         
         MOV [BP+10] , AX
         POP SI
         POP DI
         POP BP
         RETF   
        
    DNUM ENDP

    START PROC FAR
        PUSH DS
        XOR AX , AX
        PUSH AX
        
        MOV AX , DATASG
        MOV DS , AX
        
        PUSH N
        CALL FAR PTR DNUM
        POP SONUC
        
        RETF
    START ENDP
CODESG ENDS
       END START