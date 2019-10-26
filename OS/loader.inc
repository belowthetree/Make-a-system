;GDT
;                               段基址           段界限              段属性
; LABEL_GDT:          Descriptor      0,          0,                  0               ;空描述符
; LABEL_DESC_CODE32:  Descriptor      0,          SegCode32Len - 1,   DA_C + DA_32    ;非一致性代码
; LABEL_DESC_VIDEO:   Descriptor      0xb8000,    0xffff,             DA_DRW            ;显存
; KERNEL:             Descriptor      0,          0x3fffff,           DA_DRW

;LABEL_DESC_DATA:    Descriptor      0, SegDataLen - 1,     DA_DRW
; GDT结束

;选择子
SelectorVideo           equ 0x10
SelectorKernel          equ 0x18

BaseOfKernelFile        equ 0x8200
BaseOfStack             equ 0x0100
KernelEntry             equ 0x100400

