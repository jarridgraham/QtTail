#ifndef _MDICHILD__H
#define _MDICHILD__H

#include <QTextEdit>
#include <QList>
#include <QMap>
#include "highlighter.h"
#include "genericfilter.h"
#include "tail.h"
#include "format.h"

class MDIChild: public QTextEdit
{
	Q_OBJECT
	
 	QString curFile;
	Tail* worker;
	QMap<GenericFilter, QTextCharFormat> filters;
	Highlighter* highlighter;
	
	void GoToPos(int position = -1);
	QTextCharFormat setNewFormat(const Format& format);
	bool doAddFilter(GenericFilter filter);
	bool toBeSuppressed(QString text) const;
	void suppress();
protected:
	void closeEvent(QCloseEvent *event);
	
public:
	MDIChild(const QString& fileName, int defaultpointsize = 8);
	virtual ~MDIChild();
	bool addFilter(GenericFilter filter);
	void removeFilter(GenericFilter filter);
	void updateAllFilters(QList<GenericFilter> newfilters);

	QList<GenericFilter> getFilters() const { return filters.keys(); }

	bool isValid() const;
	QString currentFile() const { return curFile; }

public slots:
	void receiveLine(QString line);
};

#endif
