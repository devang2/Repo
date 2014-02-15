#include <string.h>
#include <graphics.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>

#define BK_T_C    1             /* topic backcolor */
#define BK_MODE   SOLID_FILL    /* fill_mode */
#define WORD_C    15			/* characters' color */
#define DELX      10			/* topic_border to topic distance: DELT_x */
#define DELY      4				/* topic_border to topic distance: DELT_y */
#define LW        1				/* line width */

static UC Finger_c = 12;
static UC buf[200];
static UNIT_STRUCT Unit;
static TABLE_STRUCT Table;
static hide_flag=0; /*  0 ----- cursor hide */
                    /*  1 ----- cursor display */



/*----------------------------------------------------*/
/*              draw the title of the table           */
/*----------------------------------------------------*/

void draw_topic(UI x,UI y,UC *topic)
{
UI lent;
    lent=strlen(topic)*8;
    setfillstyle(BK_MODE,BK_T_C);
    bar(x-DELX,y-DELY,x+lent+DELX,y+16+DELY);
    setcolor(12);
    line(x-DELX,y-DELY+22,x+lent+DELX,y-DELY+22);
    line(x-DELX,y-DELY+23,x+lent+DELX,y-DELY+23);
    hz16_disp(x,y,topic,WORD_C);
}

/*---------------------------------------------------*/
/*           draw table lines of the table           */
/*---------------------------------------------------*/

void draw_table(TABLE_STRUCT *table_data)
{
UI lenx=0,i,currx;
    Table= *table_data;
    hide_flag = 0;
    for(i=0;i<(*table_data).item_num;i++)
       lenx += (*table_data).item_w[i]+LW;
    setcolor ((*table_data).line_color);
    rectangle((*table_data).start_x,(*table_data).start_y,
     (*table_data).start_x+lenx,(*table_data).start_y+(*table_data).head_h+LW);

    rectangle((*table_data).start_x-1,(*table_data).start_y-1,
     (*table_data).start_x+lenx+1,(*table_data).start_y+(*table_data).unit_num*
     ((*table_data).unit_h+LW)+(*table_data).head_h+2*LW+1);

    for(i=0;i<=(*table_data).unit_num;i++)
       line((*table_data).start_x,
        (*table_data).start_y+(*table_data).head_h+LW+i*((*table_data).unit_h+LW)+LW,
        (*table_data).start_x+lenx,
        (*table_data).start_y+(*table_data).head_h+LW+i*((*table_data).unit_h+LW)+LW);

    currx=(*table_data).start_x;

    for(i=0;i<=(*table_data).item_num;i++)
    {
        line(currx,(*table_data).start_y,
        currx,
        (*table_data).start_y+(*table_data).head_h+LW+(*table_data).unit_num*
        ((*table_data).unit_h+LW)+LW);
        currx += (*table_data).item_w[i]+LW;
    }
}

/*------------------------------------------------------*/
/*        locate the cursor in (x,y)                    */
/*------------------------------------------------------*/

void locate_finger(UC unit_x,UC unit_y)
{
UI currx=0,x,y,i;

    for(i=0;i<unit_y;i++)
       currx += Table.item_w[i]+LW;

    Unit.unit_x=unit_x;
    Unit.unit_y=unit_y;
    Unit.dot_sx=Table.start_x+currx+LW;
    Unit.dot_sy=Table.start_y+LW+Table.head_h+2*LW+unit_x*(Table.unit_h+LW);
    Unit.dot_ex=Unit.dot_sx+Table.item_w[unit_y]-1;
    Unit.dot_ey=Unit.dot_sy+Table.unit_h-1;

    x      = Unit.dot_sx+2;
    y      = Unit.dot_sy+(Table.unit_h-8);

    getimage(x,y,x+15,y+23,buf);
    finger(x,y,Finger_c);
    hide_flag=1;

}

/*----------------------------------------------------------*/
/*   move cursor according to  direction and step           */
/*   direction = 0 ------- up                               */
/*               1 ------- down                             */
/*               2 ------- left                             */
/*               3 ------- right                            */
/*----------------------------------------------------------*/

void move_finger(UC direction,UC step)
{
UC unit_x,unit_y;
UI x,y;
    unit_x = Unit.unit_x;
    unit_y = Unit.unit_y;
    x      = Unit.dot_sx+2;
    y      = Unit.dot_sy+(Table.unit_h-8);
    if (hide_flag==0)  getimage(x,y,x+15,y+23,buf);
    switch(direction)
    {
        case 0:
            if(unit_x>0)
                if(unit_x>=step)  unit_x -= step;
              else unit_x=0;
            break;
        case 1:
            if(unit_x<Table.unit_num-1)
                if(unit_x<Table.unit_num-step-1) unit_x += step;
                else unit_x=Table.unit_num-1;
            break;
        case 2:
            if(unit_y>0)
                if(unit_y>=step) unit_y -= step;
                else unit_y=0;
            break;
        case 3:
            if(unit_y<Table.item_num-1)
                if(unit_y<Table.item_num-step-1) unit_y += step;
                else unit_y=Table.item_num-1;
            break;
    };
    putimage(x,y,buf,COPY_PUT);
    locate_finger(unit_x,unit_y);

}

/*------------------------------------------------------*/
/*       hide the cursor from screen                    */
/*------------------------------------------------------*/

void hide_finger(void)
{
UI x,y;
    x  = Unit.dot_sx+2;
    y  = Unit.dot_sy+(Table.unit_h-8);
    putimage(x,y,buf,COPY_PUT);
    hide_flag = 0;
}
/*------------------------------------------------------*/
/*          restore the cursor to the old place         */
/*------------------------------------------------------*/
void echo_finger(void)
{
UI x,y;

     if(hide_flag==1) return;
     x = Unit.dot_sx+2;
     y = Unit.dot_sy+(Table.unit_h-8);
    getimage(x,y,x+15,y+23,buf);
    finger(x,y,Finger_c);
    hide_flag=1;

}

/*---------------------------------------------------*/
/*           get the current unit information        */
/*---------------------------------------------------*/

void get_current(UNIT_STRUCT *cur_unit)
{
    *cur_unit=Unit;
}

/*----------------------------------------------------------------------*/
/*          get any unit information acording to the (unit_x,unit_y)    */
/*----------------------------------------------------------------------*/

void get_certain(UNIT_STRUCT *cer_unit)
{
UI currx=0,i;

    for(i=0;i<(*cer_unit).unit_y;i++)
       currx += Table.item_w[i]+LW;

    (*cer_unit).dot_sx = Table.start_x+currx+LW;
    (*cer_unit).dot_sy = Table.start_y+LW+Table.head_h+2*LW+(*cer_unit).unit_x*(Table.unit_h+LW);
    (*cer_unit).dot_ex = (*cer_unit).dot_sx+Table.item_w[(*cer_unit).unit_y]-1;
    (*cer_unit).dot_ey = (*cer_unit).dot_sy+Table.unit_h-1;

}

void set_finger_color(UC color)
{
    Finger_c=color;
}

