#include <def.inc>
#include <func.inc>

UC escape(UI x, UI y)
{
UC ret_val;
static UC *topic = "  您想退出本计费系统吗?";
static UC *select[2] = {"取消", "确认"};

	message_disp(8," ← → 选择   Enter 选中");    /*left right*/
	set_menu(7, 0, TRUE, TRUE);
	ret_val = hv_menu(x, y, 2, 2, topic, select);
	message_end();

	if (ret_val==2)
		return TRUE;
	else
		return FALSE;
}
