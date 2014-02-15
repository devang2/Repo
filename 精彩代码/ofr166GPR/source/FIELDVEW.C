/******************************* FIELDVWG.C *******************************/
/*  Michael H. Powers, USGS, Branch of Geophysics, Golden, CO            */
/*  July 1994                                                            */
/*  Supported compilers:                                                 */
/*    Intel 386/486 C Code Builder 1.1a                                  */
/*        To compile: icc fieldvw.c libhpgl.lib                          */
/*  To run:  fieldvw filename                                            */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <malloc.h>
#include <hpgl.h>      /* Gary Olhoeft's graphics library */
#include <dzt_pgms.h>  /* written by Jeff Lucius, USGS, Golden, CO */
#include <sgy_pgms.h>  /* written by Jeff Lucius, USGS, Golden, CO */
#define COMPILE_DATE "April 19, 2000"
#define MAXPALETTES  5
#define ESC         27
#define ENTER       13
#define SH_RETURN  0x0c
#define TAB          9
#define F1         -59
#define F2         -60
#define F8         -66
#define F10        -68
#define L_ARROW    -75
#define R_ARROW    -77
#define S_L_ARROW   52
#define S_R_ARROW   54
#define C_L_ARROW -115
#define C_R_ARROW -116
#define C_DN_ARROW -145
#define C_UP_ARROW -141
#define HOME       -71
#define END        -79
#define PG_UP      -73
#define PG_DN      -81
#define DN_ARROW   -80
#define UP_ARROW   -72
#define S_DN_ARROW  50
#define S_UP_ARROW  56
#define ZERO_KEY    48
#define SETA "=\n\0"
#define SETM "=:\n\0"

/************************** functions ************************************/
void rainbo16();
void fieldvewpal();
void set_pal(int num);
void plot_title(char *flnm,char *adate,char *afreq);
void plot_time_scale(int c_flag);
void plot_model_scan(int c_flag);
void keys_to_use();
void move_right_ok();
void move_left_ok();
void calc_scan_ticks();
void scan_hilight(int flag);
void time_hilight(int flag);
void scan_ticks();
void samp_time_label(int flag);
void samp_time_text_label(int flag);
void display_gain_params(double emm,double eee);
void list_model_params(double rp,double tr,double mp,double sro);
void model_instr();
void refresh_image();
void draw_reflection(double relperm,double tradius,double mps,double sroffset);
void show_cursor_depth(double relperm);
int GetSSHdrs(long *nscans,int *nsamps,int *pt0,double *ttime,double *x0,
				double *xe,double *xi,double *ssfreq,double *antsep,
				char *dflnm,FILE *dfile);
int GetMala(long *nscans,int *nsamps,double *ttime,
				double *malafreq,double *antsep,double *xi,double *tinc,
				char *dflnm,FILE *dfile);
int ReadSegyReelHdr (int *swap_bytes, long *num_traces,
										struct SegyReelHdrStruct *hdrPtr, FILE *infile);
int GetNumBytes(long *num_bytes, FILE *dfile);
int fill_plot_image(int num,int num_hdrs,int start_flag,FILE *datfile);
void set_gain(short *intscan, double *gain, int datval0);
int brkstr(char **ptranm, char *dtaanm, char *set);
int lateral_corr();
void squeez_dat(int iter,int *keep,int *skip,int *newptr,int *oldptr,int i);
void spline(double *x,double *y,int n,double yp1,double ypn,double *y2);
void splint(double *xa,double *ya,double *y2a,int n,double x,double *y);
void help_sub(int num);
/**** the following subroutine is not used and failed when last checked ***/
/* void updatemarks(char *flnm, int tracenum, int add_rem_flag);          */
/**************************************************************************/

/**************************************************************************/
void make_eps_bw(char *filein); /* screen dump B&W */
/* void make_epsf(); */ /* screen dump "color" */
/**************************************************************************/

/************************** global variables *****************************/
unsigned char **plot_array;
extern int hpgl_pen_color;
unsigned char **mgrab_screen;
char hddate[10]="", comments[1600]=""; /* ,dbfile[15]="gpdebug.txt"; */
char title1[80]="", title2[80]="";
double sx1,sx2,tickinc,spacesamp,sxpm,slabcsize,stlcsize,markmark;
double pms_hmax,pms_wx1,pms_wx2,pms_wy2,pms_lby2,stltsize;
int abs_first=0,scr_first,scr_last,padx,pady,scansperscrn,*marks,mcnt=0;
int num_bits=0,num_samps=0,nnum_samps=0,curscan,cursamp=2,t0samp=0;
int mindif=32767,samp_flag=1,time_flag=0,scanlabellorg1,scanlabellorg2;
int s10_flag=0,ss_flag=0,mala_flag=0,seg2_flag=0,segy_flag=0;
int unknwn_flag=0,gain_flag=0;
int shift=8,bytesperscan,tstep=1,sfill=1,marks_flag=0,laserjet=0,signed_flag=0;
int maxrows,maxcols,*avgscan,fhdr_bytes=-1,thdr_bytes=-1,*new_to_old;
long ntotmscans;
double v1x1,v1x2,v1y1,v1y2,v2x1,v2x2,v2y1,v2y2;
double v3x1,v3x2,v3y1,v3y2,v4x1,v4x2,v4y1,v4y2;
double v5x1,v5x2,v5y1,v5y2,v6x1,v6x2,v6y1,v6y2;
double v7x1,v7x2,v7y1,v7y2,v8x1,v8x2,v8y1,v8y2,v9x1,v9x2,v9y1,v9y2;
double scrn_ratio,curtime,trange=0.0,space,digi,pixsamp0,pixsamplast;
/* FILE *debug_file; */

/*************************************************************************/
main(int argc, char *argv[])
{
	size_t mem_avail;
	unsigned char pal[768];
	char plotfile[80]="CRT",lab[80],*pm,ans[2]="";
	char acqdate[80],ant_freq[10];
	char datfilename[MAX_PATHLEN];
  char *env_test=NULL,cv1[20],cv2[20],cv3[20],cv4[20];
	unsigned short *oneu16bitscan,ss;
	int modes[13]={56,1024,261,8514,48,800,99,259,46,640,98,257,18};
	int actparm=0,endvkey_flag=0,refonim_flag=0,endgkey_flag=0,esc_flag=0;
	int i,j,k,m,p,itemp=-1,gmode,maxscansinmem,shift_flag=0;
	int idum,jumpsamp,br_flag=0,chksm_flag=0;
	int row,col,hsps,key,num=0,set_key=0;
	int ax1,ax2,colorindex=0,data_type,newmaxcols;
	int num_hdrs=1;
	long int curloc,startloc;
	long num_scans=0,num_bytes=0,abs_last=-1;
	double factor,jumptime,mpmark=1.0,tarrad=0.01,rperm=4.0,inc=0.0,sroff=0.0;
	double x_start,x_end,x_inc=1.0,t_inc=0.,ant_sep,ss_freq,mpscan,mala_freq;
	double lamp=1.0,eamp=0.0,dtemp,lgain;
	FILE *datfile = NULL, *title_file = NULL, *out_file = NULL;
	struct DztHdrStruct hdr1,hdr2,hdr3,hdr4,*hdrval[4];
	struct SegyReelHdrStruct segyhdr;
	struct SegyTraceHdrStruct tracehdr;

	#if defined(_INTELC32_)
		char *buffer = NULL;
		size_t vbufsize = 64512;
	#endif

	gmode=0;
	if (argc<2)
	{
		printf("\nFIELDVEW Compiled: %s   Author: Michael H. Powers, USGS\n",COMPILE_DATE);
		printf("FIELDVEW displays GPR field data on the sceen as an image.\n");
		printf("\nUsage is: fieldvew filename [-gxxx] [first] [last]\n");
		printf("filename is required and is a GPR data file (common extensions optional),\n");
		printf("-gxxx is optional, xxx is a video code for your graphics as follows:\n");
		printf("  ------------------------------------------------------------------------\n");
		printf("  resolution  IBM  ATI VGA Wonder  VESA  Tseng  Paradise  Hercules GB1024\n");
		printf("  ------------------------------------------------------------------------\n");
		printf(" 640x480x16    18       18           18    18      18        18\n");
		printf(" 320x240x256   20       20           20    20      20        20\n");
		printf(" 640x480x256            98          257    46      95       640\n");
		printf(" 800x600x256            99          259    48               800\n");
		printf("1024x768x256 8514      101 (16-col) 261    56              1024\n");
		printf("  ------------------------------------------------------------------------\n");
		printf("  with only modes 261, 56, and 1024 known to be fully functional - others\n");
		printf("  may not allow display of the help screens or creation of hardcopy output;\n");
		printf("\nfirst & last are optional, and determine a range of traces from the file.\n");
    printf("\nTo read SEGY data, the last command line option must be -y.\n");
		exit(1);
	}
	strcpy(datfilename,argv[1]);

	/* Open file */
	pm=datfilename; i=strlen(datfilename);
	if (*(pm+i-4)!='.') strcat(datfilename,".dzt\0");
	if ((datfile = fopen(datfilename,"rb")) == NULL)
	{
		strcpy(acqdate,datfilename);
		if (acqdate[0])
		{
			pm=strchr(acqdate,'.');
			if (pm!=NULL) strncpy(pm+1,"dt1\0",4);
			else strcat(acqdate,".dt1\0");
		}
		strcpy(datfilename,acqdate);
		if ((datfile = fopen(datfilename,"rb")) == NULL)
		{
			strcpy(acqdate,datfilename);
			if (acqdate[0])
			{
				pm=strchr(acqdate,'.');
				if (pm!=NULL) strncpy(pm+1,"rd3\0",4);
				else strcat(acqdate,".rd3\0");
			}
			strcpy(datfilename,acqdate);
			if ((datfile = fopen(datfilename,"rb")) == NULL)
			{
				strcpy(acqdate,datfilename);
				if (acqdate[0])
				{
					pm=strchr(acqdate,'.');
					if (pm!=NULL) strncpy(pm+1,"dzt\0",4);
					else strcat(acqdate,".dzt\0");
				}
				strcpy(datfilename,acqdate);
				if ((datfile = fopen(datfilename,"rb")) == NULL)
				{
					printf("\nUnable to open file named: %s\n",argv[1]);
					exit(2);
				}
			}
		}
	}

  cv1[0]=0;  cv2[0]=0;  cv3[0]=0;  cv4[20]=0;
  if (argc>=6)
	{
		strcpy(cv1,argv[2]);
		strcpy(cv2,argv[3]);
		strcpy(cv3,argv[4]);
    strcpy(cv4,argv[5]);
    if (strstr(cv4,"-y")) segy_flag=1;
		if (strstr(cv1,"-g"))
		{
			gmode=atoi(cv1+2);
			if (argc>3) abs_first=atoi(cv2);
			if (argc>4) abs_last=atol(cv3);
		}
		else if (strstr(cv2,"-g"))
		{
			gmode=atoi(cv2+2);
			abs_first=atoi(cv1);
			if (argc>4) abs_last=atol(cv3);
		}
		else if (strstr(cv3,"-g"))
		{
			gmode=atoi(cv3+2);
			abs_first=atoi(cv1);
			abs_last=atol(cv2);
		}
    else if (strstr(cv4,"-g"))
		{
      gmode=atoi(cv4+2);
			abs_first=atoi(cv1);
			abs_last=atol(cv2);
		}
		else
		{
			printf("Type FIELDVEW with no parameters to see usage.\n");
			exit(1);
		}
	}
  if (argc==5)
	{
		strcpy(cv1,argv[2]);
		strcpy(cv2,argv[3]);
		strcpy(cv3,argv[4]);
    if (strstr(cv3,"-y")) segy_flag=1;
		if (strstr(cv1,"-g"))
		{
			gmode=atoi(cv1+2);
			if (argc>3) abs_first=atoi(cv2);
      if ((argc>4)&&(!segy_flag)) abs_last=atol(cv3);
		}
		else if (strstr(cv2,"-g"))
		{
			gmode=atoi(cv2+2);
			abs_first=atoi(cv1);
      if ((argc>4)&&(!segy_flag)) abs_last=atol(cv3);
		}
		else if (strstr(cv3,"-g"))
		{
			gmode=atoi(cv3+2);
			abs_first=atoi(cv1);
			abs_last=atol(cv2);
		}
		else
		{
			printf("Type FIELDVEW with no parameters to see usage.\n");
			exit(1);
		}
	}
	else if (argc==4)
	{
		strcpy(cv1,argv[2]);
		strcpy(cv2,argv[3]);
    if (strstr(cv2,"-y")) segy_flag=1;
		if (strstr(cv1,"-g"))
      { gmode=atoi(cv1+2); if (!segy_flag) abs_first=atoi(cv2); }
		else if (strstr(cv2,"-g")) { gmode=atoi(cv2+2); abs_first=atoi(cv1); }
    else { abs_first=atoi(cv1); if (!segy_flag) abs_last=atol(cv2); }
	}
	else if (argc==3)
	{
		strcpy(cv1,argv[2]);
		if (strstr(cv1,"-g")) gmode=atoi(cv1+2);
    else if (strstr(cv1,"-y")) segy_flag=1;
		else abs_first=atoi(cv1);
	}

	if (gmode==0)  /** determine graphics mode ********************/
	{
		/******* try all 256 color modes hi to lo res first ******/
		i=0;
		while ( (gmode=plotter_is(modes[i],"CRT"))<20 ) i++;
		/******* defaults to mode 18 VGA 640x480x16 colors *******/
	}
	gmode=plotter_is(gmode,"CRT");  plotter_off();

	/************ get environment variable for LaserJet HPGL *******/
	env_test = getenv("LASERJET"); if (strstr(env_test,"TRUE")>0) laserjet=1;

	/***************** for debugging ********************
	if ((debug_file=fopen(dbfile,"wt"))==NULL)
		{   printf("\nUnable to open dbfile %s\n",dbfile); exit(2); }
	*****************************************************/

	/***************** title info in title file ***********************/
	strcpy(acqdate,datfilename);
	if (acqdate[0])
	{
		pm=strchr(acqdate,'.');
		if (pm!=NULL) strncpy(pm+1,"tle\0",4);
		else strcat(acqdate,".tle\0");
	}
	/* Open title file */
	if ((title_file = fopen(acqdate,"rt")) != NULL)
	{
		fgets(title1,79,title_file);
		fgets(title2,79,title_file);
		fclose(title_file);
	}
	acqdate[0]=0;
	/*******************************************************************/

	/* Speed up disk access by using large (conventional memory) buffer */
	#if defined(_INTELC32_)
		realmalloc(buffer,vbufsize);
		if (buffer)  setvbuf(datfile,buffer,_IOFBF,vbufsize);
		else         setvbuf(datfile,NULL,_IOFBF,vbufsize);
	#endif

	while (1)  /******* determine data format type *******/
	{

		/* check for Mala data - Get the header info */
		itemp = GetMala(&num_scans,&num_samps,&trange,
						&mala_freq,&ant_sep,&x_inc,&t_inc,datfilename,datfile);
		if (itemp != 0) rewind(datfile);
		else { mala_flag=1; break; }

		/* check for S10 data - Get the headers from the file */
		itemp = GetDztHdrs(&num_hdrs,&num_scans,&hdr1,&hdr2,&hdr3,&hdr4,datfilename,datfile);
		if (itemp > 0) rewind(datfile);
		else
		{
			s10_flag=1;
			if (itemp<0) chksm_flag=1;
			break;
		}

		num_hdrs=1;
		/* check for Sensors & Software data - Get the header info */
		itemp = GetSSHdrs(&num_scans,&num_samps,&t0samp,&trange,&x_start,&x_end,
						&x_inc,&ss_freq,&ant_sep,datfilename,datfile);
		if (itemp != 0) rewind(datfile);
		else { ss_flag=1; break; }

    if (segy_flag)
    {
      /* check for SEGY data - Get the header info */
      itemp = ReadSegyReelHdr(&br_flag,&num_scans,&segyhdr,datfile);
      if (itemp != 0)
      {
        rewind(datfile);
        printf("\nSEGY flag specified, but file fails SEGY format check.\n");
        exit(1);
      }
      else { segy_flag=1; break; }
    }

		itemp = GetNumBytes(&num_bytes,datfile);
		if (itemp == 0) { unknwn_flag=1; break; }

		printf("\nData format unknown.  Unable to continue with this file.\n");
		#if defined(_INTELC32_)
			realfree(buffer);
		#endif
		fclose(datfile);
		exit(3);
	}

	if (s10_flag)
	{
		fhdr_bytes=num_hdrs*(sizeof(struct DztHdrStruct));
		thdr_bytes=0;
		hdrval[0]=&hdr1; hdrval[1]=&hdr2; hdrval[2]=&hdr3; hdrval[3]=&hdr4;

		if (num_hdrs>1)
		{
			num=-1;
			printf("Which of the %d datasets do you want?\n",num_hdrs);
			while ((num<0)||(num>3))
			{
				switch (num_hdrs)
				{
					case 2:  printf("Valid numbers: 1,2 "); break;
					case 3:  printf("Valid numbers: 1,2,3 "); break;
					case 4:  printf("Valid numbers: 1,2,3,4 "); break;
				}
				scanf("%d",&idum); num=idum-1;
			}
			printf("Dataset %d of %d in this file.",num+1,num_hdrs);
			sprintf(lab,"  Dataset %d of %d",num+1,num_hdrs);
			strcat(datfilename,lab);
			num_scans/=num_hdrs;    /**** remove with new dzt_pgms.h ****/
		}

		sprintf(acqdate,"%s %d, %d (%02d:%02d:%02d)",
		month_abbr[hdrval[num]->rh_create.month],
				 hdrval[num]->rh_create.day,
				 hdrval[num]->rh_create.year+1980,
				 hdrval[num]->rh_create.hour,
				 hdrval[num]->rh_create.min,
				 hdrval[num]->rh_create.sec2*2);

		ant_freq[0] = 0;
		for (i=0; ant_number[i]; i++)
		{
			if (strstr(hdrval[num]->rh_antname,ant_number[i]))
			{   strcpy(ant_freq,ant_name[i]); break; }
		}

		num_samps=hdrval[num]->rh_nsamp;
		if (num_samps<=0) { printf("\nNo samples.  Bad data set.\n"); exit(1); }
		num_bits=hdrval[num]->rh_bits;
		bytesperscan=(num_samps*(num_bits/8))*num_hdrs;
		trange=hdrval[num]->rh_range;
		signed_flag=0;
	}

	else if (ss_flag)
	{
		strcpy(acqdate,hddate);
		sprintf(ant_freq,"%d MHz",(int)ss_freq);
		num_bits=16;  thdr_bytes=128;  fhdr_bytes=0;
		bytesperscan=(num_samps*2)+thdr_bytes;
		signed_flag=1;
	}

	else if (mala_flag)
	{
		strcpy(acqdate,hddate);
		if (mala_freq<10) mala_freq*=1000.;
		sprintf(ant_freq,"%d MHz",(int)mala_freq);
		if (trange<1.0) trange*=1000.;
		num_bits=16;  thdr_bytes=0;  fhdr_bytes=0;
		bytesperscan=(num_samps*2)+thdr_bytes;
		signed_flag=1;
	}

	else if (segy_flag)
	{
		if (br_flag) /** using this to check upper/lower byte swap **/
		{
			printf("\nThis SEGY data is still structured for a Unix machine.  It needs to\n");
			printf("have the upper and lower bytes swapped to be correctly read on a PC.\n");
			exit(1);
		}
		if (segyhdr.format!=3)
		{
			printf("\nThis program can only handle 16-bit (2-byte) fixed point SEGY data,\n");
			printf("or standard GPR formats (SIR10A (.dzt), pulse-EKKO (.dt1), or DT6000).\n");
			printf("There are many seismic packages that can read and display this SEGY data.\n");
			exit(1);
		}

		/* Get trace header */
		if (fread((void *)&tracehdr,sizeof(struct SegyTraceHdrStruct),1,datfile) < 1)
		{ printf("Error reading first trace header of SEGY data.\n"); exit(1); }
		x_inc=(double)tracehdr.sx*.001;

		num_samps=segyhdr.hns;
		trange=(double)(num_samps-1.0)*segyhdr.hdt*.001;
		num_bits=16;
		fhdr_bytes=3600;
		thdr_bytes=240;
		bytesperscan=(num_samps*2)+thdr_bytes;

		fseek(datfile,((num_scans-1)*bytesperscan)+fhdr_bytes,SEEK_SET);
		/* Get trace header */
		if (fread((void *)&tracehdr,sizeof(struct SegyTraceHdrStruct),1,datfile) < 1)
		{ printf("Error reading last trace header of SEGY data.\n"); exit(1); }
		x_inc=(((double)tracehdr.sx*.001)-x_inc)/((double)num_scans-1.);
		/**** printf("x_inc=%4.4lf\n",x_inc); getch(); ****/

		segyhdr.comment[1600]=0;  lab[0]=0;
		if ((pm=strstr(segyhdr.comment,"/"))!=NULL) strncpy(lab,pm-2,8);
		if (lab[5]=='/') strcpy(acqdate,lab);
		if ((pm=strstr(segyhdr.comment,"NOMINAL FREQUENCY"))!=NULL)
			{ pm=strstr(pm,"=")+1; strncpy(ant_freq,pm,5); }
		signed_flag=1;
	}

	else if (unknwn_flag)
	{
		printf("Data type is not SIR10 or pulseEKKO.  If the data values are fixed bit\n");
		printf("(8, 12, or 16), not floating point, you can answer some questions and\n");
		printf("this program will try to read it anyway.  OK? (y or n) ");  scanf("%s",ans);
		if (strcmp(ans,"y"))
		{
			#if defined(_INTELC32_)
				realfree(buffer);
			#endif
			fclose(datfile);
			exit(1);
		}
		num_bits=16;
		signed_flag=1;
		num_samps=512;
		fhdr_bytes=0;
		thdr_bytes=0;
		trange=50.;
		strcpy(acqdate,"Unknown");

		strcpy(ant_freq,"Unknown");
		printf("Default values for reading this unknown dataset are listed below.\n");
		printf("You must set these values according to your knowledge of the dataset.\n\n");
		printf("Press a letter to change a value, Enter to accept, or Esc to exit.\n");

		do
		{
		  printf("a) bits per value = %d\n",num_bits);
		  if (signed_flag) printf("b) data is signed (positive and negative values; eg. -128 to 127)\n");
		  else printf("b) data is unsigned (no negative values; eg. 0 to 256)\n");
		  printf("c) number of samples per trace = %d\n",num_samps);
		  printf("d) number of bytes in a file header = %d\n",fhdr_bytes);
		  printf("e) number of bytes in a trace header = %d\n",thdr_bytes);
		  printf("f) total time per trace in nanoseconds = %lf\n",trange);
		  printf("g) acquisition date = %s\n",acqdate);
		  printf("h) antenna center frequency = %s\n\n",ant_freq);
		  printf("Only a-e control reading of data, f-h are for labels.\n");
			key=getch();  if( (key==0) || (key==0xE0) ) key=-getch();
			if (key==ESC)
			{
				#if defined(_INTELC32_)
					realfree(buffer);
				#endif
				fclose(datfile);
				exit(1);
			}
			if (key==ENTER) break;

			switch (key)
			{
				case 'a':
					do
					{ printf("\nData values are 8, 12, or 16 bit? (8, 12, 16) ");  scanf("%d",&num_bits); }
					while (!((num_bits==8)||(num_bits==12)||(num_bits==16)) );
					break;
				case 'b':
					if (signed_flag)
					{
						printf("Data is unsigned (no negative values).");
						signed_flag=!(signed_flag);
					}
					else
					{
						printf("Data is signed (positive and negative values).");
						signed_flag=!(signed_flag);
					}
					break;
				case 'c':
					do
					{ printf("\nNumber of samples per trace? ");  scanf("%d",&num_samps); }
					while ((num_samps<10)||(num_samps>102400L));
					break;
				case 'd':
					do
					{ printf("\nNumber of bytes in a file header? ");  scanf("%d",&fhdr_bytes); }
					while (fhdr_bytes<0);
					break;
				case 'e':
					do
					{ printf("\nNumber of bytes in trace headers? ");  scanf("%d",&thdr_bytes); }
					while (thdr_bytes<0);
					break;
				case 'f':
					do
					{ printf("\nTotal time per trace in nanoseconds? ");  scanf("%lf",&trange); }
					while (trange<=0.0);
					break;
				case 'g':
					do
					{
						printf("\nAcquisition date? ");
						fflush(stdin); fgets(acqdate,20,stdin); acqdate[strlen(acqdate)-1]=0;
					}
					while (strlen(acqdate)==0);
					break;
				case 'h':
					do
					{
						printf("\nAntenna center frequency in MHz? ");
						fflush(stdin); fgets(ant_freq,5,stdin); ant_freq[strlen(ant_freq)-1]=0;
						strcat(ant_freq," MHz");
					}
					while (strlen(ant_freq)==0);
					break;
			}

		}
		while (1);

		if (fhdr_bytes>0) fseek(datfile,fhdr_bytes,SEEK_SET);
		if (num_bits==12) bytesperscan=(num_samps*2)+thdr_bytes;
		else bytesperscan=(num_samps*(num_bits/8))+thdr_bytes;
		num_scans=(num_bytes-fhdr_bytes)/bytesperscan;


		strcpy(comments,"Parameters calculated from user input.  Data format not recognized.\n");
	}

	/********* print header info ****************************/
	printf("\n\nFIELDVEW Compiled: %s   Author: Michael H. Powers, USGS\n\n",COMPILE_DATE);
	printf("========= %s ==========\n",datfilename);
	printf("Acquired on: %s     Antenna frequency: %s\n",acqdate,ant_freq);
	printf("Range in nanoseconds: %f    Number of traces: %ld\n",trange,num_scans);
	printf("Samples per trace: %d     Bits per sample: %d\n",num_samps,num_bits);
	printf("Number of bytes per trace: %d       Graphics mode: %d\n",bytesperscan,gmode);
	if (s10_flag)
		printf("Comment: \n%.1040s\n",(char *)(hdrval[num])+hdrval[num]->rh_text);
	else if (!mala_flag)
		printf("Trace interval distance: %lf\n",x_inc);
	else
	{
		if (x_inc!=0.) printf("Trace interval distance (m): %lf\n",x_inc);
		else printf("Trace interval time (sec): %lf\n",t_inc);
	}
	if (ss_flag || unknwn_flag) printf("Comment: \n%.1040s\n",comments);
	if (chksm_flag) printf("Checksum for DZT format failed.\n");
	if (segy_flag) printf("Comment: \n%.1040s\n",segyhdr.comment);
	printf("Esc to exit, any other key to proceed\n");
	key=getch();  if ((key==0) || (key==0xe0)) key=-getch();
	if (key==ESC) exit(0);
	else if (key==119)
	{
		printf("Window divisor?\n");
		scanf("%d",&num_hdrs);
		num=0;
		trange/=num_hdrs;
		num_samps/=num_hdrs;
	}
	if (segy_flag) ss_flag=1;

	/******** Will num_scans fit in memory? *****************/
	mem_avail=_memavl();
	maxscansinmem=(int)(((double)mem_avail/512.4)-100.0);
	if (maxscansinmem<0)
	{
		printf("\nYou do not have enough memory available to run this program.\n");
		exit(1);
	}
	/** need extra to pad plot_image from top and side **/
	if ((abs_last==-1) && (abs_first==0))
		{ ntotmscans=num_scans;  abs_last=num_scans-1; }
	else if ((abs_last==-1) && (abs_first!=0))
		{ abs_last=num_scans-1; ntotmscans=(abs_last-abs_first)+1; }
	else ntotmscans=(abs_last-abs_first)+1;
	if (num_scans<ntotmscans) { ntotmscans=num_scans; abs_last=num_scans-1; }

	/* printf("af=%d al=%d nm=%d mim=%d",abs_first,abs_last,ntotmscans,maxscansinmem); */
	while ( (ntotmscans>maxscansinmem) || (ntotmscans<1) )
	{
		printf("\nYou have enough memory to hold %d scans.\n",maxscansinmem);
		printf("Please enter first and last scan for this run.\n");
		while (1)
		{
			printf("First? ");
			fflush(stdin); fgets(cv1,10,stdin); abs_first=atoi(cv1);
			if ((abs_first<num_scans)&&(abs_first>0)) break;
			printf("\n");
		}
		while (1)
		{
			printf("Last? ");
			fflush(stdin); fgets(cv1,10,stdin); abs_last=atoi(cv1);
			if (abs_last>=num_scans) abs_last=num_scans-1;
			if (abs_last<abs_first) abs_last=abs_first;
			if ((abs_last-abs_first)<maxscansinmem) break;
			printf("\n");
		}
		ntotmscans=(abs_last-abs_first)+1;
		printf("%d scans will be read from %d to %d.  Okay? (y or n)\n",ntotmscans,abs_first,abs_last);
		scanf("%s",ans);  if (strcmp(ans,"y")) ntotmscans=0;
	}

	/******* one dimension of plot_image array will be ntotmscans+padx ***/
	/*** need to find other dimension (num_samps*sfill/tstep)+pady *******/
				/****** sfill=? ****** tstep=? *******/
	/******* must execute plotter_is() to determine pixel space **********/
	if (laserjet) laserjet();
	plotter_is(gmode,plotfile);
	scrn_ratio=(double)(v_colx+1)/(double)(v_rowy+1);
	if (v_color<254) { rainbo16();  shift=12; } else fieldvewpal();
	if (num_bits==12) shift-=4;
	mgrab_screen = image(v_rowy+1,v_colx+1);

	sfill=1; tstep=1;
	while ( (num_samps*sfill)<(int)(.36*(double)v_rowy) ) sfill++;
	while ( (num_samps/tstep)>(int)(.73*(double)v_rowy) ) tstep++;
	/** .36 and .73 are calculated such that vertical dimension ***/
	/** of image on screen will not be too large or too small *****/
	nnum_samps=(num_samps/tstep)*sfill;  t0samp*=((float)sfill/(float)tstep);
	digi=trange/((double)nnum_samps-1.0); markmark=20.*((float)sfill/(float)tstep);
	/************* for debugging ************************************
	plotter_off();
	printf("\nsfill=%d, tstep=%d, num_hdrs=%d, digi=%lf\n",sfill,tstep,num_hdrs,digi);
	exit(0);
	****************************************************************/

	/******* find tick inc for time scale ****************************/
	if(trange>10000.) factor=10000.;
	else if(trange>1000.) factor=1000.;
	else if(trange>100.) factor=100.;
	else if(trange>10.) factor=10.;
	else if(trange>1.) factor=1.;
	else factor=0.1;
	p=(int)(trange/factor);
	switch (p)
	{
		case 1: space=factor/10.; break;
		case 2: space=factor/5.; break;
		case 3: space=factor/5.; break;
		case 4: space=factor/2.; break;
		case 5: space=factor/2.; break;
		default: space=factor; break;
	}
	spacesamp=space/digi;

	/****** set up 9 viewports for graphics screen ***/
	/****** viewport 1 is for data plotting, all others depend on it ****/
	v1x1=2.;
	v1x1=MAX(v1x1,(37.-(50.*((double)ntotmscans/(double)(v_colx+1)))));
	v1x2=74.-v1x1; if ((v1x2-v1x1)<0.75) { v1x1=37.; v1x2=37.75; }
	v1y1=18.;
	v1y1=MAX(v1y1,(66.67*(1.-((double)nnum_samps/(double)(v_rowy+1)))));
	v1y2=100.-(.5*v1y1);
	v2x1=1.;       v2x2=72.;           v2y1=v1y2;      v2y2=99.;
	v3x1=v1x2;     v3x2=.5*(v1x2+98.); v3y1=v1y1;      v3y2=v1y2;
	v4x1=v3x2;     v4x2=98.;           v4y1=v1y1;      v4y2=v1y2;
	v5x1=v1x1;     v5x2=v1x2;          v5y1=.5*v1y1;   v5y2=v1y1;
	v6x1=2.;       v6x2=50.;           v6y1=.333*v1y1; v6y2=v5y1;
	v7x1=50.;      v7x2=98.;          v7y1=v6y1;      v7y2=v6y2;
	v8x1=2.;       v8x2=98.;          v8y1=2.;        v8y2=v6y1;
	v9x1=v4x1;     v9x2=v4x2;          v9y1=v5y1;      v9y2=v4y1;

	viewport(0.,133.3,0.,100.); window(0.,1.,0.,1.);
	pen(v_color-5); frame();
	/******* plot label viewports ******************/
	plot_title(datfilename,acqdate,ant_freq);
	keys_to_use();
	calc_scan_ticks();
	viewport(v3x1*scrn_ratio,v3x2*scrn_ratio,v3y1,v3y2);
	pixsamp0=(double)hpgl_p2yc; pixsamplast=(double)hpgl_p1yc;
	window(0., 100., 0., trange);
	lorg(5); csize(1.7); ldir(1.57); pen(v_color); unclip(); /* white */
	label(65.,trange*.5,"Two-way time in nanoseconds");
	plot_time_scale(1);

	/******************************************************/
	/*************** read data ****************************/
	data_type=fill_plot_image(num,num_hdrs,1,datfile);
	/******************************************************/
	/******************************************************/

	if ( (mcnt==0) && (!ss_flag) )
	{
		mcnt=1; marks[0]=0;
		marks_flag=1;
	}
	newmaxcols=maxcols;

	if ((s10_flag) && (mindif==1)) mpmark=mpscan=0.02;
	if ((ss_flag) || (mala_flag)) mpmark=mpscan=x_inc;

	/***** determine first and last scan on the screen *****/
	/***** plotting starts with abs_first on left side *****/
	viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
	window(0.,100.,0.,100.);
	scansperscrn=MIN(((hpgl_p2xc-hpgl_p1xc)+1),ntotmscans);
	hsps=scansperscrn/2;
	scr_first=abs_first;
	curscan=scr_first+hsps;
	scr_last=(scr_first+scansperscrn)-1;
	curtime=(double)(cursamp-t0samp)*digi;
	jumpsamp=(int)(0.1*(double)nnum_samps);
	jumptime=(double)jumpsamp*digi;

	/********* erase countdown info ****************************/
	pen(0); lorg(5); csize(1.0);
	label(50.,80.,"Reading data ....");
	label(50.,70.,"Countdown to 0:"); pen(v_color);

	/**************** plot data on screen **********************/
	ax1=(scr_first+padx)-abs_first;
	ax2=ax1+hpgl_p2xc;
	plot_image(plot_array,ax1,ax2,0,v_rowy);

	scan_hilight(1);
	scan_ticks();
	if (scr_last<abs_last) move_left_ok();

	/***************** plot model (cursor) scan as wiggle *****/
	viewport(v4x1*scrn_ratio,v4x2*scrn_ratio,v4y2,100.);
	window(0.,100.,0.,100.);                  /**  ^  **/
	m=(hpgl_p2xc-hpgl_p1xc)+1;                /**  |  NOTE **/
	csize(.7*((double)v_colx/(double)m));     /**  |  DIFF **/
	lorg(5); label(50.,70.,"Cursor Scan");    /**  v  **/
	viewport(v4x1*scrn_ratio,v4x2*scrn_ratio,v4y1,v4y2);
	window(0., 100., 0., 100.); pen(v_color); frame();
	pms_hmax=.5*(double)(v_color+1);
	pms_wx1=-.125*(double)(v_color+1); pms_wx2=1.125*(double)(v_color+1);
	pms_wy2=(double)nnum_samps+1.; pms_lby2=pixsamp0+(nnum_samps*.05);
	plot_model_scan(1);
	time_hilight(1);
	samp_time_label(1);
	samp_time_text_label(1);

	while (1)
	{
		if (set_key==0)
		{
			key=getch();  if( (key==0) || (key==0xE0) ) key=-getch();
			ss=*(char *)0x0417;  if ((ss & 0x0001) || (ss & 0x0002)) shift_flag=1;
			if ( (key==ENTER) && (shift_flag) )  key=SH_RETURN;
			shift_flag=0;
		}
		set_key=0;

		switch (key)
		{
			case R_ARROW:
				if (curscan==scr_last) break;
				plot_model_scan(0);
				scan_hilight(0);
				curscan++;
				plot_model_scan(1);
				scan_hilight(1);
				time_hilight(1);
				scan_ticks();
				break;
			case L_ARROW:
				if (curscan==scr_first) break;
				pen(0);
				plot_model_scan(0);
				scan_hilight(0);
				curscan--;
				plot_model_scan(1);
				scan_hilight(1);
				time_hilight(1);
				scan_ticks();
				break;
			case S_R_ARROW:
				if (curscan==scr_last) break;
				plot_model_scan(0);
				scan_hilight(0);
				curscan+=10; if (curscan>scr_last) curscan=scr_last;
				pen(v_color);
				plot_model_scan(1);
				scan_hilight(1);
				time_hilight(1);
				scan_ticks();
				break;
			case S_L_ARROW:
				if (curscan==scr_first) break;
				plot_model_scan(0);
				scan_hilight(0);
				curscan-=10; if (curscan<scr_first) curscan=scr_first;
				plot_model_scan(1);
				scan_hilight(1);
				time_hilight(1);
				scan_ticks();
				break;
			case DN_ARROW:
				if (cursamp==nnum_samps-2) break;
				time_hilight(0);
				samp_time_label(0);
				cursamp++; curtime+=digi;
				time_hilight(1);
				scan_hilight(1);
				plot_model_scan(1);
				samp_time_label(1);
				break;
			case UP_ARROW:
				if (cursamp==2) break;
				time_hilight(0);
				samp_time_label(0);
				cursamp--; curtime-=digi;
				time_hilight(1);
				samp_time_label(1);
				scan_hilight(1);
				plot_model_scan(1);
				break;
			case S_DN_ARROW:
				if (cursamp==nnum_samps-2) break;
				time_hilight(0);
				samp_time_label(0);
				cursamp+=jumpsamp; curtime+=jumptime;
				if (cursamp>nnum_samps-2)
					{ cursamp=nnum_samps-2; curtime=(double)(nnum_samps-2-t0samp)*digi; }
				time_hilight(1);
				samp_time_label(1);
				scan_hilight(1);
				plot_model_scan(1);
				break;
			case S_UP_ARROW:
				if (cursamp==2) break;
				time_hilight(0);
				samp_time_label(0);
				cursamp-=jumpsamp; curtime-=jumptime;
				if (cursamp<2)
					{ cursamp=2; curtime=(2.-(double)t0samp)*digi; }
				time_hilight(1);
				samp_time_label(1);
				scan_hilight(1);
				plot_model_scan(1);
				break;
			case ZERO_KEY:
				plot_time_scale(0);
				if (time_flag) samp_time_label(0);
				t0samp=cursamp;
				curtime=0.0;
				plot_time_scale(1);
				if (time_flag) samp_time_label(1);
				break;
			case 's': case 'S':
				samp_time_text_label(0);
				samp_time_label(0);
				samp_flag=1;
				time_flag=0;
				samp_time_label(1);
				samp_time_text_label(1);
				break;
			case 't': case 'T':
				samp_time_text_label(0);
				samp_time_label(0);
				time_flag=1;
				samp_flag=0;
				samp_time_label(1);
				samp_time_text_label(1);
				break;
			case PG_DN:
				if (scr_last==abs_last)
					{ pen(0); move_left_ok();  break; }
				plot_model_scan(0);
				scan_hilight(0);
				scr_last+=hsps; curscan+=hsps; scr_first+=hsps;
				if (scr_last>abs_last)
				{
					scr_last=abs_last;
					curscan=scr_last-hsps;
					scr_first=(scr_last-scansperscrn)+1;
				}
				refresh_image();
				move_right_ok();
				break;
			case PG_UP:
				if (scr_first==abs_first)
					{ pen(0); move_right_ok(); break; }
				plot_model_scan(0);
				scan_hilight(0);
				scr_first-=hsps; curscan-=hsps; scr_last-=hsps;
				if (scr_first<abs_first)
				{
					scr_first=abs_first;
					curscan=scr_first+hsps;
					scr_last=(scr_first+scansperscrn)-1;
				}
				refresh_image();
				move_left_ok();
				break;
			case 'b': case 'B':
				br_flag=(!(br_flag));
				plot_model_scan(0);
				for (col=padx;col<newmaxcols;col++)
				{
					for (row=pady,i=0;row<maxrows;row++,i++)
					{
						if (br_flag)
						{
							itemp=(int)plot_array[row][col]-avgscan[i];
							if (itemp>255) itemp=255;
							if (itemp<0) itemp=0;
							plot_array[row][col]=(unsigned char)itemp;
						}
						else
						{
							itemp=(int)plot_array[row][col]+avgscan[i];
							if (itemp>255) itemp=255;
							if (itemp<0) itemp=0;
							plot_array[row][col]=(unsigned char)itemp;
						}
					}
				}
				refresh_image();
				viewport(v4x1*scrn_ratio,v4x2*scrn_ratio,v4y1,v4y2);
				window(0.,100.,0.,100.);  frame();
				plot_model_scan(1);
				break;
			case 'g': case 'G':
				display_gain_params(lamp,eamp);
				esc_flag=0; endgkey_flag=0;
				model_instr(); key=0; actparm=0; inc=0;
				do
				{
					switch (key)
					{
						case 0:  break;
						case F1:
							if ((gmode==1024)||(gmode==56)||(gmode==261))
							help_sub(2);
							break;
						case PG_UP: inc=.01;       break;
						case PG_DN: inc=(-.01);    break;
						case UP_ARROW: inc=.1;     break;
						case DN_ARROW: inc=(-.1);  break;
						case HOME: inc=1.;         break;
						case END: inc=(-1.);       break;
						case L_ARROW:
							actparm--;
							if (actparm<0) actparm=1;
							break;
						case TAB:
						case R_ARROW:
							actparm++;
							if (actparm>1) actparm=0;
							break;
						case ENTER:
							endgkey_flag=1; esc_flag=0;
							break;
						case ESC:
							lamp=1.0;
							eamp=0.0;
							endgkey_flag=1; esc_flag=1;
							break;
					}
					if (endgkey_flag) break;

					viewport(v8x1*scrn_ratio,v8x2*scrn_ratio,v8y1,v8y2);
					window(0., 100., 0., 100.); csize(0.6);
					if (actparm==0)
					{
						lorg(2); pen(0); sprintf(lab,"%5.1lf",lamp); label(5.,75.,lab);
						lamp+=inc; if (lamp<=0.0) lamp=1.0;
						pen(v_color-3); sprintf(lab,"%5.1lf",lamp); label(5.,75.,lab);
						pen(v_color-4); sprintf(lab,"%4.1lf",eamp); label(18.,75.,lab);
					}
					if (actparm==1)
					{
						lorg(2); pen(0); sprintf(lab,"%4.1lf",eamp); label(18.,75.,lab);
						eamp+=inc; if (eamp<0.0) eamp=0.0;
						pen(v_color-3); sprintf(lab,"%4.1lf",eamp); label(18.,75.,lab);
						pen(v_color-4); sprintf(lab,"%5.1lf",lamp); label(5.,75.,lab);
					}
					/*** refresh_image(); ****/
					key=getch();  if ((key==0) || (key==0xe0)) key=-getch();
					inc=0.0;

				} while (1);

				if (!esc_flag)
				{
					plot_model_scan(0);
					for (col=padx;col<newmaxcols;col++)
					{
						for (row=pady,i=1;row<maxrows;row++,i++)
						{
							if (i<=t0samp)lgain=0.0;
							else lgain=lamp*pow(((double)(i-t0samp)*digi),eamp);
/**********
							dtemp=(double)avgscan[i]*lgain;
							avgscan[i]=(int)dtemp;
							if (avgscan[i]>128) avgscan[i]=128;
							if (avgscan[i]<-127) avgscan[i]=-127;
************/
							dtemp=((double)plot_array[row][col]-127.)*lgain;
							if (dtemp>128.) dtemp=128.;
							if (dtemp<-127.) dtemp=-127.;
							dtemp+=127.;
							plot_array[row][col]=(unsigned char)dtemp;
						}
					}
				}
				refresh_image(); keys_to_use(); endgkey_flag=0;
				pen(0); move_right_ok();  move_left_ok(); unclip();
				viewport(v4x1*scrn_ratio,v4x2*scrn_ratio,v4y1,v4y2);
				window(0.,100.,0.,100.);  frame();
				break;
			case SH_RETURN:
				plot_model_scan(0);
				fill_plot_image(num,num_hdrs,0,datfile);
				/********* erase countdown info ****************************/
				pen(0); lorg(5); csize(1.0);
				label(50.,80.,"Reading data ....");
				label(50.,70.,"Countdown to 0:"); pen(v_color);

				refresh_image();
				viewport(v4x1*scrn_ratio,v4x2*scrn_ratio,v4y1,v4y2);
				window(0.,100.,0.,100.);  vclear(); frame();
				plot_model_scan(1);
				break;
			case 'l': case 'L':
				if (marks_flag==0) break;
				if (marks_flag==2)
				{
					mindif=abs(marks[1]-marks[0]);
					marks_flag=0;
					break;
				}
				marks_flag=0;
				plot_model_scan(0);
				newmaxcols=lateral_corr();
				refresh_image();
				break;
			case 'v': case 'V':
				if (marks_flag!=0) { key='l'; set_key=1; break; }
				list_model_params(rperm,tarrad,mpmark,sroff);
				model_instr(); key=0; actparm=0; inc=0;
				do
				{
					switch (key)
					{
						/* GRO changed from here ... */
						case 0:  break;
						case C_R_ARROW:
							if (curscan==scr_last) break;
							plot_model_scan(0);
							scan_hilight(0);
							curscan++;
							break;
							case C_L_ARROW:
							if (curscan==scr_first) break;
							plot_model_scan(0);
							scan_hilight(0);
							curscan--;
							break;
						case C_DN_ARROW:
							if (cursamp==nnum_samps-2) break;
							time_hilight(0);
							samp_time_label(0);
							cursamp++; curtime+=digi;
							show_cursor_depth(rperm);
							samp_time_label(1);
							break;
						case C_UP_ARROW:
							if (cursamp==2) break;
							time_hilight(0);
							samp_time_label(0);
							cursamp--; curtime-=digi;
							show_cursor_depth(rperm);
							samp_time_label(1);
							break;
						case F1:
							if ((gmode==1024)||(gmode==56)||(gmode==261))
							help_sub(1);
							break;
						case PG_UP: inc=.01;       break;
						case PG_DN: inc=(-.01);    break;
						case UP_ARROW: inc=.1;     break;
						case DN_ARROW: inc=(-.1);  break;
						case HOME: inc=1.;         break;
						case END: inc=(-1.);       break;
						case 'p': case 'P':
							if ((gmode==1024)||(gmode==56)||(gmode==261))
							make_eps_bw(datfilename);  /* screen dump */
							break;
						case L_ARROW:
							actparm--;
							if (actparm<0) actparm=3;
							break;
						case TAB:
						case R_ARROW:
							actparm++;
							if (actparm>3) actparm=0;
							break;
						case ESC: endvkey_flag=1; break;
					}
					if (endvkey_flag) break;

					viewport(v8x1*scrn_ratio,v8x2*scrn_ratio,v8y1,v8y2);
					window(0., 100., 0., 100.); csize(0.6);
					if (actparm==0)
					{
						lorg(2); pen(0); sprintf(lab,"%5.2lf",rperm); label(12.,75.,lab);
						rperm+=inc; if (rperm<1.0) rperm=1.0;
						pen(v_color-3); sprintf(lab,"%5.2lf",rperm); label(12.,75.,lab);
						lorg(5); pen(v_color-4); sprintf(lab,"%5.2lf",tarrad); label(43.,75.,lab);
						lorg(5); pen(v_color-4); sprintf(lab,"%5.2lf",sroff); label(75.,75.,lab);
						lorg(8); pen(v_color-4); sprintf(lab,"%5.2lf",mpmark); label(100.,75.,lab);
						show_cursor_depth(rperm);
					}
					if (actparm==1)
					{
						lorg(5); pen(0); sprintf(lab,"%5.2lf",tarrad); label(43.,75.,lab);
						tarrad+=inc; if (tarrad<0.0) tarrad=0.0;
						pen(v_color-3); sprintf(lab,"%5.2lf",tarrad); label(43.,75.,lab);
						lorg(2); pen(v_color-4); sprintf(lab,"%5.2lf",rperm); label(12.,75.,lab);
						lorg(5); pen(v_color-4); sprintf(lab,"%5.2lf",sroff); label(75.,75.,lab);
						lorg(8); pen(v_color-4); sprintf(lab,"%5.2lf",mpmark); label(100.,75.,lab);
					}
					if (actparm==2)
					{
						lorg(5); pen(0); sprintf(lab,"%5.2lf",sroff); label(75.,75.,lab);
						sroff+=inc; if (sroff<0.0) sroff=0.0;
						pen(v_color-3); sprintf(lab,"%5.2lf",sroff); label(75.,75.,lab);
						lorg(2); pen(v_color-4); sprintf(lab,"%5.2lf",rperm); label(12.,75.,lab);
						lorg(5); pen(v_color-4); sprintf(lab,"%5.2lf",tarrad); label(43.,75.,lab);
						lorg(8); pen(v_color-4); sprintf(lab,"%5.2lf",mpmark); label(100.,75.,lab);
					}
					if (actparm==3)
					{
						lorg(8); pen(0); sprintf(lab,"%5.2lf",mpmark); label(100.,75.,lab);
            mpmark+=inc; if (mpmark<0.01) mpmark=0.01;
						pen(v_color-3); sprintf(lab,"%5.2lf",mpmark); label(100.,75.,lab);
						lorg(5); pen(v_color-4); sprintf(lab,"%5.2lf",tarrad); label(43.,75.,lab);
						lorg(5); pen(v_color-4); sprintf(lab,"%5.2lf",sroff); label(75.,75.,lab);
						lorg(2); pen(v_color-4); sprintf(lab,"%5.2lf",rperm); label(12.,75.,lab);
					}
					refresh_image();
					if ((mindif>1) && (mcnt>1)) mpscan=mpmark/((double)mindif);
					else mpscan=mpmark;
					draw_reflection(rperm,tarrad,mpscan,sroff); unclip();
					/* ... to here */
					key=getch();  if ((key==0) || (key==0xe0)) key=-getch();
					inc=0.0;

				} while (1);

				refresh_image(); keys_to_use(); endvkey_flag=0;
				pen(0); move_right_ok();  move_left_ok(); unclip();
				break;
			case 'a': case 'A':
				if (plot_array[pady+5][padx+(curscan-abs_first)]==0) break;
				j=-1;
				plot_model_scan(0);
				for (i=pady+2;i<=pady+markmark;i++)
					plot_array[i][padx+(curscan-abs_first)]=0;
				/* updatemarks(datfilename,curscan,1); */
				for (i=0;i<mcnt;i++)  if (marks[i]>(padx+(curscan-abs_first))) { j=i; break; }
				if (j>-1) for (i=mcnt;i>j;i--) marks[i]=marks[i-1];
				else { if (marks[0]==0) j=0; else j=mcnt; }
				marks[j]=(padx+(curscan-abs_first));
				if (j!=0) mcnt++;
				if (mcnt>1) marks_flag=1;
				if (mcnt==2) marks_flag=2;
				refresh_image();
				break;
			case 'r': case 'R':
				if (plot_array[pady+5][padx+(curscan-abs_first)]!=0) break;
				plot_model_scan(0);
				for (i=pady+2;i<=pady+markmark;i++)
					plot_array[i][padx+(curscan-abs_first)]=127;
				/* updatemarks(datfilename,curscan,0); */
				for (i=0;i<mcnt;i++) if (marks[i]==(padx+curscan-abs_first)) { j=i; break; }
				for (i=j;i<mcnt-1;i++) marks[i]=marks[i+1];
				mcnt--;
				refresh_image();
				break;
			case C_R_ARROW:
				if (scr_last==abs_last)
					{ pen(0); move_left_ok(); break; }
				plot_model_scan(0);
				scan_hilight(0);
				scr_first++; scr_last++; curscan++;
				if (scr_last>abs_last)
					{  scr_last=abs_last; scr_first=(scr_last-scansperscrn)+1; curscan--; }
				refresh_image();
				move_right_ok();
				break;
			case C_L_ARROW:
				if (scr_first==abs_first)
					{ pen(0); move_right_ok(); break; }
				plot_model_scan(0);
				scan_hilight(0);
				scr_first--; scr_last--; curscan--;
				if (scr_first<abs_first)
					{  scr_first=abs_first; scr_last=(scr_first+scansperscrn)-1; curscan++; }
				refresh_image();
				move_left_ok();
				break;
			case HOME:
				if (scr_first==abs_first)
					{ pen(0); move_right_ok(); break; }
				pen(0); move_right_ok();
				plot_model_scan(0);
				scan_hilight(0);
				scr_first=abs_first;
				curscan=scr_first+hsps;
				scr_last=(scr_first+scansperscrn)-1;
				refresh_image();
				move_left_ok();
				break;
			case END:
				if (scr_last==abs_last)
					{ pen(0); move_left_ok(); break; }
				pen(0); move_left_ok();
				plot_model_scan(0);
				scan_hilight(0);
				scr_last=abs_last;
				curscan=scr_last-hsps;
				scr_first=(scr_last-scansperscrn)+1;
				refresh_image();
				move_right_ok();
				break;
			case 'd': case 'D':
				if (s10_flag!=1) break;
				if ((oneu16bitscan=(unsigned short *)malloc(bytesperscan))==NULL)
				{
					plotter_off();
					printf("Malloc problem in oneu16bitscan.  You need more memory.\n");
					exit(1);
				}
				out_file=fopen("fieldvw.dmp","wb");
				rewind(datfile);  fseek(datfile,fhdr_bytes,SEEK_SET);
				startloc=ftell(datfile);
				k=num*num_samps; idum=curscan;
				for (i=padx;i<=scansperscrn+padx;i++)
				{
					curscan=new_to_old[i]-padx+scr_first;
					if (curscan>=scr_last) break;
					curloc=startloc+(bytesperscan*curscan);
					fseek(datfile,curloc,SEEK_SET);
					if (fread((void *)oneu16bitscan,bytesperscan,1,datfile)<1)
					{
						plotter_off();
						printf("Problem reading scan %d",curscan);
						exit(1);
					}
					fwrite(&oneu16bitscan[k],sizeof(unsigned short),num_samps,out_file);
				}
				free(oneu16bitscan); fclose(out_file);
				curscan=idum;
				break;
			case ESC:

		/******* for debugging *******************************
		fprintf(debug_file,"num_scans=%ld\n",num_scans);
		for (i=padx;i<=scansperscrn+padx;i++)
			fprintf(debug_file,"n2o[%d] = %d\n",i,new_to_old[i]);
		fclose(debug_file);
		*****************************************************/

				plotter_off();
				free_image(plot_array,maxrows);
				fclose(datfile);
				#if defined(_INTELC32_)
					realfree(buffer);
				#endif
				free_image(mgrab_screen, v_rowy+1);
				free(avgscan); free(marks); free(new_to_old);
				exit(0);
			case 'p': case 'P':
				if ((gmode==1024)||(gmode==56)||(gmode==261))
					make_eps_bw(datfilename);  /* screen dump */
				break;
			case F1:
				if ((gmode==1024)||(gmode==56)||(gmode==261)) help_sub(0);
				break;
			case F2:
				colorindex++;
				if (colorindex==MAXPALETTES) colorindex=0;
				set_pal(colorindex);
				break;
		}
	}
}

/*************************************************************************/
/*************************************************************************/
void make_eps_bw(char *filein) /* screen dump B&W only */
{
	char stra[40] = "%!PS-Adobe-2.0 EPSF-1.2\n";
	char strb[40] = "%%Creator: FIELDVEW.C \n";
	char strc[40] = "%%BoundingBox: 108 360 540 684\n";
	char strd[40] = "%%EndComments\n";
	int i, j, k, jj, flag=0,key;
	char str[80],fileout[80],*pm;
	FILE *stream_out;
	unsigned char bw_pal[256];

	grab1024(mgrab_screen);

	strcpy(fileout,filein);
	if (fileout[0])
	{
		pm=strchr(fileout,'.');
		if (pm!=NULL) strncpy(pm+1,"eps\0",4);
		else strcat(fileout,".eps\0");
	}
	else strcpy(fileout,"fieldvew.eps");

	viewport(10.,123.,60.,90.);
	window(0.,100.,0.,100.); vclear();
	pen(v_color); frame(); lorg(5); csize(1.0);

	if ((stream_out = fopen(fileout, "wb")) == NULL)
	{
		sprintf(str,"Cannot open file %s", fileout);
		flag=1;
	}
	else sprintf(str,"%s", fileout);

	label(50.,50.,str);
	label(50.,75.,"EPS format image will be written to file:");
	label(50.,25.,"Press Esc to quit, any other key to continue");
	while ((key=getch())!=ESC)
	{
		if (flag==1) break;
		for (i=0;i<256;i++)  bw_pal[i]=(unsigned char)(255-i);   /* grey scale */
		/********* highs come out black, lows white *******************/
		vclear(); frame();
		sprintf(str,"Writing EPS format image to %s ...", fileout);
		label(50.,50.,str);

		fprintf(stream_out, "%s", stra);                   /* header */
		fprintf(stream_out, "%s", strb);
		fprintf(stream_out, "%s", strc);
		fprintf(stream_out, "%s", strd);
		/* nfprintf(stream_out, "0 0 1 setrgbcolor\n"); */
		fprintf(stream_out, "gsave\n");
		fprintf(stream_out, "gsave\n");
		fprintf(stream_out, "108 360 translate\n");
		/* fprintf(stream_out, "90 rotate\n"); */
		fprintf(stream_out, "432 324 scale\n");
		fprintf(stream_out, "/picstr 1024 string def\n");
		fprintf(stream_out, "1024 768 8 ");
		fprintf(stream_out, "[1024 0 0 -768 0 768]\n");
		fprintf(stream_out, "{currentfile picstr readhexstring pop} image\n");
		k = 0;
		for (i=0; i<768; i++)                         /* image */
		{  for (j=0; j<1024; j++)
			{
				fprintf(stream_out, "%02X", bw_pal[mgrab_screen[i][j]]);
				k++;
				if (k % 39 == 0) fprintf(stream_out, "\n");
			}
		}
		fprintf(stream_out, "\ngrestore\n");
		fprintf(stream_out, "showpage\n");
		fprintf(stream_out, "grestore\n");
		fprintf(stream_out, "%%Trailer\n");
		fclose(stream_out); break;
	}
	if (key==ESC) remove(fileout);
	plot1024(mgrab_screen);
	fieldvewpal();
}

#if 0
/********************************************************************/
void make_epsf(int gmode) /* screen dump "color" */
{
	char stra[40] = "%!PS-Adobe-3.0 EPSF-3.0\n";
	char strb[40] = "%%BoundingBox: 72 72 552 712\n";
	char strc[40] = "%%Creator: libhpgl.lib and newvew.c";
	char strd[40] = "%%EndComments\n";
	int i, j, k, jj;
	char str[80],file_out[80],*pos_per;
	FILE *stream_out;
	unsigned char tga_pal[768];

	for (i=0,j=0; i<768; i+=3,j++)
	{
		tga_pal[i]   = j;   /* grey scale */
		tga_pal[i+1] = j;
		tga_pal[i+2] = j;
	}
	tga_pal[0] = 255;   /* white */
	tga_pal[1] = 255;
	tga_pal[2] = 255;
	tga_pal[765] = 0;   /* black */
	tga_pal[766] = 0;
	tga_pal[767] = 0;

	/**************************
	{
		tga_pal[i]   = pal[i+2];
		tga_pal[i+1] = pal[i+1];
		tga_pal[i+2] = pal[i];
	}
	**************************/

	grab1024(mgrab_screen);
	plotter_off();
	while (kbhit()) getch();
	printf("Filename for screen dump EPS format image:\n");
	scanf("%[a-zA-Z01-9._-]", &file_out);
	if (strlen(file_out)>1)
	{ pos_per = strchr(file_out, '.');
	if (pos_per != NULL) strncpy(pos_per+1, "eps\0", 4);   /*  7654321-123  */
	else strcat(file_out,".eps\0");
	if ((stream_out = fopen(file_out, "wb")) == NULL)
	{   printf("\nCannot open file %s\n", file_out);
		getch();
	} else
	{   printf("\nWriting EPS format image to %s ...", file_out);
		fprintf(stream_out, "%s", stra);                   /* header */
		fprintf(stream_out, "%s", strb);
		fprintf(stream_out, "%s", strc);
		fprintf(stream_out, "%s", strd);
		fprintf(stream_out, "0 0 1 setrgbcolor\n");
		fprintf(stream_out, "gsave\n");
		fprintf(stream_out, "552 72 translate\n");
		fprintf(stream_out, "90 rotate\n");
		fprintf(stream_out, "640 480 scale\n");
		fprintf(stream_out, "/picstr 3072 string def\n");
		fprintf(stream_out, "1024 768 8\n");
		fprintf(stream_out, "[1024 0 0 -768 0 768]\n");
		fprintf(stream_out, "{currentfile picstr readhexstring pop}\n");
		fprintf(stream_out, "false 3 colorimage\n");
		k = 0;
		for (i=0; i<v_rowy+1; i++)                         /* image */
		{  for (j=0; j<v_colx+1; j++)
			 {    jj = 3*(int)mgrab_screen[i][j];
				fprintf(stream_out, "%02X", tga_pal[jj+2]);
				fprintf(stream_out, "%02X", tga_pal[jj+1]);
				fprintf(stream_out, "%02X", tga_pal[jj]);
				k++;
				if (k % 42 == 0) fprintf(stream_out, "\n");
			}
		}
		fprintf(stream_out, "\ngrestore\nshowpage\ngrestore\n");
		fclose(stream_out);
	 }
	}
	puts("");
	if (laserjet) laserjet();
	plotter_is(gmode, "CRT");
	plot1024(mgrab_screen);
	fieldvewpal();
}
#endif

/*************************************************************************/
/*************************************************************************/
void plot_title(char *flnm,char *adate,char *afreq)
{
	char lab[80];

	viewport(v2x1*scrn_ratio,v2x2*scrn_ratio,v2y1,v2y2);
	window(0., 100., 0., 100.);
	hpgl_select_font("eqnsmplx");
	lorg(5); csize(1.0); pen(v_color);
	if ((strlen(title1))>2) label(50.,70.,title1);
	else
	{
		sprintf(lab,"Field File: %s",flnm);
		label(50.,70.,lab);
	}
	csize(0.8);
	if ((strlen(title2))>2) label(50.,35.,title2);
	else
	{
		sprintf(lab,"Creation Date: %s      Antenna: %s",adate,afreq);
		label(50.,35.,lab);
	}
}

/*************************************************************************/
void plot_time_scale(int c_flag)
{
	/***** label time axis ********/
	double timeval,yloc,pt0;
	int i;
	char lab[80];

	if (c_flag) pen(v_color); else pen(0);
	viewport(v3x1*scrn_ratio,v3x2*scrn_ratio,v3y1,v3y2);
	window(0., 100.,pixsamplast,pixsamp0);  unclip();
	lorg(2); ldir(0.0); csize(1.3);
	pt0=pixsamp0-(double)t0samp;
	label(10.,pt0,"   0");
	plot(0.,pt0,-2); plot(10.,pt0,-1);
	plot(90.,pt0,-2); plot(100.,pt0,-1);
	for(i=1;pt0-((double)i*spacesamp)>=pixsamplast;i++)
	{
		timeval=(double)i*space;
		yloc=pt0-((double)i*spacesamp);
		plot(0.,yloc,-2); plot(10.,yloc,-1);
		plot(90.,yloc,-2); plot(100.,yloc,-1);
		sprintf(lab,"%4.0f",timeval);
		label(10.,yloc,lab);
	}
	for(i=-1;pt0-((double)i*spacesamp)<=pixsamp0;i--)
	{
		timeval=(double)i*space;
		yloc=pt0-((double)i*spacesamp);
		plot(0.,yloc,-2); plot(10.,yloc,-1);
		plot(90.,yloc,-2); plot(100.,yloc,-1);
		sprintf(lab,"%5.0f",timeval);
		label(10.,yloc,lab);
	}
}

/*************************************************************************/
void plot_model_scan(int c_flag)
{
	int s,pxpm;
	register int nsamps,py;
	static char lab[10];

	if (c_flag) pen(v_color); else pen(0);
	viewport(v4x1*scrn_ratio,v4x2*scrn_ratio,v4y1,v4y2);
	window(pms_wx1,pms_wx2, pixsamplast,pixsamp0);
	lorg(5); csize(1.7);
	sprintf(lab,"%d",curscan); label(pms_hmax,pms_lby2,lab);
	pxpm=padx+(curscan-abs_first);
	nsamps=(int)pixsamp0;  py=pady+1;
	plot(pms_hmax,nsamps-1,-2);
	for (;nsamps>(int)pixsamplast;nsamps--,py++)
	plot(plot_array[py][pxpm],nsamps,-1);
	plot(pms_wx1,pixsamp0,-2); plot(pms_wx2,pixsamp0,-1);
}

/*************************************************************************/
void keys_to_use()
{
	viewport(v8x1*scrn_ratio,v8x2*scrn_ratio,v8y1,v8y2);
	window(0., 100., 0., 100.);  vclear();
	pen(v_color-5); lorg(5); csize(0.6);
	label(50.,75.,"Arrow keys=Cursor up/down/left/right        Num lock arrow keys=Jump");
	label(50.,25.,"  F1=Help                                                 ESC=Quit  ");
}

/*************************************************************************/
void move_right_ok()
{
	viewport(v6x1*scrn_ratio,v6x2*scrn_ratio,v6y1,v6y2);
	window(0., 100., 0., 100.);  vclear();
	lorg(5); csize(1.2);
	label(50.,50.,"HOME, PG-UP, CTRL<-- = View left");
}

/*************************************************************************/
void move_left_ok()
{
	viewport(v7x1*scrn_ratio,v7x2*scrn_ratio,v7y1,v7y2);
	window(0., 100., 0., 100.);  vclear();
	lorg(5); csize(1.2);
	label(50.,50.,"END, PG-DN, CTRL--> = View right");
}

/*************************************************************************/
void calc_scan_ticks()
{
	/****************** label scan axis *******************************/
	double factor;
	int i,p,lscansperscrn;

	viewport(v5x1*scrn_ratio,v5x2*scrn_ratio,v5y1,v5y2);
	sx1=(double)hpgl_p1xc; sx2=(double)hpgl_p2xc;
	sxpm=0.5*(sx1+sx2);
	if (hpgl_p1xc<50) { scanlabellorg1=2; scanlabellorg2=8; }
	else { scanlabellorg1=8; scanlabellorg2=2; }
	lscansperscrn=(hpgl_p2xc-hpgl_p1xc)+1;
	slabcsize=100./(v1x2-v1x1); if (slabcsize>10.) slabcsize=10.;
	stlcsize=100./((v9x2-v9x1)*scrn_ratio); stltsize=0.8*stlcsize;

	if(lscansperscrn>10000) factor=10000.;
	else if(lscansperscrn>1000) factor=1000.;
	else if(lscansperscrn>100) factor=100.;
	else if(lscansperscrn>10) factor=10.;
	else if(lscansperscrn>1) factor=1.;
	else factor=0.1;
	p=(int)(lscansperscrn/factor);
	switch (p)
	{
		case 1: tickinc=factor/10.; break;
		case 2: tickinc=factor/5.; break;
		case 3: tickinc=factor/5.; break;
		case 4: tickinc=factor/2.; break;
		case 5: tickinc=factor/2.; break;
		default: tickinc=factor; break;
	}
	window(sx1, sx2, 0., 100.);
	csize(slabcsize); lorg(5);
	pen(v_color); label((sx1+sx2)*.5,22.,"Cursor Scan");
}

/*************************************************************************/
void scan_ticks()
{
	int i,ix1,ix2,itc;

	viewport(v5x1*scrn_ratio,v5x2*scrn_ratio,v5y1,v5y2);
	window(sx1, sx2, 0., 100.);

	plot(sx1,100.,-2); plot(sx1,75.,-1);
	plot(sx2,100.,-2); plot(sx2,75.,-1);
	ix1=(int)sx1; ix2=(int)sx2; itc=(int)tickinc;
	for(i=ix1+itc;i<ix2;i+=itc)
	{
		plot((double)i,100.,-2); plot((double)i,85.,-1);
	}
}

/*************************************************************************/
void scan_hilight(int flag)
{
	char lab[10];
	double sxm;
	register int iy,pypni,pxpm,sxmi;

	if (flag) pen(v_color); else pen(0);
	sxm=(double)(curscan-scr_first)+sx1;
	viewport(v5x1*scrn_ratio,v5x2*scrn_ratio,v5y1,v5y2);
	window(sx1, sx2, 0., 100.);
	csize(slabcsize);
	lorg(scanlabellorg1);
	sprintf(lab,"%d",scr_first); label(sx1,52.,lab);
	lorg(5);
	sprintf(lab,"%d",curscan); label(sxpm,52.,lab);
	lorg(scanlabellorg2);
	sprintf(lab,"%d",scr_last); label(sx2,52.,lab);
	plot(sxpm,70.,-2); plot(sxm,100.,-1);
	viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
	window(sx1, sx2,pixsamplast,pixsamp0);
	if (flag) { plot(sxm,pixsamplast,-2); plot(sxm,pixsamp0-2.,-1); }
	else
	{
		pypni=pady+nnum_samps-1; pxpm=padx+(curscan-abs_first);
		sxmi=(int)sxm; iy=(int)pixsamplast; if (nnum_samps<num_samps) iy++;
		for (;pypni>=pady;iy++,pypni--)
		{
			hpgl_pen_color=plot_array[pypni][pxpm];
			plot_pixel(sxmi,iy);
		}
		hpgl_pen_color=0; plot_pixel(sxmi,(int)pixsamplast);
	}
}

/*************************************************************************/
void time_hilight(int flag)
{
	int ix,pxpmi,psamp,py;
	double b=.0625*(double)(v_color+1);

	if (flag) pen(v_color); else pen(0);
	viewport(v4x1*scrn_ratio,v4x2*scrn_ratio,v4y1,v4y2);
	window(pms_wx1,pms_wx2,pixsamplast,pixsamp0);
	psamp=(int)pixsamp0-cursamp;
	plot(pms_wx1+b,(double)psamp,-2); plot(pms_wx2-b,(double)psamp,-1);
	if (!flag)
	{
		viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
		py=pady+cursamp;
		pxpmi=(scr_first+padx)-abs_first;
		for (ix=hpgl_p1xc;ix<hpgl_p2xc;ix++,pxpmi++)
		{
			hpgl_pen_color=plot_array[py][pxpmi];
			plot_pixel(ix,psamp);
		}
	}
	else
	{
		viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
		window(sx1,sx2,pixsamplast,pixsamp0);
		plot(sx1+2.,(double)psamp,-2); plot(sx2-2.,(double)psamp,-1);
	}
}

/*************************************************************************/
void samp_time_label(int flag)
{
	char lab[80];

	viewport(v9x1*scrn_ratio,v9x2*scrn_ratio,v9y1,v9y2);
	window(0., 100., 0., 100.);
	if (flag) pen(v_color); else pen(0);
	ldir(0); lorg(5); csize(stlcsize);
	if (samp_flag) sprintf(lab,"%d",(cursamp*tstep)/sfill);
	if (time_flag) sprintf(lab,"%4.2lf",curtime);
	label(50.,75.,lab);
}

/*************************************************************************/
void samp_time_text_label(int flag)
{
	char lab[80];

	viewport(v9x1*scrn_ratio,v9x2*scrn_ratio,v9y1,v9y2);
	window(0., 100., 0., 100.);
	if (flag) pen(v_color); else pen(0);
	ldir(0); lorg(5); csize(stltsize);
	if (samp_flag) label(50.,35.,"Cursor Sample");
	if (time_flag) label(50.,35.,"Cursor Time");
}

/*************************************************************************/
void list_model_params(double rp,double tr,double mp,double sro)
{
	char lab[80];
	viewport(v8x1*scrn_ratio,v8x2*scrn_ratio,v8y1,v8y2);
	window(0., 100., 0., 100.);  vclear();
	pen(v_color-5); csize(0.6);
	lorg(5); label(50.,25.,"F1=Help     CTRL-Arrow to move curve     TAB to set active param.     ESC to Quit");

	pen(v_color-4);
	lorg(2); label(0.,75.,"Rel.Perm.=");
	pen(v_color-3); sprintf(lab,"%5.2lf",rp); label(12.,75.,lab);

	pen(v_color-4); lorg(5); label(30.,75.,"Target Radius(m)=");
	sprintf(lab,"%5.2lf",tr); label(43.,75.,lab);


	label(62.,75.,"Sou-Rec Offset(m)=");
	sprintf(lab,"%5.2lf",sro); label(75.,75.,lab);


	lorg(8); label(94.,75.,"Spacing(m)=");
	sprintf(lab,"%5.2lf",mp); label(100.,75.,lab);
}

/*************************************************************************/
void display_gain_params(double emm,double eee)
{
	char lab[80];
	viewport(v8x1*scrn_ratio,v8x2*scrn_ratio,v8y1,v8y2);
	window(0., 100., 0., 100.);  vclear();
	pen(v_color-5); csize(0.6);
	lorg(5); label(50.,25.,"Enter=Accept   TAB=Switch Param.   ESC=Cancel");
	pen(v_color-4);
	lorg(2); label(2.,75.,"M=");
	pen(v_color-3); sprintf(lab,"%5.1lf",emm); label(5.,75.,lab);
	pen(v_color-4); label(15.,75.,"E=");
	sprintf(lab,"%4.1lf",eee); label(18.,75.,lab);
	lorg(8); label(98.,75.,"Amp(time)=InitAmp*M*[time(ns)^^E]");
}

/*************************************************************************/
void model_instr()
{
	viewport(v6x1*scrn_ratio,v6x2*scrn_ratio,v6y1,v6y2);
	window(0., 100., 0., 100.); vclear();
	pen(v_color-5); lorg(2); csize(1.0);
	label(0.,75.,"HOME   UP-AR   PG-UP");
	label(0.,25.,"+1.0   +0.1    +0.01");
	lorg(8); csize(1.0);
	label(100.,75.," END   DN-AR   PG-DN");
	label(100.,25.,"-1.0   -0.1    -0.01");
}

/*************************************************************************/
void show_cursor_depth(double relperm)
{
	char lab[80];
	double curdepth=0.15*curtime/sqrt(relperm);
	viewport(v7x1*scrn_ratio,v7x2*scrn_ratio,v7y1,v7y2);
	window(0., 100., 0., 100.); vclear();
	pen(v_color-2); lorg(5); csize(2.0);
	sprintf(lab,"Cursor Depth: %5.2lfm",curdepth);
	label(50.,50.,lab);
}

/*************************************************************************/
void refresh_image()
{
	int ax1,ax2;

	viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
	ax1=(scr_first+padx)-abs_first;
	ax2=ax1+hpgl_p2xc;
	plot_image(plot_array,ax1,ax2,0,v_rowy);
	plot_model_scan(1);
	scan_hilight(1);
	time_hilight(1);
	scan_ticks();
}

/*************************************************************************/
void draw_reflection(double relperm,double tradius,double mps,double sroffset)
{
	int i,j,lsf,lsl,scmx;
	double tnot,tnot2,tx[1024],trad,ttrad,vin,vin2;
	double st,et,x,xps2,xms2,hsro;
	/** 1024 in tx is max horiz. screen pixels **/

	vin=3.33333333*(sqrt(fabs(relperm)));  /* in ns per meter */
	vin2=vin*vin; /* ns^2 over meter^2 */
	trad=tradius*vin;
	ttrad=2*trad;
	tnot=(curtime*.5)+trad; /* one way time to center of target */
	if (tnot<trad) tnot=trad;
	tnot2=tnot*tnot;
	hsro=sroffset*.5;
	lsf=scr_first-curscan;
	lsl=scr_last-curscan;
	scmx=(int)MAX(fabs((double)lsf),fabs((double)lsl));
	st=(double)(-t0samp*digi);
	et=trange+st;

	viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
	window(lsf,lsl,et,st); pen(v_color-4); clip();
	plot(0.,curtime,-2);
	for (i=0;i<scmx;i++)
	{
		x=(double)i*mps;
		xps2=(x+hsro)*(x+hsro);
		xms2=(x-hsro)*(x-hsro);
		tx[i]=sqrt(tnot2+(xps2*vin2))+sqrt(tnot2+(xms2*vin2))-ttrad;
		if (tx[i]>et) break;
		plot((double)i,tx[i],-1);
	}
	plot(0.,curtime,-2);
	for (j=0;j<i;j++) plot((double)(-j),tx[j],-1);

/************* original with no sroff **********************

	tnot=curtime;  if (tnot<0.0) tnot=0.0;
	vin=3.33333333*(sqrt(fabs(relperm)));
	vin2=vin*vin;  * in ns per meter *
	trad=2.*tradius*vin;             tntr2=(tnot+trad)*(tnot+trad);
	lsf=scr_first-curscan;           lsl=scr_last-curscan;
	scmx=(int)MAX(fabs((double)lsf),fabs((double)lsl));
	st=(double)(-t0samp*digi);       et=trange+st;

	viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
	window(lsf,lsl,et,st); pen(v_color-4); clip();
	plot(0.,tnot,-2);
	for (i=0;i<scmx;i++)
	{
		x=(double)i*mps;
		tx[i]=MAX((sqrt( tntr2+(4.*x*x*vin2) )-trad),tnot);
		if (tx[i]>et) break;
		plot((double)i,tx[i],-1);
	}
	plot(0.,tnot,-2);
	for (j=0;j<i;j++) plot((double)(-j),tx[j],-1);
*****************************************************************/

}

/*************************************************************************/
void set_pal(int num)
{
	switch (num)
	{
	case 0:
		set_true_color_palette();
		break;
	case 1:
		spectrum();
		break;
	case 2:
		spectrum_rev();
		break;
	case 3:
		fieldvewpal();
		break;
	case 4:
		grey();
		break;
	}
}

/*************************************************************************/
void fieldvewpal()
{
	int kk;
	unsigned char i,pal[768];

	kk = 0; /* grey */
	for (i=0; i<249; i++)
	{
		pal[kk] = pal[kk+1] = pal[kk+2] = i;
		kk += 3;
	}
	pal[747] = 85;  pal[748] = 85;  pal[749] = 255;  /*  249 l.blue */
	pal[750] = 85;  pal[751] = 255; pal[752] = 85;   /*  250 l.green */
	pal[753] = 85;  pal[754] = 255; pal[755] = 255;  /*  251 l.cyan */
	pal[756] = 255; pal[757] = 85;  pal[758] = 85;   /*  252 l.red */
	pal[759] = 255; pal[760] = 85;  pal[761] = 255;  /*  253 l.magenta */
	pal[762] = 255; pal[763] = 255; pal[764] = 85;   /*  254 yellow */
	pal[765] = 255; pal[766] = 255; pal[767] = 255;  /*  255 l.white */
	palette_256(pal);
}

/*************************************************************************/
void rainbo16()
{
	char pal[17]={0,8,40,33,49,1,35,10,34,54,38,52,60,44,37,63,0};
	palette_ega(pal);
}

/******************************** GetDztHdrs() ******************************/
/* Read the (1 to 4) DZT headers.  The file must have been previously opened
 * using fopen.
 *
 * Parameters:
 *  int *num_hdrs  - address of variable, will hold number of headers on return
 *  int *num_scans - address of variable, will hold number of GPR scans on return
 *  struct DztHdrStruct *hdrPtr1 - address of header structure
 *  struct DztHdrStruct *hdrPtr2 - address of header structure
 *  struct DztHdrStruct *hdrPtr3 - address of header structure
 *  struct DztHdrStruct *hdrPtr4 - address of header structure
 *  FILE *infile   - pointer to FILE structure for the input file
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, attached header.
 * Calls: fread, rewind, fseek, ftell.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: June 28, 1993
 */
int GetDztHdrs (int *num_hdrs,long *num_scans,
				struct DztHdrStruct *hdrPtr1,struct DztHdrStruct *hdrPtr2,
				struct DztHdrStruct *hdrPtr3,struct DztHdrStruct *hdrPtr4,
				char *dflnm,FILE *infile)
{
	char *s,flnm[MAX_PATHLEN];
	int i, rv=0;
	unsigned short ustemp,checksum;
	long start_byte,last_byte,num_bytes;

	/* Verify parameter list */
	if (hdrPtr1 == NULL || hdrPtr2 == NULL || hdrPtr3 == NULL ||
		hdrPtr4 == NULL || infile == NULL || num_hdrs == NULL ||
		num_scans == NULL) return 1;

	s=dflnm; flnm[0]=0;
	while ((*s)!='.')  { strncat(flnm,s,1); s++; }

	if ( (strncmp(s,".dzt",4)) && (strncmp(s,".DZT",4))  && (strncmp(s,".czt",4))  && (strncmp(s,".CZT",4))) return 1;


	/* Get first header and number of headers */
	rewind(infile);
	if (fread((void *)hdrPtr1,sizeof(struct DztHdrStruct),1,infile) < 1)
		return 2;
	*num_hdrs = hdrPtr1->rh_nchan;               /* number of headers */
	if (hdrPtr1->rh_tag != 0x00FF) rv= 4;     /* is tag correct? */
	ustemp = hdrPtr1->rh_chksum;                 /* calculate check sum */
	hdrPtr1->rh_chksum = 0;
	checksum = 0;
	for (i=0; i<512; i++)
		checksum += *((unsigned short *)hdrPtr1 + i);
	hdrPtr1->rh_chksum = ustemp;
	if (checksum != ustemp) rv= -5;            /* is check sum correct? */

	/* Get rest of headers if more than one */
	if (*num_hdrs > 1)
	{   if (fread((void *)hdrPtr2,sizeof(struct DztHdrStruct),1,infile) < 1)
			return 3;
		if (hdrPtr2->rh_tag != 0x01FF) rv= 4;
		ustemp = hdrPtr2->rh_chksum;                 /* calculate check sum */
		hdrPtr2->rh_chksum = 0;
		checksum = 0;
		for (i=0; i<512; i++)
			checksum += *((unsigned short *)hdrPtr2 + i);
		hdrPtr2->rh_chksum = ustemp;
		if (checksum != ustemp) rv= -5;            /* is check sum correct? */
	}

	if (*num_hdrs > 2)
	{   if (fread((void *)hdrPtr3,sizeof(struct DztHdrStruct),1,infile) < 1)
			return 3;
		if (hdrPtr3->rh_tag != 0x02FF) rv= 4;
		ustemp = hdrPtr3->rh_chksum;                 /* calculate check sum */
		hdrPtr3->rh_chksum = 0;
		checksum = 0;
		for (i=0; i<512; i++)
			checksum += *((unsigned short *)hdrPtr3 + i);
		hdrPtr3->rh_chksum = ustemp;
		if (checksum != ustemp) rv= -5;            /* is check sum correct? */
	}
	if (*num_hdrs > 3)
	{   if (fread((void *)hdrPtr4,sizeof(struct DztHdrStruct),1,infile) < 1)
			return 3;
		if (hdrPtr4->rh_tag != 0x03FF) rv= 4;
		ustemp = hdrPtr4->rh_chksum;                 /* calculate check sum */
		hdrPtr4->rh_chksum = 0;
		checksum = 0;
		for (i=0; i<512; i++)
			checksum += *((unsigned short *)hdrPtr4 + i);
		hdrPtr4->rh_chksum = ustemp;
		if (checksum != ustemp) rv= -5;            /* is check sum correct? */
	}

	/* Determine number of GPR scans in file */
	start_byte = ftell(infile);          /* get current location (byte) */
	fseek(infile, 0L, SEEK_END);         /* go to end of file */
	last_byte = ftell(infile);           /* get last location (byte) */
	fseek(infile, start_byte, SEEK_SET); /* return file pointer to first scan */
	num_bytes = last_byte-start_byte;
	*num_scans = num_bytes / (hdrPtr1->rh_nsamp*(hdrPtr1->rh_bits/8));

	return rv;
}

/*************************************************************************/
int GetSSHdrs(long *nscans,int *nsamps,int *pt0,double *ttime,double *x0,
				double *xe,double *xi,double *ssfreq,double *antsep,
				char *dflnm,FILE *dfile)
{
	float tracehdr[25];
	int i,j,pcnt,paramlinecnt=0,dateline=10,lpline=20;
	char hdfilename[MAX_PATHLEN], *s, tmp[160];
	char first_two[3], *p[50], paramfileline[80];
	FILE *hd_file;

	rewind(dfile);
	if (fread((void *)tracehdr,25*sizeof(float),1,dfile)<1) return 1;
	if (tracehdr[5]!=2) return 1;
	/* for (i=0;i<25;i++) printf("Trace Header #%d=%f\n",i+1,tracehdr[i]); */
	/* getch(); */

	s=dflnm; hdfilename[0]=0;
	while ((*s)!='.')  { strncat(hdfilename,s,1); s++; }
	strncat(hdfilename,".hd",3);

	if ((hd_file = fopen(hdfilename,"rt")) == NULL)
	{
		printf("\npulseEKKO format should have separate header file.\n");
		printf("Unable to open %s.\n",hdfilename);
		exit(2);
	}

	while ((fgets(paramfileline,79,hd_file))!=NULL)
	{
		strncpy(first_two,paramfileline,2);
		if ( (!strncmp(first_two,"\n\0",2)) \
		|| (!strncmp(first_two," \n",2)) \
		|| (!strncmp(first_two,"  ",2)) \
		|| (!strncmp(first_two,"/*",2)) ) continue;

		/*mystery comment*/

		paramlinecnt++;

		if (paramlinecnt==1) continue;

		if (paramlinecnt<dateline)
		{
			if (paramfileline[2]=='/' && paramfileline[5]=='/')
				{ strncpy(hddate,paramfileline,8); hddate[8]=0; dateline=paramlinecnt; }
			else strcat(comments,paramfileline);
			continue;
		}

		pcnt=brkstr(p, paramfileline, SETA);
		if (!strncmp(p[0],"NUMBER OF TRACES",16)) { *nscans=atol(p[1]); continue; }
		if (!strncmp(p[0],"NUMBER OF PTS/TRC",17)) { *nsamps=atoi(p[1]); continue; }
		if (!strncmp(p[0],"TIMEZERO AT POINT",17)) { *pt0=atoi(p[1]); continue; }
		if (!strncmp(p[0],"TOTAL TIME WINDOW",17)) { *ttime=atof(p[1]); continue; }
		if (!strncmp(p[0],"STARTING POSITION",17)) { *x0=atof(p[1]); continue; }
		if (!strncmp(p[0],"FINAL POSITION",14)) { *xe=atof(p[1]); continue; }
		if (!strncmp(p[0],"STEP SIZE USED",14)) { *xi=atof(p[1]); continue; }
		if (!strncmp(p[0],"NOMINAL FREQUENCY",17)) { *ssfreq=atof(p[1]); continue; }
		if (!strncmp(p[0],"ANTENNA SEPARATION",18)) { *antsep=atof(p[1]); continue; }

		strcat(comments,p[0]); i=1;
		while (pcnt>i) { strcat(comments,"="); strcat(comments,p[i]); i++; }
		strcat(comments,"\n");
	}

	if (tracehdr[2]!=(float)(*nsamps)) return 1;
	if (tracehdr[8]!=(float)(*ttime)) return 1;

	fclose(hd_file); rewind(dfile);
	return 0;
}

/*************************************************************************/
int GetMala(long *nscans,int *nsamps,double *ttime,
				double *malafreq,double *antsep,double *xi,double *tinc,
				char *dflnm,FILE *dfile)
{
	int i,j,pcnt,paramlinecnt=0,lpline=20,t_flag=0,d_flag=0;
	char hdfilename[MAX_PATHLEN], *s, tmp[160];
	char first_two[3], *p[50], paramfileline[80];
	FILE *hd_file;

	rewind(dfile);

	s=dflnm; hdfilename[0]=0;
	while ((*s)!='.')  { strncat(hdfilename,s,1); s++; }

	if ( (strncmp(s,".RD3",4)) && (strncmp(s,".rd3",4)) ) return 1;

	strncat(hdfilename,".rad",4);

	if ((hd_file = fopen(hdfilename,"rt")) == NULL)
	{
		printf("\nMala data should have separate header file.\n");
		printf("Unable to open %s.\n",hdfilename);
		exit(2);
	}

	while ((fgets(paramfileline,79,hd_file))!=NULL)
	{
		strncpy(first_two,paramfileline,2);
		if ( (!strncmp(first_two,"\n\0",2)) \
		|| (!strncmp(first_two," \n",2)) \
		|| (!strncmp(first_two,"  ",2)) \
		|| (!strncmp(first_two,"/*",2)) ) continue;

		/*mystery comment*/

		paramlinecnt++;

		pcnt=brkstr(p, paramfileline, SETM);
		if (!strncmp(p[0],"SAMPLES",7)) { *nsamps=atoi(p[1]); continue; }
		if (!strncmp(p[0],"DISTANCE FLAG",13)) { d_flag=atoi(p[1]); continue; }
		if (!strncmp(p[0],"TIME FLAG",9)) { t_flag=atoi(p[1]); continue; }
		if (!strncmp(p[0],"TIME INTERVAL",13)) { *tinc=atof(p[1]); continue; }
		if (!strncmp(p[0],"DISTANCE INTERVAL",17)) { *xi=atof(p[1]); continue; }
		if (!strncmp(p[0],"ANTENNAS",8)) { *malafreq=atof(p[1]); continue; }
		if (!strncmp(p[0],"TIMEWINDOW",10)) { *ttime=atof(p[1]); continue; }
		if (!strncmp(p[0],"LAST TRACE",10)) { *nscans=atol(p[1]); continue; }
		if (!strncmp(p[0],"ANTENNA SEPARATION",18)) { *antsep=atof(p[1]); continue; }
		if (!strncmp(p[0],"COMMENT",7)) { strcat(comments,p[1]); continue; }

		strcat(comments,"\n");
	}
	if (d_flag) *tinc=0.;
	if (t_flag) *xi=0.;

	fclose(hd_file); rewind(dfile);
	return 0;
}

/**************************** ReadSegyReelHdr() *****************************/
/* Opens a file.  Reads the SEG-Y reel header.  Closes the file.
 * If the data stored in the header is in big-endian format (opposite of
 * the little-endian Intel scheme), "swap_bytes" is set to 1 to inform the
 * calling function and the bytes in the header are reversed (except for
 * the 3200-byte ASCII block).
 *
 * Parameters:
 *  char *filename  - pointer to buffer holding filename
 *  int *swap_bytes - address of variable, will return 0 if byte order OK,
 *                    non-zero if byte order were reversed for PC usage
 *  long *num_traces - address of variable, will hold number of GPR scans on return
 *  struct SegyReelHdrStruct *hdrPtr - address of reel header structure
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, "sgy_pgms.h".
 * Calls: fread, rewind, fseek, ftell.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: April 12, 1995
 */
int ReadSegyReelHdr (int *swap_bytes, long *num_traces,
										struct SegyReelHdrStruct *hdrPtr, FILE *infile)
{
		int i, error;
		long start_byte,last_byte,num_bytes,data_block_bytes;

		/* Verify parameter list */
		if (hdrPtr == NULL || num_traces == NULL || swap_bytes == NULL) return 1;

		/* Get header */
		if (fread((void *)hdrPtr,sizeof(struct SegyReelHdrStruct),1,infile) < 1)
				return 5;

	/* Determine if bytes must be swapped for PC usage */
		if (hdrPtr->format <= 255)
		*swap_bytes = 0;              /* order is Intel 80x86 little-endian */
		else
		*swap_bytes = 1;              /* order is Motorola 86000 big-endian */
		if (*swap_bytes)
		{   SWAP_4BYTES(hdrPtr->jobid);
				SWAP_4BYTES(hdrPtr->lino);
				SWAP_4BYTES(hdrPtr->reno);
				SWAP_2BYTES(hdrPtr->ntrpr);
				SWAP_2BYTES(hdrPtr->nart);
				SWAP_2BYTES(hdrPtr->hdt);
				SWAP_2BYTES(hdrPtr->dto);
				SWAP_2BYTES(hdrPtr->hns);
				SWAP_2BYTES(hdrPtr->nso);
				SWAP_2BYTES(hdrPtr->format);
				SWAP_2BYTES(hdrPtr->fold);
				SWAP_2BYTES(hdrPtr->tsort);
				SWAP_2BYTES(hdrPtr->vscode);
				SWAP_2BYTES(hdrPtr->hsfs);
				SWAP_2BYTES(hdrPtr->hsfe);
				SWAP_2BYTES(hdrPtr->hslen);
				SWAP_2BYTES(hdrPtr->hstyp);
				SWAP_2BYTES(hdrPtr->schn);
				SWAP_2BYTES(hdrPtr->hstas);
				SWAP_2BYTES(hdrPtr->hstae);
				SWAP_2BYTES(hdrPtr->htatyp);
				SWAP_2BYTES(hdrPtr->hcorr);
				SWAP_2BYTES(hdrPtr->bgrcv);
				SWAP_2BYTES(hdrPtr->rcvm);
				SWAP_2BYTES(hdrPtr->mfeet);
				SWAP_2BYTES(hdrPtr->polyt);
				SWAP_2BYTES(hdrPtr->vpol);
				for (i=0; i<170; i++)
						SWAP_2BYTES(hdrPtr->hunass[i]);
		}

		/* Verify we have a SEG-Y file header */
		if (hdrPtr->format < 1 || hdrPtr->format > 4) return 7;

		/* Determine number of GPR traces in file */
		*num_traces = -1;                    /* default is error value */
		start_byte = ftell(infile);          /* get current location (byte) */
		if (start_byte < 0)  return 6;       /* report error */

		fseek(infile, 0L, SEEK_END);         /* go to end of file */
		last_byte = ftell(infile);           /* get last location (byte) */
		if (last_byte < 0) return 6;         /* report error */

		fseek(infile, start_byte, SEEK_SET); /* return file pointer to first scan */
		num_bytes = last_byte - start_byte;  /* number of bytes left in file */
		data_block_bytes = hdrPtr->hns;      /* number of samples per trace */

		/* Multiply data_block_bytes by number of bytes per sample */
		switch(hdrPtr->format)
		{   case 3:
						data_block_bytes *= 2;
						break;
				default:
						data_block_bytes *= 4;
						break;
		}

		/* Add size of trace header */
		data_block_bytes += sizeof(struct SegyTraceHdrStruct);

		/* Calculate number of traces */
		*num_traces = num_bytes / data_block_bytes;

	return 0;
}

/*************************************************************************/
int fill_plot_image(int num,int num_hdrs,int start_flag,FILE *datfile)
{
	long int firstloc,startloc,midloc,templong;
	short *ones16bitscan,*s16bitgscan,ntop=(short)(pow(2.,(num_bits-1))-1);
	signed char *ones8bitscan;
	unsigned char *oneu8bitscan,u8bitval,key;
	unsigned short *oneu16bitscan;
	int i,j,k,m,ds0=0,row,col,zdat_flag=0;
	int s_dat_flag=0,u_dat_flag=0,avgscmed;
	char lab[80];
	double u_avgdiff=0.0,s_avgdiff=0.0,*davgscan;

	/********************** fill plot_image array *****************/
	/**** calculate padding for top and left side of screen ****/
	viewport(v1x1*scrn_ratio,v1x2*scrn_ratio,v1y1,v1y2);
	window(0.,100.,0.,100.);
	lorg(5); pen(v_color); csize(1.0);
	label(50.,80.,"Reading data ....");
	label(50.,70.,"Countdown to 0:");

	padx=hpgl_p1xc; /* graphics lib global */
	pady=2*(v_rowy-hpgl_p2yc-1);
	maxrows=nnum_samps+pady;
	maxcols=ntotmscans+padx;

	/******** allocate space for 2D image, bias-scans ******/
	if (start_flag)
	{
		plot_array=image(maxrows,maxcols);
		if ((new_to_old=(int *)malloc(maxcols*sizeof(int)))==NULL)
		{
			plotter_off();
			printf("Malloc problem in new_to_old.  You need more memory.\n");
			exit(1);
		}
		for (i=0;i<maxcols;i++) new_to_old[i]=i;
		if ((avgscan=(int *)malloc(nnum_samps*sizeof(int)))==NULL)
		{
			plotter_off();
			printf("Malloc problem in avgscan.  You need more memory.\n");
			exit(1);
		}
	}

	if ((davgscan=(double *)malloc(nnum_samps*sizeof(double)))==NULL)
	{
		plotter_off();
		printf("Malloc problem in davgscan.  You need more memory.\n");
		exit(1);
	}

	/****** find positions for file pointer ********************/
	rewind(datfile);  fseek(datfile,fhdr_bytes,SEEK_SET);
	startloc=ftell(datfile);
	firstloc=startloc+(bytesperscan*abs_first);
	midloc=startloc+(bytesperscan*(int)(ntotmscans/2));
	fseek(datfile,midloc,SEEK_SET);

	/******* set start of scan data *************/
	if (s10_flag)
	{
	  ds0=num*num_samps;
	  if (start_flag)
	  {
	  	if ((marks=(int *)malloc((maxcols)*sizeof(int)))==NULL)
			{
				plotter_off();
				printf("Malloc problem in marks.  You need more memory.\n");
				exit(1);
			}
		}
	}
	else ds0=(int)((float)thdr_bytes*(float)(8./(float)num_bits));

	/****** fill padded part of array with zeros *************/
	for (col=0;col<padx;col++)
	{
		for (row=0;row<maxrows;row++) plot_array[row][col]=0;
	}

	/********************************* 16 bit ******************************/
	if ( (num_bits==16)||(num_bits==12) )
	{
		if ((oneu16bitscan=(unsigned short *)malloc(bytesperscan))==NULL)
		{
			plotter_off();
			printf("Malloc problem in oneu16bitscan.  You need more memory.\n");
			exit(1);
		}
		if ((ones16bitscan=(short *)malloc(bytesperscan))==NULL)
		{
			plotter_off();
			printf("Malloc problem in ones16bitscan.  You need more memory.\n");
			exit(1);
		}

		/**************************************
		if (fread((void *)oneu16bitscan,bytesperscan,1,datfile)<1)
			{ plotter_off(); printf("Problem reading middle scan"); exit(1); }
		fseek(datfile,midloc,SEEK_SET);
		if (fread((void *)ones16bitscan,bytesperscan,1,datfile)<1)
			{ plotter_off(); printf("Problem reading middle scan"); exit(1); }

		if ( (ss_flag) && (!segy_flag) )
		{
			if (num_samps!=(int)((float *)ones16bitscan)[2])
			{
				printf("\a"); plotter_off();
				printf("Trace header info did not match header file info.\n");
				exit(1);
			}
		}
		********************************************/

		if (ss_flag || mala_flag) s_dat_flag=1;
		else if (s10_flag) u_dat_flag=1;
		else
		{
			/******* determine if data is signed or unsigned *************
			for (i=ds0;i<num_samps-1;i++)
			{
				s_avgdiff+=fabs(ones16bitscan[i+1]-ones16bitscan[i]);
				u_avgdiff+=fabs(oneu16bitscan[i+1]-oneu16bitscan[i]);
			}
			if (s_avgdiff>u_avgdiff)
			{
				u_dat_flag=1;
				for(i=0;i<num_samps;i++) ones16bitscan[i]=(short)oneu16bitscan[i]-ntop;
			}
			else s_dat_flag=1;
			*************************************************************/
			if (signed_flag) s_dat_flag=1;
			else u_dat_flag=1;
		}

		fseek(datfile,firstloc,SEEK_SET);

		/************************** 16 bit signed ****************************/
		if (s_dat_flag)
		{
			if (fread((void *)ones16bitscan,bytesperscan,1,datfile)<1)
				{ plotter_off(); printf("Problem reading 1st scan"); exit(1); }

			/******************** for debugging ******************
			sprintf(lab,"16bs sample %d = %d",ds0,ones16bitscan[ds0]);
			label(50.,50.,lab);
			sprintf(lab,"sample %d = %d",ds0+1,ones16bitscan[ds0+1]);
			label(50.,30.,lab);
			getch();
			*****************************************************/

			if ( (ss_flag) && (!segy_flag) )
			{
				/************* for debugging ****************************/
				if (num_samps!=(int)((float *)ones16bitscan)[2])
				{
					printf("\a"); plotter_off();
					printf("Trace header info did not match header file info.\n");
					exit(1);
				}
				/**********************************************************/
				if (((float *)ones16bitscan)[21]==1) zdat_flag=1;
				/*** ds0=64+(int)(((float *)ones16bitscan)[20]); ***/
			}

			/****** read 1st scan into array with conversion *********/
			for (row=0;row<pady;row++) plot_array[row][padx]=0;
			for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
			{
				if (zdat_flag) u8bitval=127;
				else
				{
					templong=(((long)((double)ones16bitscan[k])+ntop)>>shift);
					if (templong>255) templong=255;
					if (templong<0) templong=0;
					u8bitval=templong;
				}
				/** u8bitval=((unsigned short)(((short)((double)ones16bitscan[k]))+ntop))>>shift; ***/
				for (j=0;j<sfill;j++)
				{
					plot_array[row+j][padx]=u8bitval;
					davgscan[i+j]=(double)plot_array[row+j][padx];
				}
			}

			/****** read rest of scans into array with conversion ******/
			m=maxcols-padx;
			for (col=padx+1;col<maxcols;col++)
			{
				if (fread((void *)ones16bitscan,bytesperscan,1,datfile)<1)
				{
					plotter_off();
					printf("Problem reading scan #%d\n",col-padx+1);
					exit(1);
				}

				if ( (ss_flag) && (!segy_flag) )
				{
					/************* for debugging ****************************
					if (num_samps!=(int)((float *)ones16bitscan)[2])
					{
						printf("\a"); plotter_off();
						printf("Trace header info did not match header file info.\n");
						exit(1);
					}
					**********************************************************/
					if (((float *)ones16bitscan)[21]==1) zdat_flag=1;
					/*** ds0=64+(int)(((float *)ones16bitscan)[20]); ***/
				}

				for (row=0;row<pady;row++) plot_array[row][col]=0;
				for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
				{
					if (zdat_flag) u8bitval=127;
					else
					{
						templong=(((long)((double)ones16bitscan[k])+ntop)>>shift);
						if (templong>255) templong=255;
						if (templong<0) templong=0;
						u8bitval=templong;
					}
					/*** u8bitval=((unsigned short)(((short)((double)ones16bitscan[k]))+ntop))>>shift; ***/
					for (j=0;j<sfill;j++)
					{
						plot_array[row+j][col]=u8bitval;
						davgscan[i+j]+=(double)plot_array[row+j][col];
					}
				}
				m--;
				if ((m%20)==0)
				{
					pen(0); sprintf(lab,"%d",m+20); label(50.,65.,lab);
					pen(v_color); sprintf(lab,"%d",m); label(50.,65.,lab);
				}
			}
			/****************** done filling plot_image array *************/
			free(ones16bitscan); free(oneu16bitscan);
			if ( (shift==8)||(shift==4) ) avgscmed=127.;
			else if (shift==12) avgscmed=7.;
			for (i=0;i<nnum_samps;i++)
			avgscan[i]=((int)(davgscan[i]/(double)ntotmscans))-avgscmed;
			free(davgscan); return 3;
		}
		/********************** end of 16 bit signed *************************/

		/************************** 16 bit unsigned **************************/
		if (u_dat_flag)
		{
			if (fread((void *)oneu16bitscan,bytesperscan,1,datfile)<1)
				{ plotter_off(); printf("Problem reading 1st scan"); exit(1); }

			/******************** for debugging ******************
			sprintf(lab,"16bu sample %d = %hd",ds0,oneu16bitscan[ds0]);
			label(50.,50.,lab);
			sprintf(lab,"sample %d = %hd",ds0+1,oneu16bitscan[ds0+1]);
			label(50.,30.,lab);
			getch();
			*****************************************************/

			if (s10_flag)
			{
				/**************** show S10 ticks if present ************/
				if (oneu16bitscan[1]==59392)
				{
					for(i=(ds0+3);i<(ds0+21);i++) oneu16bitscan[i]=0;
					marks[0]=padx; mcnt++;
				}
			}

			/****** read 1st scan into array with conversion *********/
			for (row=0;row<pady;row++) plot_array[row][padx]=0;
			for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
			{
				for (j=0;j<sfill;j++)
				{
					plot_array[row+j][padx]=oneu16bitscan[k]>>shift;
					davgscan[i+j]=(double)plot_array[row+j][padx];
				}
			}

			/****** read rest of scans into array with conversion ******/
			m=maxcols-padx;
			for (col=padx+1;col<maxcols;col++)
			{
				if (fread((void *)oneu16bitscan,bytesperscan,1,datfile)<1)
				{
					plotter_off();
					printf("Problem reading scan #%d\n",col-padx+1);
					exit(1);
				}
				if (s10_flag)
				{
					if (oneu16bitscan[1]==59392)
					{
						for(i=(ds0+3);i<(ds0+21);i++) oneu16bitscan[i]=0;
						marks[mcnt]=col; mcnt++;
					}
					if (mcnt>2) marks_flag=1;
				}
				for (row=0;row<pady;row++) plot_array[row][col]=0;
				for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
				{
					for (j=0;j<sfill;j++)
					{
						plot_array[row+j][col]=oneu16bitscan[k]>>shift;
						davgscan[i+j]+=(double)plot_array[row+j][col];
					}
				}
				m--;
				if ((m%100)==0)
				{
					pen(0); sprintf(lab,"%d",m+100); label(50.,65.,lab);
					pen(v_color); sprintf(lab,"%d",m); label(50.,65.,lab);
				}
			}
			/****************** done filling plot_image array *************/
			free(oneu16bitscan); free(ones16bitscan);
			if ( (shift==8)||(shift==4) ) avgscmed=127.;
			else if (shift==12) avgscmed=7.;
			for (i=0;i<nnum_samps;i++)
			avgscan[i]=((int)(davgscan[i]/(double)ntotmscans))-avgscmed;
			free(davgscan); return 2;
		}
		/********************* end of 16 bit unsigned ************************/
	}
	/************************** end of 16 bit ******************************/


	/************************** 8 bit **************************************/
	if (num_bits==8)
	{
		shift-=8;
		if ((oneu8bitscan=(unsigned char *)malloc(bytesperscan))==NULL)
		{
			plotter_off();
			printf("Malloc problem in oneu8bitscan.  You need more memory.\n");
			exit(1);
		}
		if ((ones8bitscan=(signed char *)malloc(bytesperscan))==NULL)
		{
			plotter_off();
			printf("Malloc problem in ones8bitscan.  You need more memory.\n");
			exit(1);
		}
		if ((s16bitgscan=(short *)malloc(2*bytesperscan))==NULL)
		{
			plotter_off();
			printf("Malloc problem in s16bitgscan.  You need more memory.\n");
			exit(1);
		}

		if (fread((void *)oneu8bitscan,bytesperscan,1,datfile)<1)
			{ plotter_off(); printf("Problem reading middle scan"); exit(1); }
		fseek(datfile,midloc,SEEK_SET);
		if (fread((void *)ones8bitscan,bytesperscan,1,datfile)<1)
			{ plotter_off(); printf("Problem reading middle scan"); exit(1); }

		if (signed_flag) s_dat_flag=1;
		else u_dat_flag=1;

		/******* determine if data is signed or unsigned *************
		for (i=ds0;i<num_samps-1;i++)
		{
			s_avgdiff+=fabs(ones8bitscan[i+1]-ones8bitscan[i]);
			u_avgdiff+=fabs(oneu8bitscan[i+1]-oneu8bitscan[i]);
		}
		if (s_avgdiff<u_avgdiff) s_dat_flag=1;
		else
		{
			u_dat_flag=1;
			for(i=0;i<num_samps;i++) s16bitgscan[i]=(short)oneu8bitscan[i]-127;
		}
		**************************************************/
		/*************************************************************
		if (ss_flag) s_dat_flag=1;
		else
		{
			u_dat_flag=1;
			for(i=ds0;i<(num_samps+ds0);i++) s16bitgscan[i]=(short)oneu8bitscan[i]-127;
		}
		*************************************************************/

		fseek(datfile,firstloc,SEEK_SET);

		/************************** 8 bit signed *****************************/
		if (s_dat_flag)
		{
			if (fread((void *)ones8bitscan,bytesperscan,1,datfile)<1)
			{ plotter_off(); printf("Problem reading 1st scan"); exit(1); }

			/******************** for debugging ******************
			sprintf(lab,"8bs sample %d = %d",ds0,ones8bitscan[ds0]);
			label(50.,50.,lab);
			sprintf(lab,"sample %d = %d",ds0+1,ones8bitscan[ds0+1]);
			label(50.,30.,lab);
			getch();
			*****************************************************/

			/* for (i=ds0;i<num_samps;i++) ones8bitscan[i]=(signed char)((double)ones8bitscan[i]); */

			/****** read 1st scan into array with conversion *********/
			for (row=0;row<pady;row++) plot_array[row][padx]=0;
			for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
			{
				u8bitval=((unsigned char)(((signed char)((double)ones8bitscan[k]))+127))>>shift;
				for (j=0;j<sfill;j++)
				{
					plot_array[row+j][padx]=u8bitval;
					davgscan[i+j]=(double)plot_array[row+j][padx];
				}
			}

			/****** read rest of scans into array with conversion ******/
			m=maxcols-padx;
			for (col=padx+1;col<maxcols;col++)
			{
				if (fread((void *)ones8bitscan,bytesperscan,1,datfile)<1)
				{
					plotter_off();
					printf("Problem reading scan #%d\n",col-padx+1);
					exit(1);
				}
				for (row=0;row<pady;row++) plot_array[row][col]=0;
				for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
				{
					u8bitval=((unsigned char)(((signed char)((double)ones8bitscan[k]))+127))>>shift;
					for (j=0;j<sfill;j++)
					{
						plot_array[row+j][col]=u8bitval;
						davgscan[i+j]+=(double)plot_array[row+j][col];
					}
				}
				m--;
				if ((m%50)==0)
				{
					pen(0); sprintf(lab,"%d",m+50); label(50.,65.,lab);
					pen(v_color); sprintf(lab,"%d",m); label(50.,65.,lab);
				}
			}
			/****************** done filling plot_image array *************/
			free(ones8bitscan); free(oneu8bitscan); free(s16bitgscan);
			if (shift==0) avgscmed=127.; else if (shift==4) avgscmed=7.;
			for (i=0;i<nnum_samps;i++)
			avgscan[i]=((int)(davgscan[i]/(double)ntotmscans))-avgscmed;
			free(davgscan); return 1;
		}
		/********************* end of 8 bit signed ***************************/

		/************************** 8 bit unsigned ***************************/
		if (u_dat_flag)
		{
			if (fread((void *)oneu8bitscan,bytesperscan,1,datfile)<1)
				{ plotter_off(); printf("Problem reading 1st scan"); exit(1); }

			/******************** for debugging ******************
			sprintf(lab,"8bu sample %d = %d",ds0,oneu8bitscan[ds0]);
			label(50.,50.,lab);
			sprintf(lab,"sample %d = %d",ds0+1,oneu8bitscan[ds0+1]);
			label(50.,30.,lab);
			sprintf(lab,"tstep=%d, sfill=%d",tstep,sfill);
			label(50.,10.,lab);
			getch();
			*****************************************************/

			if (s10_flag)
			{
				/**************** show S10 ticks if present ************/
				if (oneu8bitscan[1]==0xE8)
				{
					for(i=(ds0+3);i<(ds0+21);i++) oneu8bitscan[i]=0;
					marks[0]=padx; mcnt++;
          if (mcnt>=(maxcols))
					{
						plotter_off();
            printf("Marks array size exceeded with %d.\n",mcnt);
						exit(0);
					}
				}
			}

			/****** read 1st scan into array with conversion *********/
			for (row=0;row<pady;row++) plot_array[row][padx]=0;
			for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
			{
				for (j=0;j<sfill;j++)
				{
					plot_array[row+j][padx]=oneu8bitscan[k]>>shift;
					davgscan[i+j]=(double)plot_array[row+j][padx];
				}
			}

			/****** read rest of scans into array with conversion ******/
			m=maxcols-padx;
			for (col=padx+1;col<maxcols;col++)
			{
				if (fread((void *)oneu8bitscan,bytesperscan,1,datfile)<1)
				{
					plotter_off();
					printf("Problem reading scan #%d\n",col-padx+1);
					exit(1);
				}
				if (s10_flag)
				{
					if (oneu8bitscan[1]==0xE8)
					{
						for(i=(ds0+3);i<(ds0+21);i++) oneu8bitscan[i]=0;
						marks[mcnt]=col; mcnt++;
            if (mcnt>=(maxcols))
						{
							plotter_off();
              printf("Marks array size exceeded with %d.\n",mcnt);
							exit(0);
						}
					}
					if (mcnt>2) marks_flag=1;
				}
				for (row=0;row<pady;row++) plot_array[row][col]=0;
				for (row=pady,k=ds0,i=0;row<maxrows;row+=sfill,k+=tstep,i+=sfill)
				{
					for (j=0;j<sfill;j++)
					{
						plot_array[row+j][col]=oneu8bitscan[k]>>shift;
						davgscan[i+j]+=(double)plot_array[row+j][col];
					}
				}
				m--;
				if ((m%50)==0)
				{
					pen(0); sprintf(lab,"%d",m+50); label(50.,65.,lab);
					pen(v_color); sprintf(lab,"%d",m); label(50.,65.,lab);
				}
			}
			/****************** done filling plot_image array *************/
			free(oneu8bitscan); free(ones8bitscan); free(s16bitgscan);
			if (shift==0) avgscmed=127.; else if (shift==4) avgscmed=7.;
			for (i=0;i<nnum_samps;i++)
			avgscan[i]=((int)(davgscan[i]/(double)ntotmscans))-avgscmed;
			free(davgscan); return 0;
		}
		/***********************end of 8 bit unsigned ************************/
	}
	/************************** end of 8 bit *******************************/
}

/*************************************************************************/
void set_gain(short *intscan, double *gain, int datval0)
{
	int i,j,k,fpt,lpt,ptsperwin;
	double maxamp[8],targetamp,dummax,gainpts[8];

	/******* find a gain function for data display ****************/
	targetamp=0.6*pow(2.,(double)(num_bits-1));

	fpt=datval0; ptsperwin=0;
	for (i=0;i<8;i++)
	{
		fpt+=ptsperwin;  maxamp[i]=1.;
		ptsperwin=num_samps/7;  if ((i==0) || (i==7)) ptsperwin/=2;
		for (j=fpt;j<(fpt+ptsperwin);j++)
		{
			dummax=fabs((double)intscan[j]);
			if (dummax>maxamp[i]) maxamp[i]=dummax;
		}
		gainpts[i]=targetamp/maxamp[i];
	}

	/********* for debugging *****************
	fprintf(debug_file,"gainpts: %5.2lf %5.2lf %5.2lf %5.2lf %5.2lf %5.2lf %5.2lf %5.2lf\n",
		gainpts[0],gainpts[1],gainpts[2],gainpts[3],gainpts[4],gainpts[5],gainpts[6],gainpts[7]);
	****************************************/

	ptsperwin=num_samps/7;
	for (i=0;i<num_samps;i++) { gain[i]=0.0; }

	for (j=0;j<7;j++)
	{
		k=j+1; fpt=j*ptsperwin; lpt=k*ptsperwin;
		for (i=fpt;i<lpt;i++)
		{
			gain[i]=gainpts[j]+((gainpts[k]-gainpts[j])*(double)(i-fpt)/(double)ptsperwin);
		}
	}
	gain[num_samps-1]=gainpts[7];

}

/************************************************************************/
int brkstr(char **ptranm, char *dtaanm, char *set)
{
	int i, counter;                  /*** ptranm = pointer array name *****/
									 /*** dtaanm = data array name ********/
	ptranm[0]=strtok(dtaanm,set);    /*** set = a defined char set ********/
	i=0;
	do
	{
		i++;
		ptranm[i]=strtok(NULL,set);
	} while (ptranm[i]!=NULL);
	counter=i;
	return(counter);  /********** returns # of substrings *****************/
}

/************************************************************************/
int GetNumBytes(long *num_bytes, FILE *dfile)
{
	long int start_byte,last_byte;

	rewind(dfile);
	start_byte = ftell(dfile);          /* get current location (byte) */
	fseek(dfile, 0L, SEEK_END);         /* go to end of file */
	last_byte = ftell(dfile);           /* get last location (byte) */
	rewind(dfile);
	*num_bytes = (int)(last_byte-start_byte)+1;

	if (*num_bytes>=512) return 0;  else return 1;
}

/*************************************************************************/
void spline(double *x,double *y,int n,double yp1,double ypn,double *y2)
{
	int i,k;
	double pp,qn,sig,un,u[512];

	if (yp1 > 0.99e30) y2[1]=u[1]=0.0;
	else
	{
	 y2[1] = -0.5;
	 u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}
	for (i=2;i<=n-1;i++)
	{
	 sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
	 pp=sig*y2[i-1]+2.0;
	 y2[i]=(sig-1.0)/pp;
	 u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
	 u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/pp;
	}
	if (ypn > 0.99e30) qn=un=0.0;
	else
	{
	 qn=0.5;
	 un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}
	y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
	for (k=n-1;k>=1;k--) y2[k]=y2[k]*y2[k+1]+u[k];
}

/*************************************************************************/
void splint(double *xa,double *ya,double *y2a,int n,double x,double *y)
{
	int klo,khi,k;
	double h,b,a;

	klo=1;
	khi=n;

	while (khi-klo > 1)
	{
	 k=(khi+klo) >> 1;
	 if (xa[k] > x) khi=k;
	 else klo=k;
	}

	h=xa[khi]-xa[klo];
	if (h == 0.0) printf("Bad XA input to routine SPLINT");
	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h;
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}

/*************************************************************************/
int lateral_corr()
{
	int i,j,k,*mdif,algo,np,op,keep[4],skip[4],iter,nmaxcols;
	double alch[32]={.1,.111,.125,.143,.167,.2,.222,.25,.286,.3,.333,.375,.4,
				.428,.444,.5,.555,.571,.6,.625,.667,.7,.714,.75,.777,.8,
				.833,.857,.875,.888,.9 } ;
	double mratio;

	if (mcnt==1) { marks_flag=1; return; }
	if ((mdif=(int *)malloc(mcnt*sizeof(int)))==NULL)
	{
		plotter_off();
		printf("Malloc problem in mdif.  You need more memory.\n");
		exit(1);
	}

	if ((abs(ntotmscans-mcnt)<2) && s10_flag) { marks_flag=0; mindif=1; return; }

	for (i=0;i<mcnt-1;i++)
	{
		j=i+1;
		mdif[i]=marks[j]-marks[i];
		if (mdif[i]<10) continue;

		/******* for debugging *******************************
		fprintf(debug_file,"i=%d  mdif=%d  marks=%d\n",i,mdif[i],marks[i]);
		*****************************************************/

		mindif=MIN((double)mindif,(double)mdif[i]);
	}

	/******* for debugging *******************************
	fprintf(debug_file,"mcnt=%d  mindif=%d\n",mcnt,mindif);
	*****************************************************/

	np=marks[0];
	for (i=0;i<np;i++) new_to_old[i]=i;
	for (i=0;i<mcnt-1;i++)
	{
		mratio=((double)mindif)/((double)mdif[i]);
		/* if (mratio==1.0) continue; */
		for (j=30;j>=0;j--)
		{
			/******* for debugging *******************************
			fprintf(debug_file,"\n j=%d  alch=%lf",j,alch[j]);
			*****************************************************/
			if (mratio>alch[j]) { algo=j+1; break; }
		}

		/******* for debugging *******************************
		fprintf(debug_file,"\n i=%d  mratio=%lf  algo=%d\n",i,mratio,algo);
		*****************************************************/
		/* plotter_off(); exit(0); */
		op=marks[i];
		switch (algo)
		{
			case 31:
				iter=1; keep[0]=1; skip[0]=0;
				break;
			case 30:
				iter=1; keep[0]=9; skip[0]=1;
				break;
			case 29:
				iter=1; keep[0]=8; skip[0]=1;
				break;
			case 28:
				iter=1; keep[0]=7; skip[0]=1;
				break;
			case 27:
				iter=1; keep[0]=6; skip[0]=1;
				break;
			case 26:
				iter=1; keep[0]=5; skip[0]=1;
				break;
			case 25:
				iter=1; keep[0]=4; skip[0]=1;
				break;
			case 24:
				iter=2; keep[0]=4; skip[0]=1;
				keep[1]=3; skip[1]=1;
				break;
			case 23:
				iter=1; keep[0]=3; skip[0]=1;
				break;
			case 22:
				iter=2; keep[0]=3; skip[0]=1;
				keep[1]=2; skip[1]=1;
				break;
			case 21:
				iter=3; keep[0]=3; skip[0]=1;
				keep[1]=2; skip[1]=1;
				keep[2]=2; skip[2]=1;
				break;
			case 20:
				iter=1; keep[0]=2; skip[0]=1;
				break;
			case 19:
				iter=1; keep[0]=2; skip[0]=1;
				keep[1]=2; skip[1]=1;
				keep[2]=1; skip[2]=1;
				break;
			case 18:
				iter=2; keep[0]=2; skip[0]=1;
				keep[1]=1; skip[1]=1;
				break;
			case 17:
				iter=3; keep[0]=2; skip[0]=1;
				keep[1]=1; skip[1]=1;
				keep[2]=1; skip[2]=1;
				break;
			case 16:
				iter=4; keep[0]=2; skip[0]=1;
				keep[1]=1; skip[1]=1;
				keep[2]=1; skip[2]=1;
				keep[3]=1; skip[3]=1;
				break;
			case 15:
				iter=1; keep[0]=1; skip[0]=1;
				break;
			case 14:
				iter=4; keep[0]=1; skip[0]=2;
				keep[1]=1; skip[1]=1;
				keep[2]=1; skip[2]=1;
				keep[3]=1; skip[3]=1;
				break;
			case 13:
				iter=3; keep[0]=1; skip[0]=2;
				keep[1]=1; skip[1]=1;
				keep[2]=1; skip[2]=1;
				break;
			case 12:
				iter=2; keep[0]=1; skip[0]=2;
				keep[1]=1; skip[1]=1;
				break;
			case 11:
				iter=3; keep[0]=1; skip[0]=2;
				keep[1]=1; skip[1]=2;
				keep[2]=1; skip[2]=1;
				break;
			case 10:
				iter=1; keep[0]=1; skip[0]=2;
				break;
			case 9:
				iter=3; keep[0]=1; skip[0]=3;
				keep[1]=1; skip[1]=2;
				keep[2]=1; skip[2]=2;
				break;
			case 8:
				iter=2; keep[0]=1; skip[0]=3;
				keep[1]=1; skip[1]=2;
				break;
			case 7:
				iter=1; keep[0]=1; skip[0]=3;
				break;
			case 6:
				iter=2; keep[0]=1; skip[0]=4;
				keep[1]=1; skip[1]=3;
				break;
			case 5:
				iter=1; keep[0]=1; skip[0]=4;
				break;
			case 4:
				iter=1; keep[0]=1; skip[0]=5;
				break;
			case 3:
				iter=1; keep[0]=1; skip[0]=6;
				break;
			case 2:
				iter=1; keep[0]=1; skip[0]=7;
				break;
			case 1:
				iter=1; keep[0]=1; skip[0]=8;
				break;
			case 0:
				iter=1; keep[0]=1; skip[0]=9;
				break;
		}
		squeez_dat(iter,keep,skip,&np,&op,i);
	}

	for (;op<maxcols;np++,op++)
	{
		new_to_old[np]=op;
		for (j=pady;j<maxrows;j++)
		plot_array[j][np]=plot_array[j][op];
	}
	nmaxcols=np;
	for (i=np;i<maxcols;i++)
		for (j=pady;j<maxrows;j++) plot_array[j][i]=0;

	free(mdif);
	return nmaxcols;
}

/*************************************************************************/
void squeez_dat(int iter,int *keep,int *skip,int *newptr,int *oldptr,int i)
{
	int j,k,l,stop_flag=0,o_p,n_p;

	o_p=(*oldptr);  n_p=(*newptr);
	for (j=pady;j<maxrows;j++)  plot_array[j][n_p]=plot_array[j][o_p];
	new_to_old[n_p]=o_p;

	while (1)
	{
		for (j=0;j<iter;j++)
		{
			for (k=o_p;k<o_p+keep[j];k++)
			{
				if (k==marks[i+1]) { stop_flag=1; n_p++; break; }
				for (l=pady;l<maxrows;l++)  plot_array[l][n_p]=plot_array[l][k];
				new_to_old[n_p]=k;
				n_p++;
			}
			if (stop_flag) { o_p=marks[i+1]; break; }
			o_p+=(keep[j]+skip[j]);
			if (o_p>=marks[i+1]) { o_p=marks[i+1]; stop_flag=1; break; }
		}
		if (stop_flag) break;
	}

	*oldptr=o_p;  *newptr=n_p;
}

/*************************************************************************/
void help_sub(int num)
{
	int key;

	grab1024(mgrab_screen);

	viewport(1.,132.3,1.,99.); window(0.,100.,0.,100.);
	vclear(); pen(v_color); frame();

	switch (num)
	{
		case 0:
			lorg(5); csize(1.0); pen(v_color-5);
			label(50.,97.5,"Field Data Profile Viewing");
			plot(5.,95.,-2); plot(95.,95.,-1);
			lorg(2); csize(0.75);
			label(2.,85.,"The following keys are active:");
			label(4.,80.,"ARROW keys move cursors, ");
			label(4.,75.,"Num-Lock ARROW keys (4,6,8,2) jump cursors,");
			label(4.,70.,"0 key sets time zero at current time cursor position,");
			label(4.,65.,"B key removes or replaces average background trace,");
			label(4.,60.,"L key does lateral correction given SIR10 marks in data");
			label(6.,55.,"D key dumps data on screen to a binary file of 16bit ints");
			label(4.,50.,"A and R add and remove SIR10 marks for lateral correction");
			label(6.,45.,"G allows time-variable gain to be applied");
			label(4.,40.,"V key starts interactive hyperbola matching velocity info,");
			label(4.,35.,"S and T display sample# or time of horizontal cursor,");
			label(4.,30.,"P creates a hardcopy EPS image file,");
			label(4.,25.,"F2 scrolls through various color palettes.");

			pen(v_color); lorg(5); csize(1.3); label(50.,5.,"Press any key to return");
			getch();
			break;
		case 1:
			lorg(5); csize(1.0); pen(v_color-5);
			label(50.,97.5,"Velocity Estimation");
			plot(5.,95.,-2); plot(95.,95.,-1);
			lorg(2); csize(0.75);
			label(2.,85.,"The following keys are active:");
			label(4.,80.,"R-ARROW, L-ARROW or TAB between active parameters,");
			label(4.,75.,"CTRL-ARROW keys to move cursors and drag hyperbola,");
			label(2.,65.,"The value of the active parameter can be varied by:");
			label(4.,60.,"HOME   UP-AR   PG-UP  increase value");
			label(4.,55.,"+1.0   +0.1    +0.01  and");
			label(4.,50.," END   DN-AR   PG-DN  decrease value");
			label(4.,45.,"-1.0   -0.1    -0.01  ");
			label(2.,40.,"HINT: Turn off Num-Lock");
			label(2.,30.,"The idea is to place the cursor intersection at the top");
			label(2.,25.,"of a diffraction hyperbola in the data that is from a");
			label(2.,20.,"buried object.  The parameters can be varied in an attempt");
			label(2.,15.,"to match the model hyperbola to the data for an estimate");
			label(2.,10.,"of average permittivity down to the object.");
			lorg(5);

			pen(v_color); csize(1.3); label(50.,4.,"Press any key to return");
			getch();
			break;
		case 2:
			lorg(5); csize(1.0); pen(v_color-5);
			label(50.,97.5,"Gain Function");
			plot(5.,95.,-2); plot(95.,95.,-1);
			lorg(2); csize(0.75);
			label(2.,85.,"The following keys are active:");
			label(4.,80.,"R-ARROW, L-ARROW or TAB between M and E,");
			label(2.,65.,"The value of the active parameter can be varied by:");
			label(4.,60.,"HOME   UP-AR   PG-UP  increase value");
			label(4.,55.,"+1.0   +0.1    +0.01  and");
			label(4.,50.," END   DN-AR   PG-DN  decrease value");
			label(4.,45.,"-1.0   -0.1    -0.01  ");
			label(2.,40.,"HINT: Turn off Num-Lock");
			label(2.,30.,"The gain applied here is cumulative and non-reversible");
			label(2.,25.,"unless you press Shift-Enter to re-read the data from disk.");
			label(2.,20.,"For this reason, gain conservatively to start.  The gain");
			label(2.,15.,"applied is according to Gain=M*time(ns)^^E where M=1 and");
			label(2.,10.,"E=0 for a gain of 1.0 to start.");
			lorg(5);

			pen(v_color); csize(1.3); label(50.,4.,"Press any key to return");
			getch();
			break;
	}
	plot1024(mgrab_screen);
	fieldvewpal();
}
#if 0
	 /***************** optional ****************************
	 pen(v_color); label(50.,5.,"Esc = Return      any other key = More");
	 key=getch();
	 if (key==ESC) break;
	 vclear();  frame();
	 lorg(5); csize(1.5);
	 label(50.,96.,"Interactive Modeling Screen");
	 plot(5.,92.,-2); plot(95.,92.,-1);
	 lorg(2);
	 label(5.,85.,"More instructions would follow here.");
	 label(5.,80.,"a previous run of this program.  It is an");
	 lorg(5);
	 ********************************************************/

/*************************************************************************/
void updatemarks(char *flnm, int tracenum, int add_rem_flag)
{
	unsigned char uc_markvalue=0xe8, uc_nomark=0xf1;
	unsigned short us_markvalue=0xe800, us_nomark=0xf100;
	int markloc;
	FILE *file_stream;

	if ( (file_stream=fopen(flnm,"r+b")) == NULL)
	{
		printf("\nFile %s will not open for update. Marks not changed.\n",flnm);
		return;
	}
	markloc=fhdr_bytes+(bytesperscan*tracenum)+thdr_bytes+1;
	fseek(file_stream,markloc,SEEK_SET);

	if (add_rem_flag)      /** adding a mark **/
	{
		if (num_bits==8)  fwrite(&uc_markvalue,1,1,file_stream);
		if ( (num_bits==16)||(num_bits==12) ) fwrite(&us_markvalue,2,1,file_stream);
	}
	else                   /** removing a mark **/
	{
		if (num_bits==8)  fwrite(&uc_nomark,1,1,file_stream);
		if ( (num_bits==16)||(num_bits==12) ) fwrite(&us_nomark,2,1,file_stream);
	}
	fclose(file_stream);
	return;
}
#endif
