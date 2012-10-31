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
#include "genericfilter.h"
#include "format.h"

bool
GenericFilter::match (const QString & s)
{
	qDebug() << "Match called";
	if ( ! filterString.isEmpty() )
	{
		qDebug() << "filter string";
		if ( s.contains(filterString) )
		{
			qDebug() << "Matched";
			return true;
		}
	}
	if ( ! filterReg.isEmpty() )
	{
		qDebug() << "regexp string";
		if ( filterReg.indexIn( s ) != -1 )
		{
			qDebug() << "regexp matched";
			return true;
		}
	}
	return false;
}

QString
GenericFilter::getString () const
{
	if ( filterString.count() > 0 )
		return filterString;

	return filterReg.pattern();
}

bool
GenericFilter::operator== (const GenericFilter & other) const
{
	if ( filterString.count() == 0 )
		return filterReg == other.filterReg;
	else
		return filterString == other.filterString;
}

uint qHash(const GenericFilter& filter)
{
	if ( ! filter.filterString.isEmpty() && ! filter.filterString.isNull() )
		return qHash(filter.name_) ^ qHash ( filter.filterString);

	return qHash(filter.name_) ^ qHash ( filter.filterReg.pattern() );
}

QDataStream &operator<<(QDataStream & out, const GenericFilter & filter)
{
	out << filter.name_;
	bool regexp = filter.filterString.isEmpty() || filter.filterString.isNull();

	out << regexp;
	if ( regexp )
		out << filter.filterReg.pattern();
	else
		out << filter.filterString;

	if ( filter.format == NULL )
		out << 0;
	else
	{
		out << 1;
		out << *(filter.format);
	}
		
	return out;
}
QDataStream &operator>>(QDataStream & in, GenericFilter & filter )
{
	in >> filter.name_;
	bool regexp;
	in >> regexp;

	if ( regexp )
	{
		QString regexpString;
		in >> regexpString;
		filter.filterReg = QRegExp ( regexpString );
	}
	else
		in >> filter.filterString;

	int switch_var;
	in >> switch_var;

	if ( switch_var != 0 )
	{
		Format* new_format = new Format();
		in >> (*new_format);
		filter.format = new_format;
	}
	
	return in;
}

GenericFilter::GenericFilter(const GenericFilter& other)
{
	filterReg = other.filterReg;
	filterString = other.filterString;
	priority = other.priority;
	name_ = other.name_;
	if ( other.format != NULL )
	{
		format = new Format();
		other.format->copyTo ( format );
	}
	else
		format = NULL;
}

