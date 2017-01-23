#include "myqlineedit.h"


MyQLineEdit::MyQLineEdit(QWidget *parent) : QLineEdit(parent)
{
	this->_selectOnMousePress = false;
	//this->_textBeforeEdit = "";
}

void MyQLineEdit::focusInEvent(QFocusEvent *e)
{
	QLineEdit::focusInEvent(e);
	selectAll();
	this->_selectOnMousePress = true;
	//this->_textBeforeEdit = this->text();
}

void MyQLineEdit::mousePressEvent(QMouseEvent *me)
{
	QLineEdit::mousePressEvent(me);
	if(_selectOnMousePress) {
		selectAll();
		_selectOnMousePress = false;
	}
}

/*QString MyQLineEdit::textBeforeEdit()
{
	return this->_textBeforeEdit;
}*/
