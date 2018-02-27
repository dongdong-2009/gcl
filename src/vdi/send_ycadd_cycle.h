#ifndef SEND_YCADD_CYCLE_H
#define SEND_YCADD_CYCLE_H

#include "protocol_global.h"


#include "send_measure_cycle_template.h"
#include "ycadd_measure.h"

class SendYcaddCycle : public SendMeasureCycleTemplate<YcaddMeasure, double>
{
public:

};

#endif // SEND_YCADD_CYCLE_H
