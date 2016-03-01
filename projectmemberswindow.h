#ifndef PROJECTMEMBERSWINDOW_H
#define PROJECTMEMBERSWINDOW_H

#include <QDialog>
#include <QList>

#include <mainwindow-full.h>

namespace Ui
{
class ProjectMembersWindow;
}

class ProjectMembersWindow : public QDialog
{
	Q_OBJECT

public:
	explicit ProjectMembersWindow ( MainWindowFull *parent, QList<int> project_ids );
	~ProjectMembersWindow();

private slots:
	void on_closeButton_clicked();

private:
	Ui::ProjectMembersWindow *ui;
};

#endif // PROJECTMEMBERSWINDOW_H
