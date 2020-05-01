; si 存放文件名地址
; di 目标位置
TmpFile equ 0x500
BaseOfRootDir equ 19
SectorOfRoot equ 14
DataOffset	equ 17

Search_File:
	mov [.DesIndex], di
	push si
	mov cx, 1
	mov bx, BaseOfRootDir
	mov di, TmpFile
	call Read_Disk

	pop si
	mov cx, 0
	mov dx, 0

.read_one_sector: ; 读取一个扇区，一个扇区16个表项，用 CX 计数
	mov di, TmpFile
	cmp cx, 16
	je  .one_sector_finish
	mov ax, 32
	mov bx, cx
	mul bx
	add di, ax
	inc cx

	mov bx, -1

.cmp_name:		; 比较文件名，长度为11
	inc bx
	cmp bx, 11
	jz  .find_filename
	mov al, byte [si + bx]
	cmp al, byte [di + bx]
	jz  .cmp_name
	jmp .read_one_sector

.one_sector_finish: ; dx 记录已读目录项数
	inc dx
	cmp dx, SectorOfRoot
	jz  .search_fail
	push si
	push cx
	push dx

	mov bx, BaseOfRootDir
	add bx, dx
	mov di, TmpFile
	call Read_Disk

	pop dx
	pop cx
	pop si

	jmp .read_one_sector

.find_filename: ; 查找成功后找寻起始簇号
	push di
	mov si, .ReadSuccess
	call .print
    pop di
	add di, 0x1a
	mov ax, word [di]
    mov [.FATEntry], ax

    call .copy_file

    ret

.copy_file:
	; 先将 FAT 表放进 0x500
	mov si, word [.FATEntry]
	push si
	mov cx, 9
	mov bx, 1
	mov di, TmpFile
	call Read_Disk
	pop si
	and si, 0xff
	mov bx, si
.copy:
	push bx
	add bx, SectorOfRoot
	add bx, DataOffset
	push bx
	; 读取一个扇区打印一个点
	mov ah, 0x0e
	mov al, '.'
	int 10h

	pop bx
	mov cx, 1
	mov di, [.DesIndex]
	call Read_Disk
	add word [.DesIndex], 0x200

	pop cx
	mov ax, 3
	mul cx
	mov bx, 2
	div bx
	add ax, TmpFile
	mov di, ax
	mov bx, word [di]
	; 判断奇偶性
	cmp dx, 0
	jz  .next
	shr bx, 4
.next:
	; 如果属于正常文件，则继续进行读取
	and bx, 0x0fff
	cmp bx, 0xff8
	jge .copy_finish
	jmp .copy


.copy_finish:
	mov si, .CopyFinish
    call .print
    ret

.search_fail:
	mov si, .ReadFail
	call .print
	ret

.print:
	mov ah, 0x0e
	mov al, [si]
	cmp al, 0
	jnz .continue
	ret
.continue:
	inc si
	int 10h
	jmp .print


.ReadFail db "Read Fail", 0
.ReadSuccess db "Read Success", 0
.CopyFinish db "CopyFinish", 0
.FATEntry dw 0
.DesIndex dw 0