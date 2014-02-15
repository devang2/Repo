
#define MAX_USER	2048
#define MAX_TRK		288
#define MAX_MFC		32
#define MAX_TSR		64

#define MAX_OFFNO	100
#define MAX_SP		6
#define MAX_DIR		14
#define HOLIDAYS	7
#define MAX_TOLLNO	3960
#define MAX_INTELNO 289

#define MAX_PABX	 20
#define MAX_PABXTELE 40
#define MAX_POS      6
#define MAX_POSTELE  10

#define CPU386		9
#define NTS			10

#define FD_WH		0xa0     /* service name for wei-hu */
#define FD_CS		0xb0     /* service name for ce-shi */
#define FD_HT		0xc0     /* service name for hua-tong */
#define FD_JF		0xd0     /* service name for ji-fei */
#define FD_SG		0xf0     /* service name for shu-ju-she-ding */
#define FD_TF		0xe0     /* service name for special TE */

/* ----------------------- BILL_ORG_STRUCT ------------------------- */
/* PUBLIC */
/* Total 39 bytes per structure */

typedef struct
	{
		UL  csn;
		UC  type;
		UC	caller[4];
		UC	callee[8];
		UC	E_year,E_month,E_day,E_hour,E_minute,E_second;
		UL	time;
		UI  caller_trk;
		UI  callee_trk;
	} BILL_ORG_STRUCT;

/* ------------------------- USER_STRUCT --------------------------- */
/* PUBLIC */
/* Total  22 bytes per structre */

typedef struct
	{
		UI Abbre     : 1;
		UI HotLine   : 1;
		UI CallBarr  : 1;
		UI NoDisturb : 1;
		UI Seek      : 1;
		UI Alarm     : 1;
		UI RegCall   : 1;
		UI CallTrans : 1;
		UI NoAnsTrans: 1;
		UI Absent    : 1;
		UI CallWait  : 1;
		UI CallBack  : 1;
		UI ThreeParty: 1;
		UI Conf      : 1;
		UI Other1    : 1;
		UI Other2    : 1;
	} NEW_SERVICE ;

typedef struct
	{
		UI	UserNo;
		UI	Account;
		UI  UserType :2;
		UI	KA       :2	;
		UI  UserLoc  :2 ;
		UI  HighLay  :2 ;
		UI  TCBnum   :8 ;
		NEW_SERVICE  NSA ;
		UI  CallBarrA:3 ;
		UI  ReleCtrl :2 ;
		UI  Pole     :1 ;
		UI  TeleType :2 ;
		UI  UserID   :2 ;
		UI  CallBarPsw  ;

		UI	MonthFee;
		UI	OutsideFee;
		UI	PhoneFee;
		UI	NS_Fee;
		UI  ConstructFee ;
		UI	OtherFee;
		UI	Rural:		1;
		UI	CountMode:	4;
		UI  Fee      :1 ;
		UC  Operator[9] ;
		UC  Lastdate[3] ;
	} USER_STRUCT ;

typedef struct
	{
		UC Used        ;
		UC UserName[17];
		UC UserAddr[21];
		UC Bank[17]    ;
		UC Baccount[11];
		UI Year        ;
		UC Month       ;
		UC Day         ;
	} USER_ACC;

/* -------------------------- SP_STRUCT --------------------------- */
/* PUBLIC */
/* total  3 bytes per structure */
/*
typedef struct
	{
		UI	ChargeMode:	1;
		UI	ReleaseMode:2;
		UI	OutGoing:	1;
		UC	No[4];
	} SP_STRUCT;
*/

typedef struct
	{
		UC	Used   	;
		UC  SpNo   	;
		UC	Outgoing;
		UC  OfficeDir;
		UI	code[6] ;
		UC 	Free		;
	} SP_STRUCT;
