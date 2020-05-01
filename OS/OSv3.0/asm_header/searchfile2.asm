; esi 存放文件名地址
; edi 目标位置
TmpFile equ 0x500
BaseOfRootdir equ 19
SectorOfRoot equ 14
DataOffset	equ 17

Search_File:
	mov [.Desindex], edi
	push esi
	mov ecx, 1
	mov ebx, BaseOfRootdir
	mov edi, TmpFile
	call Read_Disk

	pop esi
	mov ecx, 0
	mov edx, 0
	mov edi, TmpFile

.read_one_sector: ; 读取一个扇区，一个扇区16个表项，用 ecx 计数
	cmp ecx, 16
	je  .one_sector_finish
	add edi, 32
	inc ecx

	mov ebx, -1

.cmp_name:		; 比较文件名，长度为11
	inc ebx
	cmp ebx, 11
	jz  .find_filename
	mov al, byte [esi + ebx]
	cmp al, byte [edi + ebx]
	jz  .cmp_name
	jmp .read_one_sector

.one_sector_finish: ; edx 记录已读目录项数
	inc edx
	cmp edx, SectorOfRoot
	jz  .search_fail
	push esi
	push edx

	mov ebx, BaseOfRootdir
	add ebx, edx
	mov edi, TmpFile
	call Read_Disk

	pop edx
	mov ecx, 0
	mov edi, TmpFile
	pop esi

	jmp .read_one_sector

.find_filename: ; 查找成功后找寻起始簇号
	add edi, 0x1a
	mov ax, word [edi]
    mov [.FATEntry], eax

    call .copy_file

    ret

.copy_file:
	; 先将 FAT 表放进 0x500
	mov si, word [.FATEntry]
	push si
	mov ecx, 9
	mov ebx, 1
	mov edi, TmpFile
	call Read_Disk
	pop si
	and esi, 0xff
	mov ebx, esi
.copy:
	push ebx
	add ebx, SectorOfRoot
	add ebx, DataOffset

	mov ecx, 1
	mov edi, [.Desindex]
	call Read_Disk
	add word [.Desindex], 0x200

	pop ecx
	mov eax, 3
	mul ecx
	mov ebx, 2
	div ebx
	add eax, TmpFile
	mov edi, eax
	mov ebx, [edi]
	cmp edx, 0
	jz  .next
	shr ebx, 4
.next:
	; 如果属于正常文件，则继续进行读取
	and ebx, 0x0fff
	cmp ebx, 0xff8
	jge .copy_finish
	jmp .copy


.copy_finish:
    ret

.search_fail:
	ret


.ReadFail db "Read Fail", 0
.ReadSuccess db "Read Success", 0
.CopyFinish db "CopyFinish", 0
.FATEntry dd 0
.Desindex dd 0