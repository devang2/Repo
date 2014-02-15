#ifndef EVTCODE_H
	#define EVTCODE_H

#define MouseDown		0x0001
#define MouseUp			0x0002
#define MouseAuto		0x0004
#define MouseDouble		0x0008
#define MouseDownR		0x0010
#define MouseUpR		0x0020
#define MouseAutoR		0x0040
#define MouseDoubleR	0x0080
#define MouseDownM		0x0100
#define MouseUpM		0x0200
#define MouseAutoM		0x0400
#define MouseDoubleM	0x0800
#define MouseMove		0x1000

#define evNothing   0x0000
#define evMouse     0x1fff
#define evButton	0x0fff
#define evKey       0x2000
#define evMessage   0x4000

#define mbLeftButton	0x01
#define mbRightButton	0x02
#define mbMiddleButton	0x04

#define RightShiftDown  0x01
#define LeftShiftDown   0x02
#define ShiftDown       0x03
#define CtrlDown        0x04
#define AltDown         0x08
#define ScrollOn        0x10
#define NumlockOn       0x20
#define CapsOn          0x40
#define Insert          0x80

#define DEL 339
#define INS 338
#define HOME 327
#define END 335
#define PGUP 329
#define PGDN 337
#define _UP 328
#define _DOWN 336
#define LEFT 331
#define RIGHT 333

#define _F1 315
#define _F2 316
#define _F3 317
#define _F4 318
#define _F5 319
#define _F6 320
#define _F7 321
#define _F8 322
#define _F9 323
#define _F10 324

#define SHIFTF1 340
#define SHIFTF2 341
#define SHIFTF3 342
#define SHIFTF4 343
#define SHIFTF5 344
#define SHIFTF6 345
#define SHIFTF7 346
#define SHIFTF8 347
#define SHIFTF9 348
#define SHIFTF10 349

#define CTRLF1 350
#define CTRLF2 351
#define CTRLF3 352
#define CTRLF4 353
#define CTRLF5 354
#define CTRLF6 355
#define CTRLF7 356
#define CTRLF8 357
#define CTRLF9 358
#define CTRLF10 359

#define ALTF1 360
#define ALTF2 361
#define ALTF3 362
#define ALTF4 363
#define ALTF5 364
#define ALTF6 365
#define ALTF7 366
#define ALTF8 367
#define ALTF9 368
#define ALTF10 369

#define ALT1 376
#define ALT2 377
#define ALT3 378
#define ALT4 379
#define ALT5 380
#define ALT6 381
#define ALT7 382
#define ALT8 383
#define ALT9 128
#define ALT0 129

#define ALTA 286
#define ALTB 304
#define ALTC 302
#define ALTD 288
#define ALTE 274
#define ALTF 289
#define ALTG 290
#define ALTH 291
#define ALTI 279
#define ALTJ 292
#define ALTK 293
#define ALTL 294
#define ALTM 306
#define ALTN 305
#define ALTO 280
#define ALTP 281
#define ALTQ 272
#define ALTR 275
#define ALTS 287
#define ALTT 276
#define ALTU 278
#define ALTV 303
#define ALTW 273
#define ALTX 301
#define ALTY 277
#define ALTZ 300

#define BACKUP 8
#define TAB 9
#define SHIFTTAB 271
#define ESC 27
#define ENTER 13

#define CTRLA 1
#define CTRLB 2
#define CTRLC 3
#define CTRLD 4
#define CTRLE 5
#define CTRLF 6
#define CTRLG 7
#define CTRLH 8
#define CTRLI 9
#define CTRLJ 10
#define CTRLK 11
#define CTRLL 12
#define CTRLM 13
#define CTRLN 14
#define CTRLO 15
#define CTRLP 16
#define CTRLQ 17
#define CTRLR 18
#define CTRLS 19
#define CTRLT 20
#define CTRLU 21
#define CTRLV 22
#define CTRLW 23
#define CTRLX 24
#define CTRLY 25
#define CTRLZ 26

#define CTRLEND 373
#define CTRLHOME 375
#define CTRLPGDN 374
#define CTRLPGUP 132
#define CTRLRIGHT 371
#define CTRLLEFT 132

#endif

