;描述符类型
DA_32		EQU	4000h	; 32 位段
DA_LIMIT_4K	EQU	8000h	; 段界限粒度为 4K 字节
DA_CR		EQU	9Ah	; 存在的可执行可读代码段属性值

; 存储段描述符类型
DA_DR		EQU	90h	; 存在的只读数据段类型值
DA_DRW		EQU	92h	; 存在的可读写数据段属性值
DA_DRWA		EQU	93h	; 存在的已访问可读写数据段类型值
DA_C		EQU	98h	; 存在的只执行代码段属性值

DA_DPL0		EQU	  00h	; DPL = 0
DA_DPL1		EQU	  20h	; DPL = 1
DA_DPL2		EQU	  40h	; DPL = 2
DA_DPL3		EQU	  60h	; DPL = 3
;描述符
%macro Descriptor 3     ;   段基址    段界限    段属性
    dw  %2 & 0x0ffff
    dw  %1 & 0xffff
    db  (%1 >> 16) & 0xff
    dw  ((%2 >> 8) & 0xf00) | (%3 & 0xf0ff)
    db  (%1 >> 24) & 0xff
%endmacro

;门