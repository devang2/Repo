#ifndef TEXTBUT_HPP
#define TEXTBUT_HPP

#include "box.hpp"
#include "gui.hpp"

class Text:public Box
{
	int max_char ;
	int cursor_position ;
	int display_start ;
	char * text ;
public :
	Text(int x,int y,int maxChar);
	~Text();
	void Show();
	int Clicked();
	int Run();
	void Active(Bool status) ;
	char * GetText(){return text;};
};

#endif
