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


#ifndef GENERICFILTER_H
#define GENERICFILTER_H
#include <QString>
#include <QHash>
#include <QDataStream>
#include <QRegExp>
#include <QVariant>
#include "format.h"

enum filterType { REGEXP = 1, MATCH = 0 };
enum filterBehaviour { HIGHLIGHT = 0, SUPPRESS = 1 };

class GenericFilter
{
	QString name_;
	QString filterString;
	QRegExp filterReg;
	int priority;

	Format* format;
	
	friend uint qHash(const GenericFilter& filter);
	friend QDataStream &operator<<(QDataStream &, const GenericFilter &);
	friend QDataStream &operator>>(QDataStream &, GenericFilter &);
public:
	GenericFilter(): priority(-1), format(NULL) {}
	GenericFilter (const QString& filter, Format* format_ = NULL): filterString(filter),priority(-1),format(format_) {}
	GenericFilter (const QRegExp& filter, Format* format_ = NULL): filterReg(filter),priority(-1), format(format_) {}
	GenericFilter (const GenericFilter& other);
	void setPriority(int prio) { priority= prio; }
	int getPriority() const { return priority; }
	QString getName() const { return name_; }
	bool isRegExp() const { return filterReg.isValid(); }
	bool isMatch() const { return ! filterString.isEmpty(); }
	void setString(QString filter) { filterString = filter; filterReg = QRegExp(); }
	void setString(QRegExp filter) { filterReg = filter; filterString.clear(); }
	void setName(QString n) { name_ = n; }
	QString getString() const;
	bool operator<(const GenericFilter& other) const { return priority < other.priority; }
	bool operator==(const GenericFilter& other) const;
	bool match(const QString& s) const;
	bool isSuppressor() const { return format == NULL; }

	void setFormat(Format* format_) { if ( format != NULL ) format->deleteLater(); format = format_; }
	Format* getFormat() { return format; }
	
	operator QVariant() const
	{
		return QVariant::fromValue(*this);
	}
	~GenericFilter() { if ( format != NULL ) delete format; }
};

uint qHash(const GenericFilter& filter);
QDataStream &operator<<(QDataStream &, const GenericFilter &);
QDataStream &operator>>(QDataStream &, GenericFilter &);

Q_DECLARE_METATYPE(GenericFilter);

#endif // GENERICFILTER_H
