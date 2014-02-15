/* ------------------------------------------------------------------
	FILENAME:	del_disk.c
	FUNCTION:	Delete all files and subdirectorys of a disk,
				no matter whether a file is deletable or a subdirectory
				is empty.
	DATE:		26/4/93
	AUTOTHER:	LIU GUANG YU
   ------------------------------------------------------------------ */

#include	<dos.h>
#include	<stdio.h>
#include	<alloc.h>
#include	<string.h>
#include	<mem.h>

#include	<def.inc>
#include	<func.inc>

typedef	struct
{
UC	dr;       				/* 	return Driver */
UC	equpiment;              /*  equpiment word	*/
UI	per_sec_byte;           /*	bytes per sector */
UC	per_clust_sec;          /*	sectors per cluster */
UC	clust_size;
UI	bl;
UC	fat_num;             	/*	FAT	number	*/
UI	root_num;				/*	In root, maxium	file number */
UI	first_data;				/*	first data start address */
UI	max_clust;				/*	the disk maxium	clust	*/
UC	per_fat_num;			/*  sectors number of a FAT */
UI	root_start_sec;			/*	Root area start address */
}	DISK_STRUCT;

/* --------------------------------------------------------------------
  		FUNCTION:       delete the disk all file
  		CALL:
		CALL BY:		General-purposed routine.
		INPUT:			Drive:	0 -- A: driver.
								1 -- B: driver.
  		OUTPUT:			None
		RETURN:			0 -- Successful.
						1 -- Failed.
   -------------------------------------------------------------------- */

UC	del_disk(UC Drive)
{
DISK_STRUCT	*disk_s;		/*	DOS/TSR	point */
UC			*buff;			/*  Read section buffer */
union REGS  in, out;		/*  Bios interrupt	union various	*/
struct SREGS seg;			/*	Bios interrupt	struct segment various	*/
UI	start_p , i;			/*	loop various */
UI	result1, result2;		/*	save FAT result	*/
UI  fat_bytes;				/* Total bytes of the whole fat area, it equals:
							   disk_s->per_sec_byte * disk_s->per_fat_num */

	in.h.dl = Drive+1;
	in.h.ah	= 0x32;
	int86x(0x21, &in, &out, &seg);
	disk_s = MK_FP(seg.ds, out.x.bx);

	fat_bytes = disk_s->per_sec_byte*disk_s->per_fat_num;
	buff = (UC	*)mem_alloc(fat_bytes);
	if ( absread(Drive, disk_s->per_fat_num, 1, buff) != 0 )
	{
		farfree(buff);
		return 1;
	}

	for (start_p = 3; start_p<fat_bytes; start_p +=3)
	{
		result1 = ( ((UI)buff[start_p+1]<<8) + buff[start_p] ) & 0x0FFF;
		result2 = ( ((UI)buff[start_p+2]<<8) + buff[start_p+1] ) >> 4;

		if (result1  != 0xFF7)
		{
			buff[start_p] = 0;
			buff[start_p+1] &= 0xF0;
		}

		if (result2 != 0xFF7)
		{
			buff[start_p+1] &= 0x0F;
			buff[start_p+2] = 0;
		}
	}

	if ( (abswrite(Drive, disk_s->per_fat_num, 1, buff)) != 0 )
	{
		farfree(buff);
		return 1;
	}

	if ( (abswrite(Drive, disk_s->per_fat_num,
				   1+disk_s->per_fat_num, buff)) != 0 )
	{
		farfree(buff);
		return 1;
	}

	farfree(buff);
	buff = (UC	*)farmalloc(disk_s->root_num*0x20);

	if ( (absread(Drive, disk_s->first_data-disk_s->root_start_sec,
				  disk_s->root_start_sec, buff)) != 0 )
	{
		farfree(buff);
		return 1;
	}

	for (i = 0;i<disk_s->root_num*0x20;i+=0x20)
		*(buff+i) = 0;

	if ( (abswrite(Drive, disk_s->first_data-disk_s->root_start_sec,
				   disk_s->root_start_sec, buff)) != 0 )
	{
		farfree(buff);
		return 1;
	}

	farfree(buff);

	_AH = 0xd;
	geninterrupt(0x21);
	return 0;
}
