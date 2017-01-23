#include "myqlineedit.h"


MyQLineEdit::MyQLineEdit(QWidget *parent) : QLineEdit(parent)
{
	_selectOnMousePress = false;
}

void MyQLineEdit::focusInEvent(QFocusEvent *e)
{
	QLineEdit::focusInEvent(e);
	selectAll();
	_selectOnMousePress = true;
}

void MyQLineEdit::mousePressEvent(QMouseEvent *me)
{
	QLineEdit::mousePressEvent(me);
	if(_selectOnMousePress) {
		selectAll();
		_selectOnMousePress = false;
	}
}
