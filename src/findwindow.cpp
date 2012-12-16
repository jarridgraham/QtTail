#include "findwindow.h"
#include <QTextDocument>

FindWindow::FindWindow (QWidget* parent): QDialog(parent)
{
	ui.setupUi(this);
}

void
FindWindow::accept ()
{
	if (  ui.cbRegexp->isChecked() )
	{
		QRegExp exp(ui.leString->text(), ui.cbCaseSensitive->isChecked()? Qt::CaseSensitive:  Qt::CaseInsensitive );
		emit find( exp, packFlags() );
	}
	else
		emit find( ui.leString->text(), packFlags() );
// 	QDialog::accept ();
}

QTextDocument::FindFlags FindWindow::packFlags () const
{
	QTextDocument::FindFlags ret = 0;
	if ( ui.cbWholeWord->isChecked() )
		ret |=  QTextDocument::FindWholeWords;
	if ( ui.rbUp->isChecked() )
		ret |= QTextDocument::FindBackward;
	if ( ui.cbCaseSensitive->isChecked() )
		ret |= QTextDocument::FindCaseSensitively;

	return ret;
}

void FindWindow::closeEvent(QCloseEvent* event)
{
	emit close();
}

