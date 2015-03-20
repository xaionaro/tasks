#ifndef REDMINEITEMTREE_H
#define REDMINEITEMTREE_H

#include <QObject>
#include <QMutex>
#include <QJsonArray>
#include <QTreeWidgetItem>
#include <QMutex>
#include <QTimer>

#include <redmineitemtreedata.h>

class RedmineItemTree;

typedef void (*widgetItemSetTextFunct_t)(QWidget *initiator, QTreeWidgetItem *widgetItem, QJsonObject item, RedmineItemTree *tree, int level);
typedef bool (*itemFilterFunct_t)(QWidget *initiator, QJsonObject item);

class RedmineItemTree : public QObject
{
    Q_OBJECT

public:
    RedmineItemTree();

    void                set(QJsonArray array);
    QList<QJsonObject>  get();
    QJsonObject         get(int item_id);
    QJsonObject         get(QTreeWidgetItem *widgetItem);
    QList<QJsonObject>  getchildren(int item_id);
    bool                isDescendant(int descendant_id, int ancestor_id);
    void                filter(QWidget *initiator, itemFilterFunct_t filterFunct);
    void                display(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct);

    RedmineItemTreeData filtered;


public slots:
    void                display_retry();
    void                filter_retry();

private:
    void                clear();
    void                add(QJsonObject jsonObj);

    void                widgetItemResetRecursive(int item_id);
    void                widgetItemReset(int item_id);
    void                widgetItemsResetIfUpdated(int item_id, QJsonObject item);

    void                display_recursive(QTreeWidgetItem *widgetItem, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject item, int level, QHash <int, int> &toremove_ids);
    void                display_child(QTreeWidgetItem *parent, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject child, int level, QHash <int, int> &toremove_ids);
    void                display_topOne(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, int pos, QHash <int, int> &toremove_ids);

    RedmineItemTreeData             real;
    RedmineItemTreeData             filtered_old;

    QHash<int, QTreeWidgetItem*>            id2widgetItem;
    QHash<int, QJsonObject>                 row2item;
    QHash<QTreeWidgetItem *, QJsonObject>   widgetItem2item;

    struct {
        QTreeWidget *widget; QWidget *initiator; widgetItemSetTextFunct_t setTextFunct;
    } displayRetryArgs;

    struct {
        QWidget *initiator; itemFilterFunct_t filterFunct;
    } filterRetryArgs;

    QMutex displayMutex;
    QMutex displayExceptionMutex;

    QTimer displayRetryTimer;
    QTimer filterRetryTimer;

};

#endif // REDMINEITEMTREE_H
