#ifndef ROLES_H
#define ROLES_H

#include <QObject>
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>

class Roles
{
public:
    Roles();

    struct role {
        int     id;
        QString name;
        bool    assignable;
        QHash   <QString, bool> permissions;
    };

    struct role get(int role_id);
    void set(QJsonArray json_array);

private:

    QList <struct role> list;
    QHash <int,     struct role> id2role;
    //QHash <QString, struct role> name2role;

    void add(QJsonObject json_role);
    void clear();

};

#endif // ROLES_H
