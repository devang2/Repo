                                Sourcer v5.04    3-Sep-95  11:01 am   Page 1


PAGE  59,132

;ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ
;ÛÛ                                                                      ÛÛ
;ÛÛ                             INT14                                    ÛÛ
;ÛÛ                                                                      ÛÛ
;ÛÛ      Created:   3-Nov-88                                             ÛÛ
;ÛÛ      Passes:    5          Analysis Options on: QRS                  ÛÛ
;ÛÛ                                                                      ÛÛ
;ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ

.err     Target Assembler set to NONE, re-assembly not recommended


; The following equates show data references outside the range of the program.

data_1e         equ     50h                     ; (0000:0050=0E739h)
data_2e         equ     52h                     ; (0000:0052=0F000h)

;------------------------------------------------------------  seg_a   ----

seg_a           segment para public
                assume cs:seg_a  , ds:seg_a , ss:stack_seg_b

                db      0EBh, 21h, 90h
data_3          dw      offset loc_3            ; Data table (indexed access)
                                                ;  xref 3970:0069
data_4          dw      offset loc_4            ;  xref 3970:0069
data_5          dw      offset loc_7            ;  xref 3970:0069
data_6          dw      offset loc_9            ;  xref 3970:0069
data_7          dw      offset loc_10           ;  xref 3970:0069
data_8          dw      offset loc_16           ;  xref 3970:0069
data_9          dw      offset loc_23           ;  xref 3970:0069
data_10         db      17h                     ;  xref 3970:0086, 0115
                db       04h, 00h, 03h, 80h, 01h,0C0h
                db       00h, 60h, 00h, 30h, 00h, 18h
                db       00h, 0Ch, 00h, 06h, 00h

;ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ
;
;                       Program Entry Point
;
;ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ


int14           proc    far

start:
                mov     ax,43h
                xor     cx,cx                   ; Zero register
                mov     es,cx
                mov     es:data_1e,ax           ; (0000:0050=0E739h)
                mov     ax,cs
                mov     es:data_2e,ax           ; (0000:0052=0F000h)
                mov     ah,31h                  ; '1'
                mov     dx,1C3h
                Sourcer v5.04    3-Sep-95  11:01 am   Page 2

                int     21h                     ; DOS Services  ah=function 31h
                                                ;  terminate & stay resident
                                                ;   al=return code,dx=paragraphs

int14           endp

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
;                              SUBROUTINE
;
;         Called from:   3970:0058
;ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ

sub_1           proc    near
                push    ax
                mov     ax,40h
                mov     ds,ax
                pop     ax
                retn
sub_1           endp

                                                ;* No entry point to code
                sti                             ; Enable interrupts
                push    ds
                push    dx
                push    si
                push    di
                push    cx
                push    bx
                cmp     dx,4                    ; dx: port number(0,1,2,3)
                jae     loc_1                   ; Jump if above or =
                cmp     ah,6                    ; ah: 00 - initialization
                                                ;     01 - transimiting
                                                ;     02 - receiving
                                                ;     03 - read status
                                                ;     04 - expanding initialize
                                                ;     05 - exp port control
                jae     loc_1                   ; Jump if above or =
                mov     si,dx
                mov     di,dx
                call    sub_1                   ; (003B)
                shl     si,1                    ; Shift w/zeros fill
                mov     dx,[si]                 ; port base address
                or      dx,dx                   ; Zero ?
                jz      loc_1                   ; Jump if zero
                mov     bl,ah
                xor     bh,bh                   ; Zero register
                shl     bx,1                    ; Shift w/zeros fill
                jmp     word ptr cs:data_3[bx]  ;*(3970:0003=75h)    7 entries
loc_1:                                          ;  xref 3970:004D, 0052, 0061, 00AE
                                                ;            00C6, 00D7, 00F7, 00FD
                                                ;            0108, 0176
                pop     bx
loc_2:                                          ;  xref 3970:0185, 0199
                pop     cx
                pop     di
                pop     si
                pop     dx
                pop     ds
                iret                            ; Interrupt return

;ÄÄÄÄÄ Indexed Entry Point ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                Sourcer v5.04    3-Sep-95  11:01 am   Page 3

loc_3:                                          ;  xref 3970:0003, 0069
                mov     ah,al                   ;  al - initialization para
                                                ;  5~7: baud rate
                                                ;  3~4: parity
                                                ;  2  : stop bit
                                                ;  0~1: data bit
                add     dx,3
                mov     al,80h
                out     dx,al                   ; port 3FBh, RS232-1 line cntrl
                                                ;  al = 80h, set DLAB on
                mov     dl,ah
                mov     cl,4
                rol     dl,cl                   ; Rotate
                and     dx,0Eh
                mov     di,offset data_10       ; (3970:0011=17h)
                add     di,dx
                mov     dx,[si]
                inc     dx
                mov     al,cs:[di+1]
                out     dx,al                   ; port 3F9h, RS232-1 int enable
                dec     dx
                mov     al,cs:[di]
                out     dx,al                   ; port 3F8h, RS232-1 xmit buffr
                add     dx,3
                mov     al,ah
                and     al,1Fh
                out     dx,al                   ; port 3FBh, RS232-1 line cntrl
                                                ;  al = 0, set DLAB off
                sub     dx,2
                xor     al,al                   ; Zero register
                out     dx,al                   ; port 3F9h, RS232-1 int enable
                                                ;  al = 0, all interrupts off
                add     dx,4
                in      al,dx                   ; port 3FDh, RS232-1 line stat
                mov     ah,al
                inc     dx
                in      al,dx                   ; port 3FEh, RS232-1 modem stat
                jmp     short loc_1             ; (006E)

;ÄÄÄÄÄ Indexed Entry Point ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_4:                                          ;  xref 3970:0005, 0069
                push    ax
                add     dx,4
                mov     al,3
                out     dx,al                   ; port 3FCh, RS232-1 modem ctrl
                                                ;  al = 3, DTR and RTS on
                inc     dx
                inc     dx
                mov     bh,30h                  ; '0'
                call    sub_2                   ; (019C)
                jz      loc_6                   ; Jump if zero
loc_5:                                          ;  xref 3970:00CE
                pop     cx
                mov     al,cl
                or      ah,80h
                jmp     short loc_1             ; (006E)
loc_6:                                          ;  xref 3970:00BE
                dec     dx
                mov     bh,20h                  ; ' '
                call    sub_2                   ; (019C)
                Sourcer v5.04    3-Sep-95  11:01 am   Page 4

                jnz     loc_5                   ; Jump if not zero
                sub     dx,5
                pop     cx
                mov     al,cl
                out     dx,al                   ; port 3F8h, RS232-1 xmit buffr
                jmp     short loc_1             ; (006E)

;ÄÄÄÄÄ Indexed Entry Point ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_7:                                          ;  xref 3970:0007, 0069
                add     dx,4
                mov     al,1
                out     dx,al                   ; port 3FCh, RS232-1 modem ctrl
                                                ;  al = 1, DTR data term rdy
                inc     dx
                inc     dx
                mov     bh,20h                  ; ' '
                call    sub_2                   ; (019C)
                jnz     loc_8                   ; Jump if not zero
                dec     dx
                mov     bh,1
                call    sub_2                   ; (019C)
                jnz     loc_8                   ; Jump if not zero
                and     ah,1Eh
                sub     dx,5
                in      al,dx                   ; port 3F8h, RS232-1 recv buffr
                jmp     loc_1                   ; (006E)
loc_8:                                          ;  xref 3970:00E6, 00EE
                or      ah,80h
                jmp     loc_1                   ; (006E)

;ÄÄÄÄÄ Indexed Entry Point ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_9:                                          ;  xref 3970:0009, 0069
                add     dx,5
                in      al,dx                   ; port 3FDh, RS232-1 line stat
                mov     ah,al
                inc     dx
                in      al,dx                   ; port 3FEh, RS232-1 modem stat
                jmp     loc_1                   ; (006E)

;ÄÄÄÄÄ Indexed Entry Point ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_10:                                         ;  xref 3970:000B, 0069
                pop     bx
                push    bx
                push    ax
                add     dx,3
                mov     al,80h
                out     dx,al                   ; port 3FBh, RS232-1 line cntrl
                                                ;  al = 80h, set DLAB on
                push    cx
                mov     di,offset data_10       ; (3970:0011=17h)
                xor     ch,ch                   ; Zero register
                shl     cx,1                    ; Shift w/zeros fill
                add     di,cx
                mov     al,cs:[di+1]
                Sourcer v5.04    3-Sep-95  11:01 am   Page 5

                sub     dx,2
                out     dx,al                   ; port 3F9h, RS232-1 int enable
                dec     dx
                mov     al,cs:[di]
                out     dx,al                   ; port 3F8h, RS232-1 xmit buffr
                pop     cx
                pop     ax
                or      al,al                   ; Zero ?
                jz      loc_11                  ; Jump if zero
                mov     al,40h                  ; '@'
loc_11:                                         ;  xref 3970:012F
                or      al,ch
                shl     bl,1                    ; Shift w/zeros fill
                shl     bl,1                    ; Shift w/zeros fill
                or      al,bl
                or      bh,bh                   ; Zero ?
                jz      loc_15                  ; Jump if zero
                shl     bh,1                    ; Shift w/zeros fill
                shl     bh,1                    ; Shift w/zeros fill
                shl     bh,1                    ; Shift w/zeros fill
                test    bh,8
                jnz     loc_13                  ; Jump if not zero
                test    bh,20h                  ; ' '
                jz      loc_12                  ; Jump if zero
                or      al,20h                  ; ' '
loc_12:                                         ;  xref 3970:014D
                or      al,18h
                jmp     short loc_15            ; (0162)
                db      90h
loc_13:                                         ;  xref 3970:0148
                test    bh,10h
                jz      loc_14                  ; Jump if zero
                or      al,28h                  ; '('
                jmp     short loc_15            ; (0162)
                db      90h
loc_14:                                         ;  xref 3970:0159
                or      al,bh
loc_15:                                         ;  xref 3970:013D, 0153, 015D
                and     al,7Fh
                add     dx,3
                out     dx,al                   ; port 3FBh, RS232-1 line cntrl
                sub     dx,2
                xor     al,al                   ; Zero register
                out     dx,al                   ; port 3F9h, RS232-1 int enable
                                                ;  al = 0, all interrupts off
                add     dx,4
                in      al,dx                   ; port 3FDh, RS232-1 line stat
                mov     ah,al
                inc     dx
                in      al,dx                   ; port 3FEh, RS232-1 modem stat
                jmp     loc_1                   ; (006E)

;ÄÄÄÄÄ Indexed Entry Point ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_16:                                         ;  xref 3970:000D, 0069
                pop     bx
                add     dx,4
                Sourcer v5.04    3-Sep-95  11:01 am   Page 6

                or      al,al                   ; Zero ?
                jz      loc_17                  ; Jump if zero
                dec     al
                jz      loc_18                  ; Jump if zero
                jmp     loc_2                   ; (006F)
loc_17:                                         ;  xref 3970:017F
                in      al,dx                   ; port 3FCh, RS232-1 modem ctrl
                mov     bl,al
                jmp     short loc_19            ; (0193)
                db      90h
loc_18:                                         ;  xref 3970:0183
                mov     al,bl
                and     al,1Fh
                out     dx,al                   ; port 3FCh, RS232-1 modem ctrl
loc_19:                                         ;  xref 3970:018B
                inc     dx
                in      al,dx                   ; port 3FDh, RS232-1 line stat
                mov     ah,al
                inc     dx
                in      al,dx                   ; port 3FEh, RS232-1 modem stat
                jmp     loc_2                   ; (006F)

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
;                              SUBROUTINE
;
;         Called from:   3970:00BB, 00CB, 00E3, 00EB
;ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ

sub_2           proc    near
                mov     bl,[di+7Ch]
                push    bp
                push    bx
                pop     bp
                and     bp,0FFh
                rcl     bp,1                    ; Rotate thru carry
                rcl     bp,1                    ; Rotate thru carry
loc_20:                                         ;  xref 3970:01B8
                sub     cx,cx

locloop_21:                                     ;  xref 3970:01B5
                in      al,dx                   ; port 21F7h ??I/O Non-standard
                mov     ah,al
                and     al,bh
                cmp     al,bh
                je      loc_22                  ; Jump if equal
                loop    locloop_21              ; Loop if cx > 0

                dec     bp
                jnz     loc_20                  ; Jump if not zero
                or      bh,bh                   ; Zero ?
loc_22:                                         ;  xref 3970:01B3
                pop     bp
                retn
sub_2           endp


;ÄÄÄÄÄ Indexed Entry Point ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
                Sourcer v5.04    3-Sep-95  11:01 am   Page 7


loc_23:                                         ;  xref 3970:000F, 0069
                db      0, 0

seg_a           ends



;------------------------------------------------------  stack_seg_b   ----

stack_seg_b     segment word stack 'STACK'

                db      256 dup (0)

stack_seg_b     ends



                end     start
                Sourcer v5.04    3-Sep-95  11:01 am   Page 8


±±±±±±±±±±±±±±±±±±±± CROSS REFERENCE - KEY ENTRY POINTS ±±±±±±±±±±±±±±±±±±±

    seg:off    type        label
   ---- ----   ----   --------------------------------
   3970:0023   far    start

 ±±±±±±±±±±±±±±±±±± Interrupt Usage Synopsis ±±±±±±±±±±±±±±±±±±

        Interrupt 21h : DOS Services  ah=function xxh
        Interrupt 21h :  ah=31h  terminate & stay resident

 ±±±±±±±±±±±±±±±±±± I/O Port Usage Synopsis  ±±±±±±±±±±±±±±±±±±

        Port 3F8h  : RS232-1 recv buffr
        Port 3F8h  : RS232-1 xmit buffr
        Port 3F9h  : RS232-1 int enable
        Port 3F9h  :  al = 0, all interrupts off
        Port 3FBh  : RS232-1 line cntrl
        Port 3FBh  :  al = 0, set DLAB off
        Port 3FBh  :  al = 80h, set DLAB on
        Port 3FCh  : RS232-1 modem ctrl
        Port 3FCh  :  al = 1, DTR data term rdy
        Port 3FCh  :  al = 3, DTR and RTS on
        Port 3FDh  : RS232-1 line stat
        Port 3FEh  : RS232-1 modem stat
            1 Ocurrences of non-standard I/O ports used (search for ??).

€€‚‚€ƒƒ€„„€……€††€‡‡€ €    tem shutdown abnormally.



 rb ´æÔÚÎ´Éè¶¨·Ö»úÄÚÂë! %-7lu üˆ  è	FEE_REC.C;£| €Š  tcashpwd.dat wb eˆ  è
PAS_PROC.Cî™l wŠ  tğ  MESSAGE OBJ           Ãs 9Y  USR_CFG OBJ           t Ùø	  USER_INDOBJ           t ÛÈ  XÎĞXQÑ€¤Ï°)®Ò      R1      }ÓÌÃÙÒ   	ÈÔb Lwñÿ   4ZÌÒ˜6nÒ+)Ôø0Ó    ß€ R2      }ÓH‘Ò   ÈÔb Lwñÿ   4ZÌÒ‹%Ò+)Ô€LèÒ    
   C1      ‡nÆtZ¥È  AK ÈU  0£Å, 9È@»ÅÆ¬üÈ    ‘  C2      5‘ÇtZ¥È Ä)N \  @»ÅÆ˜¸AÈPièÇ¬üÈ   |  C5      øØKÅprÑ |YH €…	   _€Äp•Ñ0£Å¨SÉÑ#   †  C6      ‡nÆprÑ  AK €…	  0£Åp•Ñ@»ÅÆ¨SÉÑ    +   C7      5‘ÇprÑ 0âV ì=  $@»ÅÆİÑPièÇ¨SÉÑ     ‡  C8      5‘ÇÕÎ  XqE ¤m   @»ÅÆôlÎPièÇPÃ,Ï&     ;  C9      ÈiÆ@Ï  Ì{N l¸   ğÅTà±Î =ÀÆxÑnÏ'     )   C11     5‘Ç¸§§Ë  84W äeÔÿ  @»ÅÆ€sPËPièÇt‡GÌ'     ¢  C12     ‡nÆ¸§§Ë  úS €…	   0£Å¸AË@»ÅÆğÛşË)     )   C14     øØKÅtZ¥È  84W ˜îâÿ   _€Ä<&NÈ0£Å|±6É*     ‘ C18     pÉVÊä9ÒŒ
  úS \   8•ÿÉ8jnÑLkºÊH‘ÒJ     ^ RN1     ümÃÃøîÈŒ
  „5¯ @}ÿ„  lÈ&ÃxïÇ<ù«ÄÈ((É+    
 ­  RN2     ümÃÃxïÇŒ
  P¨C @}ÿ„  lÈ&ÃøØKÅ<ù«ÄHWÇ-    
 ©  2U5     ôtŸÊ¸SÏ    -v|bŒ
  ¨“cÊè0cÏD®ªÌÙÙĞ·        2U3     Ø4¸Ñ)Ï    Ü tF   ~ÑHëîÎÀ ÒX™Ğ     +  2K1     È³7Ôˆv×Ï  ”P± 4k   €Ñ½ÒàÙÑÎ èÔÀª.Ğ0   	  à   2U1     |<FÔøŞ…Ğ„  N×ÿ    l—İÓÀ¡tÏŒêhÕhG4Ñ1   
    2R1     È³7Ô˜$úĞŒ
  \Z (îÿ    p>ÉÓˆv×Ï èÔhG4Ñ2     ç € 2R2     L_€Ô˜$úĞŒ
  °k‰ x±ôÿ    DÔˆv×Ï„“×ÔhG4Ñ3     ø € 2R5     @3Îœ¹qÏ„  Ü €…	   @ÌÍÌ–7ÏxMŠÎ¬g”Ğÿÿ€   ƒ   2R6     ¼mêÍÜDZĞŒ
  Ü \   „9“ÍÌ–7Ï˜NÎ¬g”Ğ5     ‚   2R7     Ä—ÙÊ¬g”Ğ    Ğ¡ df   ôtŸÊt3=ĞÔEüËÒùĞ     …   2C2     €Ñ½Ò€FÏ„   A (îÿ    HfÒHëîÎØF,ÓX™Ğ6       2C3     ìw