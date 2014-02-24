#ifndef __KEYDEF_H
#define __KEYDEF_H


#define lo(f) ((f)&0xff)
#define hi(f) (lo(f>>8))

#define ENTERKEY 0x1c0d
#define CTRL_A 0x1e02
#define CTRL_B 0x3002
#define CTRL_C 0x2e03
#define CTRL_D 0x2004
#define CTRL_E 0x1205
#define CTRL_F 0x2106
#define CTRL_G 0x2207
#define CTRL_H 0x2308
#define CTRL_I 0x1709
#define CTRL_J 0x240a
#define CTRL_K 0x250b
#define CTRL_L 0x260c
#define CTRL_M 0x320d
#define CTRL_N 0x310e
#define CTRL_O 0x180f
#define CTRL_P 0x1910
#define CTRL_Q 0x1011
#define CTRL_R 0x1312
#define CTRL_S 0x1f13
#define CTRL_T 0x1414
#define CTRL_U 0x1615
#define CTRL_V 0x2f16
#define CTRL_W 0x1117
#define CTRL_X 0x2d18
#define CTRL_Y 0x1519
#define CTRL_Z 0x2c1a

#define CTRL_ENTERKEY 0x1c0a

#define UPKEY 0x4800
#define DOWNKEY 0X5000
#define LEFTKEY 0x4b00
#define RIGHTKEY 0x4d00

#define SHIFT_LEFT 0x4b34		// all in keypad
#define SHIFT_RIGHT 0x4d36
#define SHIFT_UP 0x4838
#define SHIFT_DOWN 0x5032
#define SHIFT_HOME 0x4737
#define SHIFT_PGUP 0x4939
#define SHIFT_END 0x4f31
#define SHIFT_PGDN 0x5133
#define SHIFT_KEYPAD_5 0x4c35

#define DELKEY 0x5300
#define INSKEY 0x5200
#define BSKEY 0x0e08
#define SPACEKEY 0x3920

#define PGUPKEY 0x4900
#define PGDNKEY 0x5100
#define HOMEKEY 0x4700
#define ENDKEY 0x4f00

#define ESCKEY 0x011b

#define ALT_A 0x1e00
#define ALT_B 0x3000
#define ALT_C 0x2e00
#define ALT_D 0x2000
#define ALT_E 0x1200
#define ALT_F 0x2100
#define ALT_G 0x2200
#define ALT_H 0x2300
#define ALT_I 0x1700
#define ALT_J 0x2400
#define ALT_K 0x2500
#define ALT_L 0x2600
#define ALT_M 0x3200
#define ALT_N 0x3100
#define ALT_O 0x1800
#define ALT_P 0x1900
#define ALT_Q 0x1000
#define ALT_R 0x1300
#define ALT_S 0x1f00
#define ALT_T 0x1400
#define ALT_U 0x1600
#define ALT_V 0x2f00
#define ALT_W 0x1100
#define ALT_X 0x2d00
#define ALT_Y 0x1500
#define ALT_Z 0x2c00

#define ALT_1 0x7800
#define ALT_2 0x7900
#define ALT_3 0x7a00
#define ALT_4 0x7b00
#define ALT_5 0x7c00
#define ALT_6 0x7d00
#define ALT_7 0x7e00
#define ALT_8 0x7f00
#define ALT_9 0x8000
#define ALT_0 0x8100
#define ALT_SUB 0x8200
#define ALT_ADD 0x8300
#define ALT_SPACE 0x05ff

#define CTRL_F1 0x5e00
#define CTRL_F2 0x5f00
#define CTRL_F3 0x6000
#define CTRL_F4 0x6100
#define CTRL_F5 0x6200
#define CTRL_F6 0x6300
#define CTRL_F7 0x6400
#define CTRL_F8 0x6500
#define CTRL_F9 0x6600
#define CTRL_F10 0x6700

#define ALT_F1 0x6800
#define ALT_F2 0x6900
#define ALT_F3 0x6a00
#define ALT_F4 0x6b00
#define ALT_F5 0x6c00
#define ALT_F6 0x6d00
#define ALT_F7 0x6e00
#define ALT_F8 0x6f00
#define ALT_F9 0x7000
#define ALT_F10 0x7100

#define F1KEY 0x3b00
#define F2KEY 0x3c00
#define F3KEY 0x3d00
#define F4KEY 0x3e00
#define F5KEY 0x3f00
#define F6KEY 0x4000
#define F7KEY 0x4100
#define F8KEY 0x4200
#define F9KEY 0x4300
#define F10KEY 0x4400


#define TABKEY 			0x0f09
#define SHIFT_TABKEY 	0xf00

#define BELL   		0x07
#define CTRL_LEFT	0x7300
#define CTRL_RIGHT	0x7400

// Below is new in 3/15/1994
#define CTRL_UP 	0x8d00
#define CTRL_DOWN 	0x9100

#define ALT_LEFT 	0x9b00
#define ALT_RIGHT 	0x9d00

#define CTRL_ALT_LEFT 	0xf001
#define CTRL_ALT_RIGHT 	0xf002


#endif