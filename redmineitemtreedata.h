#ifndef REDMINEITEMTREEDATA_H
#define REDMINEITEMTREEDATA_H

#include <QHash>
#include <QList>
#include <QJsonObject>

class RedmineItemTreeData
{
private:
    QList<QJsonObject>        list;
    QHash<int, QJsonObject>   id2item;

public:
    RedmineItemTreeData();

    QList<QJsonObject>  get();
    QJsonObject         get(int item_id);

    QList<QJsonObject>  getchildren(int item_id);
    void                clear();
    void                add(QJsonObject jsonObj);
    QHash<int, bool>    isToBeUpdated;
    QHash<int, int>     parent;

    // TODO: hierarchy should me moved to private
    QHash<int, QList<QJsonObject>>  hierarchy;

    RedmineItemTreeData &operator= (const RedmineItemTreeData &);
};

#endif // REDMINEITEMTREEDATA_H
