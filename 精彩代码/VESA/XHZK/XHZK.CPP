#include <stdio.h>
#include <dos.h>
#include <alloc.h>
#include <string.h>

union CTOI {
	unsigned ii;
	unsigned char cc[2];
	} _ctoi;

class CHZS {
public:
	virtual void run(void)=0;
protected:
	int MAXN;
	long Isize;
	int coden,orgn;
	unsigned *code;

	int ifrun(int n);
	void savehead(char *filename);
	void getcode(char *filezj,char *filed);
	void codesort();
	int findcode(unsigned ii);
	virtual long getoffset(unsigned char *cp);
	virtual int ifin(unsigned char c);
	} ;

class CHZ16S : public CHZS
	{
public:
	CHZ16S() { MAXN=1000; Isize=32; }
	void run(void);
protected:
	long getoffset(unsigned char *cp);
	} ;

class CHZ24S : public CHZS
	{
public:
	CHZ24S() { MAXN=900; Isize=72; }
	void run(void);
	} ;

class CHZ32S : public CHZ24S
	{
public:
	CHZ32S() { MAXN=500; Isize=128; }
	} ;

class CHZ40S : public CHZ24S
	{
public:
	CHZ40S() { MAXN=350; Isize=180; }
	} ;

class CHZUCS : public CHZS
	{
public:
	CHZUCS() { MAXN=200; Isize=6; }
	void run(void);
protected :
	int ifin(unsigned char c);
	} ;

class CTXUCS : public CHZUCS
	{
protected :
	int ifin(unsigned char c);
	} ;


int CHZS::ifrun(int n)
{
int i;
FILE *fp;

if( _argc<=n )
	{
	printf("\n Parameter too few.\n");
	return(0);
	}

for(i=2;i<n;i++)
	{
	fp=fopen(_argv[i],"rb");
	if(fp==NULL)
		{
		printf("\n The file %s no exists.\n",_argv[i]);
		return(0);
		}
	fclose(fp);
	}
return(1);
}

void CHZS::savehead(char *filename)
{
FILE *fp;

fp=fopen(filename,"wb");
fwrite(&coden,2,1,fp);
fwrite(code,coden,2,fp);
fclose(fp);
}

void CHZS::getcode(char *filezj,char *filed)
{
FILE *fp;
unsigned char c1,c2;

coden=orgn=0;
fp=fopen(filed,"rb");
if(fp!=NULL)
	{
	fread(&orgn,2,1,fp);
	fread(code,2,orgn,fp);
	fclose(fp);
	}

coden=orgn;
fp=fopen(filezj,"rb");
while(coden<MAXN) {
	if(fread(&c1,1,1,fp)<1)
		break;
	if( ifin(c1) )
		{
		if(fread(&c2,1,1,fp)<1)
			break;
		if(c2>=0xa1)
			{
			_ctoi.cc[0]=c1;
			_ctoi.cc[1]=c2;
			if( findcode(_ctoi.ii)==0 )
				{
				code[coden]=_ctoi.ii;
				coden++;
				}
			}
		}
	}
fclose(fp);
}

int CHZS::findcode(unsigned ii)
{
if(coden==0)
	return(0);

int beg,end,i,k;

beg=0;
end=coden-1;
i=coden/2;
k=0;
while(1) {
	if(end-beg<=1)
		{
		k++;
		if(k>2)
			return(0);
		}
	if(ii==code[i])
		return(1);
	else if(ii>code[i])
		{
		beg=i;
		i=(beg+end+1)/2;
		}
	else
		{
		end=i;
		i=(beg+end)/2;
		}
	}
}

void CHZS::codesort()
{
int i,f;
unsigned k;

while(1) {
	f=0;
	for(i=0;i<coden-1;i++)
		if(code[i]>code[i+1])
			{
			f=1;
			k=code[i];
			code[i]=code[i+1];
			code[i+1]=k;
			}
	if(f==0)
		break;
	}
}

long CHZS::getoffset(unsigned char *cp)
{
long off;
if(cp[0]<0xb0)
	off=(long)Isize*( ((long)cp[0]-0xa1L)*94L+(long)cp[1]-0xa1L );
else
	off=(long)Isize*( ((long)cp[0]-0xb0L)*94L+(long)cp[1]-0xa1L );
return(off);
}

int CHZS::ifin(unsigned char c)
{
return (c>=0xa1);
}

void CHZ16S::run()
{
if( ifrun(4)==0 )
	return;

unsigned i;
void *buf;
FILE *fps,*fpd;
long off;

code=(unsigned *)calloc(MAXN,2);
getcode(_argv[3],_argv[4]);
codesort();
savehead(_argv[4]);

fps=fopen(_argv[2],"rb");
fpd=fopen(_argv[4],"ab");
buf=calloc(Isize,1);
fwrite(buf,Isize,1,fpd);
for(i=0;i<coden;i++)
	{
	_ctoi.ii=code[i];
	fseek(fps,getoffset(_ctoi.cc),0);
	fread(buf,Isize,1,fps);
	fwrite(buf,Isize,1,fpd);
	}
fclose(fps);
fclose(fpd);
free(buf);
}

long CHZ16S::getoffset(unsigned char *cp)
{
long off;
off=(long)Isize*( ((long)cp[0]-0xa1L)*94L+(long)cp[1]-0xa1L );
return(off);
}

void CHZ24S::run()
{
if( ifrun(5)==0 )
	return;

unsigned i;
void *buf;
FILE *fps1,*fps2,*fpd;
long off;

code=(unsigned *)calloc(MAXN,2);
getcode(_argv[4],_argv[5]);
codesort();
savehead(_argv[5]);

fps1=fopen(_argv[2],"rb");
fps2=fopen(_argv[3],"rb");
fpd=fopen(_argv[5],"ab");
buf=calloc(Isize,1);
fwrite(buf,Isize,1,fpd);
for(i=0;i<coden;i++)
	{
	_ctoi.ii=code[i];
	off=getoffset(_ctoi.cc);
	if(_ctoi.cc[0]<0xb0)
		{
		fseek(fps2,off,0);
		fread(buf,Isize,1,fps2);
		}
	else
		{
		fseek(fps1,off,0);
		fread(buf,Isize,1,fps1);
		}
	fwrite(buf,Isize,1,fpd);
	}
fclose(fps1);
fclose(fps2);
fclose(fpd);
free(buf);
}

void CHZUCS::run()
{
if( ifrun(4)==0 )
	return;

unsigned i;
void *buf;
FILE *fps,*fpd;
long off,bhoff;
int leng;

code=(unsigned *)calloc(MAXN,2);
getcode(_argv[3],_argv[4]);
codesort();
savehead(_argv[4]);

fps=fopen(_argv[2],"rb");
fpd=fopen(_argv[4],"ab");
bhoff=2L+coden*2L+(coden+1)*6L;
leng=1;
fwrite(&bhoff,4,1,fpd);
fwrite(&leng,2,1,fpd);
bhoff += (long)leng;
for(i=0;i<coden;i++)
	{
	_ctoi.ii=code[i];
	off=getoffset(_ctoi.cc)+4L;
	fseek(fps,off,0);
	fread(&leng,2,1,fps);
	fwrite(&bhoff,4,1,fpd);
	fwrite(&leng,2,1,fpd);
	bhoff += (long)leng;
	}
buf=calloc(1024,1);
fwrite(buf,1,1,fpd);
for(i=0;i<coden;i++)
	{
	_ctoi.ii=code[i];
	fseek(fps,getoffset(_ctoi.cc),0);
	fread(&bhoff,4,1,fps);
	fread(&leng,2,1,fps);
	fseek(fps,bhoff,0);
	fread(buf,leng,1,fps);
	fwrite(buf,leng,1,fpd);
	}
fclose(fps);
fclose(fpd);
free(buf);
}

int CHZUCS::ifin(unsigned char c)
{
return( c>=0xb0 );
}

int CTXUCS::ifin(unsigned char c)
{
return( c>=0xa1 && c<0xb0 );
}

void main()
{
CHZS *A;
if(stricmp(_argv[1],"HZ16")==0)
	A=new CHZ16S;
else if(stricmp(_argv[1],"HZ24")==0)
	A=new CHZ24S;
else if(stricmp(_argv[1],"HZ32")==0)
	A=new CHZ32S;
else if(stricmp(_argv[1],"HZ40")==0)
	A=new CHZ40S;
else if(stricmp(_argv[1],"HZUC")==0)
	A=new CHZUCS;
else if(stricmp(_argv[1],"TXUC")==0)
	A=new CTXUCS;
else
	{
	printf("\nParameter error\n");
	return;
	}
A->run();
delete A;
}
