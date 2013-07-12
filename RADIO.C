#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <sys/hw.h>

#include <ctype.h>
#include <string.h>


/*
  frequency is defined in steps of 25 kHz
  but what is the meaning of the offset 0xf6c?
*/
#define FCODE(f)	((int)(((float)(f)-88.0)*40)+0xf6c)

/*
  put here the hardware base address of the card (0x20C or 0x30C)
  locations xxC thru xxE are never accessed; the card uses instead
  a word port at address xxF
*/

#define RPORT	0x30f

/*
  frequency presets
*/
int MemCh[10] =
{
    FCODE(89.3),
    FCODE(90.3),
    FCODE(92.3),
    FCODE(93.1),
    FCODE(98.5),
    FCODE(100.7),
    FCODE(107.9),
    FCODE(107.9),
    FCODE(107.9),
    FCODE(107.9)
};


/* ===================================================================== */
/*
   output to card - data is sent in a serial fashion, from LSB to MSB
   0 is encoded as '1 1 3 3', 1 as '5 5 7 7'
*/

void r_out(int v,int n)
{
	while (n--) {
	if (v&1) {
		_outp8 (RPORT,5);
		_outp8 (RPORT,5);
		_outp8 (RPORT,7);
		_outp8 (RPORT,7);
	}
	else {
		_outp8 (RPORT,1);
		_outp8 (RPORT,1);
		_outp8 (RPORT,3);
		_outp8 (RPORT,3);
	}
	v>>=1;
	}
}


/* ===================================================================== */
/*
   data sent in sequence to the card:
	(to the 16-bit port)
	16-bit frequency, in 25kHz steps, offset by 98.7 MHz (0xf6c)
	8-bit value, unknown function, seems to be always 0xa0
	(to the 8-bit port)
	1-byte volume control: 0x88 to increase, 0x48 to decrease, else 0
	1-byte control: 0xc0 to turn off, else 0xc8
*/

int radioon(int ch,int p2)
{
	_outp8(RPORT,0xc8);
	r_out(ch,16);
	r_out(p2,8);
	_outp8(RPORT,0xd8);
	_sleep2(200);

	return(((_inp8(RPORT)>>1)&0x01));

}


void radiooff(void)
{
	r_out(FCODE(88.0),16);
	r_out(0xa0,8);
	_outp8(RPORT,0);
	_outp8(RPORT,0xc0);
}


void volumeup(void)
{
	_outp8(RPORT,0x88);
	_sleep2(50);
	_outp8(RPORT,0xc8);
}


void volumedn(void)
{
	_outp8(RPORT,0x48);
	_sleep2(50);
	_outp8(RPORT,0xc8);
}


/* ===================================================================== */

int main(int argc,char *argv[])
{
 long x;
 int v,n,r;
 unsigned char d1,d2,d3,d4;
/* #ifndef linux
 #error  I feel lost here
 #endif */
 double f;
 char *p;

	printf("rem [ RevealRadio OS/2 By Sophisto (Stephen Loomis) (C) 1995 ]\r\n");
	if (argc<2)
	{
		printf("This program has the same basic functions of FMRADIO.EXE under DOS.\r\n");
		printf("Use:                                                              \r\n");
		printf("    radio <param>                                                 \r\n");
		printf("                                                                  \r\n");
		printf("where param can be                                                \r\n");
		printf("                                                                  \r\n");
		printf("    on          turn on radio card                                \r\n");
		printf("    off         turn off radio card                               \r\n");
		printf("    +           increase volume                                   \r\n");
		printf("    -           decrease volume                                   \r\n");
		printf("    1..10           turn on and switch to preset #n               \r\n");
		printf("    88.0..108.0     tune to the given (float) frequency           \r\n");
		printf("    m           Mute                                    \r\n");
		printf("    M           UnMute :) \r\n");
		printf("    @           Scan All FM BAND for Stereo Signals               \r\n");
		printf("    ?           list preset channels                              \r\n");
		return(1);
	}
	p = argv[1];
	n = 0xa0;
/*	  _portaccess(RPORT,RPORT);*/
	if (*p=='+')
		{
		  printf("Sliding Volume Up...\r\n");
		  volumeup();
		}
	else
	if (*p=='-')
		{
			printf("Sliding Volume Down...\r\n");
			volumedn();
		}
	else
	if (!strcmp("on",p))
		{
		 printf("Turning Radio On...\r\n");
		 radioon(MemCh[0],n);
		}
	else
	if (!strcmp("off",p))
		{
		 printf("Turning Radio Off...\r\n");
		 radiooff();
		}
	else
	if (isdigit(*p))
	{
		if (sscanf(p,"%lf",&f)==1)
		{
			if ((f>=11.0) /*&&(f<=108.0) */)
				{
				printf("Tunning to specific freq...");
				r = radioon(FCODE(f),n);
				if (!r) printf("[LOCKED]");
				printf("\r\n");
				}
			else if ((f>0.99)&&(f<10.01))
				{
				v=f;
				printf("Tunning to preset channel...\r\n");
				radioon(MemCh[v-1],n);
				}
		}

	}
	else
	if (*p=='?')
		{
        printf("1  (89.3)  \r\n");
        printf("2  (90.3)  \r\n");
        printf("3  (92.3)  \r\n");
        printf("4  (93.1)  \r\n");
        printf("5  (100.7)\r\n");
        printf("6  (107.9)\r\n");
        printf("7  (107.9)\r\n");
        printf("8  (107.9)\r\n");
        printf("9  (107.9)\r\n");
        printf("10 (107.9)\r\n");

		}
	else
	if (*p=='m')
		{
			_outp8(RPORT,0xc0);
		}
	else
	if (*p=='M')
		{
			_outp8(RPORT,0xc8);
		}
	else
	if (*p=='@')
		{
		 for(n=3944; n<4756;n+=8)
			{
				if (!radioon(n,0xa0))
				{
				f =(float) n;

				x = 0;
				r = 1;
				while (x++<400000)
					{
					 if (((_inp8(RPORT)>>1)&0x01)) { r=0; break;}
					}
				if (r)
					{
					 printf("radio %3.2lf \n", ((f-0xf6c)/40)+88.0);
					 printf("delay 1\n");
					}
				}
			}
		}


	return(0);
}


