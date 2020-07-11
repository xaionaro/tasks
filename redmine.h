#ifndef REDMINE_H
#define REDMINE_H

#include <qtredmine/RedmineClient.hpp>
#include <QEventLoop>

#define REDMINE_BASETIMEOUT 5000

class Redmine : public RedmineClient
{
	Q_OBJECT
	CALLBACK_DISPATCHER ( RedmineClient, Redmine, this )


private:
	QString _apiKey;
	QHash <int, QJsonDocument> users;
	QHash <int, QJsonObject>   issue_statuses;

	void   set_issue_status ( int status_id, QJsonObject status );
	void clear_issue_status();

	// Callbacks:
	void updateIssueStatuses_callback ( QNetworkReply *reply,
	                                    QJsonDocument *statuses,
	                                    void *_arg );

	void get_user_callback ( QNetworkReply *reply,
	                         QJsonDocument *user_doc,
	                         void *_arg );

	QEventLoop initBarrier;
	int        initBarrier_jobsDone;

	QNetworkReply *updateMe ( callback_t callback = NULL, void *arg = NULL );
	QNetworkReply *updateMyProject ( callback_t callback = NULL, void *arg = NULL );
	QNetworkReply *updateIssueStatuses ( Redmine::callback_t callback = NULL, void *arg = NULL );

	QJsonObject _me;
	QJsonObject _myProject;

	void updateMe_callback ( QNetworkReply *reply, QJsonDocument *me_doc, void *_arg );
	void updateMyProject_callback ( QNetworkReply *reply, QJsonDocument *myProject_doc, void *_arg );
	/*
	void init_quitStatuses (QNetworkReply *reply, QJsonDocument *statuses,  void *_null);
	void init_quitMe       (QNetworkReply *reply, QJsonDocument *me,        void *_null);
	void init_quitMyProject(QNetworkReply *reply, QJsonDocument *myProject, void *_null);
	*/
	QHash<QString, QJsonDocument> cache;

	void callback_cache ( QNetworkReply *reply, QJsonDocument *obj, void *_real_callback_info );

	QString cacheBasePath;

public:

	QString apiKey ( QString apiKey );
	QString apiKey();

	int init();

	/* Request anything by URI
	 */
	QNetworkReply *request ( RedmineClient::EMode    mode,
	                         QString                 uri,
	                         void                   *obj_ptr,
	                         callback_t              callback,
	                         void                   *callback_arg = NULL,
	                         bool                    free_arg     = false,
	                         const QString          &getParams    = "",
	                         const QByteArray       &requestData  = "",
	                         bool                    useCache     = true );

	QNetworkReply *request ( RedmineClient::EMode    mode,
	                         QString                 uri,
	                         void                   *obj_ptr,
	                         callback_t              callback,
	                         void                   *callback_arg,
	                         bool                    free_arg,
	                         const QString          &getParams,
	                         const QJsonObject      &requestJSON );

	QNetworkReply *request ( RedmineClient::EMode    mode,
	                         QString                 uri,
	                         void                   *obj_ptr,
	                         callback_t              callback,
	                         void                   *callback_arg,
	                         bool                    free_arg,
	                         const QString          &getParams,
	                         const QVariantMap      &requestVMap );

	/* Get information about current user
	 */
	QJsonObject me()
	{
		return _me;
	}
	QJsonObject myProject()
	{
		return _myProject;
	}

	/* Request all roles
	 */
	QNetworkReply *get_roles ( callback_t callback, void *arg = NULL, bool free_arg = false );

	/* Request all issues
	 */
	QNetworkReply *get_issues ( callback_t callback, void *arg = NULL, bool free_arg = false, QString customFilters = "" );
	QNetworkReply *get_issues ( void *obj_ptr, callback_t callback, void *arg = NULL, bool free_arg = false, QString customFilters = "" );

	/* Request all projects
	 */
	QNetworkReply *get_projects ( callback_t callback, void *arg = NULL, bool free_arg = false, QString filterOptions = "" );
	QNetworkReply *get_projects ( void *obj_ptr, callback_t callback, void *arg = NULL, bool free_arg = false, QString filterOptions = "" );

	/* Request all memberships
	 */
	QNetworkReply *get_memberships ( callback_t callback, void *arg = NULL, bool free_arg = false );

	/* Request all field values enumerations
	 */
	QNetworkReply *get_enumerations ( callback_t callback, void *arg = NULL, bool free_arg = false );

	/* Request all issues
	 */
	QNetworkReply *get_time_entries ( int userId, void *obj_ptr, callback_t callback, void *arg = NULL, bool free_arg = false, QString filterOptions = "" );
	QNetworkReply *get_time_entries ( void *obj_ptr, callback_t callback, void *arg = NULL, bool free_arg = false, QString filterOptions = "" );
	QNetworkReply *get_time_entries ( callback_t callback, void *arg = NULL, bool free_arg = false, QString filterOptions = "" );

	/* Get issue status info
	 */
	QJsonObject get_issue_status ( int issue_status_id );
	QJsonObject get_issue_status ( QJsonValueRef issues_status_json );
	QHash<int, QJsonObject> get_available_statuses_for ( int issue_id );

	/* Request user info (with caching). If 2nd argument to callback function
	 * is NULL then a cached value is passed.
	 */
	QNetworkReply *get_user ( int user_id,
	                          callback_t callback,
				  void *arg = NULL );

	/* Request stuffToDo index info of user with ID "user_id".
	 * Set "user_id" to zero to get info for current user.
	 *
	 * Related to plugin: https://github.com/mephi-ut/stuff_to_do_plugin
	 */
	QNetworkReply *get_stuff_to_do ( void *obj_ptr, callback_t callback,
					 int user_id = 0,
					 void *arg = NULL, bool free_arg = false,
					 QString filterOptions = "" );

	/* Parses JSON values like "2015-03-13T21:34:28.000+03:00" to QDateTime
	 */
	QDateTime parseDateTime ( QJsonValueRef dateTime_json );
	QDateTime parseDateTime ( QString dateTime_str );

	/* Load prevously saved cache into memory (disk -> mem)
	 */
	void cacheLoad();

	/* Save cache into disk (mem -> disk)
	 */
	void cacheSave();

	/*
	 */
	QUrl getUrl ( QString objectType, int objectId );

	/* Constructor/destructor
	 */

    Redmine(QString serverURL);
	~Redmine();
};

#endif // REDMINE_H
