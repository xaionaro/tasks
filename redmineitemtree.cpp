/*
    mephi-tasks — a client to NRNU MEPhI Redmine server

    Copyright (C) 2015  Dmitry Yu Okunev <dyokunev@ut.mephi.ru> 0x8E30679C

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "redmineitemtree.h"

RedmineItemTree::RedmineItemTree()
{
    this->displayRetryTimer.setSingleShot(true);
    connect(&this->displayRetryTimer, SIGNAL(timeout()), this, SLOT(display_retry()));

    this->filterRetryTimer.setSingleShot(true);
    connect(&this->filterRetryTimer,  SIGNAL(timeout()), this, SLOT(filter_retry()));
}

void RedmineItemTree::clear()
{
    this->real.clear();
}

void RedmineItemTree::add(QJsonObject jsonObj)
{
    this->real.add(jsonObj);
}

QList<QJsonObject> RedmineItemTree::get()
{
    return this->real.get();
}
QJsonObject RedmineItemTree::get(int item_id)
{
    return this->real.get(item_id);
}
QJsonObject RedmineItemTree::get(QTreeWidgetItem *widgetItem)
{
    return this->widgetItem2item[widgetItem];
}

bool RedmineItemTree::isDescendant(int descendant_id, int ancestor_id){
    QJsonObject descendant = this->get(descendant_id);
    int parent_id = 0;

    //qDebug("RedmineItemTree::isDescendant(%i, %i)", descendant_id, ancestor_id);

    while (true) {
        parent_id  = descendant["parent"].toObject()["id"].toInt();
        if (parent_id == 0)
            break;

        //qDebug("%i <> %i", parent_id, ancestor_id);

        if (parent_id == ancestor_id)
            return true;

        descendant = this->get(parent_id);
    };

    return false;
}

void RedmineItemTree::set(QJsonArray array)
{
    /* refilling this->list */

    this->clear();

    foreach (const QJsonValue &val, array)
        this->add(val.toObject());

    /* rebuilding this->real.hierarchy */

    QList<QJsonObject> item_list;

    item_list = this->get();

    foreach (const QJsonObject &item, item_list) {
        int parent_id;

        if (item.contains("parent"))
            parent_id = item["parent"].toObject()["id"].toInt();
        else
            parent_id = 0;

        this->real.hierarchy[parent_id].append(item);
    }

    return;
}

void RedmineItemTree::filter(QWidget *initiator, itemFilterFunct_t filterFunct)
{
    if (!this->displayMutex.tryLock()) {
        if (!this->displayExceptionMutex.tryLock())
            return;

        this->filterRetryArgs.initiator   = initiator;
        this->filterRetryArgs.filterFunct = filterFunct;

        this->filterRetryTimer.start(70);

        this->displayExceptionMutex.unlock();
        return;
    }

    QList<QJsonObject> item_list = this->real.get();
    QHash<int, bool> filtered_hash;

    this->filtered_old = this->filtered;
    this->filtered.clear();

    foreach (const QJsonObject &item, item_list)
        if (filterFunct(initiator, item)) {
            this->filtered.add(item);
            filtered_hash.insert(item["id"].toInt(), true);
        }

    QList<QJsonObject> filtered_list = this->filtered.get();

    foreach (const QJsonObject &item, filtered_list) {
        int parent_id;

        if (item.contains("parent"))
            parent_id = item["parent"].toObject()["id"].toInt();
        else
            parent_id = 0;

        while (!filtered_hash.contains(parent_id)) {
            QJsonObject parent = this->real.get(parent_id)["parent"].toObject();

            if (parent.contains("id"))
                parent_id = parent["id"].toInt();
            else
                parent_id = 0;

            if (parent_id == 0)
                break;
        }

        this->filtered.parent.insert(item["id"].toInt(), parent_id);
        this->filtered.hierarchy[parent_id].append(item);
    }

    this->displayMutex.unlock();
    return;
}

void RedmineItemTree::filter_retry()
{
    this->filter(this->filterRetryArgs.initiator,
                 this->filterRetryArgs.filterFunct);

    return;
}

void RedmineItemTree::widgetItemReset(int item_id)
{

    if (!this->id2widgetItem.contains(item_id))
        return;

    QTreeWidgetItem *widgetItem = this->id2widgetItem[item_id];
    this->id2widgetItem.remove(item_id);
    this->widgetItem2item.remove(widgetItem);

    //this->filtered.removechild(item_id);

    return;
}

void RedmineItemTree::widgetItemResetRecursive(int item_id)
{
    QList<QJsonObject> children = this->real.getchildren(item_id);

    if (!this->id2widgetItem.contains(item_id))
        return;


    foreach (const QJsonObject &child, children) {
        int child_id = child["id"].toInt();

        this->widgetItemResetRecursive(child_id);
        this->widgetItemReset(child_id);
    }

    QTreeWidgetItem *widgetItem = this->id2widgetItem[item_id];

    this->widgetItemReset(item_id);

    delete widgetItem;

    return;
}


void RedmineItemTree::widgetItemsResetIfUpdated(int item_id, QJsonObject item)
{
    /*
     *  Remove the project from list if it had been updated
     */

    QJsonObject item_old      = this->get(item_id);
    int         parent_id     = this->filtered.parent[item_id];
    int         parent_old_id = this->filtered_old.parent[item_id];

    if (item_old["id"].toInt() > 0) // If exists
        if ((item_old != item) || (parent_id != parent_old_id))   // If changed
            this->widgetItemResetRecursive(item_id);

    return;
}

void RedmineItemTree::display_recursive(QTreeWidgetItem *widgetItem, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject item, int level, QHash<int, int> &toremove_ids) {
    int item_id = item["id"].toInt();

    toremove_ids.remove(item_id);

    setTextFunct(initiator, widgetItem, item, this, level);

    foreach (const QJsonObject &child, this->filtered.getchildren(item_id)) {
        this->display_child(widgetItem, initiator, setTextFunct, child, level+1, toremove_ids);
    }
}

void RedmineItemTree::display_child(QTreeWidgetItem *parent, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject child, int level, QHash<int, int> &toremove_ids)
{
    int item_id = child["id"].toInt();
    QTreeWidgetItem *widgetItem;

    this->widgetItemsResetIfUpdated(item_id, child);

    if (this->id2widgetItem.contains(item_id))
        widgetItem = this->id2widgetItem[item_id];
    else {
        widgetItem = new QTreeWidgetItem(parent);
        this->id2widgetItem.insert(item_id, widgetItem);
        this->widgetItem2item.insert(widgetItem, child);
    }

    this->display_recursive(widgetItem, initiator, setTextFunct, child, level, toremove_ids);
}

void RedmineItemTree::display_topOne(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, int pos, QHash<int, int> &toremove_ids)
{
    QJsonObject item    = this->row2item[pos];
    int         item_id = item["id"].toInt();
    QTreeWidgetItem *widgetItem;

    this->widgetItemsResetIfUpdated(item_id, item);

    if (this->id2widgetItem.contains(item_id))
        widgetItem = this->id2widgetItem[item_id];
    else {
        widgetItem = new QTreeWidgetItem(widget);
        this->id2widgetItem.insert(item_id, widgetItem);
        this->widgetItem2item.insert(widgetItem, item);
    }

    this->display_recursive(widgetItem, initiator, setTextFunct, item, 0, toremove_ids);
}

void RedmineItemTree::display(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct)
{
    //qDebug("RedmineItemTree::display()");
    if (!this->displayMutex.tryLock()) {
        if (!this->displayExceptionMutex.tryLock())
            return;

        this->displayRetryArgs.widget       = widget;
        this->displayRetryArgs.initiator    = initiator;
        this->displayRetryArgs.setTextFunct = setTextFunct;

        this->displayRetryTimer.start(100);

        this->displayExceptionMutex.unlock();
        return;
    }

    /*\
     *  Preparing
    \*/

    widget->setSortingEnabled(false);

    /*\
     *  Building a table of items' id
    \*/

    QHash <int, int> toremove_ids;

    foreach (const QJsonObject &item, this->widgetItem2item)
        toremove_ids.insert(item["id"].toInt(), item["id"].toInt());


    /*\
     *  Displaying new items
    \*/

    this->row2item.clear();
    int topitems_count = 0;

    foreach (const QJsonObject &item, this->filtered.getchildren(0)) {
        this->row2item.insert(topitems_count, item);
        this->display_topOne(widget, initiator, setTextFunct, topitems_count, toremove_ids);

        topitems_count++;
    }

    /*\
     *  Removing stale items
    \*/

    foreach (const int &item_id, toremove_ids)
        this->widgetItemResetRecursive(item_id);


    /*\
     *  Redisplaying lost items (due to ascentor removing)
    \*/

    topitems_count = 0;
    foreach (const QJsonObject &item, this->filtered.getchildren(0))
        this->display_topOne(widget, initiator, setTextFunct, topitems_count++, toremove_ids);


    /*\
     *  Finishing
    \*/

    widget->setSortingEnabled(true);

    this->displayMutex.unlock();
    return;
}



void RedmineItemTree::display(QComboBox *widget, QWidget *initiator, comboBoxAddItemFunct_t addItemFunct)
{
    qDebug("RedmineItemTree::display()");
    this->displayMutex.lock();

    /*\
     *  Building a table of items' id
    \*/

    QHash <int, int> toremove_ids;

    foreach (const QJsonObject &item, this->widgetItem2item)
        toremove_ids.insert(item["id"].toInt(), item["id"].toInt());


    /*\
     *  Displaying new items
    \*/

    int itemIdx = 0;
    this->row2item.clear();

    foreach (const QJsonObject &item, this->filtered.get()) {
        this->row2item.insert(itemIdx++, item);
        addItemFunct(widget, item);
        // display item
        //this->display_topOne(widget, initiator, setTextFunct, topitems_count, toremove_ids);
    }

    /*\
     *  Removing stale items
    \*/

    foreach (const int &item_id, toremove_ids)
        this->widgetItemResetRecursive(item_id);

    /*\
     *  Finishing
    \*/

    this->displayMutex.unlock();
    return;
}


void RedmineItemTree::display_retry()
{
    this->display(this->displayRetryArgs.widget,
                  this->displayRetryArgs.initiator,
                  this->displayRetryArgs.setTextFunct);

    return;
}
