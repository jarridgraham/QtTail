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
#include <QMap>
#include <QList>
#include <QTextCharFormat>
#include "format.h"
#include "genericfilter.h"

enum FilterConfigType { DOCUMENT = 0, GLOBAL = 1 };

class FilterModel:public QAbstractTableModel
{
	Q_OBJECT

	FilterConfigType type;
	QList<GenericFilter> rawData;
public:
	FilterModel ( QList<GenericFilter> filters, QObject* parent = 0);

	void setType(const FilterConfigType& t) { type  = t; }
	
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
	virtual bool insertRows (int row, int count, const QModelIndex & parent =
			   QModelIndex ());
	virtual bool insertColumns (int column, int count,
			      const QModelIndex & parent = QModelIndex ());
	virtual bool removeRows (int row, int count, const QModelIndex & parent =
			   QModelIndex ());
	virtual bool removeColumns (int column, int count,
			      const QModelIndex & parent = QModelIndex ());
	virtual Qt::ItemFlags flags (const QModelIndex & index) const;
	virtual void sort ( int column = 0, Qt::SortOrder order = Qt::AscendingOrder );
	virtual ~ FilterModel ();
public slots:
	virtual bool submit();
signals:
	void deleteFilter(GenericFilter);
	void addFilter(GenericFilter);
	void newFilters(const QList<GenericFilter>& filters) const;
};

#endif // FILTERMODEL_H
