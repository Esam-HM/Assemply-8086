STAKSG SEGMENT PARA STACK 'STACK'
    DW 64 DUP(?)
STAKSG ENDS

DATASG SEGMENT PARA 'DATA'
    SONUC DW ?
    N     DW 10
DATASG ENDS


CODESG SEGMENT PARA 'CODE'
    ASSUME SS:STAKSG , DS:DATASG , CS:CODESG   
BASLA PROC FAR
	PUSH DS
	XOR AX , AX
	PUSH AX
        
	MOV AX , DATASG
	MOV DS , AX

	PUSH N
	CALL FAR PTR DNUM
	POP SONUC
        
	RETF
BASLA ENDP

DNUM PROC FAR
    PUSH BP
    PUSH DI
    PUSH SI
         
    MOV BP , SP
    MOV AX , [BP+10]
    MOV DI , AX


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

 
    JMP EXIT
         
    J12:
     MOV AX , 1
   
     JMP EXIT
    J0:  
     MOV AX , 0
    
    
   EXIT: 
      MOV [BP+10] , AX
      POP SI
      POP DI
      POP BP
      RETF   
        
DNUM ENDP

CODESG ENDS
	END BASLA
