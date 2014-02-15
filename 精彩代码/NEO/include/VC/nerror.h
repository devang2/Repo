/*NEO SDK V2.1.41 For DOS
  Copyleft Cker Home 2003-2005.

  Open Source Obey NEO_PL.TXT.
  http://neo.coderlife.net
  ckerhome@yahoo.com.cn

  �ļ����� : nerror.h
  ժ    Ҫ : ��ͷ�ļ��а�����NEO SDK���йش���Ĵ�������ȫ�ֱ���������������
  ��ǰ�汾 : V2.64
  ��    �� : ����
  ������� : 2005.7.2

  ȡ���汾 : V2.61
  ԭ �� �� : ����
  ������� : 2005.6.22
*/

#ifndef NERROR_H
#define NERROR_H

#define FALSE 0
#define TRUE  !FALSE

/* NEO���ɵĿ�ִ���ļ���·����ȣ������ĳ���װ��·���Ƚ�����ʵ��������ֵ */
#ifndef PATH_LENGTH
#define PATH_LENGTH  80
#endif
#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#endif
#ifndef MID
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif

#ifndef ABS
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#endif
#ifndef SGN
#define SGN(x)       (((x) >= 0) ? 1 : -1)
#endif

#ifndef  NEO_error_unused
#ifndef  NEO_chinese_err_info_unused
#define  ERR_LOG_FILE       "������־.err"

/*�û���������������Լ�����Ĵ�����Ϣ�����ģ�*/
/*You can add your own error information at here(in chinese)*/
#define  N_NEO_NOT_INIT     "NEOδ��ʼ��"
#define  NO_MEMORY          "�ڴ治��"
#define  CREAT_FILE_FALSE   "�����ļ�ʧ��"
#define  NOT_INIT_ADVMOUSE  "�߼����δ��ʼ��"
#define  ERR_VIDEO_MODE     "�����ʵ���ʾģʽ"
#define  ERR_FILE_TYPE      "�����ļ�����"
#define  NO_FILE            "�Ҳ����ļ�"
#define  NOT_INIT_MOUSE     "���δ��װ"
#define  SOURCE_FILE_BAD    "Դ�ļ�����"
#define  DEST_FILE_BAD      "Ŀ���ļ�����"
#define  WRONG_FORMAT       "�����ļ���ʽ"
#define  WRONG_SB_SETTING   "�������ô���"
#define  N_NOT_VESA_CARD    "��VESA��׼�Կ�"
#define  UNDEFINED_ERROR    "δ�������"

#else
#define  ERR_LOG_FILE       "neoerror.err"

/*�û���������������Լ�����Ĵ�����Ϣ��Ӣ�ģ�*/
/*You can add your own error information at here(in english)*/
#define  N_NEO_NOT_INIT     "NEO not init"
#define  NO_MEMORY          "No mem"
#define  CREAT_FILE_FALSE   "Can't Creat file"
#define  NOT_INIT_ADVMOUSE  "Adv-mouse has not ready"
#define  ERR_VIDEO_MODE     "Not suitable video mode"
#define  ERR_FILE_TYPE      "Err file type"
#define  NO_FILE            "Can't find file"
#define  NOT_INIT_MOUSE     "Mouse was not installed"
#define  SOURCE_FILE_BAD    "source file bad"
#define  DEST_FILE_BAD      "dest file bad"
#define  WRONG_FORMAT       "Wrong file format"
#define  WRONG_SB_SETTING   "Wrong SB settings"
#define  N_NOT_VESA_CARD    "NOT VESA Standard VideoCard"
#define  UNDEFINED_ERROR    "Undefined error"

#endif
#endif

#ifdef __TINY__
#define COMPILE_MODE "΢ģʽ(Tiny)"
#endif

#ifdef __SMALL__
#define COMPILE_MODE "Сģʽ(Small)"
#endif

#ifdef __MEDIUM__
#define COMPILE_MODE "��ģʽ(Medium)"
#endif

#ifdef __COMPACT__
#define COMPILE_MODE "����ģʽ(compact)"
#endif

#ifdef __LARGE__
#define COMPILE_MODE "��ģʽ(Large)"
#endif

#ifdef __HUGE__
#define COMPILE_MODE "��ģʽ(Huge)"
#endif



/*������������޸�!*/
char g_routines = 0;     /*���λΪ1:����ͼ��ģʽ;��2λΪ1:�߼�������̱�����;��3λΪ1:ʱ�����̱�����;��4λΪ1:����
�����������������������������̱�����;��5λΪ1:XMS���̱�����;��6λΪ1:EMS���̱�����;��7λΪ1:��Ч���̱�����*/

typedef struct Errinfo_T
{  /*����λ��*/
   char *errloc; /*����ʱ�����������ں����ĺ�����.*/
   /*����ע*/
   char *errtag;
   char  handle; /*Ϊ1ʱ�������ִ�У���������˳�*/
}Errinfo_t;
#if defined NEO_error_unused
#define NEO_sys_report_error_unused
#endif


#define get_err_method() err_method(-1)
char err_method(int method);
void throw_error(Errinfo_t err);

/*������:�ò�����3λ��ʹ��:��1λΪ1:������;
                             ��2λΪ1:��������Ϣд����־;
                             ��3λΪ1:����Ļ����ʾ������Ϣ;
  Ĭ�������һ������ǽ�method�ĵ�1,2λ��1��method=3.*/
#ifndef NEO_error_unused
char err_method(int method)
{
   static int oldmeth1 = 3;
   if (method >= 0)
   {
      oldmeth1 = method;
   }

   return oldmeth1;
}
#endif


/*---------------------------------------------------*
 *��������: �׳�һ������,������ʾ������Ϣ�������    *
 *          ��.                                      *
 *����˵��: �������,�ڱ�ͷ�ļ��ж��к궨��.         *
 *����˵��: ��.                                      *
 *��    ע: ������������.�û���ֱ�ӵ���.         *
 *---------------------------------------------------*/
#ifndef NEO_error_unused
void throw_error(Errinfo_t err)
{
   FILE *errlog_p;
   static char fir_run;
   char method = err_method(-1);

   if (method & 2) /*д�������־*/
   {
      errlog_p = fopen(ERR_LOG_FILE, "a");

      if (!fir_run)
      {
         fprintf(errlog_p, "      {ERROR LOG}\n");
         fprintf(errlog_p, "{NEO SDK V2.1.41 For DOS}\n");
#ifndef MSVC15
         fprintf(errlog_p, "{Compile   Mode} : %s.\n", COMPILE_MODE);
#endif
         fprintf(errlog_p, ">>RUN START\n");
         fir_run = 10;
      }
      fprintf(errlog_p, ">>[Error   in] : %s().\n", err.errloc);
      fprintf(errlog_p, ">>[Error info] : %s.\n\n", err.errtag);
      fclose(errlog_p);
   }
   if (method & 4) /*����Ļ����ʾ*/
   {
      fprintf(stderr, "[Error   in] : %s().\n", err.errloc);
      fprintf(stderr, "[Error info] : %s.\n\n", err.errtag);
   }
   if (err.handle != 1)
   {
      exit(err.handle);
   }
}
#endif


#endif
