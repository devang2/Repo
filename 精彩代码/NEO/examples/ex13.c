/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   ����򵥵ĳ�����ʾ�����ʹ�������ڴ�
*/

/*���±��뿪�ؿ�����EMS��֧��*/
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
   /*�����ڴ��ʹ����ʧ�ܼ��˳�*/
   if(!ems_init())
   {
      printf("Expanded memory is not present\n");
      readkey();
      exit(0);
   }
   avail = ems_avail(); /*��ÿ��õ������ڴ��߼�ҳҳ��*/
   if (!avail)
   {  /*ʧ���˳�*/
      printf("Expanded memory manager error\n");
      readkey();
      exit(0);
   }
   printf("There are %ld pages available\n\n",avail);
   /*����10ҳ(��160KB)�������ڴ�*/
   if((ems_handle = ems_alloc(10)) < 0)
   {
      printf("Insufficient pages available\n");
      readkey();
      exit(0);
   }
   /*�����뵽��10���߼�ҳ����д������ַ���*/
   for (i = 0; i < 10; i++)
   {
      sprintf(teststr,"%02d This is a test string\n",i);
      ems_map(ems_handle,0,i);/*���Ƚ������ڴ�ͳ����ڴ潨��ӳ���ϵ*/
      ems_move(0,teststr,strlen(teststr) + 1);/*�������ַ����ӳ����ڴ渴�Ƶ������ڴ�*/
   }
   /*��10���߼�ҳ���ж��������ַ���*/
   for (i = 0; i < 10; i++)
   {
      ems_map(ems_handle,0,i);/*���������ڴ�ͳ����ڴ��ӳ���ϵ*/
      ems_get(0,teststr,strlen(teststr) + 1);/*�������ڴ��ж��������ַ�������ʾ*/
      printf("READING BLOCK %d: %s\n",i,teststr);
   }
   /*��ʾ���ems_handle�ľ����*/
   printf("The ems_handle have %d pages.\n", get_handle_pages(ems_handle));
   ems_free(ems_handle);/*�ͷ������ڴ���*/
   readkey();
}
