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
#include "findwindow.h"
#include "newfilter.h"
#include "about.h"

#include <QFileDialog>
#include <QMdiSubWindow>
#include <QSettings>
#include <QPointer>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow ():QMainWindow (0), modified(false), findwindow(NULL), separator(NULL)
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

void
MainWindow::InsertIntoMenu (const QString & fileName, MDIChild* child )
{
	QFileInfo file(fileName);

	QString basename = file.fileName();
	
	QAction* nMenu = new QAction(basename,this);
	if ( separator == NULL )
		separator = Ui::MainWindow::menuWindow->addSeparator();
	Ui::MainWindow::menuWindow->addAction( nMenu );

	connect(nMenu, SIGNAL(triggered()), child, SLOT(setFocus()));

	child->setMenu ( nMenu );
}

void
MainWindow::on_actionStartingLines_triggered ()
{
	bool ok;
	
	int tNumLines = QInputDialog::getInt(this, tr("Lines to display"),
                                          tr("Lines to display when openin files (-1 means ALL):"), numLines, -1, 100000, 100, &ok);

	if ( ok )
		numLines = tNumLines;
	
}


MDIChild *MainWindow::createMDIChild(const QString& fileName)
{
	MDIChild *child = new MDIChild(fileName, 8, numLines);

	if ( child == NULL ) return child;
	
	if ( child->isValid() )
	{
		//TODO
		//child->setFontWeight( 8 ); //Defined in NewFilter too
		child->setAttribute(Qt::WA_DeleteOnClose);
		//connect(child,SIGNAL(destroyed(QObject*)),this,SLOT(mdiDestroyed(QObject*)));
		QMdiSubWindow* sub = mdiArea->addSubWindow(child);
		mdiArea->setActiveSubWindow( sub );
		child->show();

		InsertIntoMenu( fileName, child );
		
		return child;
	}
	delete child;
	return NULL;
}

void
MainWindow::on_actionTabMode_triggered ()
{
	if ( mdiArea->viewMode() == QMdiArea::TabbedView )
	{
		mdiArea->setViewMode( QMdiArea::SubWindowView );
		actionCascade->setEnabled( true );
		actionTile->setEnabled( true );
	}
	else
	{
		mdiArea->setViewMode( QMdiArea::TabbedView );
		actionCascade->setEnabled( false );
		actionTile->setEnabled( false);
	}
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
	QMdiSubWindow* sub = mdiArea->currentSubWindow();
	MDIChild* current = qobject_cast<MDIChild*> (  sub->widget() );

	Ui::MainWindow::menuWindow->removeAction ( current->getMenu() );
	current->getMenu()->deleteLater();
	sub->close();

	if (  mdiArea->currentSubWindow() == NULL )
	{
		Ui::MainWindow::menuWindow->removeAction ( separator );
		separator->deleteLater();
		separator = NULL;
	}
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	QString fileName;
	if ( modified && ! filterPool.empty() )
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
				e->ignore();
				return;
		}
	}

	foreach(QMdiSubWindow* subWindow, mdiArea->subWindowList())
	{
		subWindow->close();
	}
	
	if ( findwindow != NULL ) findwindow->close();
	QMainWindow::closeEvent(e);
}


void MainWindow::on_actionQuit_triggered()
{
	close();
}


void MainWindow::open_configuration(const QList<GenericFilter>& filters, MDIChild* child )
{
	FilterModel* model = new FilterModel(filters);
	
	model->setType(DOCUMENT);

	FilterConfig* filter = new FilterConfig(model, this);

	connect(model, SIGNAL(newFilters(const QList<GenericFilter>&)), child, SLOT(updateAllFilters(const QList<GenericFilter>&)));
	connect(filter, SIGNAL(addFilter(GenericFilter)), child, SLOT(addFilter(GenericFilter)));
	connect(filter, SIGNAL(deleteFilter(GenericFilter)), child, SLOT(removeFilter(GenericFilter)));
	filter->show();
}

void
MainWindow::addHighlightFilter (const GenericFilter & filter)
{
	addFilter2Current(filter);
}

void
MainWindow::noAction () const
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(tr("Warning"));
	msgBox.setText(tr("No action is possible without an active document!"));
	msgBox.exec();
}
void
MainWindow::on_actionFilter_configuration_triggered ()
{
	MDIChild* child = getTopMDIChild();

	if ( child == NULL )
	{
		noAction();
		return;
	}
		
	open_configuration( child->getFilters(), child );
}

void
MainWindow::on_actionReset_Highlighter_triggered ()
{
	MDIChild* child = getTopMDIChild();

	if ( child == NULL )
	{
		noAction();
		return;
	}

	child->resetFilters();
}


void
MainWindow::on_actionNew_filter_triggered ()
{
	newfilter = new NewFilter(this);

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

	connect(model,SIGNAL(newFilters(QList<GenericFilter>)),this,SLOT(updateFilterPool(QList<GenericFilter>)));

	MDIChild* child = getTopMDIChild();

	if ( child != NULL )
	{
		connect(filter, SIGNAL(addFilter(GenericFilter)), child, SLOT(addFilter(GenericFilter)));
		connect(filter, SIGNAL(deleteFilter(GenericFilter)), child, SLOT(removeFilter(GenericFilter)));
	}
	
	filter->show();	
}

void MainWindow::updateFilterPool(QList< GenericFilter > filters)
{
	modified = true;
	filterPool = filters;
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

void MainWindow::on_actionFind_triggered()
{
	if ( mdiArea->subWindowList().count() == 0 )
		return;
	
	findwindow = new FindWindow(this);

	connect( findwindow, SIGNAL(find(const QString&,  QTextDocument::FindFlags)), this,
		 SLOT(finderWrapper(const QString&, QTextDocument::FindFlags) ) );
	connect( findwindow, SIGNAL(find(const QRegExp&, QTextDocument::FindFlags)), this,
		 SLOT(finderWrapper(const QRegExp&, QTextDocument::FindFlags) ) );

	findwindow->show();
}

void MainWindow::finderWrapper(const QString& str, QTextDocument::FindFlags flags)
{
	MDIChild* active = getTopMDIChild();
	if ( active == NULL )
		return;

	QTextDocument* current_doc = active->document();

	if ( ! ( findStatus.equals( str, flags ) && findStatus.searched == active ) )
	{
		findStatus.findcur = active->textCursor();
		QTextCursor found = current_doc->find(str, findStatus.findcur, flags);
		if ( ! found.isNull() )
		{
			active->setTextCursor( found );
		}
	}

	findStatus.searched = active;
}

void MainWindow::finderWrapper(const QRegExp& str, QTextDocument::FindFlags flags)
{
	MDIChild* active = getTopMDIChild();
	if ( active == NULL )
		return;

	QTextDocument* current_doc = active->document();
	
	if ( ! findStatus.equals( str, flags ) )
	{
		findStatus.findcur = active->textCursor();
		QTextCursor found = current_doc->find(str, findStatus.findcur, flags);
		if ( ! found.isNull() )
		{
			active->setTextCursor( found );
		}
	}

	findStatus.searched = active;

	//! TODO What happens when I move the cursor to the end?
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
	QDialog* about = new About(this);

	about->show();
}

MainWindow::~MainWindow()
{
	if ( findwindow != NULL )
	{
		findwindow->close();
		findwindow->deleteLater();
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

