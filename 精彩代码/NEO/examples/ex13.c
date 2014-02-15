/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   这个简单的程序演示了如何使用扩充内存
*/

/*以下编译开关开启对EMS的支持*/
#define  NEO_ems_used

#include "neo.h"

void main()
{
   long ems_handle;
   long avail;
   int  i;
   char teststr[80];

   neo_init();
   install_keyboard();
   /*扩充内存初使化，失败即退出*/
   if(!ems_init())
   {
      printf("Expanded memory is not present\n");
      readkey();
      exit(0);
   }
   avail = ems_avail(); /*获得可用的扩充内存逻辑页页数*/
   if (!avail)
   {  /*失败退出*/
      printf("Expanded memory manager error\n");
      readkey();
      exit(0);
   }
   printf("There are %ld pages available\n\n",avail);
   /*分配10页(共160KB)的扩充内存*/
   if((ems_handle = ems_alloc(10)) < 0)
   {
      printf("Insufficient pages available\n");
      readkey();
      exit(0);
   }
   /*向申请到的10个逻辑页面里写入测试字符串*/
   for (i = 0; i < 10; i++)
   {
      sprintf(teststr,"%02d This is a test string\n",i);
      ems_map(ems_handle,0,i);/*首先将扩充内存和常规内存建立映射关系*/
      ems_move(0,teststr,strlen(teststr) + 1);/*将测试字符串从常规内存复制到扩充内存*/
   }
   /*从10个逻辑页面中读出测试字符串*/
   for (i = 0; i < 10; i++)
   {
      ems_map(ems_handle,0,i);/*建立扩充内存和常规内存的映射关系*/
      ems_get(0,teststr,strlen(teststr) + 1);/*从扩充内存中读出测试字符串并显示*/
      printf("READING BLOCK %d: %s\n",i,teststr);
   }
   /*显示句柄ems_handle的句柄数*/
   printf("The ems_handle have %d pages.\n", get_handle_pages(ems_handle));
   ems_free(ems_handle);/*释放扩充内存句柄*/
   readkey();
}
