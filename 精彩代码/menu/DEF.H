/*定义键值*/
#define Key_DOWN 0x5000      
#define Key_UP 0x4800
#define Key_LEFT 0x4b00
#define Key_RIGHT 0x4d00
#define Key_PAGEUP 0x4900
#define Key_PAGEDOWN 0x5100
#define Key_HOME 0x4700
#define Key_END  0x4f00
#define Key_CTRL_HOME 0x7700
#define Key_CTRL_END  0x7500

#define Key_CTRL_C 0x2e03
#define Key_CTRL_X 0x2d18
#define Key_CTRL_V 0x2f16
#define Key_CTRL_F 0x2106
#define Key_F3     0x3d00
#define Key_DEL 0x5300
#define Key_Backspace 0xe08
#define Key_INS    0x5200

#define Key_ALT_F 0x2100
#define Key_ALT_E 0x1200
#define Key_ALT_H 0x2300
#define Key_Ctrl_S 0x1f13

#define Key_ALT_X 0x2d00
#define Key_ENTER 0x1c0d
#define Key_ESC 0x011b

#define Key_Y   0x1579
#define Key_N   0x316e
#define Key_UY   0x1559
#define Key_UN   0x314e
#define Key_TAB   0xf09

struct MYMENU{
    int left;
    int top;
    int right;
    int bottom;

    char strMenu[10];
    char subMenu[5][20];
    void *submenubuf;
    void *linebuf;

}MyMenu[3];/*存放3个菜单项的内容（全局变量）*/

struct gongbiao
{
    int x;
    int y;

}txtGB={1,1};/*保存文本编辑窗口的光标位置（全局变量）*/

struct select   /*保存所选择文本的光标位置（全局变量），isSelect＝＝1时表有文本被选择*/
{
    int isSelect;
    struct FileLine *pline; /*iFrom指向选择开始文本行*/
    int sCol;
    int sLine;
}txtSelect;

/*当前显示位置，从iStartLine开始显示21行,从iStartCol开始显示78列（全局变量）*/
int iStartLine=0;
int iStartCol=0;
int iTotalLine=0;

char path[256];/*保存文件全路径*/
int bNeedSave=0; /*1时表示文件被修改，需要保存*/
long lFileSize;
int  flag;      /*0为插入，1为改写*/
int  readonly=0;/*1时只读*/


struct FileLine
{
    int    num;
    char *lineNR;
    struct FileLine *pro;
    struct FileLine *next;
};
struct FileLine *head,*current,*top;
char   *txtCopy=0;      /*存放剪贴板的内容*/
char   txtSearch[61]="";
/*初始化菜单变量*/
void initmenu()
{
    
    strcpy(MyMenu[0].strMenu,"File");
    strcpy(MyMenu[0].subMenu[0],"New");
    strcpy(MyMenu[0].subMenu[1],"Open");
    strcpy(MyMenu[0].subMenu[2],"Save ctrl+s");
    strcpy(MyMenu[0].subMenu[3],"Save As");
    strcpy(MyMenu[0].subMenu[4],"Quit alt+x");
    MyMenu[0].left=5;
    MyMenu[0].top=2;
    MyMenu[0].right=MyMenu[0].left+15;
    MyMenu[0].bottom=8;
    MyMenu[0].submenubuf=malloc(8*16*2);
    MyMenu[1].linebuf=malloc(16*2);

    strcpy(MyMenu[1].strMenu  ,"Edit");
    strcpy(MyMenu[1].subMenu[0],"Copy  ctrl+c");
    strcpy(MyMenu[1].subMenu[1],"Cut   ctrl+x");
    strcpy(MyMenu[1].subMenu[2],"Paste ctrl+v");
    strcpy(MyMenu[1].subMenu[3],"Find...");
    strcpy(MyMenu[1].subMenu[4],"Replace...");
    MyMenu[1].left=MyMenu[0].left+strlen(MyMenu[0].strMenu)+5;
    MyMenu[1].top=2;
    MyMenu[1].right=MyMenu[1].left+16;
    MyMenu[1].bottom=8;
    MyMenu[1].submenubuf=malloc(8*17*2);
    MyMenu[1].linebuf=malloc(17*2);


    strcpy(MyMenu[2].strMenu  ,"Help");
    strcpy(MyMenu[2].subMenu[0],"About");
    MyMenu[2].left=MyMenu[1].left+strlen(MyMenu[1].strMenu)+5;
    MyMenu[2].top=2;
    MyMenu[2].right=MyMenu[2].left+10;;
    MyMenu[2].bottom=4;
    MyMenu[2].submenubuf=malloc(4*11*2);
    MyMenu[2].linebuf=malloc(11*2);
}

void DeleteMenuBuf()
{
    int i;
    for(i=0;i<3;i++)
    {
        free(MyMenu[i].submenubuf);
        free(MyMenu[2].linebuf);
    }
}

/*文本模式下画方框 */
void box(int startx,int starty,int width,int high) /* 画矩形框函数*/
{
  int i;
  gotoxy(startx,starty);
  putch(0xda); /* 画┌ */
  for (i=startx+1;i<startx+width-1;i++)
    putch(0xc4);/* 画─（顶边） */
  
   putch(0xbf);/* 画┐ */
 
  for( i=starty+1;i<starty+high-1;i++)
  {
    gotoxy(startx,i);
    putch(0xb3);/* 画│ */
    gotoxy(startx+width-1,i);
    putch(0xb3);/* 画│ */
  }

  gotoxy(startx,starty+high-1);
  putch(0xc0); /* 画┖ */
  for (i=startx+1;i<startx+width-1;i++)/* 画─（底） */
    putch(0xc4);

    putch(0xd9);  /* 画┛ */

  gotoxy (startx,starty);
}

/*光标焦点设置为编辑窗口*/
void setTextWindow()
{
     window(2,3,79,23);
     textbackground(BLUE);
     textcolor(WHITE);
     gotoxy(txtGB.x,txtGB.y);

}

void setStateWindow()
{
	char p[81],state[100],*name,*ins,*temp;
     window(1,25,80,25);
     textbackground(LIGHTGRAY);
     textcolor(RED);
     clrscr();
     if(!strlen(path))
		 strcpy(p,"noname.txt");
	 else
	 {   temp=name=path;
		 while(name=strchr(name,'\\'))
		 {
			 name++;
			 temp=name;
		 }
		strcpy(p,temp);
	 }
	 if(bNeedSave)
		 strcat(p," *");
    if(flag)
	ins="Replace";
	else ins="Insert";

	 sprintf(state,"%s     Totalline:%d     line:%d|col:%d     %s",p,iTotalLine,txtGB.y+iStartLine,txtGB.x+iStartCol,ins);

     gotoxy(2,1);
	 cputs(state);
     setTextWindow();
}


void saveTextWindow()
{
    txtGB.x=wherex();
    txtGB.y=wherey();
}

/*画主菜单*/
void mainfram()
{
int i;
 window(1,1,80,1);
 textbackground(LIGHTGRAY);
 clrscr();
 for(i=0;i<3;i++)
 {
     textcolor(BLACK);
     gotoxy(MyMenu[i].left,1);
     cputs(MyMenu[i].strMenu);
     gotoxy(MyMenu[i].left,1);
     textcolor(RED);
     putch(MyMenu[i].strMenu[0]);
    
 }

 window(1,2,80,25);
 textbackground(BLUE);
 textcolor(LIGHTGRAY);
 clrscr();
 box(1,1,80,23);

 /*设置为未选择状态*/
 txtSelect.isSelect=0;
 txtSelect.sCol=0;
 txtSelect.sLine=0;
 txtSelect.pline=0;

 /*设置状态窗口*/
 setStateWindow();

/*设置当前窗口为文本编辑窗口*/
 setTextWindow();
};
/*画子菜单 */
int TranslateKey
(int key,int shift);
void drawsubmenu(int i)
{
    int j,line,key,num,flag,oldline;
    int noputtext=0;
  
   /*保存要改动菜单区域的数据*/
   gettext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
  
   window(1,1,80,1);
   textbackground(BLACK);
   textcolor(WHITE);
   gotoxy(MyMenu[i].left,1);
   cputs(MyMenu[i].strMenu);
/*画子菜单*/
   window(MyMenu[i].left,MyMenu[i].top,MyMenu[i].right,MyMenu[i].bottom);
   textbackground(LIGHTGRAY);
   
   clrscr();
   num=MyMenu[i].bottom-MyMenu[i].top-1;
   for(j=0;j<num;j++)
   {
       textcolor(BLACK);
       gotoxy(3,j+2);
       cputs(MyMenu[i].subMenu[j]);
       textcolor(RED);
       if((i==0&&j==3)||(i==1&&j==1))
       {
           gotoxy(4,j+2);
          putch(MyMenu[i].subMenu[j][1]);/*"cut u为红色"*/
       }else{
           gotoxy(3,j+2);
           putch(MyMenu[i].subMenu[j][0]);
       }
   }
   textcolor(BLACK);
   box(1,1,MyMenu[i].right-MyMenu[i].left,MyMenu[i].bottom-MyMenu[i].top+1);

 /*显示光标位置*/
    line=oldline=1;
    gettext(MyMenu[i].left,line+2,MyMenu[i].right,line+2,MyMenu[i].linebuf);
    window(MyMenu[i].left+1,line+2,MyMenu[i].right-2,line+2);
    textbackground(BLACK);
    textcolor(WHITE);
    clrscr();
    gotoxy(2,1);
    cputs(MyMenu[i].subMenu[line-1]);
   while(1)
   {
    if(oldline!=line)
    {
        puttext(MyMenu[i].left,oldline+2,MyMenu[i].right,oldline+2,MyMenu[i].linebuf);
        gettext(MyMenu[i].left,line+2,MyMenu[i].right,line+2,MyMenu[i].linebuf);
        window(MyMenu[i].left+1,line+2,MyMenu[i].right-2,line+2);
        textbackground(BLACK);
        textcolor(WHITE);
        clrscr();
        gotoxy(2,1);
        cputs(MyMenu[i].subMenu[line-1]);
        oldline=line;
    }
    j=i;/*当前菜单项位置MyMenu[i]*/
    key=bioskey(0);
    switch(key)
    {
    case Key_ESC: 
        flag=1;
        break;
    case Key_DOWN:
        line+=1;
        if(line>num)
            line=1;
        break;
    case Key_UP:
        line-=1;
        if(line<1)
            line=num;
        break;
    case Key_LEFT:
        j-=1;
        if(j<0)
            j=2;
        flag=2;
        
        break;
    case Key_RIGHT:
        j+=1;
        if(j>2)
            j=0;
        flag=2;
        break;

    case Key_ENTER:
        puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
        if(i==0)/*File menu*/
        {
            if(line==1)
            {
                NewFile();
                noputtext=1;
            }
            else if(line==2)
            {
                OpenFile();
                noputtext=1;
            }
            else if(line==3)
                SaveFile(0);
			else if(line==4)
				SaveFile(1);
            else if(line==5)
		TranslateKey(Key_ALT_X,0);
        }
        else if(i==1)/*Edit menu*/
        {
            switch(line)
            {
            case 1:
		TranslateKey(Key_CTRL_C,0);
                break;
            case 2:
		TranslateKey(Key_CTRL_X,0);
                break;
            case 3:
		TranslateKey(Key_CTRL_V,0);
                break;
            case 4:
		TranslateKey(Key_CTRL_F,0);
                break;
            case 5:
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
                OnReplace();
                noputtext=1;
                break;
            }
            
        }
        else if(i==2)/*Help menu*/
        {
            if(line==1)
                OnAbout();
        }
        flag=1;
        break;
    default:
        key=key&0xff;
        key=toupper(key);
        flag=1;
        switch(key)
        {
        case 'N':
            if(i==0)
            {
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
                NewFile();
                noputtext=1;
			}else continue;
            break;
        case 'O':
            if(i==0)
            {
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
                OpenFile();
				noputtext=1;
            }else continue;
            break;
        case 'S':
            if(i==0)
        	{
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
                SaveFile(0);
        	}else continue;
            break;
        case 'Q':
            if(i==0)
				TranslateKey(Key_ALT_X,0);
			else continue;
            break;
        case 'C':
            if(i==1)
	    {TranslateKey(Key_CTRL_C,0);}
			else continue;
            break;
        case 'X':
            if(i==1)
	    {TranslateKey(Key_CTRL_X,0);}
			else continue;
            break;
        case 'F':  /*查找*/
            if(i==1)
	    {TranslateKey(Key_CTRL_F,0);}
			else continue;
            break;
        case 'R':  /*查找*/
            if(i==1)
            {
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
                OnReplace();
                noputtext=1;
        	}else continue;
            break;
        case 'P':
            if(i==1)
        	{
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
                OnPaste();
        	}else continue;
            break;
        case 'A':
            if(i==2)
        	{
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
                OnAbout();
        	}else if(i==0)
			{
                puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);
				SaveFile(1);

			}else continue;
            break;
        default:
            flag=0;
		}
    }
   if(flag==1||flag==2) break;
   }
   if(!noputtext)
        puttext(MyMenu[i].left,MyMenu[i].top-1,MyMenu[i].right,MyMenu[i].bottom,MyMenu[i].submenubuf);

   if(flag==2)
       drawsubmenu( j);

   /*设置当前窗口为文本编辑窗口*/
    setTextWindow();
};


int TranslateKey(int key,int shift)
{
       int n=0,returnvalue=1;
	switch(key)
	{

	case Key_ALT_X:
	    if(bNeedSave)
		{
                if(MessageBox("Do you want to save the file?",1))
                    SaveFile(0);
        	}
            DeleteMenuBuf();
            FreeMemory();
			if(txtCopy)
				free(txtCopy);
            exit(0);
        case Key_INS:
            flag=!flag;
            setStateWindow();
            break;
        case Key_ALT_F:
            drawsubmenu(0) ;
            break;
        case Key_ALT_E:
            drawsubmenu(1) ;
            break;
        case Key_ALT_H:
            drawsubmenu(2) ;
            break;
        case Key_Ctrl_S:
            SaveFile(0);
            break;
        case Key_UP:
            setSelectState(shift);
            if(current->pro==0)
                break;

               current=current->pro;
               if(txtGB.y==1)
               {
                   ScrollWindow(4);
               }else{
                   txtGB.y-=1;
               }
			   if(!current->lineNR)
				   n=1;
			   else  n=strlen(current->lineNR)+1-iStartCol; 

               if(txtGB.x>n)/*如上一行短，光标到上一行尾*/
               {
                 txtGB.x=n;
                 if(txtGB.x<1)
                 {
                     iStartCol=iStartCol+txtGB.x-1;
                     txtGB.x=1;
                     DrawView();
                 }
               }

            break;
        case Key_DOWN:
            setSelectState(shift);
            if(current->next==0)
                break;

               current=current->next;
               if(txtGB.y==21)
               {
                   ScrollWindow(3);
               } else{
                txtGB.y++;
               }
				if(!current->lineNR)
				   n=1;
				else  n=strlen(current->lineNR)+1-iStartCol;

               if(txtGB.x>n)
               {
                 txtGB.x=n;
                 if(txtGB.x<1)
                 {
                      iStartCol=iStartCol+txtGB.x-1;
                      txtGB.x=1;
                      DrawView();
                 }
               }
            break;
        case Key_LEFT:
            setSelectState(shift);
            if(txtGB.x==1)
            {
                if(iStartCol>0)
                    ScrollWindow(2);
            }else {
                txtGB.x-=1;
                gotoxy(txtGB.x,txtGB.y);
            }
            break;
        case Key_RIGHT:
            setSelectState(shift);
	    if(!current->lineNR||(txtGB.x+iStartCol)>strlen(current->lineNR))
            {
                break;
            }
            if(txtGB.x==78)
                {
                    ScrollWindow(1);
                }else
               {
                   txtGB.x++;
                   gotoxy(txtGB.x,txtGB.y);
               }
            break;
        case Key_END:
            setSelectState(shift);
            OnEnd();
            break;
        case Key_CTRL_END:
            setSelectState(shift);
            OnCtrlEnd();
            break;
        case Key_HOME:
            setSelectState(shift);
            OnHome();
            break;
        case Key_CTRL_HOME:
            setSelectState(shift);
            OnCtrlHome();
            break;
        case Key_PAGEDOWN:
            setSelectState(shift);
           OnPageDown();
            break;
        case Key_PAGEUP:
            setSelectState(shift);
            OnPageUp();
            break;
        case Key_ENTER:
            AppendLine();
            bNeedSave=1;
            break;
        case Key_DEL:
            bNeedSave=1;
        if(txtSelect.isSelect)
                DeleteSelectText();
            else
                OnDeleteCh();
            break;
        case Key_Backspace:
            OnBackspace();
            break;

    case Key_CTRL_C:
         OnCopy();
         break;
    case Key_CTRL_V:
         returnvalue=OnPaste();
         break;
    case Key_CTRL_X:
        if(txtSelect.isSelect)
        {
            OnCopy();
            DeleteSelectText();
        }
         break;
    case Key_CTRL_F:
         OnFind();
         break;
    case Key_F3:
        if(strlen(txtSearch))
         FindNext(txtSearch,1);
         break;
    default:
            key=key&0xff;/*获取输入键的ASCII码*/
            if(!key||key==0x1b)   break;

            if(txtSelect.isSelect)
                TranslateKey(Key_DEL,0);
            OnModify(key,flag);/*flag==0为插入状态*/
        }
        DrawSelectText();
    setStateWindow();
	return returnvalue;
}

/*显示消息，tape==0显示错误消息，tape==1显示y/n消息,tape==2显示Replace message消息*/
int MessageBox(char *msg,int tape)
{
    char *buf;
    int key,n,r=0;
    char *title[]={"Error","Message","Replace message"};
    char yn[]="(y/n)";

    n=strlen(msg)+4;
    if(tape)
        n+=5;
    buf=(char*)malloc((n+1)*3*2);
    gettext((80-n)/2,8,(80+n)/2,10,buf);
    window((80-n)/2,8,(80+n)/2,10);
    textbackground(LIGHTGRAY);
    textcolor(RED);
    clrscr();

    gotoxy(3,2);
    cputs(msg);
    if(1==tape)
        cputs(yn);

    box(1,1,n,3);
    gotoxy(4,1);
    cputs(title[tape]);

   while(1)
    {
        key=bioskey(0);
        if(tape==0||tape==2)
        {
        if(key==Key_ESC)
            break;
        }else{
            if(key==Key_Y||key==Key_UY)
            {r= 1;  break;}

            if(key==Key_N||key==Key_UN)
            {break;}
    }
    }
    puttext((80-n)/2,8,(80+n)/2,10,buf);
    setTextWindow();
    return r;
}


/*以下定义文件各项操作*/

struct FileLine * NewLine()
{
    struct FileLine *temp;
    if(temp=(struct FileLine * )malloc(sizeof(struct FileLine)))
    {
        temp->lineNR=NULL;
        temp->pro=NULL;
        temp->next=NULL;
		temp->num=0;
    }
    return temp;
}

int AppendLine()
{
    int restnum=0,col=0,i;
    struct FileLine *temp;

    if((temp=NewLine())==0)
    {
        MessageBox("short of memory!",0);
        return 0;
    }
    iTotalLine++;      /**/
    temp->pro=current; /*新建行插入到当前行之后*/
    if(current->next)
		current->next->pro=temp;
    temp->next=current->next;
    current->next=temp;

    col=iStartCol+txtGB.x;
    if(current->lineNR&&strlen(current->lineNR)>col-1)/*如果插入点后有文本，移到下一行*/
    {
      restnum=strlen(current->lineNR)-col+1;
        if(restnum>temp->num-1)
        {
            free(temp->lineNR);
            temp->lineNR=(char*)malloc(restnum+1);
            if(!temp->lineNR)
            {
                free(temp);
                MessageBox("short of memory!",0);
                return 0;
            }
            temp->num=restnum+1;
            memset(temp->lineNR,0,restnum+1);
        }
        strcpy(temp->lineNR,current->lineNR+col-1);
        memset(current->lineNR+col-1,0,restnum);
    }
    current=temp;  /*point to the new line*/
    txtGB.x=1;     /*设置光标位置*/
    iStartCol=0;

    if(txtGB.y<21)
    {
      txtGB.y++;
    }else{
    
        top=top->next;
        iStartLine++;
    }
    DrawView();
	return 1;
}

int OnModify(char ch,int flag) /*flag ==0 为插入，flag==1为改写状态*/
{
    char *tempstr;
    int col,n2,i;
    bNeedSave=1;

    col=txtGB.x+iStartCol;
	if(!current->lineNR)
	{
		current->lineNR=malloc(10);
		if(!current->lineNR)
		{
			MessageBox("short of memory!",0);
			return 0;
		}
		memset(current->lineNR,0,10);
		current->num=10;
		n2=0;
	}else{
		n2=strlen(current->lineNR);}

    switch(flag)
    {
    case 0:
        if(n2==current->num-1)/*空间不足*/
            {
                current->num+=10;
                if((tempstr=(char*)malloc(current->num))==0)
                {
                    MessageBox("Short of memory!",0);
                    return 0;
                }

                memset(tempstr,0,current->num);
                strcpy(tempstr,current->lineNR);
                free(current->lineNR);
                current->lineNR=tempstr;
            }
            
        if(n2==col-1)/*行末插入字符*/
        {
                current->lineNR[n2]=ch;
                current->lineNR[n2+1]='\0';
            }else{   /*行中插入字符*/
                i=n2;
                while(i>col-1)
                {
					current->lineNR[i]=current->lineNR[i-1];
					i--;
                }
                current->lineNR[i]=ch;
            }
            break;

    case 1:

        if(n2==col-1)/*光标位于行末尾*/
            {
                if(n2==current->num-1)/*空间不足*/
                    {
                        current->num+=10;
                        /*tempstr=(char*)malloc(current->num);*/
                        if((tempstr=(char*)malloc(current->num))==0)
                        {
                            MessageBox("Short of memory!",0);
                            return -1;
                        }

                        memset(tempstr,0,current->num);
                        strcpy(tempstr,current->lineNR);
                        free(current->lineNR);
                        current->lineNR=tempstr;
                    }
                current->lineNR[n2]=ch;
                current->lineNR[n2+1]='\0';

            }else{   /*行中插入字符*/
                current->lineNR[col-1]=ch;
            }
            break;
    }
    if(txtGB.x==78)
    {
        iStartCol++;
        DrawView();
    }else{
        txtGB.x++;
        ClearCurrentLine();
        DrawCurrentLine();
    }
    setStateWindow();
    return 1;
}

int OnDeleteCh()
{
    int n,col=txtGB.x+iStartCol;
    char *p;
    struct FileLine *temp;

    if(current->lineNR&&strlen(current->lineNR)>col-1)  /*光标不处于行尾时*/
    {
       	strcpy(current->lineNR+col-1,current->lineNR+col);

    }else{
        if(current->next)                /*光标处于行尾时，合并下行到本行*/
        {
			if(current->next->lineNR&&strlen(current->next->lineNR)) 
			{
	    		if(current->lineNR)
					n=strlen(current->lineNR);
	   			else n=0;
	 
	   			n+=strlen(current->next->lineNR);
	            
           		if(!(p=(char*)malloc(n+1)))
              		{
						MessageBox("Short of memory!",0);
						return -1;
             		}
			
				if(current->lineNR)
          			strcpy(p,current->lineNR);
           		strcat(p,current->next->lineNR);
				p[n]='\0';

          		free(current->lineNR);
          		current->lineNR=p;
			}

			temp=current->next;/*将下行从链表中删除，并释放内存*/
			current->next=temp->next;
			temp->next->pro=current;
			free(temp->lineNR);
			free(temp);
			iTotalLine--;
        }
    }
    DrawView();
}

int OnBackspace()
{
    if(txtGB.x==1&&txtGB.y==1&&current==top)/*光标位于文件头退出*/
      if(iStartCol==0)    return 0;
      else
      {
    ScrollWindow(2);
    txtGB.x++;
    }
    
    if(txtGB.x>1)
    {
        txtGB.x--;
    }else{             /*txtGB.x==1*/
    if(iStartCol>0)
            ScrollWindow(2);
        else{        /*iStartCol==0,*/
                TranslateKey(Key_UP,0);
                OnEnd();
            }
    }
    OnDeleteCh();
}

int OnEnd()
{
    int n=0;
    if(current->lineNR)
	n=strlen(current->lineNR);
    if(iStartCol+78>n)
    {
        txtGB.x=n-iStartCol+1;
        gotoxy(txtGB.x,txtGB.y);
    }else{
    txtGB.x=78;
        iStartCol=n-77;
        DrawView();
    }
}

int OnHome()
{
    txtGB.x=1;
    iStartCol=0;
    DrawView();
    return 0;
}

int OnCtrlHome()
{
    txtGB.x=1;
    txtGB.y=1;
    current=top=head;
    iStartLine=0;
    DrawView();
    return 0;
}

int OnCtrlEnd()
{
    int i=0;
    while(current->next)
    {
        current=current->next;
        if(txtGB.y<21)
            txtGB.y++;
    }

    if(iTotalLine<iStartLine+21) /*当前页已显示文件结尾*/
    {
        OnEnd();
        DrawView();
        return 0;
    }

    top=current;
    while(top->pro&&i<20)/*一页21行*/
    {
        top=top->pro;
        i++;
    }
    iStartLine=iTotalLine-21;
    OnEnd();
    DrawView();
    return 0;
}

int OnPageDown()
{
    int i=0;
    if(iTotalLine<iStartLine+21) /*当前页已显示文件结尾*/
        return 0;

    for(i=0;i<20;i++)
    {
        TranslateKey(Key_DOWN,bioskey(2));
    }
}
int OnPageUp()
{
    int i=0;
    if(0==iStartLine) /*当前页已显示文件头*/
        return 0;
    for(i=0;i<20&&iStartLine>0;i++)
    {
        TranslateKey(Key_UP,bioskey(2));
    }
    return 0;
}

int DrawView()
{
    struct FileLine *oldcur;
    int i,x,y;

    window(2,3,79,23);
    textbackground(BLUE);
    clrscr();
    textcolor(WHITE);

    oldcur=current;
    current=top;
    y=txtGB.y;
    x=txtGB.x;
   for(i=0;i<21;i++)
    {
        txtGB.y=i+1;
        DrawCurrentLine();
        if(0==current->next)
            break;
        current=current->next;
    }
    current=oldcur;
    txtGB.y=y;
    txtGB.x=x;
    gotoxy(txtGB.x,txtGB.y);
}

int DrawCurrentLine()
{
    int i,n;
    char temp[80];
    memset(temp,0,80);

    gotoxy(1,txtGB.y);
    if(!current->lineNR)
		return 0;

    n=strlen(current->lineNR);
    if(n>(78+iStartCol))
    {
        for(i=0;i<78;i++)
            temp[i]=current->lineNR[iStartCol+i];
        cputs(temp);
        return 0;
    }
    if(n>iStartCol)        /*n<=iStarCol时，本行显示为空*/
         cputs(current->lineNR+iStartCol);
	return 1;
}

int ClearCurrentLine()
{
    window(2,txtGB.y+2,79,txtGB.y+2);
    textbackground(BLUE);
    clrscr();
    setTextWindow();
}

/*direction=1，2，3，4分别表示屏幕向左，右，上，下滚动一行)*/
/*txtwindow(2,3,79,23)*/
int ScrollWindow(int direction)
{
    int i;
    switch(direction)
    {
    case 1:
        iStartCol++;
    break;
    case 2:
        iStartCol--;
        break;
    case 3:
        iStartLine++;
        top=top->next;
        break;
    case 4:
        iStartLine--;
        top=top->pro;
        break;
    }
    DrawView();
}

long getfilesize(FILE *file)
{
    long size=0;
    rewind(file);
    while(fgetc(file)!=EOF) size++; 
    rewind(file);
    return size;
}

int NewFile()/*初始化新文件*/
{
    if(bNeedSave)
    {
        if(MessageBox("Do you want to save the file?",1))
            SaveFile(0);
    }

    if(!top)
    FreeMemory();

    strcpy(path,"");
    lFileSize=0;
    iTotalLine=1;
    iStartLine=0;
    iStartCol=0;
    top=current=head=NewLine();
    txtGB.x=1;
    txtGB.y=1;
    DrawView();
}
/*取文件当前行的长度（含'\n'）*/
int getfilelinelength(FILE *file)
{

  char ch;
  int i=0;
  fpos_t pos;
  fgetpos(file,&pos);
  while((ch=fgetc(file))!='\n'&&ch!=EOF)
  {
     i++;
  }
  fsetpos(file,&pos);
  if(i==0&&ch==EOF)
      return -1;
  if(ch=='\n')
	  i++;

  return i;
}
int OpenTheFile(char *name)
{
    FILE *file;
    char *tempfilebuf,ch;
    int  i,first=1,linelength; 
    struct FileLine *t,*pro;
    file=fopen(name,"r");
    if(!file)
    {
        return -1;
     }
    strcpy(path,name);/*把文件路径保存起来*/
    lFileSize=getfilesize(file);
    FreeMemory();
	if(!lFileSize)
	{
		NewFile();
		strcpy(path,name);/*把文件路径保存起来*/
		return 1;
	}
    iStartLine=0;
    iStartCol=0;
    iTotalLine=0;
    
    while(-1!=(linelength=getfilelinelength(file)))
    {
        t=NewLine();
        if(!t)
        {
            fclose(file);
            return 0;
        }

	    t->lineNR=(char *)malloc(linelength+1);
	    if(!t->lineNR)
	    {
		free(t);
		fclose(file);
		return 0;
	    }
	    t->num=linelength+1;
	    fgets(t->lineNR,linelength+1,file);
	    while(t->lineNR[strlen(t->lineNR)-1]=='\n')
	    {
		t->lineNR[strlen(t->lineNR)-1]='\0';
	    }


        if(first)/*附加到链表*/
        {
            pro=current=top=head=t;
            iStartLine=0;
            first=0;
        }else{         
            t->pro=pro;
            pro->next=t;
        }
        iTotalLine++;
        
        pro=t;
  }
    fclose(file);
    return 1;
}

int FreeMemory()
{
    struct FileLine *p,*temp;
    p=head;
    while(p)
    {
    temp=p->next;
        free(p->lineNR);
        free(p);
        p=temp;
    }
}

/*消息输入对话框
msg:返回输入内容，需用户预先分配
num: msg的大小
title：对话框标题
dis:对话框中初始显示内容*/
int InputMessageBox(char *msg,int num,char *title)
{
     char buf[41*3*2];
     int i,n;
     char ch;

    gettext(10,5,50,7,buf);
    window(10,5,50,7);
    textbackground(LIGHTGRAY);
    clrscr();
    box(1,1,40,3);
    gotoxy(5,1);
    cputs(title);
    gotoxy(2,2);
    window(11,6,48,6);
    textbackground(LIGHTGRAY);
    clrscr();
	textcolor(BLACK);

    cputs(msg);
    n=strlen(msg);
    
    while((ch=getch())!=13)
    {
        if(27==ch)
        {
        puttext(10,5,50,7,buf);
            return 0;
        }        
        if(ch!=8&&ch<32)
            continue;

        if(n==num-1&&ch!=8)
        {continue;}

        if(ch==8)
        {
            n--;
            if(n<0)
                n=0;
            msg[n]='\0';
        }else{
        msg[n++]=ch;
		msg[n]='\0';
    }
         clrscr();
         gotoxy(1,1);
		 i=n<38?0:n-37;
         cputs(msg+i);
        
    }
    puttext(10,5,50,7,buf);
    return 1;
}
/*取当前路径*/
char *current_directory(char *path)
{
   strcpy(path, "X:\\");      /* fill string with form of response: X:\ */
   path[0] = 'A' + getdisk();    /* replace X with current drive letter */
   getcurdir(0, path+3);  /* fill rest of string with current directory */
   return(path);
}

int OpenFile()/*打开文件*/
{
     char name[MAXPATH+1];/*MAXPATH在dir.h中定义为80*/
     int flag;
      memset(name, 0,MAXPATH+1); 
      if(!strlen(path))
        {
			current_directory(name);
			strcat(name,"\\");
			flag= InputMessageBox(name,MAXPATH,"Input filename");
		}else {
			strcpy(name,path);
			if( !(flag=InputMessageBox(name,MAXPATH,"Input filename")))
				return 0;
		}
    if(!strcmp(name,path)) 
	{
		MessageBox("File already is being edited.",0);
        return -1;/*文件已打开*/
	}

    if(bNeedSave)
    {
        if(MessageBox("Do you want to save the file?",1))
            SaveFile(0);
    }
    if((flag=OpenTheFile(name))==-1)
        return -1;
    
    if(0==flag)
    {
         MessageBox("No enough memory,so only part of file are read.",0);
         readonly=1;
    }else if(-1==flag)
    {
        MessageBox("Sorry,can't open the file!",0);
        return 0;
    }
    txtGB.x=1;
    txtGB.y=1;

    DrawView();
    setTextWindow();
}

int SaveFile(int SaveAs)/*保存文件*/
{
    FILE *file;
    struct FileLine *tp;
    char savename[MAXPATH+1];
    memset(savename,0,MAXPATH+1);

    if(!strlen(path)||SaveAs)  /*新建文件，未保存||另存为*/
    {
		if(SaveAs)
		{
			strcpy(savename,path);
		}else{
			current_directory(savename);
			if(savename[strlen(savename)]!='\\')
				strcat(savename,"\\");
			strcat(savename,"noname.txt");
		}
		if(0==InputMessageBox(savename,MAXPATH,"Input save as filename"))
			return 0;
		strcpy(path,savename);
	}

    file=fopen(path,"w");
    if(!file)
    {
    MessageBox("Cann't cteate file!",0);
    }else{
		tp=top;
		while(tp)
		{
			fputs(tp->lineNR,file);
			fputs("\n",file);
			tp=tp->next;
		}
    }
    bNeedSave=0;
    fclose(file);
}

int DrawSelectLine(struct FileLine *theline,int col1,int col2,int y)
{
    char lc[79];
    memset(lc,0,79);
    if(col1<iStartCol)
    col1=iStartCol;
    if(col2>iStartCol+78)
    col2=iStartCol+78;
    if(!theline->lineNR)
		return 0;

    memcpy(lc,theline->lineNR+col1,col2-col1+1);
    gotoxy(col1-iStartCol+1,y);
    cputs(lc);
}
/*txtSelect.sCol从1(txtGB.x+iStartCol)开始*/
int DrawSelectText()
{
    int cCol,sLine,x1,x2,y,i,n;
    int first_isSeen=1,last_isSeen=1;
    char str[79];
    struct FileLine *start_temp;

    if(!txtSelect.isSelect)
    return 0;

    DrawView();
    textbackground(WHITE);
    textcolor(BLACK);
    n=abs(txtSelect.sLine-txtGB.y-iStartLine)+1;  /*行数*/

    if(1==n)/*只一行内部分被选*/
    {

       if(txtGB.x+iStartCol-1<txtSelect.sCol)
        DrawSelectLine(current,txtGB.x+iStartCol-1,txtSelect.sCol,txtGB.y);
    else DrawSelectLine(current,txtSelect.sCol,txtGB.x+iStartCol-1,txtGB.y);
    return 0;
    }
    /*有多行被选时*/
    if((txtGB.y+iStartLine)>txtSelect.sLine)  /*↑下选*/
    {
    sLine=txtSelect.sLine-iStartLine;
    start_temp=txtSelect.pline;
    while(sLine<1)
    {
        start_temp=start_temp->next;
        sLine++;                       /*纵坐标加一*/
        first_isSeen=0;
    }

    if(n>txtGB.y) n=txtGB.y;
    }else{
    sLine=txtGB.y;
    start_temp=current;
    if(txtSelect.sLine>iStartLine+21)
    {
        last_isSeen=0;
    }
    if(n>21-txtGB.y+1) n=21-txtGB.y+1;
   }
    for(i=0;i<n;i++)
    {
    if(i==0&&first_isSeen)
    {
        if(txtSelect.sLine<txtGB.y+iStartLine)/*↓下选*/
         x1=txtSelect.sCol;
        else x1=txtGB.x+iStartCol-1;              /*↑上选*/
        x2=(strlen(start_temp->lineNR)>iStartCol+78)?iStartCol+78:(strlen(start_temp->lineNR)-iStartCol);
        DrawSelectLine(start_temp,x1,x2,sLine+i);
    }
    else if(i==n-1&&last_isSeen)
    {
        x1=iStartCol;
        if(txtSelect.sLine<txtGB.y+iStartLine)  /*↓下选*/
        x2=txtGB.x+iStartCol-1;
        else x2=txtSelect.sCol;
        DrawSelectLine(start_temp,x1,x2,sLine+i);
    }else{
        x1=iStartCol;
        x2=iStartCol+78;
        DrawSelectLine(start_temp,x1,x2,sLine+i);
    }
    start_temp=start_temp->next;
    }
    gotoxy(txtGB.x,txtGB.y);
    textbackground(BLUE);
    textcolor(WHITE);
    return 1;
}
/*设置为选择状态*/
int setSelectState(int shiftkey)
{
    int k=0;
    k=shiftkey&0x03;
    if(0==k)
    {
    if(txtSelect.isSelect==1)
    DrawView();

    txtSelect.isSelect=0;
    return 0;
    }
    if(1==txtSelect.isSelect)
    return 0;

    txtSelect.isSelect=1;
    txtSelect.sLine=txtGB.y+iStartLine;
    txtSelect.sCol=txtGB.x+iStartCol-1;
    txtSelect.pline=current;
    return 0;
}

int deleteline(struct FileLine *bd)
{
    bd->pro->next=bd->next;
    bd->next->pro=bd->pro;
    if(bd->num)
    free(bd->lineNR);
    free(bd);
    iTotalLine--;


}
int swap(int *n1,int *n2)
{
    int temp=*n1;
    *n1=*n2;
    *n2=temp;
}
int DeleteSelectText()/*删除所选择的内容*/
{
    struct FileLine *temp,*delend,*delfirst,*bd;
    char *linenr;
    int ny=0,nx,delnum,tempn;
    ny=abs(txtGB.y+iStartLine-txtSelect.sLine)+1;/*行数*/
    delnum=abs(ny)-2;
    if(1==ny)                        /*一行内选择*/
    {
        if(txtGB.x+iStartCol-1==txtSelect.sCol)
        {
          OnDeleteCh();
          txtSelect.isSelect=0;
          return 0;
        }else if((txtGB.x+iStartCol)>txtSelect.sCol)
        {
            strcpy(current->lineNR+txtSelect.sCol,current->lineNR+txtGB.x+iStartCol);
            txtGB.x=txtSelect.sCol+1;
        }
        else
        {
            strcpy(current->lineNR+txtGB.x+iStartCol-1,current->lineNR+txtSelect.sCol);
            txtGB.x=txtGB.x+iStartCol;
        }

        txtSelect.isSelect=0;
        DrawView();
        return 0;
    }
    if(txtGB.y+iStartLine<txtSelect.sLine)    /*↑转为↓第一行删除后半，最后一行删除前半，中间各行删除*/
    {
        tempn=txtGB.y+iStartLine;
        swap(&tempn,&txtSelect.sLine);
        txtGB.y=tempn-iStartLine;

        tempn=txtGB.x+iStartCol-1;
        swap(&tempn,&txtSelect.sCol);
        txtGB.x=tempn-iStartCol+1;

        temp=txtSelect.pline;
        txtSelect.pline=current;
        current=temp;
    }

    delend=current;

    delfirst=temp=txtSelect.pline;

    memset(delfirst->lineNR+txtSelect.sCol,0,strlen(delfirst->lineNR)-txtSelect.sCol+1);
    temp=temp->next;
    while(temp!=delend)/*2~n-1*/
        {
            bd=temp;
            temp=temp->next;
            deleteline(bd);
        }
    if(delfirst->num<(strlen(delfirst->lineNR)+strlen(temp->lineNR+txtGB.x+iStartCol)))/*合并1和n行*/
    {
        linenr=(char*)malloc(strlen(delfirst->lineNR)+strlen(temp->lineNR+txtGB.x+iStartCol)+1);
    strcpy(linenr,delfirst->lineNR);
    free(delfirst->lineNR);
    delfirst->lineNR=linenr;
     }
     strcat(delfirst->lineNR,temp->lineNR+txtGB.x+iStartCol);
     deleteline(temp);


    while(delnum>0&&top->pro)
    {
        top=top->pro;
        iStartLine--;
        delnum--;
     }
    temp=head; /*重新计算 iStartLine*/
    iStartLine=0;
    while(temp!=top)
    {
          iStartLine++;
          temp=temp->next;
    }

    temp=top;/*重新计算 txtGB.y*/
    txtGB.x=txtSelect.sCol-iStartCol+1;
    txtGB.y=1;
    current=txtSelect.pline;
    while(temp!=current)
    {
        txtGB.y++;
        temp=temp->next;
    }

   txtSelect.isSelect=0;
   DrawView();
}
int OnCopy()
{
    struct FileLine *start,*end,*temp;
    int scol,ecol,num;
	if(!txtSelect.isSelect)
	{
		MessageBox("No text be selected.",0);
		return 0;
	}

    if(txtCopy)
    {
        free(txtCopy);
        txtCopy=0;
    }

    if(current==txtSelect.pline)
    {
		if(!current->lineNR)
			return 0;
		if(txtGB.x+iStartCol-1>txtSelect.sCol)
		{
			scol=txtSelect.sCol;
			ecol=txtGB.x+iStartCol-1;
		}else{
			ecol=txtSelect.sCol;
			scol=txtGB.x+iStartCol-1;
			}
		txtCopy=malloc(ecol-scol+2);
		memset(txtCopy,0,ecol-scol+2);
		memcpy(txtCopy,current->lineNR+scol,ecol-scol+1);
		return 0;
    }
    if(txtGB.y+iStartLine>txtSelect.sLine)
    {
		start=txtSelect.pline;
		scol=txtSelect.sCol;
		ecol=txtGB.x+iStartCol-1;
		end=current;
    }else{
		start=current;
		end=txtSelect.pline;
		scol=txtGB.x+iStartCol-1;
		ecol=txtSelect.sCol;
    }
    /*计算所需内存*/
    temp=start;
	if(!temp->lineNR)
		num=1;
	else
	    num=strlen(temp->lineNR+scol)+1;/*符加'\n'1个字节*/

    temp=temp->next;
    while(temp!=end)
    {
	if(!temp->lineNR)
		num+=1;
	else num+=strlen(temp->lineNR)+1;

	temp=temp->next;
    }
    num+=(ecol+1)+1;/*最后一行不含'\n'，含'\0'*/
    if(NULL==(txtCopy=(char*)malloc(num)))
    {
        MessageBox("No memory!",0); /*no memory*/
        return 0;
    }

    /*memset(txtCopy,0,num);*/
    if(start->lineNR)
    strcpy(txtCopy,start->lineNR+scol);
    else strcpy(txtCopy,"");

    strcat(txtCopy,"\n");
    start=start->next;
    while(start!=end)
    {
		if(start->lineNR)
			strcat(txtCopy,start->lineNR);
		strcat(txtCopy,"\n");
		start=start->next;
    }
	if(start->lineNR)
	  memcpy(txtCopy+strlen(txtCopy),start->lineNR,ecol+1);

	txtCopy[num-1]='\0';

    return 0;
}
int OnPaste()
{
    int n,num;
    if(!txtCopy)
    return 0;
	txtSelect.isSelect=0;
    num=strlen(txtCopy);
    for(n=0;n<num;n++)
    {
    if(txtCopy[n]=='\n')
       TranslateKey(Key_ENTER,0);
    else
    if(!OnModify(txtCopy[n],0))
        return 0;
    }
    DrawView();
    return 0;
}
/*return NULL未找到，找到返回指针位置*/
char * FindInLine(char *sourse,char *dsd)
{
    char *p=strchr(sourse,dsd[0]);
    int i;
    if(p&&p<sourse+strlen(sourse)-strlen(dsd)+1)
    {
        if(strncmpi(p,dsd,strlen(dsd)))
        {
            FindInLine(p+1,dsd);
        }else return (p);
    }
    return NULL;
}
/*NULL未找到，1找到*/
int FindNext(char *dsd,int flag)
{
    char *p,*gbp;
    int i,line,col;
    struct FileLine *temp;


    /**/
    line=iStartLine+txtGB.y;
    temp=current;
    gbp=current->lineNR+iStartCol+txtGB.x-1;
    while(NULL==(p=FindInLine(gbp,dsd)))
    {
        if(current->next)
        {
            current=current->next;
            gbp=current->lineNR;
            line++;
        }else
        {
            if(flag)
                MessageBox("Can't find the string,",0);

            current=temp;
            return NULL;
        }
    }
    txtSelect.isSelect=1;
    txtSelect.sCol=p-current->lineNR;
    txtSelect.sLine=line;
    txtSelect.pline=current;

    
    for(i=line;i-iStartLine>21;)
    {
        /*向上滚动*/
        iStartLine++;
        top=top->next;
        
    }
    for(i=txtSelect.sCol+strlen(dsd)-iStartCol;i<0;)
    {
        iStartCol--;
    }
    for(i=txtSelect.sCol+strlen(dsd)-iStartCol;i>78;)
    {
         iStartCol++;
    }
    txtGB.y=txtSelect.sLine-iStartLine;
    txtGB.x=txtSelect.sCol-iStartCol+strlen(dsd);

    DrawSelectText();
    return 1;
}
int OnFind()
{
    char search[61];

    /*用户输入查找内容*/
    memset(search,0,61);
    if(!InputMessageBox(search,60,"search"))
    {
        return NULL;
    }
    strcpy(txtSearch,search);
    FindNext(search,1);
}
int GetReplaceTxt(char *fp,char *rp,int num)
{
    /*画窗口*/
    char buf[41*8*2],ch;
    char *s="Find    what:";/*13n*/
    char *r="Replace with:";
    char *ent="* Enter to replace all the rest *";
    char *can="* Esc to canel -----------------*";
    int  *windowflag=0,n1,n2,i;/*windowflag==0为find what*/
    gettext(20,7,60,14,buf);
    window(20,7,60,14);
    textbackground(LIGHTGRAY);
    clrscr();
    textcolor(BLACK);
    gotoxy(2,2);
    cputs(s);
    gotoxy(2,5);
    cputs(r);
    gotoxy(2,7);
    cputs(ent);
    gotoxy(2,8);
    cputs(can);

    box(16,1,25,3);/*find what input window*/
    box(16,4,25,3);/*replace with input window*/
    window(37,11,58,11);
    textbackground(LIGHTGRAY);
    textcolor(BLACK);
    clrscr();
    n2=strlen(rp);
    gotoxy(1,1);
    cputs(rp);

    window(37,8,58,8);
    textbackground(LIGHTGRAY);
    textcolor(BLACK);
    clrscr();
    n1=strlen(fp);
    gotoxy(1,1);
    cputs(fp);

    while((ch=getch())!=13)
    {
        if(27==ch) /*ESC*/
        {puttext(20,7,60,14,buf); return 0;}        
        if(9==ch) /*tab*/
        {
            windowflag=!windowflag;
            if(!windowflag)
              window(37,8,58,8);
            else
          window(37,11,58,11);
            continue;
        }
        
        if(ch!=8&&ch<32)
            continue;

        if(!windowflag)
        {
            if(n1==num-1&&ch!=8)/*8 is backspace*/
            { continue; }

            if(ch==8)
            {
                n1--;
                if(n1<0)
                    n1=0;
                fp[n1]='\0';
            }else{
                fp[n1++]=ch;
            }
            clrscr();
            gotoxy(1,1);
            i=n1<22?0:n1-21;
            cputs(fp+i);
        }else{
            if(n2==num-1&&ch!=8)/*8 is backspace*/
                {continue;}

            if(ch==8)
            {
                n2--;
                if(n2<0)
                    n2=0;
                rp[n2]='\0';
            }else{
                rp[n2++]=ch;
            }
            clrscr();
            gotoxy(1,1);
            i=n2<22?0:n2-21;
            cputs(rp+i);
        }
    }
    puttext(20,7,60,14,buf);
    return 1;
}
int OnReplace()
{
    char fp[61]="",rp[61]="";
    char *oldcopytxt=NULL,meg[30];
    int i=0;

    if(0==GetReplaceTxt(fp,rp,60))
        return 0;
    if(txtCopy)
    {
        oldcopytxt=(char*)malloc(strlen(txtCopy)+1);
		if(!oldcopytxt)
		{
			MessageBox("No memory!",0);
			return 0;
		}
        strcpy(oldcopytxt,txtCopy);
        free(txtCopy);
    }
    txtCopy=(char*)malloc(strlen(rp)+1);
 	if(!txtCopy)
	{
		MessageBox("No memory!",0);
		return 0;
	}
    strcpy(txtCopy,rp);

    while(FindNext(fp,0))
    {
		if(coreleft()<strlen(txtCopy)+1)
		{
			MessageBox("No memory!",0);
			return 0;
		}
		
        TranslateKey(Key_DEL,0);
        TranslateKey(Key_CTRL_V,0);
        i++;
    }
    free(txtCopy);
    txtCopy=NULL;
    if(oldcopytxt)
    {
        txtCopy=(char*)malloc(strlen(oldcopytxt)+1);
		if(txtCopy)
		{
		    strcpy(txtCopy,oldcopytxt);
		}
    }
    if(i!=0)
    {
        sprintf(meg,"Total replaced %d times.",i);
        MessageBox(meg,2);
    }else{
            MessageBox("Can't find this string",0);
    }
    return 1;
}
int OnAbout()
{
    char buf[41*6*2];
    int key;
    gettext(20,8,60,13,buf);
    window(22,9,60,13);
    textbackground(BLACK);
    clrscr();

    window(20,8,58,12);
    textbackground(LIGHTGRAY);
    clrscr();
    textcolor(BLACK);
    gotoxy(4,2);
    cputs("MYEDIT  version 1.0");
    gotoxy(4,3);
    cputs("Copyright@ 2005.11 dingjianzhong");
    gotoxy(4,5);
    textcolor(BLUE);
    cputs("mailto:djzbj@163.com");
    while(1)
    {
    key=bioskey(0);
    if(key==Key_ESC)
    break;
    }
    puttext(20,8,60,13,buf);
    setTextWindow();
    return 0;
}
