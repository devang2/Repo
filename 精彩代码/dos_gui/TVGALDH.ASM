;;**********************************************************************;;
;;																		;;
;; File: _drawUtl.asm													;;
;; Routines: _readDAC,_writeDAC											;;
;;																		;;
;;**********************************************************************;;

.model large
LPROG EQU 1
.code

public _read_DAC
public _write_DAC
public __selectVGABitPlane

SEQUENCER_REG		EQU 03c4h
EXTANDED_REG		EQU 03c4h
PAGE_INDEX			EQU 0EH

VIDEO_BUFFER_SEG	EQU 0a000h

VIDEO_PAGES	       	EQU 4

;;----------------------------------------------------------------------;;
;; _read_DAC. It's .CPP call declaration is								;;
;;	extern "C" far read_DAC ( int start, int count, 					;;
;;											char far * DACary );		;;
;; It read count (1..256) DAC registers from NO.start (0..255), the 	;;
;;	value is save to mem block pointed out by char far * DACary			;;
;;	with the following layout: R1, G1, B1,  R2, G2, B2,  R3, G3, B3, ...;;
;;----------------------------------------------------------------------;;
Arg_Start equ word ptr [ bp + 4 ]
Arg_Count equ word ptr [ bp + 6 ]
Arg_ArrayPtr equ dword ptr [ bp + 8 ]

ifdef LPROG		;; LPROG - large program. see "bc\ctrl\rtlinc\rules.asi"
	Arg_Start equ word ptr [ bp + 6 ]
	Arg_Count equ word ptr [ bp + 8 ]
	Arg_ArrayPtr equ dword ptr [ bp + 10 ]
endif

_read_DAC	PROC FAR
	PUSH BP
	MOV BP, SP
	PUSH ES
	PUSH DS
	PUSH DI
	PUSH SI

	LES DI, Arg_ArrayPtr
	MOV AX, Arg_Start
	MOV CX, Arg_Count
	MOV DX, 3C7H
	OUT DX, AL
	INC DX
	INC DX

DAC_In_Loop:
	IN AL, DX
	STOSB
	IN AL, DX
	STOSB
	IN AL, DX
	STOSB
	LOOP DAC_In_Loop

	POP SI
	POP DI
	POP DS
	POP ES

	MOV SP, BP
	POP BP
	RET
_read_DAC	ENDP

;;----------------------------------------------------------------------;;
;; _write_DAC. It's .CPP call declaration is							;;
;;	extern "C" far write_DAC ( int start, int count, 					;;
;;											char far * DACary );		;;
;; It write count (1..256) DAC registers from NO.start (0..255), the 	;;
;;	value is get from mem block pointed out by char far * DACary		;;
;;	with the following layout: R1, G1, B1,  R2, G2, B2,  R3, G3, B3, ...;;
;;----------------------------------------------------------------------;;
Arg_Start equ word ptr [ bp + 4 ]
Arg_Count equ word ptr [ bp + 6 ]
Arg_ArrayPtr equ dword ptr [ bp + 8 ]

ifdef LPROG
	Arg_Start equ word ptr [ bp + 6 ]
	Arg_Count equ word ptr [ bp + 8 ]
	Arg_ArrayPtr equ dword ptr [ bp + 10 ]
endif

_write_DAC	PROC FAR
	PUSH BP
	MOV BP, SP
	PUSH ES
	PUSH DS
	PUSH DI
	PUSH SI

	LDS SI, Arg_ArrayPtr
	MOV AX, Arg_Start
	MOV CX, Arg_Count
	MOV DX, 3C8H
	OUT DX, AL
	INC DX

DAC_Out_Loop:
	LODSB
	OUT DX, AL
	LODSB
	OUT DX, AL
	LODSB
	OUT DX, AL
	LOOP DAC_Out_Loop

	POP SI
	POP DI
	POP DS
	POP ES

	MOV SP, BP
	POP BP
	RET
_write_DAC	ENDP

;;----------------------------------------------------------------------;;
;; __selectVGABitPlane It's .CPP call declaration is					;;
;;	extern "C" far _selectVGABitPlane ( int planeNo, int cmd );			;;
;;																		;;
;; It select 1 VGA bit plane for the following read or write.			;;
;;  When the cmd not zero, it select a plane for write, or for read 	;;
;;----------------------------------------------------------------------;;
Arg_planeNo equ word ptr [ bp + 4 ]
Arg_cmd equ word ptr [ bp + 6 ]

ifdef LPROG
	Arg_planeNo equ word ptr [ bp + 6 ]
	Arg_cmd equ word ptr [ bp + 8 ]
endif

__selectVGABitPlane	PROC FAR
	PUSH BP
	MOV BP, SP

	MOV AX, Arg_cmd
	OR AX, AX
	JNE selectWritePlane
;;selectReadPlane
	MOV DX, 3ceH
	MOV AX, Arg_planeNo
	XCHG AL, AH
	AND AH, 3
	MOV AL, 4
	OUT DX, AX
	JMP return
selectWritePlane:
	MOV DX, 3c4H
	MOV CX, Arg_planeNo
	AND CX, 3
	MOV AX, 102H
	SHL AH, CL
	OUT DX, AX

return:
	MOV SP, BP
	POP BP
	RET
__selectVGABitPlane	ENDP
	END