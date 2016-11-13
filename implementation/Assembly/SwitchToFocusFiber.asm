PUBLIC SwitchToFocusFiber

.code

SwitchToFocusFiber PROC
	mov			rbx,qword ptr [rcx+8]
	mov			rsp,qword ptr [rcx+10h]
	mov			rbp,qword ptr [rcx+18h]
	mov			rsi,qword ptr [rcx+20h]
	mov			rdi,qword ptr [rcx+28h]
	mov			r12,qword ptr [rcx+30h]
	mov			r13,qword ptr [rcx+38h]
	mov			r14,qword ptr [rcx+40h]
	mov			r15,qword ptr [rcx+48h]
	mov			r8 ,qword ptr [rcx+50h]			; save the RIP's value
	ldmxcsr		dword ptr [rcx+58h]
	movdqa		xmm6,  xmmword ptr [rcx+60h]
	movdqa		xmm7,  xmmword ptr [rcx+70h]
	movdqa		xmm8,  xmmword ptr [rcx+80h]
	movdqa		xmm9,  xmmword ptr [rcx+90h]
	movdqa		xmm10, xmmword ptr [rcx+0A0h]
	movdqa		xmm11, xmmword ptr [rcx+0B0h]
	movdqa		xmm12, xmmword ptr [rcx+0C0h]
	movdqa		xmm13, xmmword ptr [rcx+0D0h]
	movdqa		xmm14, xmmword ptr [rcx+0E0h]
	movdqa		xmm15, xmmword ptr [rcx+0F0h]

	mov			qword ptr [rsp],r8				; store the RIP to the return addrass

	mov			rdx,qword ptr [rcx+100h]		; move the dest fiber's StackBase to the rdx
	mov			r8 ,qword ptr [rcx+108h]		; move the dest fiber's StackLimit to the r8
	;mov			r9 ,qword ptr [rcx+110h]		; move the dest fiber's ActivationContextStackPointer to the r9
	;mov			r10,qword ptr [rcx+118h]		; move the dest fiber's DeallocationStack to the r10
	;mov			r11,qword ptr [rcx+120h]		; move the dest fiber's FlsData to the r11

	mov			rax,qword ptr gs:[30h]			; save the current TEB's address to rax

	mov			qword ptr [rax+8],rdx			; store the new StackBase
	mov			qword ptr [rax+10h],r8			; store the new StackLimit
	mov			qword ptr [rax+20h],rcx			; store the new fiber's address into the TIB.FiberData
	;mov			qword ptr [rax+2C8h],r9			; store the new ActivationContextStackPointer
	;mov			qword ptr [rax+1478h],r10		; store the new DeallocationStack
	;mov			qword ptr [rax+17c8h],r11		; store the new FlsData

	ret
SwitchToFocusFiber ENDP

END
