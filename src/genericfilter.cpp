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

bool
GenericFilter::match (const QString & s)
{
	qDebug() << "Match called";
	if ( ! filterString.isEmpty() )
	{
		if ( s.contains(filterString) )
			return true;
	}
	if ( ! filterReg.isEmpty() )
	{
		//TODO 
		//Do we match in this way?
		if ( s.contains(filterReg) )
			return true;
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
	if ( suppressor != other.suppressor )
		return false;
	
	if ( filterString.count() == 0 )
	{
		if ( filterReg == other.filterReg )
			return true;
		return false;
	}
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

	out << filter.suppressor;
	
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

	in >> filter.suppressor;
	
	return in;
}
