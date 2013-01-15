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


#ifndef _MDICHILD__H
#define _MDICHILD__H

// #include <QTextEdit>
#include <QPlainTextEdit>
#include <QList>
#include <QMap>
#include "highlighter.h"
#include "genericfilter.h"
#include "tail.h"
#include "format.h"

class MDIChild: public QPlainTextEdit
{
	Q_OBJECT
	
 	QString curFile;
	Tail* worker;
	QMap<GenericFilter, QTextCharFormat> filters;
	Highlighter* highlighter;
	QAction* menu;
	
	void GoToPos(int position = -1);
	QTextCharFormat setNewFormat(const Format& format);
	bool doAddFilter(GenericFilter filter);
	bool toBeSuppressed(QString text) const;
	void suppress();
protected:
	void closeEvent(QCloseEvent *event);
	
public:
	MDIChild(const QString& fileName, int defaultpointsize = 8, int numlines = -1);
	virtual ~MDIChild();

	void resetFilters() { updateAllFilters( QList<GenericFilter>() ); }

	QList<GenericFilter> getFilters() const { return filters.keys(); }

	bool isValid() const;
	QString currentFile() const { return curFile; }
	QAction* getMenu() const { return menu; }
	void setMenu(QAction* m) { menu = m; }

public slots:
	bool addFilter(GenericFilter filter);
	void removeFilter(GenericFilter filter);
	void updateAllFilters(QList<GenericFilter> newfilters);
	void receiveLine(QString line);
	void receiveStart(QString start);
};

#endif
