; io386.asm
;
; This module contains 4 C callable procedures that allow access to the
; hardware ports. They must be defined in the calling program as follows:
;
;	extern _Far16 _Pascal OUTP8(USHORT, UCHAR);
;	extern _Far16 _Pascal OUTP16(USHORT, USHORT);
;	extern UCHAR _Far16 _Pascal INP8(USHORT);
;	extern USHORT _Far16 _Pascal INP16(USHORT);
;
; This module will compile under MASM or TASM.
;

R2SEG SEGMENT BYTE PUBLIC USE16 'CODE'
      ASSUME  CS:R2SEG, DS:NOTHING

PUBLIC OUTP8
OUTP8  PROC  FAR
  push  bp
  mov   bp, sp
  mov   dx, [bp+8]
  mov   al, [bp+6]
  out   dx, al
  pop   bp
  ret   4
OUTP8  ENDP

PUBLIC OUTP16
OUTP16 PROC  FAR
  push  bp
  mov   bp, sp
  mov   dx, [bp+8]
  mov   ax, [bp+6]
  out   dx, ax
  pop   bp
  ret   4
OUTP16 ENDP

PUBLIC INP8
INP8   PROC  FAR
  push  bp
  mov   bp, sp
  mov   dx, [bp+6]
  in    al, dx
  sub   ah, ah
  pop   bp
  ret   2
INP8 ENDP

PUBLIC INP16
INP16  PROC  FAR
  push  bp
  mov   bp, sp
  mov   dx, [bp+6]
  in    ax, dx
  pop   bp
  ret   2
INP16 ENDP

R2SEG   ENDS
   END
