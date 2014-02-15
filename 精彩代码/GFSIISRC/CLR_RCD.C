#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : clear all the telephone records in Record Storage Device(RSD)
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void clr_rcd(void)
{
    UC   flag, suc;
    UC   ret_val;
    UC   port;

    flag = valid_pass(CLR_PWD);
    if(flag == 2)		/* ESC */
    {
	return;
    }
    if(flag == FALSE)
    {
	message(PASS_ERR);
	return;
    }

    /* Conform for clearing */
    ret_val = select_1in2(6);
    if (ret_val==2)
    {
        pop_back(H_BX,H_BY,H_BX+3*H_XAD,H_BY+2*H_YAD,LIGHTGRAY);
        hz16_disp(H_BX+H_XAD, H_BY+H_YAD/2, "正在联机...", BLACK); /* connecting */

        for(port=0; port<PORT_NUM; port++)
            if(Sys_mode.com_m[port]) {
	        suc = ConnTermAndCPU(port);       /* connect with Exchanger */
	        if(suc != 1)                  /* not successful */
	            continue;

          	/* Clear the MSU */
		ToCPU(port, 0xb3); ToCPU(port, 0xb3); ToCPU(port, 0xb3);
		ToCPU(port, 0xb3); ToCPU(port, 0xb3); ToCPU(port, 0xb3);
		ToCPU(port, 0xb3); ToCPU(port, 0xb3); ToCPU(port, 0xb3);
            }
    }

    outportb(SIO_IER[0],0);
    outportb(SIO_IER[1],0);
    rid_pop();
    return;
}

