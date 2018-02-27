#ifndef PRAUPDOWNTERMINALPLUGIN_H
#define PRAUPDOWNTERMINALPLUGIN_H


#include "praupdownwin.h"


class PraUpDownTerminalPlugin : public QObject, public PraupdownPluginBase
{

    Q_OBJECT

public:
    PraUpDownTerminalPlugin();

    ~PraUpDownTerminalPlugin();

protected:
    void initImpl(PraUpdownWin * oPraUpdownWin);

    void uninitImpl();

    void fdcBeforeRefreshImpl(const PraUpdownWin *oPraUpdownWin, Fdc *oFdc);

private:
    PraUpdownWin * configselfWin;

private slots:

};


class TerminalPluginFactory : public PraupdownPluginFactoryBase
{
public:
    std::string factoryName(){
        return "TerminalPlugin";
    }

    PraupdownPluginBase* createObject(){
        return new PraUpDownTerminalPlugin();
    }

};


#endif // PRAUPDOWNTERMINALPLUGIN_H
