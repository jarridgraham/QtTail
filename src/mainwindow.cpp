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
#include "filterconfig.h"
#include "filtermodel.h"
#include "newfilter.h"

#include <QFileDialog>
#include <QMdiSubWindow>
#include <QSettings>

#include <QDebug>

MainWindow::MainWindow ():QMainWindow (0)
{
	setupUi(this);
	Ui_MainWindow::statusBar->showMessage(tr("Going"), 1000);

	QSettings settings;
	QString filename = settings.value("filtersfilename", QString()).toString();

	if ( ! filename.isEmpty() && ! filename.isNull() )
		loadFilterPool(filename);
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

void
MainWindow::loadFilterPool (QString namefile)
{
	QFile file(namefile);
	file.open(QIODevice::ReadOnly);

	QDataStream loader(&file);

	GenericFilter filter;
	QTextCharFormat format;

	while ( ! loader.atEnd() )
	{
		loader >> filter;
		loader >> format;
		filterPool.insert( filter, format );
	}

}

void
MainWindow::saveFilterPool (QString namefile)
{
	QFile file(namefile);
	file.open(QIODevice::WriteOnly);

	QDataStream writer(&file);

	for (  QHash<GenericFilter, QTextCharFormat>::const_iterator it = filterPool.constBegin(); it != filterPool.constEnd(); ++it )
	{
		writer << it.key();
		writer << it.value();
	}
}


MDIChild *MainWindow::createMDIChild(const QString& fileName)
{
	qDebug() << "Qui..." + fileName;
	MDIChild *child = new MDIChild(fileName);

	//TODO set filters!
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
	QSettings settings;
	QString filename = settings.value("filtersfilename", QString()).toString();

	saveFilterPool(filename);
}


void
MainWindow::on_actionFilter_configuration_triggered ()
{
	QAbstractTableModel* model = new FilterModel(); //TODO

	QDialog* filter = new FilterConfig(model, DOCUMENT,this);

	filter->show();
}

void
MainWindow::on_actionNew_filter_triggered ()
{
	newfilter = new NewFilter(this);

	connect(newfilter,SIGNAL(accepted()),this,SLOT(newFilter()));

	newfilter->show();
}

void
MainWindow::newFilter ()
{
	qDebug() << "newFilter handler called!";
	QPair<GenericFilter, QTextCharFormat> filter = newfilter->getFilterAndFormat();

	filterPool.insert ( filter.first, filter.second );

	qDebug() << "Filter pool " << filterPool.count();
	QMdiSubWindow* sub = mdiArea->activeSubWindow();

	if ( sub == NULL )
		return;
	
	MDIChild* currentChild = qobject_cast<MDIChild*> ( sub->widget() );

	if ( ! newfilter->isSuppressor() )
	{
		currentChild->addFilter(filter.first, filter.second);
	}
	else
	{
		currentChild->addSuppressor(filter.first);
	}
}

void
MainWindow::on_actionFilter_pool_triggered ()
{
	qDebug() << "Here I am";
	FilterModel* model = new FilterModel(); //TODO

	model->setData ( filterPool.keys() );

	QDialog* filter = new FilterConfig(model, DOCUMENT,this);

	filter->show();	
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

