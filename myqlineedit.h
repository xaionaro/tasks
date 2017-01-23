#ifndef MYQLINEEDIT_H
#define MYQLINEEDIT_H

#include <QLineEdit>

class MyQLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	MyQLineEdit(QWidget *parent);
	virtual ~MyQLineEdit() {}

	//QString textBeforeEdit();

protected:
	void focusInEvent(QFocusEvent *e);
	void mousePressEvent(QMouseEvent *me);

	bool _selectOnMousePress;
	//QString _textBeforeEdit;
};

#endif // MYQLINEEDIT_H
