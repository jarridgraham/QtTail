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
#include <QTextCharFormat>
#include <QTextDocument>
#include <QHash>

#include "newfilter.h"
#include "format.h"
#include "ui_mainwindow.h"

class MDIChild;

class MainWindow: public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

	NewFilter* newfilter;
	QList<GenericFilter> filterPool;
	bool modified;
	QDialog* findwindow;
	QAction* separator;
	
	struct
	{
		bool isRegexp;
		QRegExp reg;
		QString match;
		QTextDocument::FindFlags flags;
		MDIChild* searched;
		bool equals(QRegExp r, QTextDocument::FindFlags f) { return isRegexp && reg == r && f == flags; }
		bool equals(QString r, QTextDocument::FindFlags f) { return ! isRegexp && match == r && f == flags; }

		QTextCursor findcur;
	} findStatus;

	void loadFilterPool(QString namefile);
	void saveFilterPool(QString namefile);
	void InsertIntoMenu(const QString& fileName, MDIChild* child );
	MDIChild* createMDIChild(const QString& fileName);
	QMdiSubWindow* findMDIChild(const QString &fileName);
	MDIChild* getTopMDIChild();
	void getDefaultFormat(MDIChild* currentChild, Format& form) const;
	void open_configuration(const QList<GenericFilter>& filters, MDIChild* child );
	void noAction() const;
	QString getFilename();
private slots:
	void on_actionOpen_Filters_triggered();
	void on_actionOpen_triggered();
	void on_actionSave_Filters_triggered();
	void on_actionClose_triggered();
	void on_actionFilter_configuration_triggered();
	void on_actionNew_filter_triggered();
	void on_actionFilter_pool_triggered();
	void on_actionQTail_triggered();
	void on_actionQuit_triggered();
	void on_actionFind_triggered();
	void on_actionReset_Highlighter_triggered();

	void newFilter();
	void addFilter2Current(GenericFilter filter);
	void deleteFilter(GenericFilter filter);
	void addHighlightFilter(const GenericFilter& filter);

	void finderWrapper(const QString& str,  QTextDocument::FindFlags flags);
	void finderWrapper(const QRegExp& str,  QTextDocument::FindFlags flags);
	void updateFilterPool(QList<GenericFilter> filters);
protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent* e);
public:
	MainWindow();
	virtual ~MainWindow();
};

#endif
