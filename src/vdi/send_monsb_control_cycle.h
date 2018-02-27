#ifndef SEND_MONSB_CONTROL_CYCLE_H
#define SEND_MONSB_CONTROL_CYCLE_H


#include "send_control_cycle_template.h"
#include "monsb_measure.h"


class SendMonsbControlCycle : public SendControlCycleTemplate<MonsbMeasure, int>
{
public:
    SendMonsbControlCycle() {
        setDefaultToValue(0);
    }

};

#endif // SEND_MONSB_CONTROL_CYCLE_H
