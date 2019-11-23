Label_Get_Memory:
    mov	ax,	1301h
	mov	bx,	000Fh
	mov	dx,	0400h		;row 4
	mov	cx,	24
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	StartGetMemStructMessage
	int	10h

    mov	ebx,	0
	mov	ax,	0x00
	mov	es,	ax
	mov	di,	MemoryStructBufferAddr

Label_Get_Mem_Struct:
    mov	    eax,	0x0E820
	mov	    ecx,	20
	mov	    edx,	0x534D4150
	int	    15h
	jc	    Label_Get_Mem_Fail
	add	    di,	20

    cmp	    ebx,	0
	jne	    Label_Get_Mem_Struct
	jmp	    Label_Get_Mem_OK

Label_Get_Mem_Fail:; 显示失败
	mov	    ax,	1301h
	mov	    bx,	008Ch
	mov	    dx,	0500h		;row 5
	mov	    cx,	23
	push	ax
	mov	    ax,	ds
	mov	    es,	ax
	pop	    ax
	mov	    bp,	GetMemStructErrMessage
	int	    10h
	jmp	    $

Label_Get_Mem_OK:; 先显示成功
	mov	    ax,	1301h
	mov	    bx,	000Fh
	mov	    dx,	0600h		;row 6
	mov	    cx,	29
	push	ax
	mov	    ax,	ds
	mov	    es,	ax
	pop	    ax
	mov	    bp,	GetMemStructOKMessage
	int	    10h	

;   开始获取信息
	mov	    ax,	1301h
	mov	    bx,	000Fh
	mov	    dx,	0800h		;row 8
	mov	    cx,	23
	push	ax
	mov	    ax,	ds
	mov	    es,	ax
	pop	    ax
	mov	    bp,	StartGetSVGAVBEInfoMessage
	int	    10h

	mov	    ax,	0x00
	mov	    es,	ax
	mov	    di,	0x8000
	mov	    ax,	4F00h

	int	10h

	cmp	ax,	004Fh

	jz	.KO

;=======	Fail

	mov	ax,	1301h
	mov	bx,	008Ch
	mov	dx,	0900h		;row 9
	mov	cx,	23
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	GetSVGAVBEInfoErrMessage
	int	10h

	jmp	$

.KO:

	mov	ax,	1301h
	mov	bx,	000Fh
	mov	dx,	0A00h		;row 10
	mov	cx,	29
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	GetSVGAVBEInfoOKMessage
	int	10h

;=======	Get SVGA Mode Info

	mov	ax,	1301h
	mov	bx,	000Fh
	mov	dx,	0C00h		;row 12
	mov	cx,	24
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	StartGetSVGAModeInfoMessage
	int	10h


	mov	ax,	0x00
	mov	es,	ax
	mov	si,	0x800e

	mov	esi,	dword	[es:si]
	mov	edi,	0x8200

Label_SVGA_Mode_Info_Get:
	mov	cx,	word	[es:esi]
;=======	display SVGA mode information

	push	ax
	
	mov	ax,	00h
	mov	al,	ch
	call	Label_DispAL

	mov	ax,	00h
	mov	al,	cl	
	call	Label_DispAL
	
	pop	ax

;=======
	
	cmp	cx,	0FFFFh
	jz	Label_SVGA_Mode_Info_Finish

	mov	ax,	4F01h
	int	10h

	cmp	ax,	004Fh

	jnz	Label_SVGA_Mode_Info_FAIL	

	add	esi,	2
	add	edi,	0x100

	jmp	Label_SVGA_Mode_Info_Get
		
Label_SVGA_Mode_Info_FAIL:

	mov	ax,	1301h
	mov	bx,	008Ch
	mov	dx,	0D00h		;row 13
	mov	cx,	24
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	GetSVGAModeInfoErrMessage
	int	10h

Label_SET_SVGA_Mode_VESA_VBE_FAIL:
	jmp	$

Label_SVGA_Mode_Info_Finish:

	mov	ax,	1301h
	mov	bx,	000Fh
	mov	dx,	0E00h		;row 14
	mov	cx,	30
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	GetSVGAModeInfoOKMessage
	int	10h
	jmp Start_To_Protect

[BITS 16]
;   以 16 进制显示 AL 中的数字
Label_DispAL:

	push	ecx
	push	edx
	push	edi
	
	mov	edi,	[DisplayPosition]
	mov	ah,	0Fh
	mov	dl,	al
	shr	al,	4
	mov	ecx,	2
.begin:

	and	al,	0Fh
	cmp	al,	9
	ja	.1
	add	al,	'0'
	jmp	.2
.1:

	sub	al,	0Ah
	add	al,	'A'
.2:

	mov	[gs:edi],	ax
	add	edi,	2
	
	mov	al,	dl
	loop	.begin

	mov	[DisplayPosition],	edi

	pop	edi
	pop	edx
	pop	ecx
	
	ret
