#include <stdio.h>
#include <conio.h>
#include <bios.h>
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include <stdarg.h>
#include <bio.inc>
#include "feedef.h"
#define  MAXCOL 80

/* get DBCS display graphic array
   input:
     num:         DBC number
     pointer p:   inner code
   output:
     pointer pc2: DBC graphic array
*/
void getlib_char(int num, UC *p, UC *pc2)
{
    FILE *fp1;
    int m, x, y, j1, i;
    long int k;
    UC *p1, *p2;

    p1=malloc(32*sizeof(UC));
    p2=malloc(32*sizeof(UC));
    if((fp1=fopen("gfsdata\\cclib16.lib","r+b"))==NULL)
    {
        free(p1);
        free(p2);
        printf("Error: Cann't open DBCS library file!\n");
        exit(0);
    }
    i=0;
    while(i<num)
    {
        y=p[2*i]-160;
        x=p[2*i+1]-160;
        m=(y-1)*94+(x-1);

        if (y==0 || x==0)
        {
            free(p1);
            free(p2);
            return;
        }
        if (y>87 || y>94)
        {
            free(p1);
            free(p2);
            return;
        }
        if (y>9  && y<16)
        {
            free(p1);
            free(p2);
            return;
        }
        if(y<16)
        {
            m=(y-1)*94+(x-1);
        }
        else
        {
            m=(y-7)*94+(x-1);
        }
        k=(long)m*(long)32;
        fseek(fp1,k*sizeof(UC),SEEK_SET);
        fread(p1,sizeof(UC),32,fp1);
        pch_16(p1,p2);
        for(j1=0;j1<32;j1++)
            pc2[32*i+j1]=p2[j1];
        i=i+1;
    }
    fclose(fp1);
    free(p1);
    free(p2);
    return;
}

/* get SBCS display graphic array
   input:
     num:         SBC number
     pointer p:   inner code
   output:
     pointer pc2: SBC graphic array
*/
void getlib_asc(int num, UC* p, UC* pa2)
{
    FILE *fp1;
    int m, j1, i;
    long int k;
    UC *p1, *p2;

    p1=malloc(8*sizeof(UC));
    p2=malloc(8*sizeof(UC));
    if((fp1=fopen("gfsdata\\asc08.lib","r+b"))==NULL)
    {
        free(p1);
        free(p2);
        printf("Error: Cann't open SBCS library file!\n");
        exit(0);
    }
    i=0;
    while(i<num)
    {
        m=p[i];
        k=(long)m*(long)8;
        fseek(fp1,k*sizeof(UC),SEEK_SET);
        fread(p1,sizeof(UC),8,fp1);
        pch_8(p1,p2);
        for(j1=0;j1<8;j1++)
        {
            pa2[8*i+j1]=p2[j1];
        }
        i=i+1;
    }
    fclose(fp1);
    free(p1);
    free(p2);
    return;
}

/* SBCS display graphic array to printer graphic array transfermation */
void pch_8(UC *pp1, UC *qq1)
{
    int i, j, k;
    UC tt, tt1;
    UC d[8]={128, 64, 32, 16, 8, 4, 2, 1};

    k=0;
    for(i=0; i<8; i++) /* for binary no 0...7 */
    {
        tt1=0;
        for(j=0; j<8; j++) /* for byte no  0...7 */
        {
            tt=d[i];
            if((pp1[j] & tt) != 0)
            {
                tt=d[j];
                tt1=tt1|tt;
            }
        }
        qq1[k]=tt1;
        k=k+1;
    }
    return;
}

/* DBCS display graphic array to printer graphic array transfermation */
void pch_16(UC *pp1, UC *qq1)
{
    int ii, i, j, k, m, n;
    UC tt, tt1;
    UC d[8]={128, 64, 32, 16, 8, 4, 2, 1};

    k=0;
    for(m=0; m<2; m++) /*for upper and below level */
    {
        n=m*16;
        for(ii=0;ii<2;ii++)
        {
            for(i=0;i<8;i++) /* for binary no 0...7 */
            {
                tt1=0;
                for(j=0;j<8;j++) /* for byte no  0...7 */
                {
                    tt=d[i];
                    if((pp1[2*j+ii+n]&tt) != 0)
                    {
                        tt=d[j];
                        tt1=tt1|tt;
                    }
                }
                qq1[k]=tt1;
                k=k+1;
            }
        }
    }
 return;
}

UC pin9(int id, UC *fbuff, UI length)
{
    UC *p1, *p2;
    int i, ik;
    static int id_prn=0;
    static UC p3[200];

    p1=malloc(200*sizeof(UC));
    p2=malloc(200*sizeof(UC));

    ik=0;
    rtn_recon(fbuff, length, p1);
    if(id_prn>0)
    {
       ik=id_prn;
        for(i=0;i<ik;i++)
         {
           p2[i]=p3[i];
         }
    }
    for(i=0; i<length; i++)
    {
      p2[ik]=fbuff[i];
      p3[ik]=p2[ik];
      ik=ik+1;
      id_prn=ik;
      if((p1[i]=='C')||((ik>=MAXCOL)&&(i==length- 1)))
      {
         if(prn_cs(id,p2,ik) ==  FALSE)
         {
           id_prn=0;
           free(p1);
           free(p2);
           return FALSE;
         }
         ik=0;
         id_prn=0;
      }
    }
    free(p1);
    free(p2);
    return TRUE;
}

UC prnf(const UC *fmt, ...)
{
    UC *fbuff;
    va_list argptr;
    UI length;

    fbuff=malloc(200*sizeof(UC));
    va_start(argptr, fmt);
    length = vsprintf(fbuff, fmt, argptr);
    va_end(argptr);

    if (length==(UI)EOF)
    {
        free(fbuff);
        return FALSE;
    }

    if(Sys_mode.prt_m != LQ1600)
    {
        if(pin9(0, fbuff, length) == FALSE)
        {
            free(fbuff);
            return FALSE;
        }
    }
    else if(Sys_mode.prt_m == LQ1600 && Sys_mode.prtflg == INLIB)
    {
        if(prn_fs(fbuff, length) == FALSE)
        {
            free(fbuff);
            return FALSE;
        }
    }
    else
    {
        if(pin24(0, fbuff, length) == FALSE)
        {
            free(fbuff);
            return FALSE;
        }
    }

    free(fbuff);
    return TRUE;
}

UC prnfd(const UC *fmt, ...)
{
    UC *fbuff;
    va_list argptr;
    UI length;

    fbuff=malloc(200*sizeof(UC));
    va_start(argptr, fmt);
    length = vsprintf(fbuff, fmt, argptr);
    va_end(argptr);

    if (length==(UI)EOF)
    {
        free(fbuff);
        return FALSE;
    }

    if(Sys_mode.prt_m != LQ1600)
    {
        if(pin9(1, fbuff, length) == FALSE)
        {
            free(fbuff);
            return FALSE;
        }
    }
    else if(Sys_mode.prt_m == LQ1600 && Sys_mode.prtflg == INLIB)
    {
        if(set_dbl_char() == FALSE) return FALSE;

        if(prn_fs(fbuff, length) == FALSE)
        {
            free(fbuff);
            return FALSE;
        }

        if(rst_nomal_char() == FALSE)
            return(FALSE);
        if(set_clos_tbl() == FALSE)
            return(FALSE);

    }
    else
    {
        if(pin24(1, fbuff, length) == FALSE)
        {
            free(fbuff);
            return FALSE;
        }
    }

    free(fbuff);
    return TRUE;
}

void str_recon(UC *st, UI length, UC *p1, UC *p2_c, int *num1, UC *p2_a)
{
    int i=0, num_c=0, num_a=0;

    while(i<length)
    {
        if(st[i]>127)
        {
            p1[i]='C';
            p2_c[2*num_c]=st[i];
            i=i+1;
            p2_c[2*num_c+1]=st[i];
            p1[i]='C';
            i=i+1;
            num_c+=1;
        }
        else
        {
            p1[i]='A';
            p2_a[num_a]=st[i];
            num_a+=1;
            i+=1;
        }
    }
    *num1=num_c;
    return;
}


UC  prn_cs(int id, UC *st, UI length)
{
    int i, j, j1=0, num_c=0, i1=0, i2=0, num1, id_re=0;
    UC *p1, *p2_c, *p3, *p2_a, *p4, p5=0x00;

    if((p1=malloc(length*sizeof(UC)))==NULL)
        return FALSE;
    if((p2_a=malloc(length*sizeof(UC)))==NULL)
    {
        free(p1);
        return FALSE;
    }
    if((p2_c=malloc(length*sizeof(UC)))==NULL)
    {
        free(p1);
        free(p2_a);
        return FALSE;
    }
    str_recon(st,length, p1, p2_c, &num_c, p2_a);
    if(st[length-1]=='\n')
        id_re=1;

    if(num_c>0)
    {
        if((p3=malloc((num_c*32)*sizeof(UC)))==NULL)
        {
            free(p1);
            free(p2_a);
            free(p2_c);
            return FALSE;
        }
        getlib_char(num_c,p2_c,p3);
    }
    j1=length-2*num_c;
    if(j1>0)
    {
        if((p4=malloc((j1*8)*sizeof(UC)))==NULL)
        {
            free(p1);
            free(p2_a);
            free(p2_c);
            if(num_c>0)
                free(p3);
            return FALSE;
        }
        getlib_asc(j1,p2_a,p4);
    }
    j=0;
    num1=0;
    if(id_re==1)
        length-=1;
    for(i=0;i<length;i++)
        if(p1[i]=='C')
             num1=i+1;

    if(length>0)
    {
        if((prn_ch(27))==FALSE)  //ESC
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if((prn_ch('A'))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if((prn_ch(8))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if((prn_ch(27))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if((prn_ch('*'))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if((prn_ch(1))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if(num1==0)
        {
            if((prn_ch(8))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
            if((prn_ch(0))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
            for (i=0;i<8;i++)
            {
                if((prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
            }
        }
        else
        {
            if(id==1)
            {
                if((prn_ch(num1*16%256))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if((prn_ch(num1/16))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
            }
            else
            {
                if((prn_ch(num1*8%256))==FALSE)
                {
                   buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                   return FALSE;
                }
                if((prn_ch(num1/32))==FALSE)
                {
                   buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                   return FALSE;
                }
            }
        }
    }
    while (j<num1)
    {
        if(p1[j]!='A')
        {
            for (i=0;i<16;i++)
            {
                if((prn_ch(p3[i1*32+i]))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(id==1)
                {
                    if((prn_ch(p3[i1*32+i]))==FALSE)
                    {
                       buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
            }
            i1=i1+1;
            j+=2;
        }
        else
        {
            for (i=0;i<8;i++)
            {
                if((prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(id==1)
                {
                    if((prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
            }
            j+=1;
        }
    }
    i1=0;
    j=0;
    if(length>0)
    {
        if((prn_ch(27))==FALSE)
        {
             buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
             return FALSE;
        }
        if((prn_ch('A'))==FALSE)
        {
             buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
             return FALSE;
        }
        if((prn_ch(8))==FALSE)
        {
             buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
             return FALSE;
        }
        if((prn_ch('\n'))==FALSE)
        {
             buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
             return FALSE;
        }
        if((prn_ch(27))==FALSE)
        {
             buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
             return FALSE;
        }
        if((prn_ch('*'))==FALSE)
        {
             buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
             return FALSE;
        }
        if((prn_ch(1))==FALSE)
        {
             buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
             return FALSE;
        }
        if(id==1)
        {
            if((prn_ch(length*16%256))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
            if((prn_ch(length/16))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
        }
        else
        {
            if((prn_ch(length*8%256))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
            if((prn_ch(length/32))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
        }
    }
    while (j<length)
    {
        if(p1[j]!='A')
        {
            for (i=0;i<16;i++)
            {
                if((prn_ch(p3[i1*32+i+16]))==FALSE)
                {
                     buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                     return FALSE;
                }
                if(id==1)
                {
                    if((prn_ch(p3[i1*32+i+16]))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
            }
            i1+=1;
            j+=2;
        }
        else
        {
            for (i=0;i<8;i++)
            {
                if((prn_ch(p4[i2*8+i]))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(id==1)
                {
                    if((prn_ch(p4[i2*8+i]))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
           }
           i2+=1;
           j+=1;
       }
    }
    if(id_re==1)
        if((prn_ch('\n'))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
    return TRUE ;
}


void rtn_recon(UC *st, UI length, UC *p1)
{
    UI i=0;
    while(i<length)
    {
        if(st[i]=='\n')
        {
            p1[i]='C';
            i+=1;
        }
        else
        {
            p1[i]='A';
            i+=1;
        }
    }
    return;
}

/*Get the lib. file for chinese characters, and change into the print array */
void getlib_24char(int num, UC* p, UC* pc2)
{
    FILE *fp1;
    int m,x,y,j1,i;
    long int k;
    UC *p1;

    p1=malloc(72*sizeof(UC));
    if((fp1=fopen("gfsdata\\cclib24.lib","r+b"))==NULL)
    {
        printf("Error: Cann't open DBCS library file!\n");
        exit(0);
    }
    i=0;
    while(i<num)
    {
        y=p[2*i]-160;
        x=p[2*i+1]-160;
        m=(y-1)*94+(x-1);
        k=(long)m*(long)72;
        fseek(fp1,k*sizeof(UC),SEEK_SET);
        fread(p1,sizeof(UC),72,fp1);
        for(j1=0;j1<72;j1++)
        pc2[72*i+j1]=p1[j1];
        i=i+1;
    }
    fclose(fp1);
    free(p1);
    return;
}


/*Get the lib. file for ASCII characters, and change into the print array */
void getlib_24asc(int num, UC* p, UC* pa2)
{
    FILE *fp1;
    int m,j1,i;
    long int k;
    UC *p1, *p2;

    p1=malloc(16*sizeof(UC));
    p2=malloc(16*sizeof(UC));
    if((fp1=fopen("gfsdata\\asc16.lib","r+b"))==NULL)
    {
        printf("Error: Cann't open SBCS library file!\n");
        exit(0);
    }
    i=0;
    while(i<num)
    {
        m=p[i];
        k=(long)m*(long)16;
        fseek(fp1,k*sizeof(UC),SEEK_SET);
        fread(p1,sizeof(UC),16,fp1);
        pch_24(p1,p2);
        for(j1=0;j1<16;j1++)
            pa2[16*i+j1]=p2[j1];
        i=i+1;
    }
    fclose(fp1);
    free(p1);
    free(p2);
    return;
}


void pch_24(UC *pp1, UC *qq1)
{
   int i,j,k,m,n;
   UC tt,tt1;
   UC d[8]={128,64,32,16,8,4,2,1};

   k=0;
   for(m=0;m<2;m++) /*for upper and below level */
   {
       n=m*8;
       {
           for(i=0;i<8;i++) /* for binary no 0...7 */
           {
               tt1=0;
               for(j=0;j<8;j++) /* for byte no  0...7 */
               {
                  tt=d[i];
                  if((pp1[j+n]&tt) !=(char)0)
                  {
                      tt=d[j];
                      tt1=tt1|tt;
                  }
               }
               qq1[k]=tt1;
               k=k+1;
           }
       }
   }
   return;
}


UC pin24(int id, UC *fbuff, UI length)
{
    UC *p1,*p2;
    int i,ik;

    p1=malloc(200*sizeof(UC));
    p2=malloc(200*sizeof(UC));

    ik=0;
    rtn_recon(fbuff,length,p1);
    for(i=0;i<length;i++)
    {
        p2[ik]=fbuff[i];
        ik=ik+1;
        if((p1[i]=='C')||(i==length- 1))
        {
            if(prn_24cs(id,p2,ik) == FALSE)
            {
               free(p1);
               free(p2);
               return FALSE;
            }
            ik=0;
        }
    }
    free(p1);
    free(p2);
    return TRUE;
}


UC  prn_24cs(int id, UC *st, UI length)
{
    int i, j, j1=0, num_c=0, i1=0, i2=0, id_re=0;
    UC *p1, *p2_c, *p3, *p2_a, *p4, p5=0x00;

    if((p1=malloc(length*sizeof(UC)))==NULL)
        return FALSE;
    if((p2_a=malloc(length*sizeof(UC)))==NULL)
    {
        free(p1);
        return FALSE;
    }
    if((p2_c=malloc(length*sizeof(UC)))==NULL)
    {
        free(p1);
        free(p2_a);
        return FALSE;
    }
    str_recon(st,length,p1,p2_c,&num_c,p2_a);
    if(st[length-1]=='\n')
    {
        id_re=1;
        length-=1;
    }
    if(num_c>0)
    {
        if((p3=malloc((num_c*72)*sizeof(UC)))==NULL)
        {
            free(p1);
            free(p2_a);
            free(p2_c);
            return FALSE;
        }
        getlib_24char(num_c,p2_c,p3);
    }
    j1=length-2*num_c;
    if(j1>0)
    {
        if((p4=malloc((j1*16)*sizeof(UC)))==NULL)
        {
            free(p1);
            free(p2_a);
            free(p2_c);
            if(num_c>0)
                free(p3);
            return FALSE;
        }
        getlib_24asc(j1,p2_a,p4);
    }
    j=0;
    i1=0;
    if(length>0)
    {
        if(( prn_ch(27))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if(( prn_ch('*'))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if(( prn_ch(39))==FALSE) //39: Tri_density,33:Double_density.
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if(id==1)
        {
            if(( prn_ch(length*24%256))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
            if(( prn_ch(length*24/256))==FALSE)
            {
                buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                return FALSE;
            }
        }
        else
        {
           if(( prn_ch(length*12%256))==FALSE)
           {
               buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
               return FALSE;
           }
           if(( prn_ch(length*12/256))==FALSE)
           {
               buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
               return FALSE;
           }
        }
    }
    while (j<length)
    {
        if(p1[j]!='A')
        {
            for (i=0;i<24;i++)
            {
                if(( prn_ch(p3[i1*72+3*i]))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p3[i1*72+3*i+1]))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p3[i1*72+3*i+2]))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(id==1)
                {
                    if(( prn_ch(p3[i1*72+3*i]))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p3[i1*72+3*i+1]))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p3[i1*72+3*i+2]))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
           }
           i1+=1;
           j+=2;
        }
        else
        {
            for (i=0;i<2;i++)
            {
                if(( prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(id==1)
                {
                    if(( prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
            }
            for (i=0;i<8;i++)
            {
                if(( prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p4[i2*16+i]))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p4[i2*16+i+8]))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(id==1)
                {
                    if(( prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p4[i2*16+i]))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p4[i2*16+i+8]))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
            }
            for (i=0;i<2;i++)
            {
                if(( prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(( prn_ch(p5))==FALSE)
                {
                    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                    return FALSE;
                }
                if(id==1)
                {
                    if(( prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                    if(( prn_ch(p5))==FALSE)
                    {
                        buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
                        return FALSE;
                    }
                }
            }
            i2+=1;
            j+=1;
         }
    }
    if(id_re==1)
    {
        if(( prn_ch(27))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if(( prn_ch('A'))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if(( prn_ch(8))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
        if(( prn_ch('\n'))==FALSE)
        {
            buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
            return FALSE;
        }
    }
    buffree(p1, p2_a, p2_c, p3, p4, num_c, j1);
    return TRUE ;
}

void buffree(UC *p1, UC *p2_a, UC *p2_c, UC *p3, UC *p4, int num_c, int j1)
{
    free(p1);
    free(p2_a);
    free(p2_c);
    if(num_c>0)
        free(p3);
    if(j1>0)
        free(p4);
    return;
}
