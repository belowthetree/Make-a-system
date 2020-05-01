;描述符类型
DA_32		EQU	4000h	; 32 位段
DA_LIMIT_4K	EQU	8000h	; 段界限粒度为 4K 字节
DA_CR		EQU	9Ah	; 存在的可执行可读代码段属性值

; 存储段描述符类型
DA_DR		EQU	90h	; 存在的只读数据段类型值
DA_DRW		EQU	92h	; 存在的可读写数据段属性值
DA_DRWA		EQU	93h	; 存在的已访问可读写数据段类型值
DA_C		EQU	98h	; 存在的只执行代码段属性值
; 系统段描述符类型值说明
;----------------------------------------------------------------------------
DA_LDT		EQU	  82h	; 局部描述符表段类型值
DA_TaskGate	EQU	  85h	; 任务门类型值
DA_386TSS	EQU	  89h	; 可用 386 任务状态段类型值
DA_386CGate	EQU	  8Ch	; 386 调用门类型值
DA_386IGate	EQU	  8Eh	; 386 中断门类型值
DA_386TGate	EQU	  8Fh	; 386 陷阱门类型值

DA_DPL0		EQU	  00h	; DPL = 0
DA_DPL1		EQU	  20h	; DPL = 1
DA_DPL2		EQU	  40h	; DPL = 2
DA_DPL3		EQU	  60h	; DPL = 3
;描述符
%macro Descriptor 3     ;   段基址    段界限    段属性
    dw  %2 & 0xffff
    dw  %1 & 0xffff
    db  (%1 >> 16) & 0xff
    dw  ((%2 >> 8) & 0xf00) | (%3 & 0xf0ff)
    db  (%1 >> 24) & 0xff
%endmacro

;门
%macro Gate 4
	dw	%2 & 0xffff				; 偏移 1				(2 字节)
	dw	%1					; 选择子				(2 字节)
	dw	(%3 & 0x1f) | ((%4 << 8) & 0xff00)	; 属性					(2 字节)
	dw	((%2 >> 16) & 0xffff)			; 偏移 2				(2 字节)
%endmacro

