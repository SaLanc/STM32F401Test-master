#include "Sample.h"
#include "MS5803.h"

bool getSample_Blocking(ParaBeep_t *ParaBeep)
{
    // bool goodSample = true;

    ParaBeep->sample.sampleFeet = MS5803_GetAltitudeBlocking(&ParaBeep->sample);

    return true;
}