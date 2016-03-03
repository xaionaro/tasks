#ifndef SHOWTIMEWINDOW_H
#define SHOWTIMEWINDOW_H

#include <QWidget>

#include "common.h"
#include "redmineitemtree.h"

namespace Ui {
	class ShowTimeWindow;
}

class ShowTimeWindow : public QWidget
{
	Q_OBJECT
	CALLBACK_DISPATCHER ( Redmine, ShowTimeWindow, this )

public:
	explicit ShowTimeWindow(QWidget *parent = 0);
	~ShowTimeWindow();

	private slots:
	void on_closeButton_clicked();

	void on_date_selectionChanged();

	void on_timeEntries_itemSelectionChanged();

	private:
	Ui::ShowTimeWindow *ui;

	void updateUsers();
	void updateTimeEntries ( int userId );
	void updateUsers_callback       ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	void updateTimeEntries_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );

	//RedmineItemTree users;
	//RedmineItemTree timeEntries;
	QJsonArray timeEntries;
private:
	void timeEntries_display();
};

#endif // SHOWTIMEWINDOW_H
