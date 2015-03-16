
#include <QResizeEvent>

#include "mainwindow-full.h"
#include "ui_mainwindow-full.h"

MainWindowFull::MainWindowFull(QWidget *parent) :
    MainWindowCommon(parent),
    ui(new Ui::MainWindowFull)
{
    ui->setupUi(this);

    this->setWindowTitle("Система «Задачи» НИЯУ МИФИ");

    this->setCorner(Qt::TopLeftCorner,    Qt::LeftDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    ui->navigationDock->installEventFilter(this);
    ui->issueDock->installEventFilter(this);
    ui->filtersDock->installEventFilter(this);

    return;
}


void MainWindowFull::on_resize_navigationDock(QResizeEvent *event) {
    qDebug("navigationDock Resized (New Size) - Width: %d Height: %d",
        event->size().width(),
        event->size().height());

    return;
}

void MainWindowFull::on_resize_issueDock(QResizeEvent *event) {
    qDebug("issueDock Resized (New Size) - Width: %d Height: %d",
        event->size().width(),
        event->size().height());

    return;
}

void MainWindowFull::on_resize_filtersDock(QResizeEvent *event) {
    qDebug("filtersDock Resized (New Size) - Width: %d Height: %d",
        event->size().width(),
        event->size().height());

    return;
}

bool MainWindowFull::eventFilter(QObject *obj, QEvent *event) {    
    if (event->type() == QEvent::Resize) {
        Ui::MainWindowFull *ui = this->ui;
        if (obj == ui->navigationDock)
            this->on_resize_navigationDock(static_cast<QResizeEvent*>(event));
        else
        if (obj == ui->issueDock)
            this->on_resize_issueDock     (static_cast<QResizeEvent*>(event));
        else
        if (obj == ui->filtersDock)
            this->on_resize_filtersDock   (static_cast<QResizeEvent*>(event));
    }
    return QWidget::eventFilter(obj, event);
}

MainWindowFull::~MainWindowFull()
{
    delete ui;
}
