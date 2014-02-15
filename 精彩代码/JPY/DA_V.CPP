void da_v(float value)
{
 int lowby,highby,an_data;
 an_data=floor(((value+2048)/4096)*4096);
 lowby=an_data&0xff;
 highby=an_data>>8;
 outportb(0x283,lowby);
 outportb(0x284,highby);
}