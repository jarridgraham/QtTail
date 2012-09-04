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
#include <QEvent>
#include <QMenu>
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

void FilterConfig::add2current()
{
	action = 1;
}

void FilterConfig::delCurrent()
{
	action = 2;
}


void FilterConfig::menuAdd(const QModelIndex& index)
{
	QAction* add2current = new QAction(tr("Add to current file"), this);
	add2current->setToolTip(tr("Add this filter to the current tailed file"));
	connect(add2current, SIGNAL(clicked()), this, SLOT(add2current()));
	
	QAction* delthis = new QAction(tr("Delete"), this);
	delthis->setToolTip(tr("Delete this filter from global pool"));
	connect(delthis,SIGNAL(clicked()), this, SLOT(delthis()));
	
	QMenu contextual(this);
	contextual.addAction(add2current);
	contextual.addAction(delthis);
	
	QAbstractItemModel* model = ui.tableFilters->model();
	
	if ( action == 2 )
	{
	
		QVariant filter = model->data(index);
		
		if ( filter.canConvert<GenericFilter>() )
		{
// 			GenericFilter realFilter = filter.value();
			emit addFilter(filter.value<GenericFilter>());
		}
	}
	else if ( action == 1 )
	{
		model->removeRow( index.row() );
	}
	
	action = 0;
}


FilterConfig::FilterConfig (QAbstractTableModel * mod, QWidget * parent):
	QDialog(parent), Model(mod)
{
	ui.setupUi(this);
	ui.tableFilters->setModel(mod);
	
	connect(ui.tableFilters, SIGNAL(clicked(const QModelIndex &)), 
		this, SLOT(menuAdd(const QModelIndex&)));

	ui.tableFilters->horizontalHeader()->setResizeMode(QHeaderView::Stretch);;
}

