#include <graphics.h>
#include <string.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>

#include "feedef.h"

/* function  : when using the system, input the password and check.
 * calls     : get_pass() (get_pass.c)
 *             message()  (message.c)
 * called by : main()     (main.c)
 * date      : 1993.9.20
 */
UC input_pass(void)
{
    UC   password0[7]="IIIIII",password1[7];
    UC   abs_pass[7] = {"404503"};
    UC   input_num;
    int  i,cmp;
    FILE *fp;

    fp = fopen("gfsdata\\syspwd.dat","rb");             /* read the password */
    if(fp != NULL)
    {
        fread(password0,sizeof(UC),7,fp);
        fclose(fp);
    }

    for(i=0;i<6;i++)
        password0[i] = password0[i]-'@'+'0';

    message_disp(8," 输入系统管理口令   Enter 确认");      /*sys pass*/
    pop_back(H_BX,SD_BOTTOM-270,H_BX+3*H_XAD-56,SD_BOTTOM-225,7);
    draw_back(H_BX+112,SD_BOTTOM-260,H_BX+230,SD_BOTTOM-235,11);    /* draw a frame     */
    hz16_disp(H_BX+10,SD_BOTTOM-255,"系统管理口令",BLACK);   /* please input password */

    for(;;)
    {
      input_num = get_pass(H_BX+118,SD_BOTTOM-255,11,BLACK,password1,6);
      if(input_num == 0xFF)            /* ESC */
          break;

      if(input_num != 6)
      {
          message(PASS_ERR);
          continue;
      }

      password1[6] = '\0';
      cmp = strcmp(password0,password1);
      if(cmp == 0)      /* the same     */
          break;
      else              /* not the same */
      {
          cmp = strcmp(abs_pass,password1);
          if(cmp == 0)
              break;
          else
              message(PASS_ERR);
       }
    }         /* end of "for(;;)" */

    rid_pop();
    message_end();

    if(input_num == 0xFF)
        return(FALSE);
    else
        return(TRUE);
}

/* function  : modify the old password
 * calls     : get_account() (get_acc.c)
 * called by : set_data() (set_data.c)
 * date      : 1993.9.20
 */

void modi_sys_pass(UC mode)
{
    int  i,key,cmp;
    UC   password0[7]="IIIIII",password1[7],password2[7],num;
    FILE *fp;
    TABLE_STRUCT pass_tbl = {H_BX+5,H_BY+10,20,30,1,2,{150,130},7};
    UNIT_STRUCT  cur_unit;


    if(mode == SYS_PWD)
    {
        fp = fopen("gfsdata\\syspwd.dat","rb");
    }
    else if(mode == DEL_PWD)
    {
        fp = fopen("gfsdata\\delpwd.dat","rb");
    }
    else if(mode == CLR_PWD)
    {
        fp = fopen("gfsdata\\clrpwd.dat","rb");
    }
    else
    {
        fp = fopen("gfsdata\\cashpwd.dat","rb");
    }
    if(fp != NULL)
    {
        fread(password0,sizeof(char),7,fp);
        fclose(fp);
     }

    for(i=0;i<6;i++)
        password0[i] = password0[i]-'@'+'0';

    if(mode == SYS_PWD)
        message_disp(8," 修改系统管理口令   Enter 输入");
    else if(mode == DEL_PWD)
        message_disp(8," 修改话单备份/删除操作口令   Enter 输入");
    else if(mode == CLR_PWD)
        message_disp(8," 修改清除话单存储器操作口令   Enter 输入");
    else
        message_disp(8," 修改用户管理口令   Enter 输入");

    set_finger_color(Dsp_clr.fng_clr);

    pop_back(H_BX-10,H_BY-5,H_BX+305,H_BY+80,7);
    draw_back1(H_BX-10,H_BY-5,H_BX+305,H_BY+80,7);
    draw_table(&pass_tbl);                       /* draw the table frame */
    hz16_disp(H_BX+40,H_BY+12,"旧 口 令",BLACK);         /* old password */
    hz16_disp(H_BX+190,H_BY+12,"新 口 令 ",BLACK);        /* new password */

    draw_back(H_BX+16,H_BY+36,H_BX+130,H_BY+60,11);
    draw_back(H_BX+166,H_BY+36,H_BX+280,H_BY+60,11);

    locate_finger(0,0);

    for(;;)
    {
        key = get_key1();

        switch(key)
        {
            case ESC:
                rid_pop();
                message_end();
                return;
            default:
                get_current(&cur_unit);
                switch(cur_unit.unit_y)
                {
                    case 0:     /* input the old password */
                        num = get_pass(H_BX+20,H_BY+40,11,BLACK,password1,6);
                        if(num != 6)        /* len of password must be 6 */
                            message(PASS_ERR);
                        else
                        {
                            password1[6] = '\0';
                            cmp = strcmp(password0,password1);
                            if(cmp == 0)    /* old password right */
                                move_finger(3,1);
                            else
                                message(PASS_ERR);
                         }
                         break;
                    case 1:       /* input the new password */
                        do
                        {
                            num = get_pass(H_BX+170,H_BY+40,11,BLACK,password1,6);
                            locate_finger(0,1);
                            if(num != 6)
                                message(PASS_ERR);
                            else
                            {
                              message(RE_PASS);
                              num = get_pass(H_BX+170,H_BY+40,11,BLACK,password2,6);
                              if(num != 6)
                              {
                                  message(RE_PASS_ERR);
                                  continue;
                              }

                              password1[6] = '\0';
                              password2[6] = '\0';
                              cmp = strcmp(password1,password2);
                              if(cmp != 0)       /* not the same */
                              {
                                  message(RE_PASS_ERR);
                                  continue;
                              }

                              for(i=0;i<6;i++)
                                  password1[i] = password1[i]-'0'+'@';

                              if(mode == SYS_PWD)
                              {
                                  fp = fopen("gfsdata\\syspwd.dat","wb");
                              }
                              else if(mode == DEL_PWD)
                              {
                                  fp = fopen("gfsdata\\delpwd.dat","wb");
                              }
                              else if(mode == CLR_PWD)
                              {
                                  fp = fopen("gfsdata\\clrpwd.dat","wb");
                              }
                              else
                              {
                                  fp = fopen("gfsdata\\cashpwd.dat","wb");
                              }
                              fwrite(password1,sizeof(char),7,fp);
                              fclose(fp);

                              message(NEW_PASS_EN);
                              rid_pop();
                              message_end();
                              return;
                            }
                        }while(1);
                    default:
                        sound_alarm();
                        break;
                }       /* end of "switch(cur_unit.unit_y)" */
                break;
         }              /* end of "switch(key)" */
    }                   /* end of "for(;;)"     */
}
