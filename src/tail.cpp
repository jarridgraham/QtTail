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


#include "tail.h"

#include <QFile>
#include <QTextStream>
#include <QFile>
#include <QStringList>

Tail::Tail (QString file, QObject* parent): fileName(file),abort(false), QThread(parent)
{

}

Tail::~Tail ()
{
	mutex.lock();
	abort = true;
	mutex.unlock();
}

void Tail::stopProcess()
{
	mutex.lock();
	abort = true;
	mutex.unlock();
}

void
Tail::goToPosition (QTextStream& in)
{
	QStringList lines;

	QString line;
	
	while (  ! in.atEnd() )
	{
		line = in.readLine()
		lines.push_back ( in.readLine() );

		if ( lines.count() > 5 )
		{
			lines.pop_front();
		}
	}
	foreach(line,lines)
		emit sendLine(line);
}


void
Tail::run ()
{
	QFile file(fileName);
	if ( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		emit Error(tr("Cannot open %1 - %2 ").arg(fileName).arg(QFile.errorString));
		return;
	}

	QTextString in(file);

	goToPosition(in);

	while (!abort )
	{
		line = in.readLine();
		emit sendLine(line);
	}
}
