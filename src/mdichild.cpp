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

#include "mdichild.h"
#include <QDebug>
#include <QFile>
#include <QTextCursor>

void
MDIChild::closeEvent (QCloseEvent * event)
{

}

MDIChild::MDIChild(const QString& fileName): curFile(fileName)
{
	qDebug() << "MDIChild coming";
	worker = new Tail(fileName, this);
	if ( worker != NULL && worker->isValid() )
	{
		connect(worker, SIGNAL(sendLine(QString)), this, SLOT(receiveLine(QString)), Qt::QueuedConnection);
		worker->start();

		setWindowTitle(fileName);

		highlighter = new Highlighter(document());
		if ( highlighter == NULL )
		{
			delete worker;
			curFile = QString();
			return;
		}

	}
	else
		curFile = QString();
	
}

bool
MDIChild::isValid () const
{
	return worker->isValid() && ( curFile.size() > 0 );
}

MDIChild::~MDIChild ()
{
}

void MDIChild::receiveLine (QString line)
{
	qDebug() << "Line received:" << line ;

	textCursor().insertText(line);
	//append(line);
}

