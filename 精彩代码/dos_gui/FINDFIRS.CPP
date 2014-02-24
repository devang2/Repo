
/* findfirst and findnext example */

#include <stdio.h>
#include <dir.h>
#include <ERRNO.H>

int main(void)
{
   struct ffblk ffblk;
   int done;

   char a[255];

   printf("Directory listing of \n");
   scanf ("%s",a);
   done = findfirst(a,&ffblk,0);
   while (!done)
   {
      printf("  %s\n", ffblk.ff_name);
      done = findnext(&ffblk);
   }
   int b=ENOENT; 
   printf ("\ENOENT=%d",b);
   return 0;
}
