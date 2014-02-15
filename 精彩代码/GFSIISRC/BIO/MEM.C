#include <alloc.h>
#include "def.inc"
#include "func.inc"

void *mem_alloc(UL nbytes)
{
	void far *ptr;

	ptr = farmalloc(nbytes);
	if (ptr==NULL)
		exit_scr(1,"\n\rINIT_SCR/mem_alloc(): Not enough memory: %ld", nbytes);
	return(ptr);
}

void mem_free(void far *ptr)
{
    if(ptr != 0)
	farfree(ptr);
    return;
}
