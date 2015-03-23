#include "roles.h"

Roles::Roles()
{
}

struct Roles::role Roles::get(int role_id)
{
    return this->id2role[role_id];
}

void Roles::clear()
{
    this->id2role.clear();
    this->list.clear();
}

void Roles::add(QJsonObject json_role)
{
    struct role role;

    role.id         = json_role["id"].toInt();
    role.name       = json_role["name"].toString();
    role.assignable = json_role["assignable"].toBool();

    QJsonArray json_permissions = json_role["permissions"].toArray();

    role.permissions.clear();
    foreach (const QJsonValue &perm_val, json_permissions)
        role.permissions.insert(perm_val.toString(), true);

    this->list.append(role);
    this->id2role.insert(role.id, role);
}

void Roles::set(QJsonArray json_array)
{
    this->clear();

    foreach (const QJsonValue &val, json_array)
        this->add(val.toObject());
}
