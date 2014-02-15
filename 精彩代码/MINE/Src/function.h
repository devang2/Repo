/**
**@ 说明：本头文件中定义了一组通用的函数，以方便程序员开发程序使用
**@
**@ 作者：余付平
**@
**@ 单位：中国沈阳 东北大学 软件学院 022 班
**@ 
**/


#ifndef TYPE_H
   #define TYPE_H
   #include "type.h"
#endif



/*————————— 函数声明 —————————*/
/*进行自定义的数据类型的对象的比较*/
Bool equalRGB(RGB a,RGB b);
Bool equalPoint(CPoint a,CPoint b);
Bool equalZone(CZone a,CZone b);
Bool equalSize(CSize a,CSize b);

/* 计算区域a和区域b的公共区域p,即a和b的相交区域。p允许是a或b或其他CZone对象的指针
   注意a、b、p必须使用同一坐标系系统!   如果有公共区域，则返回OK；否则返回FAILED。
*/
MSG publicZone(CZone a,CZone b,CZone * p);



/*———————————————————————*/




/*———————函数实现————————*/
Bool equalRGB(RGB a,RGB b)
{
    return ( a.red==b.red && a.green==b.green && a.blue==b.blue);
}


Bool equalPoint(CPoint a,CPoint b)
{
    return ( a.x==b.x && a.y==b.y);
}


Bool equalZone(CZone a,CZone b)
{
    return ( equalPoint(a.leftUp, b.leftUp) && equalPoint(a.rightDown, b.rightDown) );
}


Bool equalSize(CSize a,CSize b)
{
    return ( a.width==b.width && a.height==b.height);
}

Bool inZone(CZone z, CPoint p)
{
	return ( p.x>=z.leftUp.x && p.x<=z.rightDown.x && p.y>=z.leftUp.y && p.y<=z.rightDown.y );
}

Bool publicZone(CZone a,CZone b,CZone * p)
{
    int ax1,ax2,ay1,ay2,bx1,bx2,by1,by2,px1,px2,py1,py2;

    ax1=a.leftUp.x;
    ay1=a.leftUp.y;
    ax2=a.rightDown.x;
    ay2=a.rightDown.y;

    bx1=b.leftUp.x;
    by1=b.leftUp.y;
    bx2=b.rightDown.x;
    by2=b.rightDown.y;

    /*1、先检查区域a和区域b是否有相交区域,过滤掉没有相交区域的情况*/
    if( ax2<bx1 || bx2<ax1 || ay2<by1 || by2<ay1)
    {
        return false;
    }

    /*2、计算a、b的公共区域*/
    if(ax1<=bx1) { px1=bx1; }   /* px1取ax1、bx1两者的较大者 */
    else         { px1=ax1; }

    if(ay1<=by1) { py1=by1; }   /* py1取ay1、by1两者的较大者 */
    else         { py1=ay1; }

    if(ax2<=bx2) { px2=ax2; }   /* px2取ax2、bx2两者的较小者 */
    else         { px2=bx2; }

    if(ay2<=by2) { py2=ay2; }   /* py2取ay2、by2两者的较小者 */
    else         { py2=by2; }

    /*3、把计算出来的值赋值给区域p */
    p->leftUp.x=px1;
    p->leftUp.y=py1;
    p->rightDown.x=px2;
    p->rightDown.y=py2;

    return true;
}

/*————————————————————*/
