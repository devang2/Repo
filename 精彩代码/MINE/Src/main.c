#include "landmine.h"



void main()
{
   int  gMode,gDriver;
   Bool goOn=true;

   ClickInfor infor;
   CPoint     point;

   /* 初始化图形模式为320×200×256色 */
   initgraph(&gDriver, &gMode, "this is the game of landmine");

   /* 安装鼠标 */
   setTimerFreq( REFRESH_FREQ );
   setDoubleClickDelay( DOUBLE_CLICK_DELAY );
   installMouse();

   /* 初始化游戏，获取重要图像的句柄 */
   if(initGame())
   {
       /* 监听鼠标事件 */
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

   /* 结束游戏，释放图片句柄，关闭图形模式 */
   freePicHandles();
   closegraph();
}

