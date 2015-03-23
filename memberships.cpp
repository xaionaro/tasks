#include "memberships.h"

Memberships::Memberships()
{
}

struct Memberships::membership Memberships::get(int membership_id)
{
    return this->id2membership[membership_id];
}

QList<struct Memberships::membership> Memberships::get_byproject(int project_id)
{
    return this->projectId2membership[project_id];
}

void Memberships::clear()
{
    this->projectId2membership.clear();
    this->id2membership.clear();
    this->list.clear();
}

void Memberships::add(QJsonObject json_membership)
{
    struct membership membership;

    membership.id         = json_membership["id"].toInt();
    membership.project_id = json_membership["project"].toObject()["id"].toInt();
    membership.user_id    = json_membership["user"]   .toObject()["id"].toInt();
    membership.user_name  = json_membership["user"]   .toObject()["name"].toString();

    QJsonArray json_roles = json_membership["roles"].toArray();

    membership.roles.clear();
    foreach (const QJsonValue &role_val, json_roles) {
        QJsonObject role = role_val.toObject();
        struct membership_role membership_role;

        membership_role.role_id     = role["id"]       .toInt();
        membership_role.isInherited = role["inherited"].toBool();

        membership.roles.append(membership_role);
    }

    this->list.append(membership);
    this->id2membership.insert(membership.id, membership);
    this->projectId2membership[membership.project_id].append(membership);
}

void Memberships::set(QJsonArray json_array)
{
    this->clear();

    foreach (const QJsonValue &val, json_array)
        this->add(val.toObject());
}
