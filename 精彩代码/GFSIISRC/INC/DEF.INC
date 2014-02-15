#define SC signed char
#define UC unsigned char
#define SI signed int
#define UI unsigned int
#define UL unsigned long int
#define SL signed long int

#define TRUE  0x01
#define FALSE 0

#define MAX_X	   	639		/* maxium screen resolution dots define */
#define MAX_Y		479

#define SD_LEFT		96		/* small dialogue window coordinates define */
#define SD_TOP		69
#define SD_RIGHT	634
#define SD_BOTTOM	443

#define LD_LEFT		5 		/* large dialogue window coordinates define */
#define LD_TOP		69
#define LD_RIGHT	634
#define LD_BOTTOM	443

#define  MENU_HELP 	0
#define  ESC_HELP	0
#define  PRIVI_HELP	0
#define  MAX_NODE_NUM	200

#define MAX_EMB_HANDLES 16  /* Maxium EMB  handles. */

typedef struct
    {
		UI start_x,start_y;	/*表格左上角点的屏幕坐标*/
		UC head_h;         	/* 表头高度 */
		UC unit_h;         	/* 表项高度 */
		UC unit_num;       	/*表项个数  */
		UC item_num;       	/*表栏个数  */
		UC item_w[20];     	/*每个表栏的宽度 */
		UC line_color;     	/*表格线的颜色   */
	}TABLE_STRUCT;

typedef struct
    {
		UI dot_sx,dot_sy;
		UI dot_ex,dot_ey;
		UC unit_x,unit_y;
	}UNIT_STRUCT;

typedef struct
	{
		UI sx, sy;
		UI ex, ey;
	}MOUSE_BAND_STRUCT;

extern TABLE_STRUCT;
extern UNIT_STRUCT;
