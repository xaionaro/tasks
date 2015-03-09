#include "mainwindow.h"
#include "helpwindow.h"
#include "ui_mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->updateTasks();
    this->createIconGroupBox();
    this->createTrayActions();
    this->createTrayIcon();

    this->setIcon(0);

    this->trayIcon->show();
}


static void get_issues_callback(void *_ui, QNetworkReply *reply, QJsonDocument *json) {
    Ui::MainWindow *ui = static_cast<Ui::MainWindow *>(_ui);

    QJsonObject answer = json->object();
    QJsonArray  issues = answer["issues"].toArray();

    foreach (const QJsonValue &_issue, issues) {
        QJsonObject issue = _issue.toObject();
        qDebug("Issue: #%i:\t%s", issue["id"].toInt(), issue["subject"].toString().toUtf8().data());
        ui->issues->addItem(issue["subject"].toString());
    }

    return;
}
/*
static void get_issues_callback_wrapper(void *_this, QNetworkReply *reply, QJsonDocument *json) {
    static_cast<MainWindow *>(_this)->get_issues_callback(reply, json);
}*/

int MainWindow::updateTasks() {
    redmine->get_issues(get_issues_callback, ui);
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
