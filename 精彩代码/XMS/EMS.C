#include <process.h>
#include <stdio.h>
#ifndef TOTEM_EMS

#define TOTEM_EMS
#include <dos.h>
#include <string.h>

char EMS=0;

char test_ems()
{ 
	int i;
	union REGS inregs,outregs;
	struct SREGS segs;
	struct DeviceHeader {
		struct DeviceHeader far *link;
		unsigned attributes;
		unsigned strategy_offset;
		unsigned interrupt_offset;
		char name_of_number_of_units[8];
	} far *dev;
	unsigned char major,minor,c[13];
	inregs.x.ax=0x3001;
	intdos(&inregs,&outregs);
	major=outregs.h.al;
	minor=outregs.h.ah;
	if (major<2) return(0);
	else
	{
		inregs.h.ah=0x52;
		intdosx(&inregs,&outregs,&segs);
		if (major==2)
			dev=(struct DeviceHeader far *)MK_FP(segs.es+1,outregs.x.bx+7);
		else
			if ((major==3) && (minor==0))
				dev=(struct DeviceHeader far *)MK_FP(segs.es+2,outregs.x.bx+8);
			else
				dev=(struct DeviceHeader far *)MK_FP(segs.es+2,outregs.x.bx+2);
			c[8]=NULL;
			while (FP_OFF(dev)!=0xffff)
			{
				if (dev->attributes & 0x8000)
				{
					for (i=0;i<8;i++)
						c[i]=dev->name_of_number_of_units[i];
				}
				if (!strcmp(c,"EMMXXXX0"))
				{
					EMS=1;
					return(1);
				}
				dev=dev->link;
			}
	}
	return(0);
}

char get_EMS_status()
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x40
				int 0x67
				mov tmp,ah
		}
	}
	return(tmp);
}

char get_page_frame_segment(unsigned *segment)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x41
				int 0x67
				mov tmp,ah
		}
		*segment=_BX;
	}
	return(tmp);
}

char get_number_of_pages(unsigned *avail,unsigned *total)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x42
				int 0x67
				mov tmp,ah
		}
		*avail=_BX;
		*total=_DX;
	}
	return(tmp);
}

char allocate_memory(unsigned *handle,unsigned page_numbers)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x43
				mov bx,page_numbers
				int 0x67
				mov tmp,ah
		}
		*handle=_DX;
	}
	return(tmp);
}

char map_memory(char physical_page,unsigned logical_page,unsigned handle)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x44
				mov al,physical_page
				mov bx,logical_page
				mov dx,handle
				int 0x67
				mov tmp,ah
		}
	}
	return(tmp);
}

char release_memory(unsigned handle)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x45
				mov dx,handle
				int 0x67
				mov tmp,ah
		}
	}
	return(tmp);
}

char get_EMM_version(unsigned char *version)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x46
				int 0x67
				mov tmp,ah
		}
		*version=_AL;
	}
	return(tmp);
}

char save_mapping_context(unsigned handle)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x47
				mov dx,handle
				int 0x67
				mov tmp,ah
		}
	}
	return(tmp);
}

char restore_mapping_context(unsigned handle)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x48
				mov dx,handle
				int 0x67
				mov tmp,ah
		}
	}
	return(tmp);
}

char get_number_of_EMM_handles(unsigned *handle_numbers)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x4b
				int 0x67
				mov tmp,ah
		}
		*handle_numbers=_BX;
	}
	return(tmp);
}

char get_pages_owned_by_handle(unsigned handle,unsigned *page_numbers)
{
	char tmp=0xff;
	if (EMS)
	{
		asm {
			mov ah,0x4c
				mov dx,handle
				int 0x67
				mov tmp,ah
		}
		*page_numbers=_BX;
	}
	return(tmp);
}

char get_pages_for_all_handles(unsigned char *buffer,unsigned *handle_numbers)
{
	char tmp=0xff;
	if (EMS)
	{
		unsigned bseg=FP_SEG(buffer),boff=FP_OFF(buffer);
		asm {
			mov ah,0x4d
				mov di,boff
				mov es,bseg
				int 0x67
				mov tmp,ah
		}
		*handle_numbers=_BX;
	}
	return(tmp);
}

#endif

void main(void)
{
	char result;
	int result_num;
	unsigned char far *buffer;
	unsigned avail,total;
	unsigned char version;
	unsigned page_numbers,handle;
	unsigned segment;
	unsigned handle_numbers;
	unsigned logical_page,physical_page;
	unsigned char far *segment_buffer=(char far *)0xE0000000L;
	result_num=test_ems();
	if(result_num==1)
		printf("\nEms is installed.");
	else
		exit(0);
	
	result=get_EMS_status();
	if (result=='\x0')
		printf("\nEmm is active.");
	else
		exit(0);
	get_EMM_version(&version);
	printf("\nEMM version is %d",version);
	
	get_number_of_pages(&avail,&total);
	printf("\nTotal is %u,unused is %u",total,avail);
	
	get_page_frame_segment(&segment);
	printf("\nThe page frame segment is %u.",segment);
	
	get_number_of_EMM_handles(&handle_numbers);
	printf("\nThe active handles is %u",handle_numbers);
	
	page_numbers=4;
	result=allocate_memory(&handle,page_numbers);
	if(result=='\x0')
		printf("\nAllocate memory is success,\nthe handle is %u",handle);
	else
		exit(0);
	
	get_pages_owned_by_handle(handle,&page_numbers);
	printf("\nHandle has %u page",page_numbers);
	
	get_number_of_EMM_handles(&handle_numbers);
	printf("\nThe active handles is %u",255-handle_numbers);
	
	//use content begin
	
	
	
	
	physical_page=0;
	logical_page=2;
	result=map_memory(physical_page,logical_page,handle);
	if(result=='\x0')
		printf("\nMap_memory,logical_page is %u,\nphysical_page is %u,handle is %u",logical_page,physical_page,handle);
	
	buffer="abcdefghijkl";
	puts((const char *)buffer);
	asm	push ds;
	asm	les di,segment_buffer;
	asm	lds si,buffer;
	asm 	mov cx,20;
	asm 	cld;
	asm	rep movsw;
	asm	pop ds;
	
	buffer="a";
	puts((const char *)buffer);
	physical_page=0;
	logical_page=2;
	result=map_memory(physical_page,logical_page,handle);
	if(result=='\x0')
		printf("\nMap_memory,logical_page is %u,\nphysical_page is %u,handle is %u\n",logical_page,physical_page,handle);
	asm	push ds;
	asm	les di,buffer;
	asm	lds si,segment_buffer;
	asm 	mov cx,20;
	asm 	cld;
	asm	rep movsw;
	asm	pop ds;
	puts((const char *)buffer);
	
	//use content end
	
	
	result=release_memory(handle);
	if(result=='\x0')
		printf("\nRelease is success.");
	else
		exit(0);
	
	
}
