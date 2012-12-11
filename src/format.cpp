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


#include "format.h"

Format::Format (QTextCharFormat tcf, QObject* parent):QObject (parent)
{
	setBackground( tcf.background().color() );
	setForeground( tcf.foreground().color() );
	setBold( tcf.fontWeight() >= QFont::Bold );
	setItalic( tcf.fontItalic() );
	setPoints ( tcf.fontPointSize() );
}


QDataStream &operator<<(QDataStream &in, const Format &out)
{
	if ( out.isBackgroundSet() )
	{
		in << "back";
		in << out.background();
	}
	if ( out.isForegroundSet() )
	{
		in << "fore";
		in << out.foreground();
	}
	if ( out.isBoldSet() )
	{
		in << "bold";
		in << out.bold();
	}
	if ( out.isItalicSet() )
	{
		in << "italic";
		in << out.italic();
	}
	if ( out.isFontSet() )
	{
		in << "font";
		in << out.font();
	}
	if ( out.isPointsSet() )
	{
		in << "points";
		in << out.points();
	}
	
	return in;
}

void
Format::copyTo (Format * to)
{
	if ( isBackgroundSet() )
		to->setBackground( background() );
	if ( isForegroundSet() )
		to->setForeground( foreground() );
	if ( isFontSet() )
		to->setFont( font() );
	if ( isItalicSet() )
		to->setItalic( italic() );
	if ( isPointsSet() )
		to->setPoints( points() );
	if ( isBoldSet() )
		to->setBold ( bold() );
}


//NOTE order of strings is very important: it needs to be synced between the two operators!
QDataStream &operator>>(QDataStream & out, Format &in)
{
	QString lookahead;
	out >> lookahead;
	if ( lookahead == "back" )
	{
		QColor background;
		out >> background;
		in.setBackground(background);
		out >> lookahead;
	}
	if ( lookahead == "fore" )
	{
		QColor foreground;
		out >> foreground;
		in.setForeground(foreground);
		out >> lookahead;
	}
	if ( lookahead == "bold" )
	{
		bool bold;
		out >> bold;
		in.setBold(bold);
		out >> lookahead;
	}
	if ( lookahead == "italic" )
	{
		bool italic;
		out >> italic;
		in.setItalic(italic);
		out >> lookahead;
	}
	if ( lookahead == "font" )
	{
		QFont font;
		out >> font;
		in.setFont(font);
		out >> lookahead;
	}
	if ( lookahead == "points") 
	{
		double points;
		out >> points;
		in.setPoints(points);
	}
	return out;
}
