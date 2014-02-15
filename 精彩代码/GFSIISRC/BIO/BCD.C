#include <stdio.h>
#include <stdlib.h>

#include <def.inc>

/* ----------------------------------------------------------------------
	FUNCTION:	Translate a string of Compacted BCD format to BCD format.
	CALLED BY:	This is a general-purposed routined.
	INPUT:		*c_bcd	the source buffer for the C-BCD codes.
				max_num	the maxium number of BCD codes wanted to be
						converted. it determines the buffer size.
	OUTPUT:		*bcd	the target buffer for the BCD codes.
	RETURN:		Number of BCD codes been converted.
				When max_num BCDs been converted or come up to a non-BCD code,
				converting stops, and the very number is returned.
				If a non-BCD is reached before max_num BCDs be converted,
				the target BCD buffer *bcd ends with ONE 0x0F.
  ----------------------------------------------------------------------- */
UI cbcd_bcd(UC *bcd, UC *c_bcd, UI max_num)
{
UI  c_bcd_index;     		/* cbcd[] index */
UI  bcd_index;				/* bcd[] index */
UC  tmp_ch;

	if (max_num==0)
		return 0;

	c_bcd_index = 0;
	bcd_index = 0;
	while (TRUE)
	{
		tmp_ch = c_bcd[c_bcd_index];

		if ( tmp_ch < 0xA0 )
		{
			bcd[bcd_index++] = tmp_ch>>4;		/* the higher 4 bits < 9 */
			if ( bcd_index==max_num )
				break;
		}
		else
			break;

		tmp_ch &= 0x0F;
		if ( tmp_ch<0x0A )
		{
			bcd[bcd_index++] = tmp_ch;		/* the lower 4 bits < 9 */
			if ( bcd_index==max_num )
				break;
		}
		else
			break;

		c_bcd_index++;
	}

	if ( bcd_index<max_num )
		bcd[bcd_index] = 0x0F;		/* if not enough bcd code be translate,
										make the end flag: 0x0F. */
	return bcd_index;
}

/* ----------------------------------------------------------------------
	FUNCTION:	Translate a string of BCD format to Compacted BCD format.
	CALLED BY:	This is a general-purposed routined.
	INPUT:		*bcd	the source buffer for the BCD codes.
				max_num	the maxium number of BCD codes wanted to be
						converted. it determines the buffer size.
	OUTPUT:		*c_bcd	the target buffer for the C_BCD codes.
	RETURN:		Number of BCD codes been converted.
				When max_num BCDs been converted or come up to an non-BCD code,
				converting stops, and the very number is returned.
				If a non-BCD is reached before max_num BCDs been converted,
				the target C-BCD buffer *c_bcd ends with ONE 0x0F.
  ----------------------------------------------------------------------- */
UI bcd_cbcd(UC *c_bcd, UC *bcd, UI max_num)
{
UI	loop;
UI  c_bcd_index;

	if (max_num==0)
		return 0;

	for ( loop=0; loop<max_num; loop++ )
	{
		c_bcd_index = loop>>1;

		if ( bcd[loop]<0x0A )
		{
			if ( (c_bcd_index<<1)==loop )		/* loop is even */
				c_bcd[c_bcd_index] = (c_bcd[c_bcd_index]&0x0F) | (bcd[loop]<<4);
			else								/* loop is odd */
				c_bcd[c_bcd_index] = (c_bcd[c_bcd_index]&0xF0) | bcd[loop];
		}
		else
		{
			if ( (c_bcd_index<<1)==loop )
				c_bcd[c_bcd_index] = c_bcd[c_bcd_index] | 0xF0;
			else
				c_bcd[c_bcd_index] = c_bcd[c_bcd_index] | 0x0F;

			return loop;
		}
	}
	return max_num;
}

/* ----------------------------------------------------------------------
	FUNCTION:	Translate a string of BCD format to ASCII format.
	CALLED BY:	This is a general-purposed routined.
	INPUT:		*bcd	the source buffer for the BCD codes.
				max_num	the maxium number of BCD codes wanted to be
						converted. it determines the buffer size.
	OUTPUT:		*str	the target buffer for the string.
	RETURN:		Number of BCD codes been converted. (NOT including the '\0'
				at the string tail).
				No matter why the converting stopped, the string ends with
				ONE '\0'.
				When max_num BCDs been converted or come up to an non-BCD
				code, converting stops.
  ----------------------------------------------------------------------- */
UI bcd_str(UC *str, UC *bcd, UI max_num)
{
UI	loop = 0;

	if (max_num==0)
		return 0;

	while ( loop<max_num && bcd[loop]<0x0A )
	{
		str[loop] = bcd[loop] + '0';
		loop++;
	}
	str[loop] = '\0';
	return loop;
}


/* ----------------------------------------------------------------------
	FUNCTION:	Translate a string of ASCII format to BCD format.
	CALLED BY:	This is a general-purposed routined.
	INPUT:		*str	the source buffer for the digital string.
				max_num	the maxium number of BCD codes wanted to be
						converted. it determines the buffer size.
	OUTPUT:		*bcd	the target buffer for the BCD codes.
	RETURN:		Number of BCD codes been converted.
				When max_num BCDs been converted or come up to an non-BCD
				code, converting stops, and the number of BCDs been
				converted is returned.
				If a non-BCD is reached before max_num BCDs been converted,
				the target BCD buffer *bcd ends with ONE 0x0F.
  ----------------------------------------------------------------------- */
UI str_bcd(UC *bcd, UC *str, UI max_num)
{
UI	loop = 0;

	if (max_num==0)
		return 0;

	while ( loop<max_num && str[loop]>='0' && str[loop]<='9' )
	{
		bcd[loop] = str[loop] - '0';
		loop++;
	}

	if ( loop!=max_num )
		bcd[loop] = 0x0F;	/* the tail mark 0x0F */

	return loop;
}

/* ----------------------------------------------------------------------
	FUNCTION:	Translate a string of Compacted BCD format to ASCII format.
	CALLED BY:	This is a general-purposed routined.
	INPUT:		*c_bcd	the source buffer for the C_BCD codes.
				max_num	the maxium number of BCD codes wanted to be
						converted. it determines the buffer size.
	OUTPUT:		*str	the target buffer for the string.
	RETURN:		Number of BCD codes been converted.
				No matter why the converting stopped, the string ends with
				ONE '\0'.
				When max_num BCDs been converted or come up to an non-BCD
				code, converting stops, and the string length (the number
				of BCDs been converted) is returned (NOT including the '\0'
				at the string tail).
  ----------------------------------------------------------------------- */
UI cbcd_str(UC *str, UC *c_bcd, UI max_num)
{
UI  c_bcd_index;     		/* c_bcd[] index */
UI  str_index;				/* str[] index */
UC  tmp_ch;

	if (max_num==0)
		return 0;

	c_bcd_index = 0;
	str_index = 0;

	while (TRUE)
	{
		tmp_ch = c_bcd[c_bcd_index];

		if ( tmp_ch < 0xA0 )
		{
			str[str_index++] = (tmp_ch>>4) + '0';	/* the higher 4 bits < 9 */
			if ( str_index==max_num )
				break;
		}
		else
			break;

		tmp_ch &= 0x0F;
		if ( tmp_ch<0x0A )
		{
			str[str_index++] = tmp_ch + '0';	/* the lower 4 bits < 9 */
			if ( str_index==max_num )
				break;
		}
		else
			break;

		c_bcd_index++;
	}

	str[str_index] = '\0';		/* make the string's end flag: '\0'. */
	return str_index;
}


/* ----------------------------------------------------------------------
	FUNCTION:	Translate a string of ASCII format to Compacted BCD format.
	CALLED BY:	This is a general-purposed routined.
	INPUT:		*str	the source buffer for the digital string.
				max_num	the maxium number of BCD codes wanted to be
						converted. it determines the buffer size.
	OUTPUT:		*c_bcd	the target buffer for the C-BCD codes.
	RETURN:		Number of BCD codes been converted.
				When max_num BCDs been converted or come up to an non-BCD
				code, converting stops, and the number of BCDs been
				converted is returned.
				If a non-BCD is reached before max_num BCDs been converted,
				the target C-BCD buffer *c_bcd ends with ONE 0x0F.
  ----------------------------------------------------------------------- */
UI  str_cbcd(UC *c_bcd, UC *str, UI max_num)
{
UI	loop;
UI  c_bcd_index;

	if (max_num==0)
		return 0;

	for ( loop=0; loop<max_num; loop++ )
	{
		c_bcd_index = loop>>1;

		if ( str[loop]<='9' && str[loop]>='0' )
		{
			if ( (c_bcd_index<<1)==loop )	/* loop is even */
				c_bcd[c_bcd_index] = (c_bcd[c_bcd_index] & 0x0F)
									 | ( (str[loop]-'0')<<4 );
			else               	            /* loop is odd */
				c_bcd[c_bcd_index] = (c_bcd[c_bcd_index] & 0xF0)
									 | ( str[loop]-'0');
		}
		else
		{
			if ( (c_bcd_index<<1)==loop )
				c_bcd[c_bcd_index] = c_bcd[c_bcd_index] | 0xF0;
			else
				c_bcd[c_bcd_index] = c_bcd[c_bcd_index] | 0x0F;

			return loop;
		}
	}
	return max_num;
}

/* --------------------------------------------------------------------
	FUNCTION:	Convert a bcd string to an unsigned long integer.
	CALLED BY:	This is a general-purposed routine.
	INPUT:      *bcd	the buffer keeping the BCD codes.
				max_num	the num of BCDs wanted to be converted ( <=9 ).
	OUTPUT:		*hex	if converting successful, the HEX value stored here.
	RETURN:		The number of BCDs that been converted.
   ---------------------------------------------------------------------- */
UI bcd_hex(UL *hex, UC *bcd, UI max_num)
{
UL tmp;
UC loop;

	if (max_num==0 || max_num>9)
		return 0;

	loop = 0;
	tmp = 0;
	while (loop<max_num && bcd[loop]<0x0A )
	{
		tmp = (tmp<<3) + (tmp<<1) + bcd[loop];
		loop++;
	}

	if (loop!=0)
		*hex = tmp;
	return loop;
}

/* --------------------------------------------------------------------
	FUNCTION:	Convert an unsigned long integer to a BCD string.
	CALLED BY:	This is a general-purposed routine.
	INPUT:      hex		the HEX value that want to converted to BCD codes.
				max_num	the num of BCDs wanted to be converted.
						it determines the buffer's size.
	OUTPUT:		*bcd	if converting successful, the BCD codes stored here.
	RETURN:		The number of BCDs that been converted.
				If converting stoped before max_num BCDs been converted,
				the target C-BCD buffer *c_bcd ends with ONE 0x0F.
   ---------------------------------------------------------------------- */
UI hex_bcd(UC *bcd, UL hex, UI max_num)
{
UC buf[11];

	if (max_num==0)
		return 0;

	ultoa(hex, buf, 10);
	return str_bcd(bcd, buf, max_num);
}

/* --------------------------------------------------------------------
	FUNCTION:	Convert a Compacted BCD string to an unsigned long integer.
	CALLED BY:	This is a general-purposed routine.
	INPUT:      *c_bcd	the buffer keeping the BCD codes.
				max_num	the num of BCDs wanted to be converted ( <=9 ).
	OUTPUT:		*hex	if converting successful, the HEX value stored here.
	RETURN:		The number of BCDs that been converted.
   ---------------------------------------------------------------------- */
UI cbcd_hex(UL *hex, UC *c_bcd, UI max_num)
{
UL tmp;
UC loop;
UC c_bcd_index;

	if (max_num==0 || max_num>9)
		return 0;

	loop = 0;
	tmp = 0;
	while ( loop < max_num )
	{
		c_bcd_index = loop >> 1;

		if ( (c_bcd_index << 1) == loop )
		{
		    if ( c_bcd[c_bcd_index] < 0xA0 )
		        tmp = (tmp << 3) + (tmp << 1) + (c_bcd[c_bcd_index] >> 4);
                    else
		        break;
		}
		else
		{
		    if ( (c_bcd[c_bcd_index] & 0x0F) < 0x0A )
		        tmp = (tmp << 3) + (tmp << 1) + (c_bcd[c_bcd_index] & 0x0F);
		    else
		        break;
		}
		loop++;
        }
	if (loop!=0)
		*hex = tmp;
	return loop;
}

/* --------------------------------------------------------------------
	FUNCTION:	Convert an unsigned long integer to a Compacted BCD string.
	CALLED BY:	This is a general-purposed routine.
	INPUT:      hex		the HEX value that want to converted to BCD format.
				max_num	the number of BCDs wanted to be converted.
						it determines the buffer's size.
	OUTPUT:		*c_bcd	if converting successful, the BCD codes stored here.
	RETURN:		The number of BCDs that been converted.
				If converting stoped before max_num BCDs been converted,
				the target C-BCD buffer *c_bcd ends with ONE 0x0F.
   ---------------------------------------------------------------------- */
UI hex_cbcd(UC *c_bcd, UL hex, UI max_num)
{
UC buf[11];

	if (max_num==0)
		return 0;

	ultoa(hex, buf, 10);
	return str_cbcd(c_bcd, buf, max_num);
}

