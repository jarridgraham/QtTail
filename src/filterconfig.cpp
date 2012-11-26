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

void FilterConfig::changeEvent(QEvent *e)
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

void
FilterConfig::contextMenuEvent (QContextMenuEvent *event)
{
	QAction* add2current = new QAction(tr("Add to current file"), this);
	add2current->setToolTip(tr("Add selected filter to the current tailed file"));

	QAction* delthis = new QAction(tr("Delete"), this);
	delthis->setToolTip(tr("Delete selected filter from global pool"));

	QModelIndexList selection = getSelectedItems();

	if ( selection.isEmpty() )
		return;

	QAction* edit = NULL;
	if ( selection.count() == 1 )
	{
		edit = new QAction(tr("Edit filter"), this);
		edit->setToolTip(tr("Edit selected filter"));
	}

	QMenu contextual(this);
	if ( Model->data(QModelIndex(), TypeRole) == GLOBAL )
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
		GenericFilter filter = Model->data( bookmark, Qt::UserRole ).value<GenericFilter>();
		editWindow = new NewFilter(this, filter);
		connect(editWindow,SIGNAL(accepted()),this,SLOT(updateFilter()));
		editWindow->show();
	}
}

void FilterConfig::updateFilter()
{
	GenericFilter filter = editWindow->getFilter();

	filter.setPriority( bookmark.row() );

	Model->setData ( bookmark, filter, Qt::UserRole );
}

void
FilterConfig::deleteMultipleRows ( const QModelIndexList& list )
{
	foreach ( QModelIndex item, list )
	{
		GenericFilter filter = Model->data(item, Qt::UserRole).value<GenericFilter>();
		Model->removeRow( item.row() );
		emit deleteFilter ( filter );
	}
}

void
FilterConfig::addMultipleRows ( const QModelIndexList& list )
{
	foreach ( QModelIndex item, list )
	{
		GenericFilter filter = Model->data(item, Qt::UserRole).value<GenericFilter>();
		emit addFilter ( filter );
	}
}

void FilterConfig::sort()
{
	Model->sort(2);
}

QModelIndexList FilterConfig::getSelectedItems() const
{
	QItemSelectionModel* SelectionModel = ui.tableFilters->selectionModel();

	QModelIndexList selected = SelectionModel->selectedRows();
	
	return selected;
}

FilterConfig::FilterConfig(QAbstractItemModel* mod, QWidget* parent): QDialog(parent ),Model(mod)
{
	ui.setupUi(this);
	ui.tableFilters->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableFilters->setModel(mod);
	ui.tableFilters->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	connect(ui.pbSort, SIGNAL(clicked()), this, SLOT(sort()) );
}

void FilterConfig::accept()
{
	Model->submit();
	QDialog::accept();
}


