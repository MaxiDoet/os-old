section .data
int_counter db 0

section .text
extern kpanic

extern int_handler
int_common_handler:
	; Save all registers
	push ebp
	push edi
	push esi

	push edx
	push ecx
	push ebx
	push eax

	push esp

	call int_handler
	;add esp, 4
	mov esp, eax

	; Restore all registers
	pop eax
	pop ebx
	pop ecx
	pop edx

	pop esi
	pop edi
	pop ebp

	add esp, 8
	;add esp, 4

	; Return
	iret

global int_handler_0
int_handler_0:
push 0
push 0
jmp int_common_handler

global int_handler_1
int_handler_1:
push 0
push 1
jmp int_common_handler

global int_handler_2
int_handler_2:
push 0
push 2
jmp int_common_handler

global int_handler_3
int_handler_3:
push 0
push 3
jmp int_common_handler

global int_handler_4
int_handler_4:
push 0
push 4
jmp int_common_handler

global int_handler_5
int_handler_5:
push 0
push 5
jmp int_common_handler

global int_handler_6
int_handler_6:
push 0
push 6
jmp int_common_handler

global int_handler_7
int_handler_7:
push 0
push 7
jmp int_common_handler

global int_handler_8
int_handler_8:
push 0
push 8
jmp int_common_handler

global int_handler_9
int_handler_9:
push 0
push 9
jmp int_common_handler

global int_handler_10
int_handler_10:
push 0
push 10
jmp int_common_handler

global int_handler_11
int_handler_11:
push 0
push 11
jmp int_common_handler

global int_handler_12
int_handler_12:
push 0
push 12
jmp int_common_handler

global int_handler_13
int_handler_13:
push 0
push 13
jmp int_common_handler

global int_handler_14
int_handler_14:
push 0
push 14
jmp int_common_handler

global int_handler_15
int_handler_15:
push 0
push 15
jmp int_common_handler

global int_handler_16
int_handler_16:
push 0
push 16
jmp int_common_handler

global int_handler_17
int_handler_17:
push 0
push 17
jmp int_common_handler

global int_handler_18
int_handler_18:
push 0
push 18
jmp int_common_handler

global int_handler_19
int_handler_19:
push 0
push 19
jmp int_common_handler

global int_handler_20
int_handler_20:
push 0
push 20
jmp int_common_handler

global int_handler_21
int_handler_21:
push 0
push 21
jmp int_common_handler

global int_handler_22
int_handler_22:
push 0
push 22
jmp int_common_handler

global int_handler_23
int_handler_23:
push 0
push 23
jmp int_common_handler

global int_handler_24
int_handler_24:
push 0
push 24
jmp int_common_handler

global int_handler_25
int_handler_25:
push 0
push 25
jmp int_common_handler

global int_handler_26
int_handler_26:
push 0
push 26
jmp int_common_handler

global int_handler_27
int_handler_27:
push 0
push 27
jmp int_common_handler

global int_handler_28
int_handler_28:
push 0
push 28
jmp int_common_handler

global int_handler_29
int_handler_29:
push 0
push 29
jmp int_common_handler

global int_handler_30
int_handler_30:
push 0
push 30
jmp int_common_handler

global int_handler_31
int_handler_31:
push 0
push 31
jmp int_common_handler

global int_handler_32
int_handler_32:
push 0
push 32
jmp int_common_handler

global int_handler_33
int_handler_33:
push 0
push 33
jmp int_common_handler

global int_handler_34
int_handler_34:
push 0
push 34
jmp int_common_handler

global int_handler_35
int_handler_35:
push 0
push 35
jmp int_common_handler

global int_handler_36
int_handler_36:
push 0
push 36
jmp int_common_handler

global int_handler_37
int_handler_37:
push 0
push 37
jmp int_common_handler

global int_handler_38
int_handler_38:
push 0
push 38
jmp int_common_handler

global int_handler_39
int_handler_39:
push 0
push 39
jmp int_common_handler

global int_handler_40
int_handler_40:
push 0
push 40
jmp int_common_handler

global int_handler_41
int_handler_41:
push 0
push 41
jmp int_common_handler

global int_handler_42
int_handler_42:
push 0
push 42
jmp int_common_handler

global int_handler_43
int_handler_43:
push 0
push 43
jmp int_common_handler

global int_handler_44
int_handler_44:
push 0
push 44
jmp int_common_handler

global int_handler_45
int_handler_45:
push 0
push 45
jmp int_common_handler

global int_handler_46
int_handler_46:
push 0
push 46
jmp int_common_handler

global int_handler_47
int_handler_47:
push 0
push 47
jmp int_common_handler

global int_handler_48
int_handler_48:
push 0
push 48
jmp int_common_handler

global int_handler_49
int_handler_49:
push 0
push 49
jmp int_common_handler

global int_handler_50
int_handler_50:
push 0
push 50
jmp int_common_handler

global int_handler_51
int_handler_51:
push 0
push 51
jmp int_common_handler

global int_handler_52
int_handler_52:
push 0
push 52
jmp int_common_handler

global int_handler_53
int_handler_53:
push 0
push 53
jmp int_common_handler

global int_handler_54
int_handler_54:
push 0
push 54
jmp int_common_handler

global int_handler_55
int_handler_55:
push 0
push 55
jmp int_common_handler

global int_handler_56
int_handler_56:
push 0
push 56
jmp int_common_handler

global int_handler_57
int_handler_57:
push 0
push 57
jmp int_common_handler

global int_handler_58
int_handler_58:
push 0
push 58
jmp int_common_handler

global int_handler_59
int_handler_59:
push 0
push 59
jmp int_common_handler

global int_handler_60
int_handler_60:
push 0
push 60
jmp int_common_handler

global int_handler_61
int_handler_61:
push 0
push 61
jmp int_common_handler

global int_handler_62
int_handler_62:
push 0
push 62
jmp int_common_handler

global int_handler_63
int_handler_63:
push 0
push 63
jmp int_common_handler

global int_handler_64
int_handler_64:
push 0
push 64
jmp int_common_handler

global int_handler_65
int_handler_65:
push 0
push 65
jmp int_common_handler

global int_handler_66
int_handler_66:
push 0
push 66
jmp int_common_handler

global int_handler_67
int_handler_67:
push 0
push 67
jmp int_common_handler

global int_handler_68
int_handler_68:
push 0
push 68
jmp int_common_handler

global int_handler_69
int_handler_69:
push 0
push 69
jmp int_common_handler

global int_handler_70
int_handler_70:
push 0
push 70
jmp int_common_handler

global int_handler_71
int_handler_71:
push 0
push 71
jmp int_common_handler

global int_handler_72
int_handler_72:
push 0
push 72
jmp int_common_handler

global int_handler_73
int_handler_73:
push 0
push 73
jmp int_common_handler

global int_handler_74
int_handler_74:
push 0
push 74
jmp int_common_handler

global int_handler_75
int_handler_75:
push 0
push 75
jmp int_common_handler

global int_handler_76
int_handler_76:
push 0
push 76
jmp int_common_handler

global int_handler_77
int_handler_77:
push 0
push 77
jmp int_common_handler

global int_handler_78
int_handler_78:
push 0
push 78
jmp int_common_handler

global int_handler_79
int_handler_79:
push 0
push 79
jmp int_common_handler

global int_handler_80
int_handler_80:
push 0
push 80
jmp int_common_handler

global int_handler_81
int_handler_81:
push 0
push 81
jmp int_common_handler

global int_handler_82
int_handler_82:
push 0
push 82
jmp int_common_handler

global int_handler_83
int_handler_83:
push 0
push 83
jmp int_common_handler

global int_handler_84
int_handler_84:
push 0
push 84
jmp int_common_handler

global int_handler_85
int_handler_85:
push 0
push 85
jmp int_common_handler

global int_handler_86
int_handler_86:
push 0
push 86
jmp int_common_handler

global int_handler_87
int_handler_87:
push 0
push 87
jmp int_common_handler

global int_handler_88
int_handler_88:
push 0
push 88
jmp int_common_handler

global int_handler_89
int_handler_89:
push 0
push 89
jmp int_common_handler

global int_handler_90
int_handler_90:
push 0
push 90
jmp int_common_handler

global int_handler_91
int_handler_91:
push 0
push 91
jmp int_common_handler

global int_handler_92
int_handler_92:
push 0
push 92
jmp int_common_handler

global int_handler_93
int_handler_93:
push 0
push 93
jmp int_common_handler

global int_handler_94
int_handler_94:
push 0
push 94
jmp int_common_handler

global int_handler_95
int_handler_95:
push 0
push 95
jmp int_common_handler

global int_handler_96
int_handler_96:
push 0
push 96
jmp int_common_handler

global int_handler_97
int_handler_97:
push 0
push 97
jmp int_common_handler

global int_handler_98
int_handler_98:
push 0
push 98
jmp int_common_handler

global int_handler_99
int_handler_99:
push 0
push 99
jmp int_common_handler
