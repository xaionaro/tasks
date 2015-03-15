
#include <QResizeEvent>

#include "mainwindow-full.h"
#include "ui_mainwindow-full.h"

MainWindowFull::MainWindowFull(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowFull)
{
    ui->setupUi(this);
    this->setCorner(Qt::TopLeftCorner,    Qt::LeftDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    ui->navigationDock->installEventFilter(this);
}

bool MainWindowFull::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Resize && obj == this->ui->navigationDock) {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        qDebug("Dock Resized (New Size) - Width: %d Height: %d",
             resizeEvent->size().width(),
             resizeEvent->size().height());
    }
    return QWidget::eventFilter(obj, event);
}

MainWindowFull::~MainWindowFull()
{
    delete ui;
}
