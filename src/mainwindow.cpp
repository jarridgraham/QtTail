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
#include <QPointer>

#include <QDebug>

const int DEFAULT_FONT_WEIGHT = 8;

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
	Format* format = new Format();

	while ( ! loader.atEnd() )
	{
		loader >> filter;
		loader >> *format;
		filterPool.insert( filter, format );
	}

}

void
MainWindow::saveFilterPool (QString namefile)
{
	QFile file(namefile);
	file.open(QIODevice::WriteOnly);

	QDataStream writer(&file);

	for (  QHash<GenericFilter, Format*>::const_iterator it = filterPool.constBegin(); 
	     it != filterPool.constEnd(); ++it )
	{
		writer << it.key();
		writer << *(it.value());
	}
}


MDIChild *MainWindow::createMDIChild(const QString& fileName)
{
	qDebug() << "Qui..." + fileName;
	MDIChild *child = new MDIChild(fileName);

	//TODO set filters!
	child->setFontWeight( DEFAULT_FONT_WEIGHT );
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
	FilterModel* model = new FilterModel();

	model->setData( filterPool.keys() );
	model->setType(DOCUMENT);
	
	QDialog* filter = new FilterConfig(model, this);

	filter->show();
}

void
MainWindow::on_actionNew_filter_triggered ()
{
	newfilter = new NewFilter(this, DEFAULT_FONT_WEIGHT);

	connect(newfilter,SIGNAL(accepted()),this,SLOT(newFilter()));

	newfilter->show();
}


void MainWindow::addFilter2Current(const GenericFilter& filter, bool suppressor)
{
	qDebug() << "addFilter2Current";
	QList<QMdiSubWindow *> subs = mdiArea->subWindowList(QMdiArea::StackingOrder);
	
	if ( subs.count() == 0 )
	{
		qDebug() << "No Subs";
		return;
	}
		
	QMdiSubWindow* sub = subs[0];
	
	MDIChild* currentChild = qobject_cast< MDIChild*>( sub->widget() );
	
	qDebug() << "going on adding filter";
	if ( suppressor )
	{
		currentChild->addSuppressor(filter);
	}
	else
	{
		qDebug() << "Current child, adding filter";
		Format form;
		getDefaultFormat(currentChild, form);
		currentChild->addFilter(filter, form);
	}
}

void MainWindow::getDefaultFormat(MDIChild* currentChild, Format& ret) const
{
	QTextCharFormat temp = currentChild->currentCharFormat();
	
	QBrush forebrush = ret.foreground();
	QBrush backbrush = ret.background();
	QColor foreground(forebrush.color());
	QColor background(backbrush.color());
	
	foreground.setRed( 255 - foreground.red() );
	foreground.setGreen(255 - foreground.green());
	foreground.setBlue(255 - foreground.blue());
	
	background.setRed( 255 - background.red() );
	background.setGreen(255 - background.green());
	background.setBlue(255 - background.blue());
	
	ret.setForeground(foreground);
	ret.setBackground(background);
}


void
MainWindow::newFilter ()
{
	qDebug() << "newFilter handler called!";
	QPair<GenericFilter, Format*> filter = newfilter->getFilterAndFormat();
	
	filterPool.insert ( filter.first, filter.second );
	
	QList<QMdiSubWindow *> subs = mdiArea->subWindowList(QMdiArea::StackingOrder);
	
	if ( subs.count() == 0 ) 
	{
		qDebug() << "No Subs";
		return;
	}
		
	QMdiSubWindow* sub = subs[0];
	
	MDIChild* currentChild = qobject_cast<MDIChild*> ( sub->widget() );

	if ( ! newfilter->isSuppressor() )
	{
		qDebug() << "2 Current Child adding filter";
		currentChild->addFilter(filter.first, *(filter.second));
	}
	else
	{
		qDebug() << "2 Current adding suppressor";
		currentChild->addSuppressor(filter.first);
	}
}

void
MainWindow::on_actionFilter_pool_triggered ()
{
	FilterModel* model = new FilterModel();

	model->setData ( filterPool.keys() );
	model->setType ( GLOBAL );
	
	QDialog* filter = new FilterConfig(model, this);

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

MainWindow::~MainWindow()
{
	foreach ( Format* f, filterPool)
	{
		delete f;
	}
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

