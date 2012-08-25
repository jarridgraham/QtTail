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


#ifndef FILTERCONFIG_H
#define FILTERCONFIG_H

#include <QDialog>
#include "ui_filtersconfig.h"

enum FilterConfigType { DOCUMENT = 0, GLOBAL = 1 };

class FilterConfig : public QDialog
{
	Q_OBJECT

	Ui::FilterConfigDialog ui;
	FilterConfigType type;
	QAbstractTableModel* Model;
protected:
	void changeEvent(QEvent *e);
public: 
	FilterConfig (QAbstractTableModel* mod, FilterConfigType Type = DOCUMENT, QWidget * parent = 0);
	QAbstractTableModel* model() const { return Model; }
	virtual ~FilterConfig () { }

	// TODO return values from dialog

	
};

#endif // FILTERCONFIG_H
