// QR_Encode.h : TQR_Encode 定义的接口和类的声明

#ifndef QR_ENCODE_INCLUDED
#define QR_ENCODE_INCLUDED

/////////////////////////////////////////////////////////////////////////////
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
//常数
#define NULL    0
#define FALSE   0
#define TRUE    1

//误差校正水平
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

//数据模式
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

//版本（模型）组
#define QR_VRESION_S	0 // 1 ～ 9
#define QR_VRESION_M	1 // 10 ～ 26
#define QR_VRESION_L	2 // 27 ～ 40

#define MAX_ALLCODEWORD	 3706 //最大码字
#define MAX_DATACODEWORD 2956 //最大的数据码字（版本40-L)
#define MAX_CODEBLOCK	  153 //最大的数据码块数
#define MAX_MODULESIZE	  177 //最大的象素数


/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		//RS编号
	int ncAllCodeWord;	//数据码字数
	int ncDataCodeWord;	//数据码字(RS码字)

} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
//二维码版本（型号）的信息
typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // 版本（型号）数量(1～40)
	int ncAllCodeWord; //总的代码字

	// 以下是数组索引错误率(0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	// 数据码字（总的代码字- RS码字）

	int ncAlignPoint;	//对齐模式坐标
	int nAlignPoint[6];	//对齐模式中心坐标
	

	RS_BLOCKINFO RS_BlockInfo1[4]; //RS信息块(1)
	RS_BLOCKINFO RS_BlockInfo2[4]; //RS信息块(2)

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
// TQR_Encode类

class TQR_Encode
{
//
public:
	TQR_Encode();
	~TQR_Encode();

public:
	int m_nLevel;		// 纠错水平
	int m_nVersion;		//版本（型号）
	int m_bAutoExtent;	//版本（型号）指定自动检举
	int m_nMaskingNo;	//一些掩蔽模式

public:
	int m_nSymbleSize;
	unsigned char m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// bit5:功能模块（不包括掩蔽）Flag
	// bit4:数据得出功能模块
	// bit1:编码数据代码
	// bit0:编码后的数据绘制遮罩
	// 20h确定的逻辑和功能模块， 11h确定的逻辑和最终的int值

private:
	int m_ncDataCodeWordBit; //数据码字少量长度
	unsigned char m_byDataCodeWord[MAX_DATACODEWORD]; //数据录入码区

	int m_ncDataBlock;
	unsigned char m_byBlockMode[MAX_DATACODEWORD];
	int m_nBlockLength[MAX_DATACODEWORD];

	int m_ncAllCodeWord; //总的代码字（遥感数据，其中包括纠错）
	unsigned char m_byAllCodeWord[MAX_ALLCODEWORD]; //计算总码字
	unsigned char m_byRSWork[MAX_CODEBLOCK]; //RS码字计算工作

//数据编码
public:
	int EncodeData(int nLevel, int nVersion, int bAutoExtent, int nMaskingNo, const char* lpsSource, int ncSource = 0);

private:
	int GetEncodeVersion(int nVersion, const char* lpsSource, int ncLength);
	int EncodeSourceData(const char* lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(unsigned char nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, unsigned short wData, int ncData);

	int IsNumeralData(unsigned char c);
	int IsAlphabetData(unsigned char c);
	int IsKanjiData(unsigned char c1, unsigned char c2);

	unsigned char AlphabetToBinaly(unsigned char c);
	unsigned short KanjiToBinaly(unsigned short wc);

	void GetRSCodeWord(unsigned char* lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

//功能模块相关的位置
private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};

/////////////////////////////////////////////////////////////////////////////

#endif
