;--------------------------------------------------------
;                   XMSA.ASM
;        Extended memory control functions.
;              Complie with TASM 2.0
;         Write by Tanwenhong 1996.12.20
;--------------------------------------------------------

_AOFF			equ		6	;FAR STACK OFFSET

procstart	MACRO		name
		PUBLIC name
name	proc	far
ENDM

procend		MACRO		name
name	endp
ENDM

XMS_TEXT	SEGMENT BYTE PUBLIC 'CODE'
		ASSUME CS:XMS_TEXT,DS:_DATA

;---------------------------------------------------------
;this function initalizes the driver
;	called as
;	init_xms();
;		if succes return 1
;		else return 0 no driver
;---------------------------------------------------------
procstart	_init_xms
		push bp
		mov bp,sp

		mov ax,4300h
		int 2fh
		cmp al,80h
		je init_xms_fun

		xor ax,ax
		mov CS:[XMS_ErrNo],0ffh
		jmp init_xms_ret

init_xms_fun:
		mov ax,4310h
		int 2fh
		mov word ptr cs:[_CONTOFF],bx
		mov word ptr cs:[_CONTSEG],es

init_xms_ret:

		pop bp
		ret
procend	_init_xms

;----------------------------------------------------
;This function get the XMS version
;	Call as:
;	_xms_ver()
;		if succes return version else return 0
;----------------------------------------------------
procstart	_xms_ver

		xor	ax,ax
		Call	CS:[CONTROL]

		or		ax,ax
		jz		xms_ver_err

		jmp	xms_ver_ret

xms_ver_err:
		mov	Cs:[XMS_ErrNo],BL

xms_ver_ret:
		ret

procend		_xms_ver

;---------------------------------------------------------
;This function moves extended memory
;		Called as:
;	  	move_xmem(p)
;p	=	pointer to move structure(protyps in XMS.H)
;
;Retruns 1 if successfull	else return 0
;NOTE:
;		Move extented memory
;---------------------------------------------------------
procstart _move_xmem

		ARG xmove_off:WORD,xmove_seg:WORD

		push bp
		mov bp,sp

		push si
		push di
		push ds

		mov si,[xmove_off]	;OFFSET OF STRUCTURE
		mov ds,[xmove_seg]	;SEGMENT OF STRUCTURE

		mov ah,0Bh			;do the move
		call cs:[CONTROL]
		or		ax,ax
		jz		move_xmem_err

		mov	ax,1
		jmp	move_xmem_ret
move_xmem_err:
		mov CS:[XMS_ErrNo],BL
move_xmem_ret:

		pop ds
		pop di
		pop si
		pop bp
		ret
procend		_move_xmem

;-------------------------------------------------------
;This function free extended memory	block
;	called as
;	free_xmem(h);
;	int h;	//handel to deallocate
;				//return 1 if successfull
;				//else return 0
;-------------------------------------------------------
procstart 	_free_xmem

		push	bp
		mov 	bp,sp

		mov dx,[bp+_AOFF]	;handle
		mov ah,0Ah
		call cs:[CONTROL]	;BYE MEMORY...

		or		AX,AX
		jz		free_xmem_err
		mov	ax,1
		jmp	free_xmem_ret
free_xmem_err:
		mov CS:[XMS_ErrNo],BL
free_xmem_ret:

		pop bp
		ret
procend	_free_xmem

;----------------------------------------------------------
;		This function quaries the max extended memory
;block size and total free extended memory size
;	called as
;	int maxfree_xmb(void);
;	Return max extended free memory block size(Kb) if success
;else return 0 .
;-----------------------------------------------------------
procstart	_maxfree_xmb

		mov	ah,8
		Call	Cs:[CONTROL]

		or		ax,ax
		jnz maxfree_xmb_ret
		mov   cs:[XMS_ErrNo],bl
maxfree_xmb_ret:

		ret
procend	_maxfree_xmb

;----------------------------------------------------
;int totalfree_xmb()
;----------------------------------------------------
procstart	_totalfree_xmb

		mov	ah,8
		Call	Cs:[CONTROL]

		or		ax,ax
		jnz totalfree_xmb_suc

		mov   cs:[XMS_ErrNo],bl
		jmp	totalfree_xmb_ret

totalfree_xmb_suc:
		mov	ax,dx

totalfree_xmb_ret:
			ret
procend	_totalfree_xmb

;-----------------------------------------------------------
;This function allocates extended memory
;	called as
;	alloc_xmem(size);
;	int size;	//number of killobytes to allocate
;					//return handle of 0 if error
;					//else return Handle of allocated XMS
;-----------------------------------------------------------
procstart	_alloc_xmem

		push	bp
		mov 	bp,sp

		mov dx,[bp+_AOFF]		;DX:=Size of extended memory block
		mov ah,9          	;Function 9h Alloc extended memory block
		call cs:[CONTROL]		;BYE MEMORY...

		or ax,ax
		jz alloc_xmem_err

		mov ax,dx				;AX:=Handle of XMS block
		jmp alloc_xmem_ret
alloc_xmem_err:
		mov CS:[XMS_ErrNo],BL
alloc_xmem_ret:
		pop bp
		ret
procend		_alloc_xmem

;-----------------------------------------------------------------
;This function will alloc high memory block
;Call	as:
;       void far * alloc_umb(size16bytes)
;size: you want to alloc memory size(per 16 bytes)
;		if sucess return a far point else return NULL
;-----------------------------------------------------------------
procstart	_alloc_umb

                ARG     size16bytes:WORD

		push		bp
		mov		bp,sp

		mov		ah,10h
                mov             dx,size16bytes
		Call		CS:[CONTROL]
		or			ax,ax
		jz			alloc_umb_err
		mov		dx,bx
		xor		ax,ax
		jmp		alloc_umb_ret
alloc_umb_err:
		mov		CS:[XMS_ErrNo],BL
		xor		ax,ax
		xor		dx,dx
alloc_umb_ret:

		pop		bp
		ret

procend	_alloc_umb

;-----------------------------------------------------------------
;This function will free high memory block
;Call	as:
;	void far * free_umb(void far * umb_ptr)
;umb_ptr: the alloc high memory block point
;		if sucess return 1 else return 0
;-----------------------------------------------------------------
procstart	_free_umb

		ARG	umb_ptr_off:WORD,umb_ptr_seg:WORD

		push		bp
		mov		bp,sp

		mov		ah,11h
		mov		dx,umb_ptr_seg
		Call		CS:[CONTROL]
		or			ax,ax
		jz			alloc_umb_err
		mov		ax,1
		jmp		alloc_umb_ret
alloc_hbm_err:
		mov		CS:[XMS_ErrNo],BL
		xor		ax,ax
alloc_hbm_ret:

		pop		bp
		ret

procend	_free_umb


;----------------------------------------------------------------
;This function return the last error number
;Call as:
;	xms_errno()
;		All function of XMS will return a error number if function
;	is fail,you can get the errno by call this function.
;----------------------------------------------------------------
procstart	_xms_errno
		xor	ah,ah
		mov	Al,CS:[XMS_ErrNo]
		ret
procend		_xms_errno

;-------------------------------------------------------------
;This function converts a pointer to an intel long
;	Called as
;	long Ptr2Long(p)
;-------------------------------------------------------------
procstart	_Ptr2Long

		push bp
		mov bp,sp

		mov ax,[bp+_AOFF]		;offset of pointer
		mov dx,[BP+_AOFF+2]	;segment of pointer

		pop bp
		ret
procend		_Ptr2Long

;------------------------------------------------------------
;this function is a dummy return for unset procedures
;------------------------------------------------------------
_DUMMY	PROC 	FAR
			xor	ax,ax
			ret
_DUMMY	ENDP

;----------- D A T A  S E C T I O N ---------------

CONTROL	LABEL	DWORD
_CONTOFF	DW		_DUMMY
_CONTSEG	DW		XMS_TEXT

XMS_ErrNo	DB		0

XMS_TEXT		ENDS

DGROUP	GROUP	_DATA,_BSS
_DATA		SEGMENT WORD PUBLIC 'DATA'
_DATA		ENDS
_BSS		SEGMENT WORD PUBLIC 'BSS'
_BSS 		ENDS

			END
