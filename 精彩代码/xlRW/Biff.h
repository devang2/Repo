// Biff.h

#ifndef BIFF_H
#define BIFF_H

// Record BOF
#define BIFF7			0x0500	// Versin for BIFF7
#define BIFF8			0x0600	// Version for BIFF8 and BIFF8X

#define WORKBOOK		0x0005	// Workbook globals
#define WORKSHEET		0x0010	// Worksheet
#define CHART			0x0020	// Chart

// Ordered by record identifier(Only 5,7,8,8x)
#define XL_BOF			0x0809	// 开始
#define XL_EOF			0x000A	// 结尾
#define XL_BOUNDSHEET	0x0085	// Sheets
#define	XL_DIMENSION	0x0200	// 尺寸
#define XL_SST			0x00FC	// SST
#define XL_ROW			0x0208	// 行
#define XL_FORMAT		0x041E	// 格式

#define XL_LABEL		0x0204
#define XL_STRING		0x0207	// 字符串
#define XL_NUMBER		0x0203	// 数字
#define XL_FORMULA		0x0006	// 公式
#define XL_RK			0x027E	

#endif // BIFF_H