#ifndef MEMBERSHIPS_H
#define MEMBERSHIPS_H

#include <QObject>
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>

class Memberships
{
public:
	Memberships();

	struct membership_role {
		int  role_id;
		bool isInherited;
	};

	struct membership {
		int                           id;
		int                           project_id;
		int                           user_id;
		QString                       user_name;
		QList<struct membership_role> roles;
	};

	struct membership get ( int membership_id );
	QList<struct membership> get_byproject ( int project_id );
	void set ( QJsonArray json_array );

private:

	QList <struct membership> list;
	QHash <int, struct membership> id2membership;
	QHash <int, QList<struct membership>> projectId2membership;

	void add ( QJsonObject json_membership );
	void clear();
};

#endif // MEMBERSHIPS_H
