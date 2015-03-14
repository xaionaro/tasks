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

#include "mainwindow.h"
#include "helpwindow.h"
#include "ui_mainwindow.h"
#include "common.h"

#include <QStringList>
#include <QDesktopServices>
#include <QDateTime>
#include <QMenu>
#include <QList>

void MainWindow::issuesSetup()
{
    QTableWidget *issues = ui->issues;

    // Columns:

    QStringList columns;
    QSize itemSize;

    columns << "Название" << "Исполнитель" << "Срок" << "Статус";

    issues->setColumnCount(columns.size());

    issues->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    issues->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    issues->horizontalHeader()->resizeSection(1, 150);

    issues->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    issues->horizontalHeader()->resizeSection(2, 100);

    issues->setHorizontalHeaderLabels(columns);

    //issues->verticalHeader()->setDefaultSectionSize(18);
    issues->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Signals:

    connect(issues, SIGNAL(cellDoubleClicked(int, int)),
            this,   SLOT(issues_doubleClick(int, int)));
    return;
}

void MainWindow::issues_doubleClick(int row, int column)
{
    (void)column;
    QString url = QString(SERVER_URL "/issues/%1").arg(this->issue_row2issue[row]["id"].toInt());

    QDesktopServices::openUrl(url);
    return;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*qDebug("MainWindow::MainWindow(): %p", this);*/

    this->setWindowTitle("Система «Задачи» НИЯУ МИФИ: Поручения ректора");

    //Qt::WindowFlags flags = this->windowFlags();
    //this->setWindowFlags(flags | Qt::WindowStaysOnTopHint);

    connect(redmine, SIGNAL(callback_call      (void*,callback_t,QNetworkReply*,QJsonDocument*,void*)),
            this,    SLOT(  callback_dispatcher(void*,callback_t,QNetworkReply*,QJsonDocument*,void*)) );

    this->issuesSetup();

    this->updateTasks();
    this->createIconComboBox();
    this->createTrayActions();
    this->createTrayIcon();

    this->status(GOOD);
    this->setIcon(GOOD);

    this->trayIcon->show();

    this->timerUpdateTasks = new QTimer(this);
    connect(this->timerUpdateTasks, SIGNAL(timeout()), this, SLOT(updateTasks()));
    this->timerUpdateTasks->start(10000);

    return;
}

struct append_assignee_arg {
    int   pos;
};

void MainWindow::append_assignee(QNetworkReply *reply, QJsonDocument *coassignee_doc, void *_arg) {
    (void)reply;
    struct append_assignee_arg *arg = (struct append_assignee_arg *)_arg;
    QJsonObject coassignee = coassignee_doc->object()["user"].toObject();

    QTableWidget *issues = this->ui->issues;
    int              pos = arg->pos;

    /*qDebug("pos: %i; answer is: %s", pos, coassignee_doc->toJson().data());*/

    QString firstname = coassignee["firstname"].toString();
    QString lastname  = coassignee["lastname"].toString();
    QString initials  = firstname.left(1) + " " + firstname.left(firstname.indexOf(" ")+2).right(1); // TODO: Move this to class Redmine

    QTableWidgetItem *item = issues->item(pos, 1);
    item->setText(
                item->text() + "\n  " +
                lastname + " " + initials
            );

    delete arg;
    return;
}

void MainWindow::issues_clear()
{
    /*qDebug("MainWindow::issues_clear(): %p", this);*/
    QTableWidget *issues = this->ui->issues;
    int row_count= issues->rowCount();

    while (row_count-- > 0)
    {
        issues->removeRow(row_count);
    }

    this->issue_row2issue.clear();
    return;
}

void MainWindow::issue_set(int pos, QJsonObject issue)
{
    QTableWidget     *issues = this->ui->issues;
    QTableWidgetItem *item;
    QJsonObject       issue_status = issue["status"].toObject();
    bool              isClosed = redmine->get_issue_status(issue_status["id"].toInt())["is_closed"].toBool();
    QColor            closedBgColor = QColor(192, 255, 192);

    //qDebug("Issue: #%i:\t%s", issue["id"].toInt(), issue["subject"].toString().toUtf8().data());

    // New row:
    issues->insertRow(pos);

    //     Issue name:
    item = new QTableWidgetItem(issue["subject"].toString());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    if (isClosed) item->setBackgroundColor(closedBgColor);
    issues->setItem(pos, 0, item);

    //     Assignee:
    //         Assignee:
    QString assignee_str    = issue["assigned_to"].toObject()["name"].toString();
    //         Setting the assignee value:
    item = new QTableWidgetItem(assignee_str);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    if (isClosed) item->setBackgroundColor(closedBgColor);
    issues->setItem(pos, 1, item);
    item = issues->item(pos, 1);
    //         Co-assignees (asynchronous):
    QJsonArray customFields = issue["custom_fields"].toArray();
    foreach (const QJsonValue &customField, customFields) {
        if (customField.toObject()["name"].toString() == "Соисполнители") {
            QJsonArray coassignees_id_obj = customField.toObject()["value"].toArray();
            foreach (const QJsonValue &coassignee_id_obj, coassignees_id_obj) {
                // Don't try to use .toInt() directly, the answer will always be "0":
                int coassignee_id = coassignee_id_obj.toString().toInt();

                struct append_assignee_arg *append_assignee_arg_p;
                append_assignee_arg_p = new struct append_assignee_arg;
                // TODO: fix a memleak if redmine->get_user doesn't success

                append_assignee_arg_p->pos     = pos;

                redmine->get_user(coassignee_id, (Redmine::callback_t)&MainWindow::append_assignee, (void *)append_assignee_arg_p);
            }

            break;
        }
    }

    //     Due date:
    QString due_date_str = issue["due_date"].toString();
    QDateTime now, date;
    now  = QDateTime::currentDateTime();
    date = QDateTime::fromString(due_date_str, "yyyy-MM-dd");

    item = new QTableWidgetItem(due_date_str);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    if (isClosed) item->setBackgroundColor(closedBgColor);
    if ((due_date_str != "") && (now.toTime_t() - (3600*24-1) > date.toTime_t()) && (!isClosed)) {
        item->setBackgroundColor(QColor(255, 192, 192));
        this->statusWorsenTo(BAD);
    }
    issues->setItem(pos, 2, item);

    //     Status:
    item = new QTableWidgetItem(issue_status["name"].toString());
    if (isClosed) item->setBackgroundColor(closedBgColor);
    issues->setItem(pos, 3, item);

    this->issue_row2issue.insert(pos, issue);
    return;
}

bool issueCmpFunct_statusIsClosed_lt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    int issue_statusIsClosed_a = redmine->get_issue_status(issue_a["status"].toObject()["id"].toInt())["is_closed"].toBool();
    int issue_statusIsClosed_b = redmine->get_issue_status(issue_b["status"].toObject()["id"].toInt())["is_closed"].toBool();

    return issue_statusIsClosed_a < issue_statusIsClosed_b;
}

void MainWindow::get_issues_callback(QNetworkReply *reply, QJsonDocument *json, void *arg) {
    (void)reply; (void)arg;
    /*qDebug("MainWindow::get_issues_callback(): %p %p", this, arg);*/
    //MainWindow *win = static_cast<MainWindow *>(_win);
    QList<QJsonObject> issues_list;

    if (this->status() == MainWindow::BAD)
        this->status(MainWindow::GOOD);

    this->issues_clear();

    int issues_count = 0;

    QJsonObject answer = json->object();
    QJsonArray  issues = answer["issues"].toArray();

    foreach (const QJsonValue &issue, issues)
        issues_list.append(issue.toObject());

    qSort(issues_list.begin(), issues_list.end(), issueCmpFunct_statusIsClosed_lt);

    foreach (const QJsonObject &issue, issues_list)
        this->issue_set(issues_count++, issue);

    this->setIcon(this->status());
    return;
}

int MainWindow::updateTasks() {
    redmine->get_issues((Redmine::callback_t)&MainWindow::get_issues_callback, this);
    return 0;
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete this->ui;
    delete this->timerUpdateTasks;
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionHelp_triggered()
{
    HelpWindow *win = new HelpWindow();
    win->show();
    return;
}

void MainWindow::showOnTop() {
#ifdef Q_OS_WIN32
    // raise() doesn't work :(

    Qt::WindowFlags flags_old   = this->windowFlags();
    Qt::WindowFlags flags_ontop = flags_old | Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags_ontop);
    this->show();
    this->setWindowFlags(flags_old);
    this->show();
#else
    this->show();
    this->raise();
#endif

    return;
}

void MainWindow::toggleShowHide() {
    if (this->isVisible())
        this->hide();
    else {
        this->showOnTop();
    }

    return;
}

void MainWindow::createTrayActions()
{
    showHideAction = new QAction(tr("Показать/Спрятать"), this);
    connect(showHideAction, SIGNAL(triggered()), this, SLOT(toggleShowHide()));

    quitAction = new QAction(tr("Завершить"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->toggleShowHide();
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        break;
    }
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showHideAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::setIcon(EIcon index)
{
    //qDebug("icon: %i", index);
    QIcon icon = this->iconComboBox.itemIcon(index);
    this->trayIcon->setIcon(icon);
    this->setWindowIcon(icon);

    this->trayIcon->setToolTip(this->iconComboBox.itemText(index));
}

void MainWindow::createIconComboBox()
{
    this->iconComboBox.addItem(QIcon(":/images/good.png"), tr("Просроченных задач нет"));
    this->iconComboBox.addItem(QIcon(":/images/bad.png"),  tr("Есть просроченные задачи"));
    return;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->issues->resize(this->width(), this->height()-50);
    ui->labelTasksNRNUMEPhI->move((this->width() - ui->labelTasksNRNUMEPhI->width())/2, this->height() - 20);

    QWidget::resizeEvent(event);
    return;
}



void MainWindow::on_issues_itemSelectionChanged()
{
    QTableWidget                     *issues             = this->ui->issues;
    int                               columns_count      = issues->columnCount();
    QList<QTableWidgetSelectionRange> selected_list      = issues->selectedRanges();

    foreach (QTableWidgetSelectionRange range, selected_list)
        if (range.leftColumn() != 0 || range.rightColumn() != columns_count-1)

            issues->setRangeSelected(
                    QTableWidgetSelectionRange(
                        range.topRow(),    0,
                        range.bottomRow(), columns_count-1
                    ),
                    true
                );

}
