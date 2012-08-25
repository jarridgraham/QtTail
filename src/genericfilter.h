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

enum filterType { REGEXP = 1, MATCH = 0 };
enum filterBehaviour { HIGHLIGHT = 0, SUPPRESS = 1 };

class GenericFilter
{
	QString name_;
	QString filterString;
	QRegExp filterReg;
	int priority;
	bool suppressor;
	friend uint qHash(const GenericFilter& filter);
	friend QDataStream &operator<<(QDataStream &, const GenericFilter &);
	friend QDataStream &operator>>(QDataStream &, GenericFilter &);
public:
	GenericFilter(): priority(-1), suppressor(false) {}
	GenericFilter (const QString& filter, bool suppress = false): filterString(filter),suppressor(suppress) {}
	GenericFilter (const QRegExp& filter, bool suppress = false): filterReg(filter), suppressor(suppress) {}
	void setPriority(int prio) { priority= prio; }
	int getPriority() const { return priority; }
	QString getName() const { return name_; }
	void setString(QString filter) { filterString = filter; filterReg = QRegExp(); }
	void setString(QRegExp filter) { filterReg = filter; filterString.clear(); }
	void setName(QString n) { name_ = n; }
	QString getString() const;
	bool operator<(const GenericFilter& other) const { return priority < other.priority; }
	bool operator==(const GenericFilter& other) const;
	bool match(const QString& s);
	bool isSuppressor() const { return suppressor; }
};

uint qHash(const GenericFilter& filter);
QDataStream &operator<<(QDataStream &, const GenericFilter &);
QDataStream &operator>>(QDataStream &, GenericFilter &);

#endif // GENERICFILTER_H
