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


#ifndef FORMAT_H
#define FORMAT_H

#include <QObject>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QTextCharFormat>
#include <QDataStream>

class Format: public QObject
{
	Q_OBJECT

	QColor m_background;
	bool i_background;
	QColor m_foreground;
	bool i_foreground;
	double m_points;
	bool i_points;
	QFont m_font;
	bool i_font;
	bool m_italic;
	bool i_italic;
	bool m_bold;
	bool i_bold;
	
	Q_PROPERTY(QColor background READ background WRITE setBackground);
	Q_PROPERTY(QColor foreground READ foreground WRITE setForeground);
	Q_PROPERTY(double points READ points WRITE setPoints);
	Q_PROPERTY(QFont font READ font WRITE setFont);
	Q_PROPERTY(bool italic READ italic WRITE setItalic);
	Q_PROPERTY(bool bold READ bold WRITE setBold);
		
public:
	Format(QTextCharFormat tcf, QObject* parent=0);
	Format(QObject* parent=0): QObject(parent), i_background(false), i_foreground(false), m_points(-1.0), i_points(false), 
		i_font(false), m_italic(false), i_italic(false), 
		m_bold(false), i_bold(false){}

	void copyTo(Format* to); 
		
	QColor background() const { return m_background; }
	QColor foreground() const { return m_foreground; }
	double points() const {  return m_points; }
	QFont font() const { return m_font; }
	bool italic() const { return m_italic; }
	bool bold() const { return m_bold; }
	
	bool isBackgroundSet() const { return i_background; }
	bool isForegroundSet() const { return i_foreground; }
	bool isPointsSet() const { return i_points; }
	bool isFontSet() const { return i_font; }
	bool isItalicSet() const { return i_italic; }
	bool isBoldSet() const { return i_bold; }
	
	void setBackground(const QColor& color) { m_background = color; i_background = true; }
	void setForeground(const QColor& color) { m_foreground = color; i_foreground = true; }
	void setPoints(double size) { m_points = size; i_points = true; }
	void setFont(const QFont& font) { m_font = font; i_font = true; }
	void setItalic( bool italic ) { m_italic = italic; i_italic = true; }
	void setBold( bool bold ) { m_bold = bold; i_bold = true; }
	
	QBrush backgroundBrush() const { return QBrush(m_background); }
	QBrush foregroundBrush() const { return QBrush(m_foreground); }
};

QDataStream &operator<<(QDataStream &, const Format &);
QDataStream &operator>>(QDataStream &, Format &);

#endif // FORMAT_H
