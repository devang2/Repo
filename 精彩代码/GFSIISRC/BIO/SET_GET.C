#include <def.inc>

UC Get_mode_settled = FALSE;
UC Get_bk_color, Get_char_color, Get_cursor_color;
UC Get_ret_bk_color, Get_ret_char_color;

void set_get_color(UC bk_c, UC char_c, UC cursor_c, UC ret_bk_c, UC ret_char_c)
{
	Get_bk_color = bk_c;
	Get_char_color = char_c;
	Get_cursor_color = cursor_c;
	Get_ret_bk_color = ret_bk_c;
	Get_ret_char_color = ret_char_c;

	Get_mode_settled = TRUE;
}
