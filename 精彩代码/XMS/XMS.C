#include <stdio.h>
#include <process.h>
#include <alloc.h>
#include <dos.h>
#ifndef TOTEM_XMS

#define TOTEM_XMS

#include <dos.h>

char XMS=0,HMA=0,UMB=0;

void (far *xms)(void);

char test_xms()
{
  asm {
    mov ax,0x4300
    int 0x2f
  }
  if (_AL==0x80) XMS=1;
  return(XMS);
}

void get_driver_address()
{
  if (XMS)
  {
    asm {
      mov ax,0x4310
      int 0x2f
    }
    xms=(void (far *)())(((long)(_ES)<<16)+_BX);
  }
}


void get_xms_version(unsigned *xms_ver,unsigned *int_ver)
{
  if (XMS)
  {
    asm mov ah,0x00
    xms();
    *xms_ver=_AX;
    *int_ver=_BX;
    HMA=_DX;
  }
}

char Request_HMA(unsigned size)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x01
      mov dx,size
    }
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char Release_HMA()
{
  char error_code=0xff;
  if (XMS)
  {
    asm mov ah,0x02
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char global_enable_A20()
{
  char error_code=0xff;
  if (XMS)
  {
    asm mov ah,0x03
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char global_disable_A20()
{
  char error_code=0xff;
  if (XMS)
  {
    asm mov ah,0x04
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char local_enable_A20()
{
  char error_code=0xff;
  if (XMS)
  {
    asm mov ah,0x05
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char local_disable_A20()
{
  char error_code=0xff;
  if (XMS)
  {
    asm mov ah,0x06
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char query_A20_state(unsigned *state)
{
  char error_code=0xff;
  if (XMS)
  {
    asm mov ah,0x07
    xms();
    *state=_AX;
    error_code=_BL;
  }
  return(error_code);
}

char query_free_xms(unsigned *max_block,unsigned *total)
{
  char error_code=0xff;
  if (XMS)
  {
    asm mov ah,0x08
    xms();
    *max_block=_AX;
    *total=_DX;
    error_code=_BL;
  }
  return(error_code);
}

char allocate_xms(unsigned size,unsigned *handle)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x09
      mov dx,size
    }
    xms();
    *handle=_DX;
    error_code=_BL;
  }
  return(error_code);
}

char free_xms(unsigned handle)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x0a
      mov dx,handle
    }
    xms();
    error_code=_BL;
  }
  return(error_code);
}

typedef struct xms_mov {
  unsigned long byte_count;             //移动的字节数
  unsigned source_handle;     		//源句柄
  unsigned long source_offset;          //源偏移量
  unsigned destination_handle;          //目的句柄
  unsigned long destination_offset;     //目的偏移量
} xmm;

char move_xms(xmm *xmm_ptr)
{
  char error_code=0xff;
  if (XMS)
  {
    unsigned xseg=FP_SEG(xmm_ptr),xoff=FP_OFF(xmm_ptr);
    asm {
      mov ah,0x0b
      mov si,xoff
      mov ds,xseg
    }
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char lock_XMS_block(unsigned handle)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x0c
      mov dx,handle
    }
    xms();
    error_code=_BL;
  }
  return(error_code);
}

char unlock_XMS_block(unsigned handle)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x0d
      mov dx,handle
    }
    xms();
    error_code=_BL;
  }
  return(error_code);
}

typedef struct XMS_handle_info {
  unsigned char lock_count; 	//块锁定计数值
  unsigned char free_handle;	//可用的句柄
  unsigned size;                //句柄分配的块的容量(单位:K)
}xhi;
char get_handle_info(unsigned handle,xhi *handle_info)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x0e
      mov dx,handle
    }
    xms();
    error_code=_BL;
    handle_info->lock_count=_BH;
    handle_info->free_handle=_BL;
    handle_info->size=_DX;
  }
  return(error_code);
}

char reallocate_xms_block(unsigned handle,unsigned size)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x0f
      mov dx,handle
      mov bx,size
    }
    xms();
    error_code=_BL;
  }
  return(error_code);
}

typedef struct UMB_info {
  unsigned UMB_segment;
  unsigned size;
}ui;
char request_UMB(unsigned size,ui *info)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x10
      mov dx,size
    }
    xms();
    error_code=_BL;
    info->UMB_segment=_BX;
    info->size=_DX;
  }
  return(error_code);
}

char release_UMB(unsigned segment)
{
  char error_code=0xff;
  if (XMS)
  {
    asm {
      mov ah,0x10
      mov dx,segment
    }
    xms();
    error_code=_BL;
  }
  return(error_code);
}

#endif
void main()
{
	unsigned xms_ver,int_ver,max_block,total,size=1,handle;
	xmm xmm;
	FILE *fp;
	long fl;
	char *buf1,*buf2;
	int i;

	if(test_xms()!=0x80)
	{
		printf("No XMS Driver!");
		exit(-1);
	}
	get_driver_address();
	get_xms_version(&xms_ver,&int_ver);
	printf("\nXMS Driver Version %x,%x\n",xms_ver,int_ver);
	query_free_xms(&max_block,&total);
	printf("Max_block: %u KB,Total:%uKB\n",max_block,total);

	fp=fopen("c:\\autoexec.bat","rb");
	if(fp==NULL)
	{
		printf("\nopen file error!");
		exit(-1);
	}
	fseek(fp,0L,SEEK_END);
	fl=ftell(fp);
	if(fl%2==1)
		fl++;
	rewind(fp);
	buf1=(char *)malloc(fl);
	buf2=(char *)malloc(fl);
	if(buf1==NULL||buf2==NULL)
	{
		printf("Memory error");
		exit(-1);
	}
	fread(buf1,fl,1,fp);


	allocate_xms(size,&handle);
	xmm.byte_count=fl;
	xmm.source_handle=0;
	xmm.source_offset=(unsigned long)buf1;

	xmm.destination_handle=handle;
	xmm.destination_offset=0;
	move_xms(&xmm);

	xmm.byte_count=fl;
	xmm.source_handle=handle;
	xmm.source_offset=0;

	xmm.destination_handle=0;
	xmm.destination_offset=(unsigned long)buf2;
	move_xms(&xmm);

	for(i=0;i<fl;i++)
		printf("%c",buf1[i]);
	for(i=0;i<fl;i++)
		printf("%c",buf2[i]);
	free_xms(handle);
	free(buf1);
	free(buf2);
	fclose(fp);
}
