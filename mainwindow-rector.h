#ifndef MAINWINDOW_RECTOR_H
#define MAINWINDOW_RECTOR_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>
#include <QNetworkReply>
#include <QMap>

#include "mainwindow-common.h"
#include "redmine.h"

namespace Ui
{
class MainWindowRector;
}

class MainWindowRector : public MainWindowCommon
{
	Q_OBJECT
	CALLBACK_DISPATCHER ( Redmine, MainWindowRector, NULL )

public:
	enum EIcon {
		GOOD = 0,
		BAD
	};
	typedef EIcon EStatus;

	EStatus status        ()
	{
		return this->_status;
	}
	EStatus status        ( EStatus newstatus )
	{
		return this->_status = newstatus;
	}
	EStatus statusWorsenTo ( EStatus newstatus )
	{
		return this->_status = qMax ( this->_status, newstatus );
	}

	void setIcon ( EIcon index );
	explicit MainWindowRector ( QWidget *parent = 0 );
	~MainWindowRector();


private slots:
	void on_actionExit_triggered();
	void on_actionHelp_triggered();
	void issues_doubleClick ( int row, int column );
	void toggleShowHide();
	void iconActivated ( QSystemTrayIcon::ActivationReason reason );
	int updateTasks();

	void on_issues_itemSelectionChanged();

	void append_assignee ( QNetworkReply *reply, QJsonDocument *coassignee_doc, void *_arg );

	void sortColumnSwitch ( int columnIdx );

private:
	void issue_add ( QJsonObject issue );
	void issues_clear();
	void issue_display_oneissue ( int pos );
	void issues_display();
	QList<QJsonObject> issues_get();

	Ui::MainWindowRector *ui;

	QComboBox iconComboBox;

	QAction *showHideAction;
	QAction *quitAction;

	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;

	void createIconComboBox();
	void createTrayActions();
	void createTrayIcon();

	void resizeEvent ( QResizeEvent *event );
	void issuesSetup();
	QHash<int, QJsonObject> issue_row2issue;
	QList <QJsonObject> issues_list;
	QTimer *timerUpdateTasks;

	void showOnTop();

	void get_issues_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );

	EStatus _status;

	QMap <int, enum ESortColumn> sortColumnAscByIdx;
	QMap <int, enum ESortColumn> sortColumnDescByIdx;

};

#endif // MAINWINDOW_RECTOR_H
