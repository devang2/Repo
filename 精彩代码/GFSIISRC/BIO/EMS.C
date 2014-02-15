/* EMS.C
 * 		Updated: 1993/3/5
 *		This  is  the source file of extaned memory access functions	 *
 *		Source:	Xu Shaomin                                               *
 *      Update: Luke Kong                                                        *
*/

#include  <stdio.h>
#include  <def.inc>

#define PRIV_VAL                0x93
#define TABLE_SIZE              0x30
#define SCR_TABLE               0x10
#define DST_TABLE               0x18

#define SEG_BOUND               0x00
#define LOW_ADD                 0x02
#define HIGH_ADD                0x04
#define ACCESS_PRIV             0x05
#define RESERVED                0x06

/*  ------     EXTANED  MEMORY  FUNCTION  PROTOTYPES     ------  */
UI		EM_amount(void);
UC		EM_read(void far *destin, UL source, UI BLK_SIZE);
UC		EM_write( UL destin, void far *source, UI BLK_SIZE);

/* =========================== EM_amount ================================ */
/*  Get the amount of expand memory in bytes */

UI EM_amount(void)
{

	asm             mov  al, 0x31
	asm             out  0x70, al
	asm             in   al, 0x71
	asm             mov  ah, al
	asm             mov  al, 0x30
	asm             out  0x70, al
	asm             in   al, 0x71
	return(_AX);
}

/* =========================== EM_read ============================ */
/*  Read BLK_SIZE bytes from source( in EM ) to destin */

UC EM_read(void far *destin, UL source, UI BLK_SIZE)
{
  int       i;
  UC        GDT[TABLE_SIZE];
  UC        far *GDT_ptr;      /* GDT table pointer */
  UL        far *scr_ptr;      /* pointer to source data area */

  /* restore the register value */
  GDT_ptr = GDT;
  scr_ptr = &source;

  asm        push ds
  asm        LDS  ax, destin    /* read the address of source memory */

  /* -----------------------------------------
	 Exchange the segment and relative address to the actural
	 physical address in AX and BL register
   ------------------------------------------- */
  asm        mov bx, ds
  asm        mov cl, 4
  asm        shl bx, cl

  asm        add ax, bx
  asm        jnc non_carry
  asm        mov bx, ds
  asm        and bx, 0xF000
  asm        mov cl, 12
  asm        shr bx, cl

  asm        inc bl
  asm        jmp real_addr

non_carry:
  asm        mov bx, ds
  asm        and bx, 0xF000
  asm        mov cl, 12
  asm        shr bx, cl

real_addr:
  asm        and bx, 0x0F       /* disable I A-20 buses to
				   avoid the segment rollment */
  asm        pop ds
  asm        push ax
  asm        push bx

  /* prepare source item for GDT table */
  for( i = 0; i < TABLE_SIZE; i++)
	      *(GDT_ptr + i) = 0x00;

  asm        pop cx
  asm        pop ax

  *(GDT_ptr + DST_TABLE + HIGH_ADD)               = _CL;
  *(UI far *)(GDT_ptr + DST_TABLE + LOW_ADD)     = _AX;

  /* prepare range item and identification for GDT table */
  *(UI far *)(GDT_ptr + SCR_TABLE + SEG_BOUND)   =
  *(UI far *)(GDT_ptr + DST_TABLE + SEG_BOUND)   = 0xFFFF;
  *(GDT_ptr + SCR_TABLE + ACCESS_PRIV)            =
  *(GDT_ptr + DST_TABLE + ACCESS_PRIV)            = PRIV_VAL;

  /* prepare destine item for GDT table */
  asm        push ds
  asm        LDS  si, scr_ptr
  asm        LES  di, GDT_ptr
  asm        add  di, (SCR_TABLE + LOW_ADD)
  asm        mov  cx, 0x03
  asm        cld
  asm        rep movsb
  asm        pop ds

  /* write data to expand memory according to the GDT table */

  BLK_SIZE /= 2;

  asm        LES si, GDT_ptr
  asm        mov cx, BLK_SIZE

  asm        mov ah, 0x87
  asm        int 0x15


  if( _AH == 0 )      return(TRUE);
  else                return(FALSE);
}




/* =========================== EM_write ============================== */
/*  Write BLK_SIZE bytes from source to destin( in EM )*/

UC EM_write( UL destin, void far *source, UI BLK_SIZE)
{
   int        i;
   UC         GDT[0x30];
   UC         far *GDT_ptr;      /* GDT table pointer */
   UL         far *dst_ptr;      /* pointer to destination address */

   /* restore the register value */
   GDT_ptr = GDT;
   dst_ptr = &destin;

   asm         push ds
   asm         LDS  ax, source   /* read the address of sorce memory */

  /* -----------------------------------------
   **  Exchange the segment and relative address to the actural
   **  physical address in AX and BL register
   ------------------------------------------- */
  asm        mov bx, ds
  asm        mov cl, 4
  asm        shl bx, cl

  asm        add ax, bx
  asm        jnc non_carry
  asm        mov bx, ds
  asm        and bx, 0xF000
  asm        mov cl, 12
  asm        shr bx, cl

  asm        inc bl
  asm        jmp real_addr

non_carry:
  asm        mov bx, ds
  asm        and bx, 0xF000
  asm        mov cl, 12
  asm        shr bx, cl

real_addr:
  asm        and bx, 0x0F       /* disable I A-20 buses to
				   avoid the segment rollment */
  asm        pop ds
  asm        push ax
  asm        push bx

  /* prepare source item for GDT table */
  for( i = 0; i < TABLE_SIZE; i++)
	      *(GDT_ptr + i) = 0x00;

  asm        pop cx
  asm        pop ax

  *(GDT_ptr + SCR_TABLE + HIGH_ADD )              = _CL;
  *(UI far *)(GDT_ptr + SCR_TABLE + LOW_ADD)     = _AX;

  /* prepare range item and identification for GDT table */
  *(UI far *)(GDT_ptr + SCR_TABLE + SEG_BOUND)   =
  *(UI far *)(GDT_ptr + DST_TABLE + SEG_BOUND)   = 0xFFFF;
  *(GDT_ptr + SCR_TABLE + ACCESS_PRIV)            =
  *(GDT_ptr + DST_TABLE + ACCESS_PRIV)            = PRIV_VAL;

  /* prepare destine item for GDT table */
  asm        push ds
  asm        LDS  si, dst_ptr
  asm        LES  di, GDT_ptr
  asm        add  di, (DST_TABLE + LOW_ADD)
  asm        mov  cx, 0x03
  asm        cld
  asm        rep  movsb
  asm        pop  ds

  /* write data to expand memory according to the GDT table */
  BLK_SIZE /= 2;

  asm        LES si, GDT_ptr
  asm        mov cx, BLK_SIZE

  asm        mov ah, 0x87
  asm        int 0x15


  if( _AH == 0 )       return(TRUE);
  else                 return(FALSE);
}





