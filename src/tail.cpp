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
#include <QStringList>
#include <QMutexLocker>
#include <QScopedPointer>
#include <QDebug>

Tail::Tail (QString fileName, QObject* parent): QThread(parent), abort(false), counter(0),in(fileName), valid(true)
{
	if ( !in.open(QFile::ReadOnly | QFile::Text) )
	{
		valid = false;
	}
	else
	{
		QScopedPointer<QTimer> tTimer( new QTimer() );
		connect(tTimer.data(),SIGNAL(timeout()),this,SLOT(resetCounter()));
		timer = tTimer.take();
		qDebug() << "Tail created!";
	}
}

Tail::~Tail ()
{
	stopProcess();
	qDebug() << "~Tail";
	delete timer;
}

void
Tail::resetCounter ()
{
	counter = 0;
}


void Tail::stopProcess()
{
	QMutexLocker lock(&mutex);
	abort = true;
	waiter.wakeAll();
	lock.unlock();
	wait();
}

void
Tail::goToPosition ()
{
	QStringList lines;
	QString line;
	
	while (  ! in.atEnd() )
	{
		line = in.readLine();
		//qDebug() << "line: " << line.trimmed();
		lines.push_back ( line );

		if ( lines.count() > 5 )
			lines.pop_front();
	}
	foreach(line,lines)
		emit sendLine(line);
}

void Tail::checkLine()
{
	QMutexLocker locker(&mutex);
	waiter.wakeAll();
}


void
Tail::run ()
{
	QString line;	
	goToPosition();

	qDebug() << "Thread: " << currentThreadId();
	
	QMutexLocker lock(&mutex);
	forever 
	{
// 		qDebug() << "Reading 1 line of text" << currentThreadId();
		waiter.wait(&mutex, 1500);
		if (abort)
			return;

		if ( timer->isActive() == false )
			timer->start(1000);
		
		while ( ! in.atEnd() )
		{	
			line = in.read(1024);
			if ( counter >= 1000 )
				continue;

			if (abort)
				return;
			
// 			qDebug() << "Error? " << in.errorString();
// 			qDebug() << "Sending line " << line;
			if ( line.length() > 0 )
				emit sendLine(line);

			++counter;
			if ( counter ==  1000 )
			{
				//qDebug() << "Too many lines... skipping.";
				emit sendLine(tr("...\nToo fast... skipping!\n"));
			}
		}
	}
}
