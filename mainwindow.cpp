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

void MainWindow::issuesSetup()
{
    QTableWidget *issues = ui->issues;

    // Columns:

    QStringList columns;
    QSize itemSize;

    columns << "Название" << "Исполнитель" << "Срок";

    issues->setColumnCount(columns.size());

    issues->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    issues->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    issues->horizontalHeader()->resizeSection(1, 150);

    issues->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    issues->horizontalHeader()->resizeSection(2, 100);

    issues->setHorizontalHeaderLabels(columns);

    // Signals:

    connect(issues, SIGNAL(cellDoubleClicked(int, int)),
            this,   SLOT(on_issues_doubleClick(int, int)));
    return;
}

void MainWindow::on_issues_doubleClick(int row, int column)
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

    this->issuesSetup();

    this->updateTasks();
    this->createIconGroupBox();
    this->createTrayActions();
    this->createTrayIcon();

    this->setIcon(0);

    this->trayIcon->show();
}


void MainWindow::issue_set(int pos, QJsonObject issue)
{
    QTableWidget     *issues = this->ui->issues;
    QTableWidgetItem *item;

    qDebug("Issue: #%i:\t%s", issue["id"].toInt(), issue["subject"].toString().toUtf8().data());

    // New row:
    issues->insertRow(pos);

    //     Issue name:
    item = new QTableWidgetItem(issue["subject"].toString());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    issues->setItem(pos, 0, item);

    //     Assignee:
    item = new QTableWidgetItem(issue["assigned_to"].toObject()["name"].toString());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    issues->setItem(pos, 1, item);

    //     Due date:
    QString due_date_str = issue["due_date"].toString();
    QDateTime now, date;
    now  = QDateTime::currentDateTime();
    date = QDateTime::fromString(due_date_str, "yyyy-MM-dd");

    item = new QTableWidgetItem(due_date_str);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    if ((due_date_str != "") && (now > date))
        item->setBackgroundColor(QColor(255, 192, 192));
    issues->setItem(pos, 2, item);

    this->issue_row2issue.insert(pos, issue);
    //qDebug("Test: %i|%i", issue["id"].toInt(), this->issue_row2issue[0]["id"].toInt());
    return;
}

static void get_issues_callback(void *_win, QNetworkReply *reply, QJsonDocument *json) {
    (void)reply;

    MainWindow     *win = static_cast<MainWindow *>(_win);

    int issues_count = 0;

    QJsonObject answer = json->object();
    QJsonArray  issues = answer["issues"].toArray();

    foreach (const QJsonValue &issue, issues)
        win->issue_set(issues_count++, issue.toObject());

    return;
}
/*
static void get_issues_callback_wrapper(void *_this, QNetworkReply *reply, QJsonDocument *json) {
    static_cast<MainWindow *>(_this)->get_issues_callback(reply, json);
}*/

int MainWindow::updateTasks() {
    redmine->get_issues(get_issues_callback, this);
    return 0;
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::createTrayActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::setIcon(int index)
{
    QIcon icon = iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(iconComboBox->itemText(index));
}

void MainWindow::createIconGroupBox()
{
    iconGroupBox = new QGroupBox(tr("Tray Icon"));

    iconLabel = new QLabel("Icon:");

    iconComboBox = new QComboBox;
    iconComboBox->addItem(QIcon(":/images/bad.png"),   tr("Bad"));
    iconComboBox->addItem(QIcon(":/images/heart.png"), tr("Heart"));
    iconComboBox->addItem(QIcon(":/images/trash.png"), tr("Trash"));

    showIconCheckBox = new QCheckBox(tr("Show icon"));
    showIconCheckBox->setChecked(true);

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconLabel);
    iconLayout->addWidget(iconComboBox);
    iconLayout->addStretch();
    iconLayout->addWidget(showIconCheckBox);
    iconGroupBox->setLayout(iconLayout);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->issues->resize(this->width(), this->height()-41);

    QWidget::resizeEvent(event);
    return;
}


