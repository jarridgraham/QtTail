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

#include <QtAlgorithms>
#include "filtermodel.h"
#include <QDebug>

enum Columns { NAME = 0, STRING = 1, PRIO = 2, LAST = PRIO  };
	

FilterModel::FilterModel ( QObject * parent):QAbstractTableModel (parent)
{
}

void
FilterModel::setData (const QList < GenericFilter >& filters)
{
	rawData = filters;
}

void
FilterModel::setData (const GenericFilter & filter)
{
	rawData.append( filter );
}


QVariant
FilterModel::data (const QModelIndex & index, int role) const
{
	if ( !index.isValid() )
		return QVariant();

	if ( index.row() > rawData.count() || index.column() > LAST )
		return QVariant();

	if ( role == Qt::DisplayRole )
	{
		if ( index.column() == NAME )
			return rawData.at(index.row()).getName();
		if ( index.column() == STRING )
			return rawData.at(index.row()).getString();
		if ( index.column() == PRIO )
			return rawData.at(index.row()).getPriority();
	}
	return QVariant();
}

int
FilterModel::columnCount (const QModelIndex & parent) const
{
	return LAST+1;
}

int
FilterModel::rowCount (const QModelIndex & parent) const
{
	return rawData.count();
}


bool
FilterModel::setData (const QModelIndex & index, const QVariant & value,
		      int role)
{
	if ( type == DOCUMENT )
		return false;
	if ( index.isValid() && role == Qt::EditRole && index.column() == PRIO )
	{
		bool ok = false;
		int priority = value.toInt( &ok );
		if ( ok )
		{
			rawData[(index.row())].setPriority( priority );
			emit dataChanged( index, index  );
			return true;
		}
	}
	return false;
}

QVariant
FilterModel::headerData (int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if ( orientation == Qt::Vertical )
	{
		return QVariant();
		if ( section == NAME )
			return tr("Name");

		if ( section == STRING )
			return tr("String");

		if ( section == PRIO )
			return tr("Priority");
	}

	if ( orientation == Qt::Horizontal )
	{		
		if ( section == NAME )
			return tr("Name");

		if ( section == STRING )
			return tr("String");

		if ( section == PRIO )
			return tr("Priority");
	}

	return QVariant();
}

bool
FilterModel::insertRows (int row, int count, const QModelIndex & parent)
{
	if ( row > rawData.count() )
		return false;
	
	beginInsertRows(QModelIndex(), row, row+count );

	for (int irow = 0; irow < count; ++irow)
	{
		rawData.insert(row, GenericFilter());
	}
	
	endInsertRows();
	return true;
}

bool
FilterModel::insertColumns (int column, int count, const QModelIndex & parent)
{
	return false;
}

bool
FilterModel::removeRows (int row, int count, const QModelIndex & parent)
{
	beginRemoveRows(QModelIndex(), row, row+count);

	for ( int irow = 0; irow < count; ++irow )
		rawData.removeAt(row);
	
	endRemoveRows();
	return true;
}

bool
FilterModel::removeColumns (int column, int count, const QModelIndex & parent)
{
	return false;
}

Qt::ItemFlags FilterModel::flags (const QModelIndex & index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     if ( index.column() != PRIO )
	     return Qt::ItemIsSelectable;

     return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void FilterModel::sort(int column, Qt::SortOrder order)
{
	qStableSort(rawData);
}

