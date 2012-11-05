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

#include <QDebug>
#include "highlighter.h"

Highlighter::Highlighter (QTextEdit* parent, QMap<GenericFilter, QTextCharFormat>* filters_ ): QSyntaxHighlighter(parent), filters(filters_)
{

}

Highlighter::~Highlighter ()
{

}

void
Highlighter::highlightBlock (const QString & text)
{
	if ( text.isEmpty() ) 
		return;
	
	qDebug() << "highlightBlock count: " << filters->count() << "text: " << text;
	// We need a logic for strings that match multiple filter
	foreach (GenericFilter g, filters->keys() )
	{
		qDebug() << "Text: " << text << " g: " << g.getString(); 
		if ( ! g.isSuppressor() && g.match(text) )
		{
			qDebug() << "red: " << filters->value(g).foreground().color().red();
			qDebug() << "blue: " << filters->value(g).foreground().color().blue();
			qDebug() << "green: " << filters->value(g).foreground().color().green();
			//qDebug() << "Value " << qobject_cast<QTextCharFormat> ( filters->value(g) ).
			setFormat(0, text.length(), filters->value(g) );
		}
	}
}

