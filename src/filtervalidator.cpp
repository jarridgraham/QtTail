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


#include "filtervalidator.h"

#include <QRegExpValidator>

QValidator::State FilterValidator::validate (QString & input, int & pos) const 
{
	if ( myValidator != NULL )
	{
		return myValidator->validate( input, pos);
	}
	return QValidator::Acceptable;
}

FilterValidator::FilterValidator (const filterType& type, QObject* obj): state(type), QValidator(obj)
{
	if ( state == REGEXP )
		myValidator = new QRegExpValidator();
	else
		myValidator = NULL;
	
}
 
void FilterValidator::changeState(filterType type)
{
	if ( type == REGEXP )
		myValidator = new QRegExpValidator();
	else
	{
		delete myValidator;
		myValidator = NULL;
	}
}


void
FilterValidator::fixup (QString & s) const
{
	QValidator::fixup (s);
}
