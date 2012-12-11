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
	for ( int i = 8; i < 64; i+=4)
	{
		QString item = QString::number(i);
 		ui.fontSizeComboBox->addItem(item, static_cast<double>(i));
		if ( i <= defaultFontWeight ) ++n;
	}

	
	if ( defaultFontWeight > 0 && n >= 0 )
	{
		qDebug() << "Default: " << defaultFontWeight << " n: " << n;
		ui.fontSizeComboBox->setCurrentIndex( n );
	}
	
	setConnections();

	fgDialog = bgDialog = NULL;
}

NewFilter::NewFilter (QWidget * parent, GenericFilter filter_, int defaultFontWeight):
	QDialog(parent)
{
	ui.setupUi(this);

	ui.lineFilter->setValidator( new FilterValidator(MATCH, this)  );

	int n = -1;
	for ( int i = 8; i < 64; i+=4)
	{
		QString item = QString::number(i);
 		ui.fontSizeComboBox->addItem(item, QVariant(static_cast<double>(i)));
		if ( i <= defaultFontWeight ) ++n;
	}

	if ( defaultFontWeight > 0 && n >= 0 )
	{
		ui.fontSizeComboBox->setCurrentIndex( n );
	}
	
	setConnections();
	setFilter(filter_);

	if ( ! filter_.isSuppressor() )
		setFormat ( filter_.getFormat() );
	else 
		ui.groupBoxHighlight->setEnabled ( false );

	fgDialog = bgDialog = NULL;
}

void
NewFilter::setConnections ()
{
	connect(ui.comboBoxFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilterType(int)));
	connect(ui.comboBoxMatch, SIGNAL(currentIndexChanged(int)),this, SLOT(changeFilterMatch(int)));
	connect(this, SIGNAL(typeChanged(filterType)),ui.lineFilter->validator(),SLOT(changeState(filterType)));
}

void
NewFilter::on_pbColorFG_clicked ()
{
	QColor foreground = colorParse( ui.lineEditColor->text() );

	if ( fgDialog == NULL )
		fgDialog = new QColorDialog( foreground, this );

	connect(fgDialog,SIGNAL(colorSelected(const QColor&)),this,SLOT(fgColor(const QColor&)));
	fgDialog->show();
}

void
NewFilter::fgColor (const QColor& foreground)
{
// 	QColor foreground = fgDialog->selectedColor();
	fgDialog->deleteLater();
	fgDialog = NULL;

	ui.lineEditColor->setText ( getColorString( foreground ) );
}


void
NewFilter::bgColor (const QColor& background)
{
	//QColor background = bgDialog->selectedColor();
	bgDialog->deleteLater();
	bgDialog = NULL;

	ui.lineEditBackgroundColor->setText( getColorString( background ) );
}


void
NewFilter::on_pbColorBG_clicked ()
{
	QColor background = colorParse( ui.lineEditBackgroundColor->text() );

	if ( bgDialog == NULL )
		bgDialog = new QColorDialog( background, this );

	connect(bgDialog,SIGNAL(colorSelected(const QColor&)),this,SLOT(bgColor(const QColor&)));
	bgDialog->show();
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
	return QString("#%1%2%3").arg(color.red(),2,16,QChar('0')).arg(color.green(),2,16,QChar('0')).arg(color.blue(),2,16,QChar('0')).toUpper();
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
	if ( format->isPointsSet() )
	{
		int index = ui.fontSizeComboBox->findData( QVariant( format->points() ));
		if ( index < 0 ) index = 0;
		ui.fontSizeComboBox->setCurrentIndex(index);
	}
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
// 	qDebug() << "color: " << ret << " r: " << r << " g: " << g << " b: " << b;
// 	qDebug() << ret.red() << " - " << ret.green() << " - " << ret.blue();
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

