#ifndef REDMINEITEMTREE_H
#define REDMINEITEMTREE_H

#include <QMutex>
#include <QJsonArray>
#include <QTreeWidgetItem>
#include <QMutex>

#include <redmineitemtreedata.h>

class RedmineItemTree;

typedef void (*widgetItemSetTextFunct_t)(QWidget *initiator, QTreeWidgetItem *widgetItem, QJsonObject item, RedmineItemTree *tree, int level);
typedef bool (*itemFilterFunct_t)(QWidget *initiator, QJsonObject item);

class RedmineItemTree
{
public:
    RedmineItemTree();

    void                set(QJsonArray array);
    QList<QJsonObject>  get();
    QJsonObject         get(int item_id);
    QJsonObject         get(QTreeWidgetItem *widgetItem);

    QList<QJsonObject>  getchildren(int item_id);

    void                filter(QWidget *initiator, itemFilterFunct_t filterFunct);
    void                display(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct);

private:
    void                clear();
    void                add(QJsonObject jsonObj);

    void                widgetItemResetRecursive(int item_id);
    void                widgetItemReset(QJsonObject item);
    void                widgetItemsResetIfUpdated(int item_id, QJsonObject item);

    void                display_recursive(QTreeWidgetItem *widgetItem, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject item, int level, QHash <int, int> &toremove_ids);
    void                display_child(QTreeWidgetItem *parent, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject child, int level, QHash <int, int> &toremove_ids);
    void                display_topOne(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, int pos, QHash <int, int> &toremove_ids);

    RedmineItemTreeData             real;
    RedmineItemTreeData             filtered;

    QHash<int, QTreeWidgetItem*>            id2widgetItem;
    QHash<int, QJsonObject>                 row2item;
    QHash<QTreeWidgetItem *, QJsonObject>   widgetItem2item;

    QMutex displayMutex;

};

#endif // REDMINEITEMTREE_H
