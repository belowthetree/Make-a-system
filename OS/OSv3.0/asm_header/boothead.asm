	OEMNAME 		db "TISUboot"	; 厂商名
	BytesPerSector 	dw 512	; 每扇区字节数
	SectorPerCluster db 8	; 每簇扇区数
	ResvSectors 	dw 1		; 保留扇区数 32
	NumFAT 			db 2		; 文件分配表FAT数 2
	RootDir			dw 0		; 根目录 0
	SmallSector 	dw 0		; 小扇区数 0
	Media 	 		db 0xf0			; 介质类型
	RvsSectorPerFAT dw 0	; 每个FAT扇区数 0
	SectorPerTrk 	dw 63		; 每磁道扇区数
	heads 			dw 9				; 磁头数
	HidSector 	 	dw 0, 0		; 隐藏扇区数
	TotalSector 	dw 2880, 0  ; 总扇区数
	SectorPerFAT 	dw 9, 0 	; FAT扇区数
	extFlag 	 	dw 0 		; 扩展标志 0
	fsversion 	 	dw 0 		; 文件系统版本 0
	RootCluster 	dw 2, 0 	; 根目录所在簇 2
	fsInfo 	 		dw 1 		; 文件系统信息扇区
	BackupBootSec 	dw 6		; 备份启动扇区
	Reserved 		dw 0, 0, 0, 0, 0, 0 ; 保留 0
	DrvType 		db 0x80 		; 软盘 0，硬盘 80h
	Rev1 			db 0
	BootSig 	 	db 0x29 		; 标志 28 或 29
	ID 	 			dw 1, 1
	DiskName 		db "BootDisk   "
	FSID 			db "FAT32   "