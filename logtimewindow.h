#ifndef LOGTIMEWINDOW_H
#define LOGTIMEWINDOW_H

#include <QScrollArea>
#include <QComboBox>

#include "common.h"
#include "redmineitemtree.h"

#include "redmineclass_time_entry.h"


namespace Ui
{
class LogTimeWindow;
}

class LogTimeWindow : public QScrollArea
{
	Q_OBJECT
	CALLBACK_DISPATCHER ( Redmine, LogTimeWindow, this )

public:
	explicit LogTimeWindow ( QWidget *parent = 0 );
	~LogTimeWindow();

	QList<QJsonObject> issues_get_byProjectId ( int project_id );

	/*
	 *  TODO: the next public stuff (but not slots) should be moved to
	 *  the protected section:
	 */
	RedmineItemTree projects;
	RedmineItemTree issues;
	int selected_project_id;
	QHash<int, QList<QJsonObject>> issues_byProjectId;
	QHash<int, QList<QJsonObject>> issuesFiltered_byProjectId;

signals:
	void on_destructor();

private slots:
	void on_cancel_clicked();
	void on_accept_clicked();
	void on_issue_itemClicked ( QTreeWidgetItem *item, int column );
	void on_issue_itemSelectionChanged();
	void on_project_currentIndexChanged ( int index );
	void on_issue_doubleClicked ( const QModelIndex &index );

	void on_comment_editingFinished();
	void on_saveSuccess();
	void on_saveTimeout();
	void on_saveFailure ( QNetworkReply *reply );

    void on_continousLoggingStartButton_clicked();

    void on_continousLoggingStopButton_clicked();

	void on_issue_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	void on_comment_textEdited(const QString &arg1);


	void on_comment_textChanged(const QString &arg1);

	void updateUntilTime();

private:
	int updateLastLogTime();
	void get_time_entries_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	QMutex updateProjectsMutex;
	QMutex updateIssuesMutex;

	int updateIssues();
	int updateProjects();

	void get_issues_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	void issues_display();
	void setIssuesFilterItems ( QComboBox *box, QHash<int, QJsonObject> table_old, QHash<int, QJsonObject> table, QString keyname );

	void get_projects_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
	void projects_display();

	void saveCurrentEntry();

	QMutex projectsDisplayMutex;
	QMutex projectsDisplayExceptionMutex;

	QTimer projectsDisplayRetryTimer;
	QTimer *continousLoggingTimer;

	QHash<int, int> selected_issues_id;
	RedmineClass_TimeEntry timeEntry;

	Ui::LogTimeWindow *ui;
};

#endif // LOGTIMEWINDOW_H
