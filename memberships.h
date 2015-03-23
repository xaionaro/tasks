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
        QList<struct membership_role> roles;
    };

    membership get(int membership_id);
    void set(QJsonArray json_array);

private:

    QList <struct membership> list;
    QHash <int, struct membership> id2membership;

    void add(QJsonObject json_membership);
    void clear();
};

#endif // MEMBERSHIPS_H
