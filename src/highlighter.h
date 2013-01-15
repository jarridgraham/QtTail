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


#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QMap>
#include "genericfilter.h"

// Note: most of its implementation is in highlightBlock - How can we Unit test this?
class Highlighter: public QSyntaxHighlighter
{
	QMap<GenericFilter,QTextCharFormat>* filters;
protected:
	void highlightBlock(const QString &text);
public:
	Highlighter (QTextDocument* parent, QMap<GenericFilter, QTextCharFormat>* filters_);
	virtual ~ Highlighter ();
};

#endif // HIGHLIGHTER_H
