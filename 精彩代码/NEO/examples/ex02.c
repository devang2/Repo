/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   这个例子是从Allegro V3.12的例程包中移植过来的,原文件为examples\ex5.c
   原作者是Shawn Hargreaves.这个程序示范怎样使用键盘例程
*/

#include "neo.h"

main()
{
   int k;


   install_keyboard(); 

   /* keyboard input can be accessed with the readkey() function */
   printf("\nPress some keys (ESC to finish)\n");
   do
   {
      k = readkey();
      printf("readkey() returned %-6d (0x%04X)\n", k, k);
   } while ((k & 0xFF) != 27);

   /* the ASCII code is in the low byte of the return value */
   printf("\nPress some more keys (ESC to finish)\n");
   do
   {
      k = readkey();
      printf("ASCII code is %d\n", k & 0xFF);
   } while ((k & 0xFF) != 27);

   /* the hardware scancode is in the high byte of the return value */
   printf("\nPress some more keys (ESC to finish)\n");
   do
   {
      k = readkey();
      printf("Scancode is %d\n", k >> 8);
   } while ((k & 0xFF) != 27);

   /* key qualifiers */
   printf("\nPress some more keys (ESC to finish)\n");
   do
   {
      k = readkey();
      printf("You pressed ");
      printf("'%c' [0x%02x]", (k & 0xFF), (k & 0xFF));
      printf("\n");
   } while ((k & 0xFF) != 27);

   /* various scancodes are defined in allegro.h as KEY_* constants */
   printf("\nPress F6\n");
   k = readkey();
   while ((k & 0x00ff) != KEY_F6)
   {
      printf("Wrong key, stupid! I said press F6\n");
      k = readkey();
   }
   printf("Thank you\n");

   /* for detecting multiple simultaneous keypresses, use the key[] array */
   printf("\nPress a combination of numbers (ESC to finish)\n");
   do
   {
      if (_get_asc() == '0') printf("0"); else printf(" ");
      if (_get_asc() == '1') printf("1"); else printf(" ");
      if (_get_asc() == '2') printf("2"); else printf(" ");
      if (_get_asc() == '3') printf("3"); else printf(" ");
      if (_get_asc() == '4') printf("4"); else printf(" ");
      if (_get_asc() == '5') printf("5"); else printf(" ");
      if (_get_asc() == '6') printf("6"); else printf(" ");
      if (_get_asc() == '7') printf("7"); else printf(" ");
      if (_get_asc() == '8') printf("8"); else printf(" ");
      if (_get_asc() == '9') printf("9"); else printf(" ");
      printf("\r");
      fflush(stdout);
   }while (!(get_asc() == 27));

   printf("\n");
   clear_keybuf();
   remove_keyboard();
}
