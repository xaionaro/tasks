#ifndef HTMLDELEGATE_H
#define HTMLDELEGATE_H

#include <QStyledItemDelegate>

class HTMLDelegate : public QStyledItemDelegate
{
public:
	HTMLDelegate();

protected:
	QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	void paint ( QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index ) const;
};

#endif // HTMLDELEGATE_H
