#include "logmemo.h"

#include <QTime>
#include <QTextCursor>


CLogMemo::CLogMemo (QPlainTextEdit *m)
{
  memo = m;
  setObjectName ("logmemo");
  memo->setFocusPolicy (Qt::ClickFocus);
  memo->setUndoRedoEnabled (false);
  memo->setReadOnly (true);
}


void CLogMemo::log (const QString &text)
{
  QTextCursor cr = memo->textCursor();
  cr.movePosition (QTextCursor::Start);
  cr.movePosition (QTextCursor::Down, QTextCursor::MoveAnchor, 0);
  memo->setTextCursor (cr);

  QTime t = QTime::currentTime();

  memo->textCursor().insertHtml ("[" + t.toString("hh:mm:ss") + "] " + text + "<br>");

  cr = memo->textCursor();
  cr.movePosition (QTextCursor::Start);
  cr.movePosition (QTextCursor::Down, QTextCursor::MoveAnchor, 0);
  memo->setTextCursor (cr);
}
