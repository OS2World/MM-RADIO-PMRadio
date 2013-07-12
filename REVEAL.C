#define INCL_DOSFILEMGR
#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include <os2.h>
#include <stddef.h>
#include <stdio.h>
#include <conio.h>

#define FCODE(f)    ((int)(((float)(f)-88.0)*40)+0xf6c)

extern _Far16 _Pascal OUTP8(USHORT, UCHAR);
extern _Far16 _Pascal OUTP16(USHORT, USHORT);
extern UCHAR _Far16 _Pascal INP8(USHORT);
extern USHORT _Far16 _Pascal INP16(USHORT);

HFILE	hfTimer;
ULONG   ulAction, ulBytes,x;
MPARAM	retVal,f;
unsigned int tune = 0,tmptune,r,scan;
int n;
float fFreq,dftune=98.5;
unsigned char sFreq[30];

void revealo(unsigned char outbyte)
{

    OUTP8( 0x20F ,outbyte );

}

unsigned char reveali()
{
	unsigned char inbyte;

   inbyte= INP8( 0x20F );

   return (inbyte);

}

void r_out(int v,int n)
{
	while (n--) {
	if (v&1) {
        revealo(5);
		revealo(5);
        revealo(7);
		revealo(7);
	}
	else {
        revealo(1);
		revealo(1);
        revealo(3);
		revealo(3);
	}
	v>>=1;
	}
}


int radioon(int ch,int p2)
{
	revealo(0xc8);
	r_out(ch,16);
	r_out(p2,8);
	revealo(0xd8);
    DosSleep(200);

	return(((reveali()>>1)&0x01));
}

/* Function Prototypes */
MRESULT EXPENTRY MainDlgProc ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

#define MAXWIN 100

HWND  SophistosWindow;
ULONG UserWindowID;
HAB   hab;
char  p_st = 0, m_st=0;

int Windows = 0;

int main (void)
{

HMQ hmq;
QMSG qmsg;

/* Create anchor and message queue */
hab = WinInitialize (0);
hmq = WinCreateMsgQueue (hab, 0);



/* Display dialog box */
SophistosWindow = WinLoadDlg
   ( HWND_DESKTOP, HWND_DESKTOP, MainDlgProc, (HMODULE) NULL, 100, NULL );

WinSendMsg(SophistosWindow, WM_SETICON,
	   (MPARAM)WinLoadPointer(HWND_DESKTOP,0,555),NULL);


Windows++;

WinShowWindow ( SophistosWindow, TRUE );

/* Main message dispatch loop. */
do {
   if ( WinGetMsg ( hab, (PQMSG) &qmsg, (HWND) NULL, 0, 0 ) ) WinDispatchMsg ( hab, (PQMSG) &qmsg );
   } while ( Windows );


/* Destroy Queue and anchor block */
WinDestroyMsgQueue ( hmq );
WinTerminate ( hab );

return(0);
}

MRESULT EXPENTRY MainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

switch (msg) {
   case WM_INITDLG : {
	  /* Dialog Initialization done here */
     WinSendDlgItemMsg(hwnd,116,CSM_SETRANGE,MPFROMLONG(0L),MPFROMLONG(199l));

	  return(0);
	  }
   case WM_CLOSE : {
	  /* This is received when the dialog is being closed */

	  WinDismissDlg ( hwnd, TRUE );
	  Windows--;
	  return(0);
	  }
   case WM_CONTROL :
	{
     if ( SHORT1FROMMP(mp1) == 116  && ( SHORT2FROMMP(mp1) == CSN_CHANGED || SHORT2FROMMP(mp1) == CSN_TRACKING)  )
	   {
        tune = 3952+(SHORT1FROMMP(mp2)*4);
		fFreq = (float)tune;
		radioon(tune,0xa0);
		sprintf(sFreq,"%3.1lf Mhz", ((fFreq-0xf6c)/40)+88.0);
		WinSetDlgItemText(hwnd, 104, (PSZ)sFreq);
        sprintf(sFreq,"PM-Radio %3.1lfMhz",((fFreq-0xf6c)/40)+88.0);
		WinSetWindowText(hwnd, (PSZ)sFreq);
	   }
     if ( SHORT1FROMMP(mp1) == 103  && ( SHORT2FROMMP(mp1) == SPBN_UPARROW || SHORT2FROMMP(mp1) == SPBN_DOWNARROW)  )
        {
         WinSendDlgItemMsg(hwnd, 116, CSM_QUERYVALUE,MPFROMP(&retVal), NULL);
         scan = SHORT1FROMMP(retVal);

         if ( SHORT2FROMMP(mp1) == SPBN_DOWNARROW )
          {

         for(n=scan-1;n>=0;n--)
			{
			 r =0;
			   if (!radioon(3952+(n*4),0xa0))
				{
				x = 0;
				r = 1;
                while (x++<10000)
					{
					 if (((reveali()>>1)&0x01)) { r=0; break;}
					}
				if (r)
					{
                    WinSendDlgItemMsg(hwnd, 116, CSM_SETVALUE, MPFROMLONG(n),NULL);
                    r = 3;
					}
				}
			  if ( r==3) break;
			}
          }
      if ( SHORT2FROMMP(mp1) == SPBN_UPARROW )
        {
         for(n=scan+1;n<200;n++)
			{
			 r =0;
			   if (!radioon(3952+(n*4),0xa0))
				{
				x = 0;
				r = 1;
                while (x++<10000)
					{
					 if (((reveali()>>1)&0x01)) { r=0; break;}
					}
				if (r)
					{
                    WinSendDlgItemMsg(hwnd, 116, CSM_SETVALUE, MPFROMLONG(n),NULL);

					r = 3;
					}
				}
			  if ( r==3) break;
			}

        }

        }
     if ( SHORT1FROMMP(mp1) == 102 )
        {
         if ( p_st == 0 )
            {
            tmptune=(int)(FCODE(dftune)-3952)/7.92;
            WinSendDlgItemMsg(hwnd, 116, CSM_SETVALUE,MPFROMLONG(tmptune),NULL);
            p_st = 1;
            }
         else
            {
              r_out(FCODE(98.5),16);
              r_out(0xa0,8);
              revealo(0);
              revealo(0xc0);
              p_st=0;
            }
         WinSendDlgItemMsg(hwnd, 102, BM_SETCHECK, MPFROMSHORT(p_st),NULL);

        }
     if ( SHORT1FROMMP(mp1) == 101 )
        {
         if ( m_st == 0 )
            {
             revealo(0xc0);
             m_st = 1;
            }
         else
            {
             revealo(0xc8);
             m_st=0;
            }
         WinSendDlgItemMsg(hwnd, 101, BM_SETCHECK, MPFROMSHORT(m_st),NULL);
        }
     if ( SHORT1FROMMP(mp1) == 105  && ( SHORT2FROMMP(mp1) == SPBN_UPARROW || SHORT2FROMMP(mp1) == SPBN_DOWNARROW)  )
        {
         if ( SHORT2FROMMP(mp1) == SPBN_UPARROW )
            {
            revealo(0x88);
			DosSleep(40);
			revealo(0xc8);

            }

         if ( SHORT2FROMMP(mp1) == SPBN_DOWNARROW )
            {
            revealo(0x48);
			DosSleep(40);
			revealo(0xc8);
            }
        }

	  return(0);
	 }
   case WM_MINMAXFRAME:
   {
		 //Catch the maximize notification -- don't do anything.
	   if( ((PSWP)mp1)->fl & SWP_MAXIMIZE)
		  return TRUE;
	   else if( ((PSWP)mp1)->fl & SWP_MINIMIZE)
	   {
        WinShowWindow( WinWindowFromID( hwnd, 102), FALSE);
		break;
	   }
	   else if( ((PSWP)mp1)->fl & SWP_RESTORE)
	   {
        WinShowWindow( WinWindowFromID( hwnd, 102), TRUE);
		break;
	   }
	  return(0);
	}
}



/* This processes all messages that we don't handle.  All Windows/Dialogs have default processors */
return ( WinDefDlgProc ( hwnd, msg, mp1, mp2 ) );
}

