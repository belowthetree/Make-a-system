SecCnt equ 0x1f2
LBALow equ 0x1f3
LBAMID equ 0x1f4
LBAHIGH equ 0x1f5
DEVICE equ 0x1f6
COMMAND equ 0x1f7
DISKDATA equ 0x1f0

; ecx 扇区数
; ebx 逻辑号
; di 写入位置
Read_Disk:
	; 读取扇区数
	mov al, cl
	mov edx, SecCnt
	out dx, al

	; LBA 地址
	; 低地址
	mov al, bl
	mov edx, LBALow
	out dx, al
	; 中地址
	shr ebx, 8
	mov al, bl
	mov edx, LBAMID
	out dx, al
	; 高地址
	shr ebx, 8
	mov al, bl
	mov edx, LBAHIGH
	out dx, al
	; 高四位
	shr ebx, 8
	and bl, 0x0f
	or 	bl, 0xe0
	mov al, bl
	mov edx, DEVICE
	out dx, al

	; 设置 command，读取扇区
	mov al, 0x20
	mov edx, COMMAND
	out dx, al

.check_disk:
	; 检查磁盘状态
	in  al, dx
	and al,0x88         ;第4位为1表示硬盘准备好数据传输，第7位为1表示硬盘忙
    cmp al,0x08
	jne .check_disk

	;
	mov eax, ecx
	mov edx, 256
	mul edx
	mov ecx, eax
	mov ebx, edi
	mov edx, DISKDATA

.read_data:
	in  ax, dx
	mov word [ebx], ax
	add ebx, 2

	loop .read_data
	ret
