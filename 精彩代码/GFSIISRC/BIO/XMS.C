/* -----------------------------------------------------------------------
	FILENAME:	EMS.C
	FUNCTION:	This file offers some EMB-memory accessing routines.
	AUTHOR:		Bob Kong
	UPDATE: 	1994/02/14.
	MODIFIED:	1994/04/26. By Bob Kong.
   ----------------------------------------------------------------------- */
#pragma   inline
#include  <dos.h>
#include  <stdio.h>
#include  <def.inc>

void far (*XMS_entry)(void);		/* the XMS-control(HIMEM.SYS) enterence */
/*  ------     EXTANED  MEMORY  FUNCTION  PROTOTYPES     ------  */
UC		XMS_init(void);
UI		EMB_size(void);

UI      EMB_alloc(UI size);
UC		EMB_free(UI EMB_handle);
UC		EMB_free_all(void);
UC		EMB_read(void far *destin, UI EM_handle, UL offset, UL length);
UC		EMB_write(UI EMB_handle, UL offset, void far *source, UL length);

UI      EMB_get_real_handle(UI EMB_handle);
UI		EMB_set_real_handle(UI real_handle);

typedef struct
	{
		UL length;
		UI src_handle;
		UL src_offset;
		UI dst_handle;
		UL dst_offset;
	}XMS_STRUCT;
static XMS_STRUCT xms_ctrl;
static UI Real_handle[MAX_EMB_HANDLES];

/* -----------------------------------------------------------------------
	FUNCTION:	check if XMS (offered by HIMEM.SYS etc.) is installed.
				if installed, get the XMS-controller's enterence address.
	INPUT:		None.
	OUTPUT:		If installed, set the global pointer: XMS_entry,
				then return TRUE.
				If not, return FALSE.
   ----------------------------------------------------------------------- */
UC XMS_init(void)
{
UI loop;

	asm mov ax, 0x4300
	asm int 0x2f
	asm cmp al, 0x80
	asm jne XMS_not_present
	asm mov ax, 0x4310
	asm int 0x2f
	XMS_entry = MK_FP(_ES, _BX);
	for ( loop=0; loop<MAX_EMB_HANDLES; loop++)
		Real_handle[loop] = 0;
	return TRUE;

XMS_not_present:
	return FALSE;
}

/* -----------------------------------------------------------------------
	FUNCTION: 	Return the biggest EMB block's size. not the total amount
				of all EMB blocks.
				The HMA or UMB blocks are not considered.
				This is the maxium amount of EMB once you can allocate.
	INPUT:		None.
	OUTPUT:		The value referenced above in KB unit.
   ---------------------------------------------------------------------- */
UI	EMB_size(void)
{

	_AH = 0x08;
	_BX = 0;
	XMS_entry();
	return _AX;
}

/* -----------------------------------------------------------------------
	FUNCTION: 	Allocate an EMB block you request.
	INPUT:		The EMB block's size you request (Unit: KBs).
	OUTPUT:		On success, a handle (index of the EMB block allocated)
				is returned.
				The handle is used by other EMB-accessing routines.
				On error, 0xFFFF is returned.
   ---------------------------------------------------------------------- */
UI EMB_alloc(UI size_request)
{
UI loop, tmp;

	for ( loop=0; loop<MAX_EMB_HANDLES; loop++)
		if (Real_handle[loop]==0)
			break;

	if (loop==MAX_EMB_HANDLES)
		return 0xFFFF;

	asm mov ah, 0x09
	asm mov dx, size_request
	XMS_entry();
	if (_AX==1)
	{
		tmp = _DX;
		Real_handle[loop] = tmp;
		return loop;
	}
	else
		return 0xFFFF;
}

/* -----------------------------------------------------------------------
	FUNCTION: 	Release an EMB block you specified.
	INPUT:		The EMB block's handle returned earlier.
	OUTPUT:		0 if succeed.
				Error code if failed.
   ---------------------------------------------------------------------- */
UC EMB_free(UI EMB_handle)
{
UI tmp;

	if (EMB_handle>=MAX_EMB_HANDLES || (tmp=Real_handle[EMB_handle])==0)
		return 0x82;

	asm mov ah, 0x0A
	asm mov dx, tmp
	XMS_entry();
	if (_AX==1)
	{
		Real_handle[EMB_handle] = 0;
		return 0;
	}
	else
		return _BL;
}

/* -----------------------------------------------------------------------
	FUNCTION: 	Release all EMB block you allocated.
	INPUT:		None.
	OUTPUT:		0 if succeed.
				Error code if failed.
   ---------------------------------------------------------------------- */
UC EMB_free_all(void)
{
UI loop;
UC tmp;

	for (loop=1; loop<MAX_EMB_HANDLES; loop++)
	{
		if (Real_handle[loop]!=0)
		{
			if ( (tmp=EMB_free(loop))!=0 )
				return tmp;
		}
	}
	return 0;
}

/* -----------------------------------------------------------------------
	FUNCTION: 	Copy a memory block from EMB to conventional memory.
	INPUT:      *dest:		The destination address to where the EMB copied.
				handle:		The EMB block's handle returned earlier.
				offset:		Offset of the very EMB block (source address).
				length:     Length of memory you want to copy.
	OUTPUT:		0 if succeed.
				error code if failed.
   ---------------------------------------------------------------------- */
UC EMB_read(void far *dest, UI EMB_handle, UL offset, UL length)
{
void far *tmp;

	if (EMB_handle>=MAX_EMB_HANDLES || Real_handle[EMB_handle]==0)
		return 0xA3;

	xms_ctrl.length = length;
	xms_ctrl.src_handle = Real_handle[EMB_handle];
	xms_ctrl.src_offset = offset;
	xms_ctrl.dst_handle = 0;
	xms_ctrl.dst_offset = ( ((UL )FP_SEG(dest)) << 16 ) + FP_OFF(dest);

	tmp = &xms_ctrl;
	asm lds si, tmp
	asm mov ah, 0x0B
	XMS_entry();
	return (_AX==1)? 0 : _BL;
}

/* -----------------------------------------------------------------------
	FUNCTION: 	Copy a memory block from conventional memory to EMB.
	INPUT:		handle:		The EMB block's handle returned earlier.
				offset:		Offset of the very EMB block (destination address).
				*source:	The source address from where the EMB copied.
				length:     Length of memory you want to copy.
	OUTPUT:		0 if succeed.
				error code if failed.
   ---------------------------------------------------------------------- */
UC EMB_write(UI EMB_handle, UL offset, void far *source, UL length)
{
void far *tmp;

	if (EMB_handle>=MAX_EMB_HANDLES || Real_handle[EMB_handle]==0)
		return 0xA5;

	xms_ctrl.length = length;
	xms_ctrl.src_handle = 0;
	xms_ctrl.src_offset = ( ((UL )FP_SEG(source)) << 16 ) + FP_OFF(source);
	xms_ctrl.dst_handle = Real_handle[EMB_handle];
	xms_ctrl.dst_offset = offset;

	tmp = &xms_ctrl;
	asm lds si, tmp
	asm mov ah, 0x0B
	XMS_entry();
	return (_AX==1)? 0 : _BL;
}

/* -----------------------------------------------------------------------
	FUNCTION:	Get the Real-handle of an EMB according to its EMB_handle.
	INPUT:
				EMB_handle:	the EMB_handle of an EMB returned from EMB_alloc();
	OUTPUT:
				The Real-handle of the EMB.
				0 if failed.
   ----------------------------------------------------------------------- */
UI	EMB_get_real_handle(UI EMB_handle)
{
	if (EMB_handle>=MAX_EMB_HANDLES || Real_handle[EMB_handle]==0)
		return 0;

	return Real_handle[EMB_handle];
}

/* -----------------------------------------------------------------------
	FUNCTION:	Allocate an EMB_handle to an EMB.
	INPUT:
				Real_handle: The real handle of an EMB.
	OUTPUT:
				An EMB_handle, can be used by routines such as EMB_read();
				Return 0xFFFF if failed.
   ----------------------------------------------------------------------- */
UI EMB_set_real_handle(UI real_handle)
{
UI loop;

	for ( loop=0; loop<MAX_EMB_HANDLES; loop++)
		if (Real_handle[loop]==0)
			break;

	if (loop==MAX_EMB_HANDLES)
		return 0xFFFF;
	Real_handle[loop] = real_handle;
	return loop;
}
