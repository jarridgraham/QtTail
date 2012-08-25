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


#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "genericfilter.h"

class FilterModel:public QAbstractTableModel
{
	Q_OBJECT

	QList<GenericFilter> rawData;
public:
	explicit FilterModel ( QObject * parent = 0);

	void setData(const QList<GenericFilter>& filters);
	void setData(const GenericFilter& filter);

	
	virtual QVariant data (const QModelIndex & index, int role =
			 Qt::DisplayRole) const;
	virtual int columnCount (const QModelIndex & parent = QModelIndex ())const;
	virtual int rowCount (const QModelIndex & parent = QModelIndex ())const;
	virtual bool setData (const QModelIndex & index, const QVariant & value,
			int role = Qt::EditRole);
	virtual QVariant headerData (int section, Qt::Orientation orientation,
			       int role = Qt::DisplayRole) const;
// 	virtual QMap < int, QVariant > itemData (const QModelIndex & index) const;
	virtual bool insertRows (int row, int count, const QModelIndex & parent =
			   QModelIndex ());
	virtual bool insertColumns (int column, int count,
			      const QModelIndex & parent = QModelIndex ());
	virtual bool removeRows (int row, int count, const QModelIndex & parent =
			   QModelIndex ());
	virtual bool removeColumns (int column, int count,
			      const QModelIndex & parent = QModelIndex ());
	virtual Qt::ItemFlags flags (const QModelIndex & index) const;
	virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );
	virtual ~ FilterModel () {}
};

#endif // FILTERMODEL_H
