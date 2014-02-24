#include <stdio.h>
#include <stdlib.h>

char buf[_MAX_PATH];

void main(int argc, char *argv[])
{
  char a[255];
  scanf ("%s",a);
  {
	if (_fullpath(buf, a, _MAX_PATH) == NULL)
	  printf("Unable to obtain full path of %s\n",a);
	else
	  printf("Full path of %s is %s\n",a,buf);
  }
}


