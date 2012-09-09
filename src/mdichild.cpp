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

MDIChild::MDIChild(const QString& fileName, int default_point_size): curFile(fileName)
{
	qDebug() << "MDIChild coming";
	highlightFilter = new QMap<GenericFilter, QTextCharFormat>();
	
	GoToPos();
	
	worker = new Tail(fileName, this);
	if ( worker != NULL && worker->isValid() )
	{
		connect(worker, SIGNAL(sendLine(QString)), this, SLOT(receiveLine(QString)), Qt::QueuedConnection);
		worker->start();

		setWindowTitle(fileName);
		setReadOnly(true);
		setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

		QFont f = font();
		f.setPointSize( default_point_size);
		setFont( f );
		
		highlighter = new Highlighter(this, highlightFilter);
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
	delete highlightFilter;
}

void MDIChild::GoToPos(int position)
{
	if ( position == -1 )
	{
		QTextCursor c = textCursor();
		c.movePosition(QTextCursor::End);
		setTextCursor(c);
	}
	else
	{
		QTextCursor c = textCursor();
		c.setPosition(position);
		setTextCursor(c);		
	}
}


void MDIChild::receiveLine (QString line)
{
	QTextCursor position = textCursor();
	
	GoToPos();
	textCursor().insertText(line);
	
	setTextCursor( position );
}

QTextCharFormat MDIChild::setNewFormat(const Format& format)
{
	QTextCharFormat newFormat = currentCharFormat();
	
	if ( format.isBackgroundSet() )
	{
		qDebug() << "Background ok";
		newFormat.setBackground( format.backgroundBrush() );
	}
	if ( format.isForegroundSet() )
	{
		qDebug() << "Foreground ok";
		newFormat.setForeground( format.foregroundBrush() );
	}
	if ( format.isBoldSet() && format.bold() )
		newFormat.setFontWeight( QFont::Bold );
	else
		newFormat.setFontWeight( QFont::Normal );
	
	if ( format.isItalicSet() )
		newFormat.setFontItalic ( format.italic() );
	
	if ( format.isFontSet() )
		newFormat.setFont( format.font() );
	if ( format.isPointsSet() )
		newFormat.setFontPointSize( format.points() );
	
	qDebug() << "Fore: " << newFormat.foreground() << " Back: " << newFormat.background();
	
	return newFormat;
}



bool MDIChild::addFilter(GenericFilter filter, const Format& format)
{
	qDebug() << "addFilter" << filter.getName() << " isforegroundset? " << format.isForegroundSet();
	if ( filter.isSuppressor() )
		return false;
	if ( highlightFilter->contains( filter ) )
		return false;

// 	filter.setPriority( highlightFilter->count() );
	highlightFilter->insert(filter, setNewFormat(format) );
	return true;
}

bool MDIChild::addSuppressor(const GenericFilter& filter)
{
	qDebug() << filter.getName();
	if ( ! filter.isSuppressor() )
		return false;
	
	if ( suppressiveFilter.contains( filter ) )
		return false;
	suppressiveFilter.append(filter);
	return true;
}

