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

#define RPORT   0x20f

