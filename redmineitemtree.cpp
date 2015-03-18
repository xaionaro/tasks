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

void RedmineItemTree::set(QJsonArray array)
{
    /* refilling this->list */

    this->clear();

    foreach (const QJsonValue &val, array)
        this->add(val.toObject());

    /* rebuilding this->hierarchy */

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
    QList<QJsonObject> item_list = this->real.get();
    QHash<int, bool> filtered_hash;

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

        this->filtered.hierarchy[parent_id].append(item);
    }

    return;
}

void RedmineItemTree::widgetItemResetRecursive(int item_id)
{
    QList<QJsonObject> children = this->filtered.getchildren(item_id);

    foreach (const QJsonObject &child, children) {
        int child_project_id = child["id"].toInt();

        this->widgetItemResetRecursive(child_project_id);

        delete this->id2widgetItem[child_project_id];
        this->id2widgetItem.remove(child_project_id);
    }
}


void RedmineItemTree::widgetItemsResetIfUpdated(int item_id, QJsonObject item)
{
    /*
     *  Remove the project from list if it had been updated
     */

    QJsonObject item_old = this->get(item_id);

    if (item_old["id"].toInt() > 0) // If exists
        if (item_old != item)
            this->widgetItemResetRecursive(item_id);

    return;
}

void RedmineItemTree::display_recursive(QTreeWidgetItem *widgetItem, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject item, int level, QHash <int, bool> &toremove_ids) {
    int item_id = item["id"].toInt();

    toremove_ids.remove(item_id);

    setTextFunct(initiator, widgetItem, item, this, level);

    foreach (const QJsonObject &child, this->filtered.getchildren(item_id)) {
        this->display_child(widgetItem, initiator, setTextFunct, child, level+1, toremove_ids);
    }
}

void RedmineItemTree::display_child(QTreeWidgetItem *parent, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, QJsonObject child, int level, QHash <int, bool> &toremove_ids)
{
    int item_id = child["id"].toInt();
    QTreeWidgetItem *widgetItem;

    this->widgetItemsResetIfUpdated(item_id, child);

    if (this->id2widgetItem.contains(item_id))
        widgetItem = this->id2widgetItem[item_id];
    else {
        widgetItem = new QTreeWidgetItem(parent);
        this->id2widgetItem.insert(item_id, widgetItem);
    }

    this->display_recursive(widgetItem, initiator, setTextFunct, child, level, toremove_ids);
}

void RedmineItemTree::display_topOne(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct, int pos, QHash <int, bool> &toremove_ids)
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
    }

    this->display_recursive(widgetItem, initiator, setTextFunct, item, 0, toremove_ids);
}

void RedmineItemTree::display(QTreeWidget *widget, QWidget *initiator, widgetItemSetTextFunct_t setTextFunct)
{
    qDebug("RedmineItemTree::display()");

    // TODO: uncomment the next line. This function is not thread-safe
    //this->displayMutex.lock();

    /*\
     *  Building a table of items' id
    \*/

    QHash <int, bool> toremove_ids;

    foreach (const QJsonObject &item, this->get())
        toremove_ids.insert(item["id"].toInt(), true);


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

    foreach (const int &item_id, toremove_ids) {
        this->widgetItemResetRecursive(item_id);
    }

    // TODO: uncomment the next line. This function is not thread-safe
    //this->displayMutex.unlock();
}
