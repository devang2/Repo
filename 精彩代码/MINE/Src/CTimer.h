/* ����ͷ�ļ� */
#ifndef TYPE_H
   #define TYPE_H
   #include "type.h"
#endif

#ifndef DOS_H
    #define DOS_H
    #include "dos.h"
#endif



/* ȫ�ֱ��� */
UINT g_HZ=18;                     /* ��ǰʱ���жϴ���Ƶ�� */
void interrupt(* oldINT)();  
Bool hasBind = false;




/* �궨�� */
#define TIME_SLICE_BEGIN(cycles)  \
{   \
    UINT far * ___TMR___ = (UINT far *)0x0000046CL; \
    UINT ___END___; \
    *___TMR___=0;   \
    ___END___ =cycles;

   
#define TIME_SLICE_END() \
    while( *___TMR___<___END___);\
}


#define COSTIME(code) \
{\
    UINT hz=g_HZ;         \
    UINT far * time=(UINT far *) 0x0000046CL;\
    setTimerFreq(62500);  \
    code;                 \
    printf("\nexcute this code cost %lf ms!\n",(*time)*0.016);  \
    setTimerFreq(hz);     \
}




/* �������� */
void setTimerFreq( UINT hz );                        /* ����ʱ��Ƭ�������ļ���Ƶ�� */
UINT getCurrFreq(void);                              /* ��ȡ��ǰʱ��Ƭ����������Ƶ�� */
void bindToTimer( void interrupt ( * newINT )() );   /* ���µ��жϺ����󶨵�ʱ��Ƭ�ж��� */
void cancelBind();                                   /* ȡ��ʱ���жϰ� */
void timeTask(BYTE hour, BYTE min, BYTE sec, void interrupt ( * task )() );  /* ��ʱ���� */
void finishedTimeTask();                             /* ������ʱ���� */


/* ����ʵ�� */
void setTimerFreq( UINT hz )
{
    UINT far * timer=(UINT far *) 0x0000046CL;  /* timer ָ��ʱ��Ƭ������ */
    UINT cnt = 1193180/hz;                      /* 1193180 ��8253��ʱ��������Ƶ�ʣ���Ƶ�������Ƶ��/Ҫ��ʱ��Ƶ�� */
    
    if(hz>=18)   
    {
        outportb(0x43, 0x3c);
        outportb(0x40, cnt & 0x00FF);
        outportb(0x40, (cnt >> 8) & 0x00FF);
        g_HZ=hz;
    }
    *timer=0;
}


UINT getCurrFreq(void)
{
    return g_HZ;
}


void bindToTimer( void interrupt ( * newINT )() )
{
    if( !hasBind )
    {
        oldINT=getvect(0x1c);     
    }

    setvect( 0x1c, newINT );   /* ͨ����д 1CH ���жϣ���ʵ�ְ��û��Զ�����жϰ󶨵�ʱ��Ƭ�� */
    hasBind=true;
}


void cancelBind()
{
    if( hasBind )
    {
        setvect( 0x1c, oldINT );
    }
}


BYTE toBCDCode(BYTE data)
{
    BYTE tensDigit[10]={0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90 };
    BYTE result;

    result = tensDigit[data/10] + (data%10);

    return result;
}

void timeTask(BYTE hour, BYTE min, BYTE sec, void interrupt ( * task )() )
{
    hour=toBCDCode(hour);
    min =toBCDCode(min);
    sec =toBCDCode(sec);

    setvect( 0x4A, task);
    
    asm MOV CH,hour;
    asm MOV CL,min;
    asm MOV DH,sec;

    asm MOV AH,6;
    asm INT 1AH;
}



void finishedTimeTask()
{
    asm MOV AH,07H;
    asm INT 1AH;
}
