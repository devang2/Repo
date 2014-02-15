#include <dos.h>
#include <alloc.h>

#include "\vga\xms.h"

struct EMB {
	long Leng;
	unsigned SourceHandle;
	long SourceOfs;
	unsigned DestinHandle;
	long DestinOfs;
	};

int XMS::OK=0;
static void far *XMSaddr;

int XMS::init(void)
{
static struct REGPACK rg;

rg.r_ax=0x4300;
intr(0x2f,&rg);
if( (rg.r_ax&0x00ff) == 0x80 )
	{
	rg.r_ax=0x4310;
	intr(0x2f,&rg);
	XMSaddr=MK_FP(rg.r_es,rg.r_bx);
	OK=1;
	}
else
	OK=0;
return(OK);
}

unsigned XMS::freesize(void)
{
if(OK==0)
	return(0);
asm {
	mov ah,8
	call XMSaddr
	}
return _DX;
}

unsigned XMS::largestblock(void)
{
if(OK==0)
	return(0);
asm {
	mov ah,8
	call XMSaddr
	}
return _AX;
}

XMS::XMS(int size)
{
if(OK==0)
	{
	handle=0;
	return;
	}

asm {
	mov ah,9
	mov dx,size
	call XMSaddr
	}
handle=_DX;
}

XMS::~XMS()
{
if(handle==0)
	return;

int hd=handle;
asm {
	mov ah,0ah
	mov dx,hd
	call XMSaddr
	}
}

int XMS::realloc(int size)
{
if(handle==0)
	return(0);

int hd=handle;
asm {
	mov ah,0fh
	mov bx,size
	mov dx,hd
	call XMSaddr
	}
return _AX;
}

int XMS::move(struct EMB *emb)
{
asm {
	push ds
	mov ah,0bh
	push ds
	pop es
	lds si,emb
	call es:XMSaddr
	pop ds
	}
return _AX;
}

int XMS::put(void *dp,void *sp,long leng)
{
struct EMB emb;

if(leng&1L)
	leng++;
emb.Leng=leng;
emb.SourceHandle=0;
emb.SourceOfs=(long)sp;
emb.DestinHandle=handle;
emb.DestinOfs=(long)dp;
return move(&emb);
}

int XMS::get(void *dp,void *sp,long leng)
{
int v;
struct EMB emb;

if(leng&1L)
	{
	char *p,*d;

	leng--;
	if( leng>0 )
		{
		emb.Leng=leng;
		emb.SourceHandle=handle;
		emb.SourceOfs=(long)sp;
		emb.DestinHandle=0;
		emb.DestinOfs=(long)dp;
		move(&emb);
		}

	p=(char *)malloc(2);
	emb.Leng=2L;
	emb.SourceHandle=handle;
	emb.SourceOfs=(long)sp+leng;
	emb.DestinHandle=0;
	emb.DestinOfs=(long)p;
	v=move(&emb);
	d=(char*)dp;
	d[leng]=p[0];
	free(p);
	}
else
	{
	emb.Leng=leng;
	emb.SourceHandle=handle;
	emb.SourceOfs=(long)sp;
	emb.DestinHandle=0;
	emb.DestinOfs=(long)dp;
	v=move(&emb);
	}
return(v);
}
