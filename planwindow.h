#ifndef PLANWINDOW_H
#define PLANWINDOW_H

#include <QWidget>

#include "mainwindow-full.h"
//#include "htmldelegate.h"

namespace Ui {
	class PlanWindow;
}

class PlanWindow : public QWidget
{
		Q_OBJECT
		CALLBACK_DISPATCHER ( Redmine, PlanWindow, this )

	public:
		explicit PlanWindow(MainWindowFull *mainWindow, QWidget *parent = 0);
		~PlanWindow();

	signals:
		void on_destructor();
		void on_updatedStuffToDo();

	public slots:
		void consider_updatedIssues();

	private slots:
		void on_user_currentIndexChanged(int index);

	private:
		void get_stuff_to_do_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg );
		void plannedIssues_display();
		void updateStuffToDo();

		MainWindowFull *mainWindow;
		Ui::PlanWindow *ui;

		RedmineItemTree issues;
		QMutex updateStuffToDoMutex;
		//HTMLDelegate delegate;
		int selectedUserId;
};

#endif // PLANWINDOW_H
