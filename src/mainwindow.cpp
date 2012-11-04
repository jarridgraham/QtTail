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
#include <QMessageBox>

#include <QDebug>

const int DEFAULT_FONT_WEIGHT = 8;

MainWindow::MainWindow ():QMainWindow (0), modified(false)
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

	int prio = filterPool.count();

	while ( ! loader.atEnd() )
	{
		loader >> filter;
		loader >> *format;
		filter.setPriority(prio++);
		filter.setFormat( format );
		filterPool.append( filter );
	}
	modified = false;
}

void
MainWindow::saveFilterPool (QString namefile)
{
	QFile file(namefile);
	file.open(QIODevice::WriteOnly);

	QDataStream writer(&file);

	GenericFilter item;
	foreach ( item, filterPool )
	{
		writer << item;
	}
	modified = true;
}


MDIChild *MainWindow::createMDIChild(const QString& fileName)
{
	qDebug() << "Qui..." + fileName;
	MDIChild *child = new MDIChild(fileName, DEFAULT_FONT_WEIGHT);

	if ( child->isValid() )
	{

		child->setFontWeight( DEFAULT_FONT_WEIGHT );
		child->setAttribute(Qt::WA_DeleteOnClose);
		connect(child,SIGNAL(destroyed(QObject*)),this,SLOT(mdiDestroyed(QObject*)));
		QMdiSubWindow* sub = mdiArea->addSubWindow(child);
		mdiArea->setActiveSubWindow( sub );
		child->show();

		return child;
	}
	return NULL;
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
	mdiArea->currentSubWindow()->close();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	qDebug() << "closeEvent mainwindow";
	QMainWindow::closeEvent(e);
}


void MainWindow::on_actionQuit_triggered()
{
	QString fileName;
	if ( modified )
	{
		QMessageBox msgBox;
		msgBox.setText(tr("Filter pool has been modified!"));
		msgBox.setInformativeText(tr("Do you want to save your filter pool?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setWindowTitle(tr("Save on quit?"));
		int ret = msgBox.exec();
		switch (ret)
		{
			case QMessageBox::Save:
				
				fileName = getFilename();
				
				if ( fileName.isEmpty() )
				{
					Ui_MainWindow::statusBar->showMessage(tr("Filters not saved, not quitting..."), 1000);
					return;
				}
				
				saveFilterPool(fileName);
				break;
			case QMessageBox::Discard:
				break;
			case QMessageBox::Cancel:
				return;
		}
	}
	
	foreach(QMdiSubWindow* subWindow, mdiArea->subWindowList())
	{
		subWindow->close();
	}	
	close();
}


void MainWindow::open_configuration(const QList<GenericFilter>& filters )
{
	FilterModel* model = new FilterModel(filters);
	
	model->setType(DOCUMENT);

	connect(model, SIGNAL(addFilter(GenericFilter)), this, SLOT(addFilter2Current(GenericFilter)));
	connect(model, SIGNAL(deleteFilter(GenericFilter)), this, SLOT(deleteFilter(GenericFilter)));

	QDialog* filter = new FilterConfig(model, this);

	filter->show();
}

void
MainWindow::addHighlightFilter (const GenericFilter & filter)
{
	addFilter2Current(filter);
}


void
MainWindow::on_actionFilter_configuration_triggered ()
{
	MDIChild* child = getTopMDIChild();

	open_configuration( child->getFilters() );
}

//! @todo this method is not needed (the same as menu item)
void
MainWindow::on_actionSuppression_configuration_triggered ()
{
	MDIChild* child = getTopMDIChild();
	
	open_configuration( child->getFilters() );
}


void
MainWindow::on_actionNew_filter_triggered ()
{
	newfilter = new NewFilter(this, DEFAULT_FONT_WEIGHT);

	connect(newfilter,SIGNAL(accepted()),this,SLOT(newFilter()));

	newfilter->show();
}


void MainWindow::addFilter2Current(GenericFilter filter)
{
	qDebug() << "addFilter2Current";
	
	MDIChild* currentChild = getTopMDIChild();
	if ( currentChild == NULL )
		return;
	
	qDebug() << "going on adding filter";

	//! TODO Is getDefaultFormat needed?
	if ( ! filter.isSuppressor() )
	{
		Format* form = new Format();
		getDefaultFormat(currentChild, *form);
		filter.setFormat( form );
	}
	currentChild->addFilter( filter );
}

void MainWindow::deleteFilter(GenericFilter filter)
{
	qDebug() << "deleteFilter";

	MDIChild* currentChild = getTopMDIChild();
	if ( currentChild == NULL )
		return;

	currentChild->removeFilter( filter );
}

MDIChild* MainWindow::getTopMDIChild ()
{
	QMdiSubWindow* active = mdiArea->currentSubWindow();
	if ( active == NULL )
		return NULL;

	MDIChild* r = qobject_cast<MDIChild*> ( active->widget() );
	return r;
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
	
	GenericFilter filter = newfilter->getFilter();
	
	filter.setPriority( filterPool.count() );
	filterPool.append( filter );
	
	modified = true;

	MDIChild* currentChild  = getTopMDIChild();

	if ( currentChild == NULL )
		return;

	currentChild->addFilter( filter );

}

void
MainWindow::on_actionFilter_pool_triggered ()
{
	FilterModel* model = new FilterModel(filterPool);

	model->setData ( filterPool );
	model->setType ( GLOBAL );
	
	QDialog* filter = new FilterConfig(model, this);

	connect ( filter, SIGNAL(addFilter(const GenericFilter&)), this, SLOT(addHighlightFilter(const GenericFilter&)));
	
	filter->show();	
}

QString MainWindow::getFilename()
{
	QString fileName = QFileDialog::getOpenFileName(this);

	QFile checker(fileName);
	if ( checker.exists() )
	{
		int ret = QMessageBox::warning(this, tr("Save"), tr("File exists\n"
				"Do you want to overwrite it?"), QMessageBox::Yes | QMessageBox::No);

		if ( ret != QMessageBox::Yes )
		{
			return QString();
		}
	}
	return fileName;
}


void
MainWindow::on_actionSave_Filters_triggered ()
{
	QString fileName = getFilename();
	
	if ( fileName.isEmpty() )
	{
		Ui_MainWindow::statusBar->showMessage(tr("Filters not saved"), 1000);
		return;
	}

	saveFilterPool(fileName);
	Ui_MainWindow::statusBar->showMessage(tr("Filters saved"), 1000);
}

void MainWindow::on_actionOpen_Filters_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this);

	loadFilterPool(fileName);

	Ui_MainWindow::statusBar->showMessage(tr("Filters loaded"), 1000);
}


void
MainWindow::on_actionQTail_triggered ()
{

}

MainWindow::~MainWindow()
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

