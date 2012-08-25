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


#ifndef FILTERVALIDATOR_H
#define FILTERVALIDATOR_H

#include "genericfilter.h"
#include <QValidator>

class FilterValidator:public QValidator
{
	Q_OBJECT
	
	filterType state;	//Regexp or not?

	QValidator* myValidator;
public:
	virtual QValidator::State validate (QString &, int &) const;
	explicit FilterValidator (const filterType& type, QObject* obj = 0 );
	virtual void fixup (QString &) const;
	virtual ~ FilterValidator () { delete myValidator; } 
public slots:
	void changeState(filterType type);
};

#endif // FILTERVALIDATOR_H
