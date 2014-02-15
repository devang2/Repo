#include "landmine.h"



void main()
{
   int  gMode,gDriver;
   Bool goOn=true;

   ClickInfor infor;
   CPoint     point;

   /* ��ʼ��ͼ��ģʽΪ320��200��256ɫ */
   initgraph(&gDriver, &gMode, "this is the game of landmine");

   /* ��װ��� */
   setTimerFreq( REFRESH_FREQ );
   setDoubleClickDelay( DOUBLE_CLICK_DELAY );
   installMouse();

   /* ��ʼ����Ϸ����ȡ��Ҫͼ��ľ�� */
   if(initGame())
   {
       /* ��������¼� */
       while(goOn)
       TIME_SLICE_BEGIN(1)
       {

          refreshMouse();

          if( !showTime(false) )
          {
              goOn=haveAnotherTry();
          }

          infor = fetchClickInfor();
          point = getMousePosition();

          if(goOn)
          {
             goOn = analysis(point, infor);
          }
       }
       TIME_SLICE_END()
   }

   /* ������Ϸ���ͷ�ͼƬ������ر�ͼ��ģʽ */
   freePicHandles();
   closegraph();
}

