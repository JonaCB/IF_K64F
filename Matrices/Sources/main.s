/* This assembly file uses GNU syntax */
	.text
	.section	.rodata
	.align	2
	
contador: .space 2	@unsigned short contador
MatrizC: .space 6*4		@localidades de la memoria
	
.LC0:
	
	.text
	.align	2
	.global	main
	.type main function
	
main:
	ldr 	R8, =MatrizA		@App MatrizA
	ldr 	R3, =MatrizC		@App MatrizC
	mov 	R6, #RenA			@i
	
cicloi:
	mov 	R5, #ColB			@j
	ldr 	R2, =MatrizB		@App MatrizB
cicloj:
	mov 	R4, #ColARenB		@k
	eor		R0,R0			@R0 = 0
ciclok:
	ldr		R7,(R8)					@Elemento matriz A
	ldr		R1,(R2)					@Elemento matriz B
	mul		R7,R1					@R7 = R7*R8
	add		R0,R7					@acumulamos
	add		R8,#4					@Siguiente elemento en A
	add		R2, #4*ColB				@Siguiente elemento en B
	@subs	R4,#1	
	sub		R4,#1
	cmp 	R4,#0				
	bne		ciclok
	
	str		R0,(R3)					@guardamos resultado
	add		R3, #4					@incrementamos apuntador de C
	sub		R8, #ColARenB*4			@Ajuste ap Matriz A
	sub		R2, #(ColARenB*ColB-1)*4@Ajuste ap Matriz B
	@subs	r5, #1					@Ajustar apuntador j
	sub		r5, #1
	cmp		r5, #0;
	bne		cicloj
	add		R8, #ColARenB*4			@Reajuste porque ya se multiplicó ese renglon por todas las columnas
	
	@subs 	R6,#1
	sub		R6, #1
	cmp		R6, #0
	bne		cicloi
	
	mov r3, #0
	
	.align	2
.L3:
	.word	.LC0
	.end
	
	MatrizA: .word 1,2,3,4,5,6,7,8,9			@queda en Mem codigo, constante
	MatrizB: .word 1,2,3,4,5,6
	
	
	.set RenA, 3
	.set ColB, 2
	.set ColARenB,3
	.end
