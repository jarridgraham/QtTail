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
#include <QStringList>

NewFilter::NewFilter (QWidget * parent, int defaultFontWeight):QDialog (parent), suppressor(false)
{
	ui.setupUi(this);

	ui.lineFilter->setValidator( new FilterValidator(MATCH, this)  );
	
	int n = -1;
	QStringList fontSizes;
	for ( int i = 8; i < 64; i+=4)
	{
		fontSizes.append( QString::number(i));
		if ( i <= defaultFontWeight ) ++n;
			
	}
	
	ui.fontSizeComboBox->insertItems(0, fontSizes);
	
	if ( defaultFontWeight > 0 && n >= 0 )
	{
		qDebug() << "Default: " << defaultFontWeight << " n: " << n;
		ui.fontSizeComboBox->setCurrentIndex( n );
	}
	
	setConnections();
}

NewFilter::NewFilter (QWidget * parent, GenericFilter filter_, int defaultFontWeight):
	QDialog(parent)
{
	ui.setupUi(this);

	ui.lineFilter->setValidator( new FilterValidator(MATCH, this)  );

	int n = -1;
	QStringList fontSizes;
	for ( int i = 8; i < 64; i+=4)
	{
		fontSizes.append( QString::number(i));
		if ( i <= defaultFontWeight ) ++n;

	}

	ui.fontSizeComboBox->insertItems(0, fontSizes);

	if ( defaultFontWeight > 0 && n >= 0 )
	{
		ui.fontSizeComboBox->setCurrentIndex( n );
	}
	
	setConnections();
	setFilter(filter_);

	if ( ! filter_.isSuppressor() )
		setFormat ( filter_.getFormat() );
}

void
NewFilter::setConnections ()
{
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

// QPair < GenericFilter, Format*> NewFilter::getFilterAndFormat () const
// {
// 	GenericFilter filter = getFilter();
// 
// 	Format* format;
// 	
// 	if ( !filter.isSuppressor() )
// 	{
// 		format = getFormat();
// 	}
// 
// 	return QPair<GenericFilter, Format*>(filter, format);
// }

GenericFilter
NewFilter::getFilter () const
{
	if ( ui.comboBoxMatch->currentIndex() == MATCH )
	{
		GenericFilter ret(ui.lineFilter->text());
		ret.setName( ui.lineEditName->text());
		if ( ! suppressor )
			ret.setFormat( getFormat() );
		return ret;
	}

	if ( ui.comboBoxMatch->currentIndex() == REGEXP )
	{
		QRegExp expr(ui.lineFilter->text());
		GenericFilter ret(expr);
		ret.setName( ui.lineEditName->text());
		if ( ! suppressor )
			ret.setFormat( getFormat() );		
		return ret;
	}

	return GenericFilter(QString(""));
}

QString NewFilter::getName() const
{
	return ui.lineEditName->text();
}

QString NewFilter::getColorString(const QColor& color) const
{
	QString retVal("#%1%2%3");
	retVal.arg(color.red(),2,10).arg(color.green(),2,10).arg(color.blue(),2,10);

	qDebug() << "retVal: " << retVal;
	return retVal;
}

void NewFilter::setFilter(const GenericFilter& filter )
{
	ui.lineEditName->setText(filter.getName());
	ui.lineFilter->setText( filter.getString() );

	if ( filter.isSuppressor() )
	{
		suppressor = true;
		ui.comboBoxFilter->setCurrentIndex( 1 );
	}
	else
	{
		ui.comboBoxFilter->setCurrentIndex( 0 );
		suppressor = false;
	}
	if ( filter.isMatch() )
		ui.comboBoxMatch->setCurrentIndex( 0 );
	if ( filter.isRegExp() )
		ui.comboBoxMatch->setCurrentIndex( 1 );
}



void NewFilter::setFormat(const Format* format)
{
	if ( format->isFontSet() )
		ui.fontComboBox->setCurrentFont( format->font() );
	if ( format->isBoldSet() )
		ui.checkBoxBold->setChecked( format->bold() );
	if ( format->isItalicSet() )
		ui.checkBoxItalic->setChecked( format->italic() );
	
	if ( format->isBackgroundSet() )
		ui.lineEditBackgroundColor->setText( getColorString( format->background() ) );

	if ( format->isForegroundSet() )
		ui.lineEditColor->setText ( getColorString( format->foreground() ) );
}


Format* NewFilter::getFormat() const
{
	if ( suppressor )
		return 0;

	Format* ret = new Format();
	ret->setFont ( ui.fontComboBox->currentFont() );

	QString fontSize = ui.fontSizeComboBox->currentText();
	
	ret->setPoints( fontSize.toDouble() );

	ret->setForeground( getForeGroundColor() );
	ret->setBackground( getBackGroundColor() );
	ret->setBold( ui.checkBoxBold->isChecked() );
	ret->setItalic( ui.checkBoxItalic->isChecked() );
	
	return ret;
	
}

QColor NewFilter::colorParse(QString col) const
{
	int r,g,b;
	
	if ( col.count() < 7 && col[0] != '#' )
		return QColor();

	r = col.mid(1,2).toInt(0, 16);
	g = col.mid(3,2).toInt(0, 16);
	b = col.mid(5,2).toInt(0, 16);

	QColor ret(r,g,b);
	qDebug() << "color: " << ret << " r: " << r << " g: " << g << " b: " << b;
	qDebug() << ret.red() << " - " << ret.green() << " - " << ret.blue();
	return ret;
}

QColor NewFilter::getBackGroundColor() const
{
	return colorParse ( ui.lineEditBackgroundColor->text() );
}

QColor NewFilter::getForeGroundColor() const
{
	return colorParse(ui.lineEditColor->text() );
}

