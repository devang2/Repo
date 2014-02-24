#include "radios.h"

//33333333333333333333333333333333333333333333333333333333333333333333333/
radios_group_class::radios_group_class (int ID,char *title_hotkey,int left,int top,int width,int height )
	:group_class(ID,title_hotkey,left,top,width,height )
{
	flag=TRUE;
}

int radios_group_class::key_pressed_handler  (int key_num)
{
	switch (key_num) {
		case SPACEKEY:
			return TRUE;
		default:return group_class::key_pressed_handler(key_num);
	}
}


int radios_group_class::control_change_value	(int new_value )
{   //for first coming
	if ( pcontrol_current_selected !=NULL ) {
		if (pcontrol_current_selected->current_value !=pcontrol_current_selected->min_value)
			if (!flag)pcontrol_current_selected->control_change_value (pcontrol_current_selected->min_value);
			else pcontrol_current_selected->unselect();
	}
	flag =FALSE;
	return group_class::control_change_value	(new_value );
}

int radios_group_class::msg_handler 	(MSG& message)
{
	switch (message.Action){
		case GroupValueChangedMSG:
			if (message.fptr == this ) {
//				if (pcontrol_current_selected->current_value ==pcontrol_current_selected->min_value)
				{
					pcontrol_current_selected->control_change_value (pcontrol_current_selected->max_value);
				}
				return TRUE;
			}
			break;
		default: ;// return FALSE;
	}
	return group_class::msg_handler (message);
//	return FALSE;
}

