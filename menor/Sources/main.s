/* This assembly file uses GNU syntax */
	.text
	.section	.rodata
	.align	2
	
	
.LC0:
	
	.text
	.align	2
	.global	main
	.type main function
	
main:
	push {r3, lr}
	add	r3, sp, #4
	

	
	
	mov	r3, #0
	mov	r0, r3
	pop {r3, pc}
	
	.align	2
.L3:
	.word	.LC0
	.end
