/*
    QTail is a simple graphical implementation of tail -f
    Copyright (C) 2012  Tazio Ceri <praise@praisenet.darktech.org>

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

#ifndef  __MAINWINDOW__H
#define  __MAINWINDOW__H

#include <QWidget>
#include "ui_mainwindow.h"
class MDIChild;

class MainWindow: public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

	void readSettings();

	MDIChild* createMDIChild(const QString& fileName);
	QMdiSubWindow* findMDIChild(const QString &fileName);
private slots:
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionClose_triggered();
	void on_actionHighlight_triggered();
	void on_actionSuppress_triggered();
	void on_actionQTail_triggered();
protected:
	void changeEvent(QEvent *e);
	void closeEvent(QEvent* e){};

public:
	MainWindow();
};

#endif
