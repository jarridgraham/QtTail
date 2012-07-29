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
#include <QMutexLocker>
#include <QDebug>

Tail::Tail (QObject* parent): QThread(parent), abort(false), valid(true)
{
	if ( !in.open(QFile::ReadOnly | QFile::Text) )
	{
		valid = false;
		//_error = QString(tr("Cannot open %1 - %2 ")).arg(fileName).arg(QFile.errorString);
	}
	qDebug() << "Tail instantiated!" << currentThreadId();
}

bool Tail::addFile(QString fileName)
{
	QFile current = new QFile(fileName, this);
	if ( !current.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
		//valid = false;
		//_error = QString(tr("Cannot open %1 - %2 ")).arg(fileName).arg(QFile.errorString);
	}
}


Tail::~Tail ()
{
	QMutexLocker lock(&mutex);
	abort = true;
	waiter.wakeOne();
}

void Tail::stopProcess()
{
	QMutexLocker lock(&mutex);
	abort = true;
	waiter.wakeOne();
}

void
Tail::goToPosition ()
{
	QStringList lines;
	QString line;
	
	while (  ! in.atEnd() )
	{
		line = in.readLine();
		lines.push_back ( in.readLine() );

		if ( lines.count() > 5 )
		{
			lines.pop_front();
		}
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
	
	QTimer* polling = new QTimer(this);
	connect(polling,SIGNAL(timeout()),this,checkLine());
	polling->setInterval(1000);
	polling->setSingleShot(false);
	
	QMutexLocker lock(&mutex);
	forever 
	{
		qDebug() << "Reading 1 line of text" << currentThreadId();
		waiter.wait(&mutex);
		if (abort)
			return;
		line = in.readLine();
		if ( line.length() > 0 )
			emit sendLine(line);
	}
}
