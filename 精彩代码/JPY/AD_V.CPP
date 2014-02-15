float ad_v()
{
 int poll,lowby,highby;
 float in_v,ad_v;
 outportb(0x280,0x00);
 do {
  poll=inportb(0x285);
  poll=poll&128;
 } while(poll!=128);
 lowby=inportb(0x281);
 highby=inportb(0x282)&15;
 ad_v=highby*256+lowby;
 in_v=((ad_v-2047)*20/4096)*1000*(-1);
 printf("in_v %f ",in_v);
return(in_v);
}