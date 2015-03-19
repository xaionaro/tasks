#ifndef REDMINEITEMTREEDATA_H
#define REDMINEITEMTREEDATA_H

#include <QHash>
#include <QList>
#include <QJsonObject>

class RedmineItemTreeData
{
public:
    RedmineItemTreeData();

    QList<QJsonObject>  get();
    QJsonObject         get(int item_id);

    QList<QJsonObject>  getchildren(int item_id);
    void                clear();
    void                add(QJsonObject jsonObj);

    // TODO: hierarchy should me moved to private
    QHash<int, QList<QJsonObject>>  hierarchy;

private:

    QList<QJsonObject>                      list;
    QHash<int, QJsonObject>                 id2item;
};

#endif // REDMINEITEMTREEDATA_H
