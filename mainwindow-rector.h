#ifndef MAINWINDOW_RECTOR_H
#define MAINWINDOW_RECTOR_H

#include <QMainWindow>
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

	explicit MainWindowRector ( QWidget *parent = 0 );
	~MainWindowRector();

protected slots:
	void toggleShowHide();

private slots:
	void on_actionExit_triggered();
	void on_actionHelp_triggered();
	void issues_doubleClick ( int row, int column );
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

	QAction *showHideAction;
	QAction *quitAction;

	void createTrayActions();

	void resizeEvent ( QResizeEvent *event );
	void issuesSetup();
	QHash<int, QJsonObject> issue_row2issue;
	QList <QJsonObject> issues_list;
	QTimer *timerUpdateTasks;

	void get_issues_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );

	QMap <int, enum ESortColumn> sortColumnAscByIdx;
	QMap <int, enum ESortColumn> sortColumnDescByIdx;

	void iconActivated ( QSystemTrayIcon::ActivationReason reason );

};

#endif // MAINWINDOW_RECTOR_H
