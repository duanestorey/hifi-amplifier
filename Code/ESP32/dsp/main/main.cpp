#include <stdio.h>
#include "dsp.h"

extern "C" void app_main(void)
{
    DSP *dsp = new DSP();
    dsp->start();
}
