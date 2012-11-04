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


#include "filterconfig.h"
#include "genericfilter.h"
#include "newfilter.h"
#include <QEvent>
#include <QMenu>
#include <QItemSelectionModel>
#include <QDebug>

void FilterConfig :: changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type())
	{
		case QEvent::LanguageChange:
			ui.retranslateUi(this);
			break;
		default:
			break;
	}
}

//! @note If single line we can edit!
void
FilterConfig::contextMenuEvent (QContextMenuEvent *event)
{
	QAction* add2current = new QAction(tr("Add to current file"), this);
	add2current->setToolTip(tr("Add selected filter to the current tailed file"));

	QAction* delthis = new QAction(tr("Delete"), this);
	delthis->setToolTip(tr("Delete selected filter from global pool"));

	QModelIndexList selection = getSelectedItems();

	QAction* edit = NULL;
	if ( selection.count() == 1 )
	{
		edit = new QAction(tr("Edit filter"), this);
		edit->setToolTip(tr("Edit selected filter"));
	}

	QMenu contextual(this);
	contextual.addAction(add2current);
	contextual.addAction(delthis);
	if ( edit != NULL ) contextual.addAction(edit);

	QAction* selected = contextual.exec( event->globalPos() );
	
	if ( selected == delthis )
	{
		deleteMultipleRows( selection );
	}
	else if ( selected == add2current )
	{
		addMultipleRows ( selection );
	}
	else if ( selected == edit )
	{
		bookmark = selection.front();
		GenericFilter filter = Model->data( bookmark ).value<GenericFilter>();
		editWindow = new NewFilter(this, filter);
		connect(editWindow,SIGNAL(accepted()),this,SLOT(updateFilter()));
		editWindow->show();
	}
}

void FilterConfig::updateFilter()
{
	GenericFilter filter = editWindow->getFilter();

	Model->setData ( bookmark, filter );
}

void
FilterConfig::deleteMultipleRows ( const QModelIndexList& list )
{
	foreach ( QModelIndex item, list )
	{
		Model->removeRow( item.row() );
	}
}

void
FilterConfig::addMultipleRows ( const QModelIndexList& list )
{
	foreach ( QModelIndex item, list )
	{
		GenericFilter filter;
	}
}

QModelIndexList FilterConfig::getSelectedItems() const
{
	QItemSelectionModel* SelectionModel = ui.tableFilters->selectionModel();

	QModelIndexList selected = SelectionModel->selectedRows();
	
	return selected;
}

FilterConfig::FilterConfig(QAbstractTableModel* mod, QWidget* parent): QDialog(parent ),Model(mod)
{
	ui.setupUi(this);
	ui.tableFilters->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableFilters->setModel(mod);

	const QItemSelectionModel* SelectionModel = ui.tableFilters->selectionModel();

	QObject::connect (ui.tableFilters,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(contextMenuEvent(const QPoint &)));

	ui.tableFilters->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

