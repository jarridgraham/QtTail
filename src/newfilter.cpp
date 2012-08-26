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


#include "newfilter.h"
#include "filtervalidator.h"

#include <QDebug>
#include <QRegExp>

NewFilter::NewFilter (QWidget * parent):QDialog (parent), suppressor(false)
{
	ui.setupUi(this);

	ui.lineFilter->setValidator( new FilterValidator(MATCH, this)  );
	
	connect(ui.comboBoxFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilterType(int)));
	connect(ui.comboBoxMatch, SIGNAL(currentIndexChanged(int)),this, SLOT(changeFilterMatch(int)));
	connect(this, SIGNAL(typeChanged(filterType)),ui.lineFilter->validator(),SLOT(changeState(filterType)));
}

void
NewFilter::changeFilterType (int type)
{
	qDebug() << "Type: " << type;
	if (  type == 0 )
		suppressor = false;
	else
		suppressor = true;

	ui.groupBoxHighlight->setEnabled ( ! suppressor );

}

void
NewFilter::changeFilterMatch (int type)
{
	qDebug() << "Match: " << type;
	if ( type == 0 )
	{
		emit typeChanged(MATCH);
	}
	else
	{
		emit typeChanged(REGEXP);
	}
}

NewFilter::~NewFilter ()
{
	qDebug() << "New filter dialog closed";
}



void
NewFilter::changeEvent (QEvent * e)
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

QPair < GenericFilter, QTextCharFormat > NewFilter::getFilterAndFormat () const
{
	GenericFilter filter = getFilter();

	QTextCharFormat format;
	
	if ( !filter.isSuppressor() )
	{
		format = getFormat();
	}

	return QPair<GenericFilter, QTextCharFormat>(filter, format);
}

GenericFilter
NewFilter::getFilter () const
{
	if ( ui.comboBoxMatch->currentIndex() == MATCH )
	{
		GenericFilter ret(ui.lineFilter->text());
		ret.setName( ui.lineEditName->text());
		return ret;
	}

	if ( ui.comboBoxMatch->currentIndex() == REGEXP )
	{
		QRegExp expr(ui.lineFilter->text());
		GenericFilter ret(expr);
		ret.setName( ui.lineEditName->text());
		return ret;
	}

	return GenericFilter(QString(""));
}

QString NewFilter::getName() const
{
	return ui.lineEditName->text();
}

QTextCharFormat NewFilter::getFormat() const
{
	if ( suppressor )
		return QTextCharFormat();

	QTextCharFormat ret;
	ret.setFont ( ui.fontComboBox->currentFont() );

	QString fontSize = ui.fontSizeComboBox->currentText();
	
	ret.setFontPointSize( fontSize.toDouble() );

	QColor forecolor = getForeGroundColor();
	QBrush foreground(forecolor);
	ret.setForeground(foreground);

	QColor backcolor = getBackGroundColor();
	QBrush background(backcolor);
	ret.setBackground(background);

	return ret;
	
}

QColor NewFilter::getBackGroundColor() const
{
	int r, g, b;
	QString l = ui.lineEditBackgroundColor->text();

	if ( l.count() < 7 && l[0] != '#' )
		return QColor();

	r = ui.lineEditColor->text().mid(1,2).toInt();
	g = ui.lineEditColor->text().mid(3,2).toInt();
	g = ui.lineEditColor->text().mid(5,2).toInt();

	return QColor(r, g, b);
}

QColor NewFilter::getForeGroundColor() const
{
	int r, g, b;
	QString l = ui.lineEditColor->text();

	if ( l.count() < 7 && l[0] != '#' )
		return QColor();

	r = ui.lineEditColor->text().mid(1,2).toInt();
	g = ui.lineEditColor->text().mid(3,2).toInt();
	b = ui.lineEditColor->text().mid(5,2).toInt();

	return QColor(r, g, b);
}

