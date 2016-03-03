#ifndef MAINWINDOWCOMMON_H
#define MAINWINDOWCOMMON_H

#include <QMainWindow>
#include <QTableWidgetSelectionRange>
#include <QMutex>
#include <QSystemTrayIcon>

#include "redmineitemtree.h"
#include "roles.h"
#include "memberships.h"
#include "enumerations.h"
#include "logtimewindow.h"
#include "showtimewindow.h"

#include "common.h"

class MainWindowCommon : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindowCommon ( QWidget *parent = 0 );
	~MainWindowCommon();

	QList<QJsonObject> issues_get_byProjectId ( int project_id );

	/*
	 *  TODO: the next public stuff (but not slots) should be moved to
	 *  the protected section:
	 */
	RedmineItemTree projects;
	RedmineItemTree issues;

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

protected:

	static const int SORT_DEPTH = 3;

	enum ESortColumn {
		SORT_UNDEFINED = 0,

		SORT_NAME_ASC,
		SORT_NAME_DESC,

		SORT_ASSIGNEE_ASC,
		SORT_ASSIGNEE_DESC,

		SORT_DUE_TO_ASC,
		SORT_DUE_TO_DESC,

		SORT_STATUS_POS_ASC,
		SORT_STATUS_POS_DESC,

		SORT_UPDATED_ON_ASC,
		SORT_UPDATED_ON_DESC,

		SORT_STATUS_ISCLOSED_ASC,
	};

	typedef bool ( *sortfunct_t ) ( const QJsonObject &issue_a, const QJsonObject &issue_b );

	// TODO: deduplicate sortLogicalIndex+sortOrder and sortColumn
	int sortLogicalIndex = -1;
	Qt::SortOrder sortOrder;

	enum ESortColumn sortColumn[SORT_DEPTH];
	QMap <enum ESortColumn, sortfunct_t> sortFunctMap;

	/*
	 *  TODO: the next protected stuff (but not slots) should be moved to
	 *  the private section:
	 */
	QHash<int, QList<QJsonObject>> issues_byProjectId;

	Roles roles;
	Memberships memberships;
	Enumerations enumerations;

	QComboBox iconComboBox;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;

	void createIconComboBox();
	void createTrayIcon();

	void showOnTop();

	//virtual void createTrayActions();

protected slots:
	int updateEnumerations();
	int updateMemberships();
	int updateProjects();
	int updateIssues();
	int updateRoles();
	void openLogTimeWindow();
	void on_closeLogTimeWindow();
	void openShowTimeWindow();
	void on_closeShowTimeWindow();

signals:

protected slots:
	void toggleShowHide();

public slots:

private:
	/* projects */

	virtual void projects_display();
	virtual void issues_display();
	void get_enumerations_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	void get_memberships_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	void get_projects_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	void get_issues_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	void get_roles_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );


	QMutex updateProjectsMutex;
	QMutex updateIssuesMutex;


	EStatus _status;
	LogTimeWindow  *logTimeWindow;
	ShowTimeWindow *showTimeWindow;
};

#endif // MAINWINDOWCOMMON_H
