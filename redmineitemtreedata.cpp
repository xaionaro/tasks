#include "redmineitemtreedata.h"

RedmineItemTreeData::RedmineItemTreeData()
{
}

void RedmineItemTreeData::clear()
{
    this->list.clear();
    this->id2item.clear();
    this->hierarchy.clear();
}

void RedmineItemTreeData::add(QJsonObject jsonObj)
{
    this->list.append(jsonObj);
    this->id2item.insert(jsonObj["id"].toInt(), jsonObj);
}

QList<QJsonObject> RedmineItemTreeData::get()
{
    return this->list;
}
QJsonObject RedmineItemTreeData::get(int item_id)
{
    return this->id2item[item_id];
}

QList<QJsonObject> RedmineItemTreeData::getchildren(int item_id)
{
    return this->hierarchy[item_id];
}
