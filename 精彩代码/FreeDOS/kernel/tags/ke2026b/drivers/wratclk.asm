;
; File:
;                          wratclk.asm
; Description:
;                  WriteATClock - sysclock support
;
;                       Copyright (c) 1995
;                       Pasquale J. Villani
;                       All Rights Reserved
;
; This file is part of DOS-C.
;
; DOS-C is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version
; 2, or (at your option) any later version.
;
; DOS-C is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
; the GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public
; License along with DOS-C; see the file COPYING.  If not,
; write to the Free Software Foundation, 675 Mass Ave,
; Cambridge, MA 02139, USA.
;
; $Header$
;

                %include "..\kernel\segs.inc"

segment	HMA_TEXT

;
;       VOID WriteATClock(bcdDays, bcdHours, bcdMinutes, bcdSeconds)
;       BYTE *bcdDays;
;       BYTE bcdHours;
;       BYTE bcdMinutes;
;       BYTE bcdSeconds;
;
                global  _WriteATClock
_WriteATClock:
                push    bp
                mov     bp,sp
;               bcdSeconds = 10
;               bcdMinutes = 8
;               bcdHours = 6
;               bcdDays = 4
                mov     ch,byte [bp+6]      ;bcdHours
                mov     cl,byte [bp+8]      ;bcdMinutes
                mov     dh,byte [bp+10]     ;bcdSeconds
                mov     dl,0
                mov     ah,3
                int     1ah
                mov     bx,word [bp+4]      ;bcdDays
                mov     dx,word [bx]
                mov     cx,word [bx+2]
                mov     ah,5
                int     1ah
                pop     bp
                ret

; Log: wratclk.asm,v 
;
; Revision 1.3  1999/08/10 17:21:08  jprice
; ror4 2011-01 patch
;
; Revision 1.2  1999/03/29 17:08:31  jprice
; ror4 changes
;
; Revision 1.1.1.1  1999/03/29 15:40:34  jprice
; New version without IPL.SYS
;
; Revision 1.2  1999/01/22 04:16:40  jprice
; Formating
;
; Revision 1.1.1.1  1999/01/20 05:51:00  jprice
; Imported sources
;
;
;   Rev 1.2   29 Aug 1996 13:07:12   patv
;Bug fixes for v0.91b
;
;   Rev 1.1   01 Sep 1995 18:50:42   patv
;Initial GPL release.
;
;   Rev 1.0   02 Jul 1995  8:01:18   patv
;Initial revision.
;
