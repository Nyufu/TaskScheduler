PUBLIC Yield

extern StowCurrentFiberAndGiveNew:PROC

.code

Yield PROC
	mov			rax,qword ptr gs:[30h]			; save the current TEB's address to rax
	mov			rcx,qword ptr [rax+20h]			; save the current fiber's address to rcx

	mov			r8,qword ptr [rsp]

	mov			qword ptr [rcx+8],rbx
	mov			qword ptr [rcx+10h],rsp
	mov			qword ptr [rcx+18h],rbp
	mov			qword ptr [rcx+20h],rsi
	mov			qword ptr [rcx+28h],rdi
	mov			qword ptr [rcx+30h],r12
	mov			qword ptr [rcx+38h],r13
	mov			qword ptr [rcx+40h],r14
	mov			qword ptr [rcx+48h],r15
	mov			qword ptr [rcx+50h],r8
	stmxcsr		dword ptr [rcx+58h]
	;fnstcw		 word ptr [rcx+5Ch]
	movdqa		xmmword ptr [rcx+60h] ,xmm6
	movdqa		xmmword ptr [rcx+70h] ,xmm7
	movdqa		xmmword ptr [rcx+80h] ,xmm8
	movdqa		xmmword ptr [rcx+90h] ,xmm9
	movdqa		xmmword ptr [rcx+0A0h],xmm10
	movdqa		xmmword ptr [rcx+0B0h],xmm11
	movdqa		xmmword ptr [rcx+0C0h],xmm12
	movdqa		xmmword ptr [rcx+0D0h],xmm13
	movdqa		xmmword ptr [rcx+0E0h],xmm14
	movdqa		xmmword ptr [rcx+0F0h],xmm15

	mov			rbx,rax							; save the TEB to rbx becase the original value alread saved.
	call		StowCurrentFiberAndGiveNew		; call the StowCurrentFiberAndGiveNew with the current fiber in the rcx and after the return the rcx will contain the new fiber
	mov			rcx,rbx							; restore the TEB from rbx

	mov			rbx,qword ptr [rax+8]
	mov			rsp,qword ptr [rax+10h]
	mov			rbp,qword ptr [rax+18h]
	mov			rsi,qword ptr [rax+20h]
	mov			rdi,qword ptr [rax+28h]
	mov			r12,qword ptr [rax+30h]
	mov			r13,qword ptr [rax+38h]
	mov			r14,qword ptr [rax+40h]
	mov			r15,qword ptr [rax+48h]
	mov			r8 ,qword ptr [rax+50h]			; save the RIP's value
	ldmxcsr		dword ptr [rax+58h]
	movdqa		xmm6,  xmmword ptr [rax+60h]
	movdqa		xmm7,  xmmword ptr [rax+70h]
	movdqa		xmm8,  xmmword ptr [rax+80h]
	movdqa		xmm9,  xmmword ptr [rax+90h]
	movdqa		xmm10, xmmword ptr [rax+0A0h]
	movdqa		xmm11, xmmword ptr [rax+0B0h]
	movdqa		xmm12, xmmword ptr [rax+0C0h]
	movdqa		xmm13, xmmword ptr [rax+0D0h]
	movdqa		xmm14, xmmword ptr [rax+0E0h]
	movdqa		xmm15, xmmword ptr [rax+0F0h]

	mov			qword ptr [rsp],r8				; store the RIP for return there

	mov			rdx,qword ptr [rax+100h]		; move the dest fiber's StackBase to the rdx
	mov			r8 ,qword ptr [rax+108h]		; move the dest fiber's StackLimit to the r8
	;mov			r9 ,qword ptr [rax+110h]		; move the dest fiber's ActivationContextStackPointer to the r9
	;mov			r10,qword ptr [rax+118h]		; move the dest fiber's DeallocationStack to the r10
	;mov			r11,qword ptr [rax+120h]		; move the dest fiber's FlsData to the r11

	mov			qword ptr [rcx+8],rdx			; store the new StackBase
	mov			qword ptr [rcx+10h],r8			; store the new StackLimit
	mov			qword ptr [rcx+20h],rax			; store the new fiber's address into the TIB.FiberData
	;mov			qword ptr [rcx+2C8h],r9			; store the new ActivationContextStackPointer
	;mov			qword ptr [rcx+1478h],r10		; store the new DeallocationStack
	;mov			qword ptr [rcx+17c8h],r11		; store the new FlsData

	ret
Yield ENDP

END
