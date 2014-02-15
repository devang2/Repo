#define SC signed char
#define UC unsigned char
#define SI signed int
#define UI unsigned int
#define UL unsigned long int
#define SL signed long int

#define  G_BX  		94		/* gen menu begin X Coord. */
#define  G_BY  		73 		/* gen menu begin Y Coord. */
#define  G_YAD 		32 	        /* gen menu addition in Y Dir. */
#define  Z_YAD 		32 	        /* zen menu addition in Y Dir. */

#define  M_BX  		3
#define  M_BY  		68
#define  M_EX  		90
#define  M_YAD 		34 //38

#define  S_BX  		91
#define  S_BY  		36
#define  S_XAD 		91
#define  S_EY  		67

#define  H_BX  		210
#define  H_BY  		190
#define  H_XAD 		100
#define  H_YAD 		40

#define  D_LEFT   	91
#define  D_TOP    	68
#define  D_RIGHT  	639
#define  D_BOTTOM 	448

#define  H_X_M    	3
#define  H_Y_M    	1
#define  M_MENU   	11   //10
#define  S_MENU   	6

#define  C_EX     	183
#define  T_EX     	546

#define  MAX_LEVEL 	5
#define  MAX_NODE 	400

typedef  struct
	{
		UI sub1;
		UC sub_number;
		UC curr;
	}  HMENU;

