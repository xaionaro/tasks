#include "projectmemberswindow.h"
#include "ui_projectmemberswindow.h"

ProjectMembersWindow::ProjectMembersWindow ( MainWindowFull *parent, QList<int> project_ids ) :
	QDialog ( parent ),
	ui ( new Ui::ProjectMembersWindow )
{
	this->ui->setupUi ( this );
	foreach ( const int &project_id, project_ids ) {
		//QJsonObject project = parent->projects.get(project_id);
		//redmine->get_members(project_id);
	}
}

ProjectMembersWindow::~ProjectMembersWindow()
{
	delete ui;
}

void ProjectMembersWindow::on_closeButton_clicked()
{
	delete this;
}
