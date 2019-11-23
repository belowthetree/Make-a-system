 	mov word    [SectorNumber], SectorNumOfRootDirStart
Label_Search_In_Root_Dir_Begin:
    cmp     word [RootDirSizeForLoop], 0
    jz      Label_No_LoaderBin
    dec     word [RootDirSizeForLoop]
    mov     ax, 00h
    mov     es, ax
    mov     bx, 8000h
    mov     ax, [SectorNumber]
    mov     cl, 1
    call    Func_ReadOneSector
    mov     si, KernelFileName
    mov     di, 8000h
    cld
    mov     dx, 10h
;   每个目录项有 32 字节，一个簇有16个目录项
Label_Search_For_LoaderBin:
    cmp     dx, 0
    jz      Label_Goto_Next_Sector_In_Root_Dir
    dec     dx
    mov     cx, 11

Label_Cmp_FileName:
    cmp     cx, 0
    jz      Label_FileName_Found
    dec     cx
    lodsb
    cmp     al, byte [es:di]
    jz      Label_Go_On
    jmp     Label_Different

Label_Go_On:
    inc     di
    jmp     Label_Cmp_FileName

Label_Different:
    and     di, 0ffe0h
    add     di, 20h
    mov     si, KernelFileName
    jmp     Label_Search_For_LoaderBin

Label_Goto_Next_Sector_In_Root_Dir:
    add     word [SectorNumber], 1
    jmp     Label_Search_In_Root_Dir_Begin
;   没有找到 Loader.bin
Label_No_LoaderBin:
	jmp $
    mov     bx, 000ch
    mov     dx, 0100h
    mov     cx, 14
    mov     ax, ds
    mov     es, ax
    mov     ax, 1301h
    mov     bp, NoKernelMessage
    int     10h
    call    Stop

;   解析FAT表项，获取地址, AH=FAT表项号
Func_GetFATEntry:
    push    es
    push    bx
    push    ax
    mov     ax, 00
    mov     es, ax
    pop     ax
    mov     byte [Odd], 0
    mov     bx, 3
    mul     bx
    mov     bx, 2
    div     bx
    cmp     dx, 0
    jz      Label_Even
    mov     byte [Odd], 1

Label_Even:
    xor     dx, dx
    mov     bx, [BPB_BytesPerSec]
    div     bx
    ; 余数是 dx
    push    dx
    mov     bx, 8000h
    add     ax, SectorNumOfFAT1Start
    mov     cl, 2
    call    Func_ReadOneSector

    pop     dx
    add     bx, dx
    ; 获取第二个读取的扇区开头的字节（如果是fff，说明是结束扇区）
    mov     ax, [es:bx]
    cmp     byte [Odd], 1
    jnz     Label_Even_2
    shr     ax, 4

Label_Even_2:
    and     ax, 0fffh
    pop     bx
    pop     es
    ret

;   读取软盘：AX=起始扇区号，CL=扇区数，ES:BX=缓冲地址
Func_ReadOneSector:
    push    bp
    mov     bp, sp
    sub     esp, 2
    mov     byte [bp - 2], cl
    push    bx
    mov     bl, [BPB_SecPerTrk]
    div     bl
    ; ah 目标扇区号，al 目标磁道号
    inc     ah
    mov     cl, ah
    mov     dh, al
    shr     al, 1
    mov     ch, al
    and     dh, 1
    pop     bx
    mov     dl, [BS_DrvNum]
Label_Go_On_Reading:
    mov     ah, 2
    mov     al, byte [bp - 2]
    int     13h
    jc Label_Go_On_Reading
    add     esp, 2
    pop bp
    ret
;   找到后
Label_FileName_Found:
    mov     ax, RootDirSectors
    and     di, 0ffe0h
    ; 令 di 指向起始簇地址，获取起始簇号
    add     di, 01ah
    mov     cx, word [es:di]
    push    cx
    add     cx, ax
    add     cx, SectorBalance
    mov     eax, BaseTmpOfKernelAddr
    mov     es, eax
    mov     bx, OffsetTmpOfKernelFile
    mov     ax, cx

;   加载文件，读取位置已经压栈
Label_Go_On_Loading_File:
    push    ax
    push    bx
    mov     ah, 0eh
    mov     al, '.'
    mov     bl, 0fh
    int     10h
    pop     bx
    pop     ax

    mov     cl, 1
    call    Func_ReadOneSector
    pop     ax

;;;;;;;;;;;;;;;;;;;;;;;	
	push	cx
	push	eax
	push	fs
	push	edi
	push	ds
	push	esi
;   一次转移 512 个字节
	mov		cx,	200h
	mov 	ax, BaseOfKernelFile
	mov 	fs, ax
;   内核目标地址
	mov 	edi, dword [OffsetOfKernelFileCount]

	mov		ax,	BaseTmpOfKernelAddr
	mov		ds,	ax
	mov		esi,	OffsetTmpOfKernelFile

Label_Mov_Kernel:;      [0:src] ; [0x10000:dest]
	mov		al,	byte	[ds:esi]
	mov		byte	[fs:edi],	al
	inc		esi
	inc		edi
	loop	Label_Mov_Kernel

	mov		eax,	0x1000
	mov		ds,	eax
;   移动完成后增加目标地址
	mov		dword	[OffsetOfKernelFileCount],	edi

	pop		esi
	pop		ds
	pop		edi
	pop		fs
	pop		eax
	pop		cx

    ;读取
	call	Func_GetFATEntry
	cmp		ax,	0FFFh
	jz		Label_File_Loaded
	push	ax
	mov		dx,	RootDirSectors
	add		ax,	dx
	add		ax,	SectorBalance

	jmp		Label_Go_On_Loading_File

Label_File_Loaded:
	mov		ax, 0B800h
	mov		gs, ax
	mov		ah, 0Fh				; 0000: 黑底    1111: 白字
	mov		al, 'G'
	mov		[gs:((80 * 0 + 39) * 2)], ax	; 屏幕第 0 行, 第 39 列。

KillMotor:
	push	dx
	mov		dx,	03F2h
	mov		al,	0	
	out		dx,	al
	pop		dx

