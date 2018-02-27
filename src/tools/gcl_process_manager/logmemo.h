#ifndef LOGMEMO_H
#define LOGMEMO_H

#include <QObject>
#include <QPlainTextEdit>
#include <QString>


class CLogMemo: public QObject
{
  Q_OBJECT

public:

  QPlainTextEdit *memo;
  CLogMemo (QPlainTextEdit *m);

public slots:

  void log (const QString &text);
};

#endif // LOGMEMO_H
