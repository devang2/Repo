/**
**@ ˵������ͷ�ļ��ж�����һ��ͨ�õĺ������Է������Ա��������ʹ��
**@
**@ ���ߣ��ึƽ
**@
**@ ��λ���й����� ������ѧ ���ѧԺ 022 ��
**@ 
**/


#ifndef TYPE_H
   #define TYPE_H
   #include "type.h"
#endif



/*������������������ �������� ������������������*/
/*�����Զ�����������͵Ķ���ıȽ�*/
Bool equalRGB(RGB a,RGB b);
Bool equalPoint(CPoint a,CPoint b);
Bool equalZone(CZone a,CZone b);
Bool equalSize(CSize a,CSize b);

/* ��������a������b�Ĺ�������p,��a��b���ཻ����p������a��b������CZone�����ָ��
   ע��a��b��p����ʹ��ͬһ����ϵϵͳ!   ����й��������򷵻�OK�����򷵻�FAILED��
*/
MSG publicZone(CZone a,CZone b,CZone * p);



/*����������������������������������������������*/




/*������������������ʵ�֡���������������*/
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

    /*1���ȼ������a������b�Ƿ����ཻ����,���˵�û���ཻ��������*/
    if( ax2<bx1 || bx2<ax1 || ay2<by1 || by2<ay1)
    {
        return false;
    }

    /*2������a��b�Ĺ�������*/
    if(ax1<=bx1) { px1=bx1; }   /* px1ȡax1��bx1���ߵĽϴ��� */
    else         { px1=ax1; }

    if(ay1<=by1) { py1=by1; }   /* py1ȡay1��by1���ߵĽϴ��� */
    else         { py1=ay1; }

    if(ax2<=bx2) { px2=ax2; }   /* px2ȡax2��bx2���ߵĽ�С�� */
    else         { px2=bx2; }

    if(ay2<=by2) { py2=ay2; }   /* py2ȡay2��by2���ߵĽ�С�� */
    else         { py2=by2; }

    /*3���Ѽ��������ֵ��ֵ������p */
    p->leftUp.x=px1;
    p->leftUp.y=py1;
    p->rightDown.x=px2;
    p->rightDown.y=py2;

    return true;
}

/*����������������������������������������*/
