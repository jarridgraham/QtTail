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

#include "mainwindow.h"
#include "mdichild.h"

#include <QFileDialog>
#include <QMdiSubWindow>
#include <QDebug>

MainWindow::MainWindow ():QMainWindow (0), worker(0)
{
	setupUi(this);
	Ui_MainWindow::statusBar->showMessage(tr("Going"), 1000);
}

void MainWindow ::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type())
	{
		case QEvent::LanguageChange:
			retranslateUi(this);
			break;
		default:
			break;
	}
}

void MainWindow::readSettings()
{
	//TODO
}


MDIChild *MainWindow::createMDIChild(const QString& fileName)
{
	if ( worker == 0 )
		worker = new Tail(this);
	qDebug() << "Qui..." + fileName;
	MDIChild *child = new MDIChild(fileName);
	
	connect(worker,SIGNAL(sendLine(QString, QString)), child,SLOT(receiveLine(QString, QString)));
	
	mdiArea->addSubWindow(child);
	child->show();

	return child;
}

void
MainWindow::on_actionOpen_triggered ()
{

	QString fileName = QFileDialog::getOpenFileName(this);

	if ( fileName.isEmpty() )
		return;

	Ui_MainWindow::statusBar->showMessage(tr("File ") + fileName + tr(" selected"), 1000);
	QMdiSubWindow* existing;
	existing = findMDIChild(fileName);
	if ( existing != 0 )
		mdiArea->setActiveSubWindow( existing );

	createMDIChild(fileName);
}

void
MainWindow::on_actionClose_triggered ()
{
	//! @todo TODO Closing to manage!
}


void
MainWindow::on_actionSuppress_triggered ()
{

}

void
MainWindow::on_actionHighlight_triggered ()
{

}

void
MainWindow::on_actionSave_triggered ()
{

}

void
MainWindow::on_actionQTail_triggered ()
{

}

QMdiSubWindow *MainWindow::findMDIChild(const QString &fileName)
{
	QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

	foreach (QMdiSubWindow *window, mdiArea->subWindowList())
	{
		MDIChild *mdiChild = qobject_cast<MDIChild *>(window->widget());
		if (mdiChild->currentFile() == canonicalFilePath)
			return window;
	}
	return 0;
}

