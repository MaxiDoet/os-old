section .data
int_counter db 0

section .text
extern kpanic

global int_handler_0
int_handler_0:
call kpanic
hlt

global int_handler_1
int_handler_1:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 '
hlt

global int_handler_2
int_handler_2:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 '
hlt

global int_handler_3
int_handler_3:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 '
hlt

global int_handler_4
int_handler_4:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 '
hlt

global int_handler_5
int_handler_5:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 '
hlt

global int_handler_6
int_handler_6:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 '
hlt

global int_handler_7
int_handler_7:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 '
hlt

global int_handler_8
int_handler_8:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 '
hlt

global int_handler_9
int_handler_9:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 '
hlt

global int_handler_10
int_handler_10:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 0 '
hlt

global int_handler_11
int_handler_11:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 1 '
hlt

global int_handler_12
int_handler_12:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 2 '
hlt

global int_handler_13
int_handler_13:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 3 '
hlt

global int_handler_14
int_handler_14:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 4 '
hlt

global int_handler_15
int_handler_15:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 5 '
hlt

global int_handler_16
int_handler_16:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 6 '
hlt

global int_handler_17
int_handler_17:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 7 '
hlt

global int_handler_18
int_handler_18:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 8 '
hlt

global int_handler_19
int_handler_19:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'1 9 '
hlt

global int_handler_20
int_handler_20:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 0 '
hlt

global int_handler_21
int_handler_21:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 1 '
hlt

global int_handler_22
int_handler_22:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 2 '
hlt

global int_handler_23
int_handler_23:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 3 '
hlt

global int_handler_24
int_handler_24:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 4 '
hlt

global int_handler_25
int_handler_25:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 5 '
hlt

global int_handler_26
int_handler_26:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 6 '
hlt

global int_handler_27
int_handler_27:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 7 '
hlt

global int_handler_28
int_handler_28:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 8 '
hlt

global int_handler_29
int_handler_29:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'2 9 '
hlt

global int_handler_30
int_handler_30:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 0 '
hlt

global int_handler_31
int_handler_31:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 1 '
hlt

global int_handler_32
int_handler_32:
mov ax, 0x10
mov gs, ax
xor ax, ax
mov al, byte [gs:int_counter]
inc al
mov byte [gs:int_counter], al
mov bl, 18
div bl
;mov byte [gs:0xB8000], al
mov al, 0x20
out 0x20, al
iret

extern keyboard_irq_handler
global int_handler_33
int_handler_33:
;mov ax, 0x10
;mov gs, ax
;mov dword [gs:0xB8000],'3 3 '
;call keyboard_irq_handler
mov al, 0x20
out 0x20, al
iret

global int_handler_34
int_handler_34:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 4 '
hlt

global int_handler_35
int_handler_35:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 5 '
hlt

global int_handler_36
int_handler_36:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 6 '
hlt

global int_handler_37
int_handler_37:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 7 '
hlt

global int_handler_38
int_handler_38:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 8 '
hlt

global int_handler_39
int_handler_39:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'3 9 '
hlt

global int_handler_40
int_handler_40:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 0 '
hlt

global int_handler_41
int_handler_41:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 1 '
hlt

global int_handler_42
int_handler_42:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 2 '
hlt

global int_handler_43
int_handler_43:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 3 '
hlt

global int_handler_44
int_handler_44:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 4 '
hlt

global int_handler_45
int_handler_45:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 5 '
hlt

global int_handler_46
int_handler_46:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 6 '
hlt

global int_handler_47
int_handler_47:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 7 '
hlt

global int_handler_48
int_handler_48:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 8 '
hlt

global int_handler_49
int_handler_49:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'4 9 '
hlt

global int_handler_50
int_handler_50:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 0 '
hlt

global int_handler_51
int_handler_51:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 1 '
hlt

global int_handler_52
int_handler_52:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 2 '
hlt

global int_handler_53
int_handler_53:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 3 '
hlt

global int_handler_54
int_handler_54:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 4 '
hlt

global int_handler_55
int_handler_55:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 5 '
hlt

global int_handler_56
int_handler_56:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 6 '
hlt

global int_handler_57
int_handler_57:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 7 '
hlt

global int_handler_58
int_handler_58:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 8 '
hlt

global int_handler_59
int_handler_59:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'5 9 '
hlt

global int_handler_60
int_handler_60:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 0 '
hlt

global int_handler_61
int_handler_61:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 1 '
hlt

global int_handler_62
int_handler_62:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 2 '
hlt

global int_handler_63
int_handler_63:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 3 '
hlt

global int_handler_64
int_handler_64:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 4 '
hlt

global int_handler_65
int_handler_65:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 5 '
hlt

global int_handler_66
int_handler_66:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 6 '
hlt

global int_handler_67
int_handler_67:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 7 '
hlt

global int_handler_68
int_handler_68:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 8 '
hlt

global int_handler_69
int_handler_69:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'6 9 '
hlt

global int_handler_70
int_handler_70:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 0 '
hlt

global int_handler_71
int_handler_71:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 1 '
hlt

global int_handler_72
int_handler_72:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 2 '
hlt

global int_handler_73
int_handler_73:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 3 '
hlt

global int_handler_74
int_handler_74:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 4 '
hlt

global int_handler_75
int_handler_75:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 5 '
hlt

global int_handler_76
int_handler_76:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 6 '
hlt

global int_handler_77
int_handler_77:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 7 '
hlt

global int_handler_78
int_handler_78:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 8 '
hlt

global int_handler_79
int_handler_79:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'7 9 '
hlt

global int_handler_80
int_handler_80:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 0 '
hlt

global int_handler_81
int_handler_81:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 1 '
hlt

global int_handler_82
int_handler_82:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 2 '
hlt

global int_handler_83
int_handler_83:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 3 '
hlt

global int_handler_84
int_handler_84:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 4 '
hlt

global int_handler_85
int_handler_85:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 5 '
hlt

global int_handler_86
int_handler_86:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 6 '
hlt

global int_handler_87
int_handler_87:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 7 '
hlt

global int_handler_88
int_handler_88:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 8 '
hlt

global int_handler_89
int_handler_89:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'8 9 '
hlt

global int_handler_90
int_handler_90:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 0 '
hlt

global int_handler_91
int_handler_91:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 1 '
hlt

global int_handler_92
int_handler_92:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 2 '
hlt

global int_handler_93
int_handler_93:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 3 '
hlt

global int_handler_94
int_handler_94:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 4 '
hlt

global int_handler_95
int_handler_95:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 5 '
hlt

global int_handler_96
int_handler_96:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 6 '
hlt

global int_handler_97
int_handler_97:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 7 '
hlt

global int_handler_98
int_handler_98:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 8 '
hlt

global int_handler_99
int_handler_99:
mov ax, 0x10
mov gs, ax
mov dword [gs:0xB8000],'9 9 '
hlt
