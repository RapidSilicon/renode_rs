//
// Copyright (c) 2010-2022 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
#include "ahb.h"
#include <cmath>

BaseAHB::BaseAHB(uint32_t dataWidth, uint32_t addrWidth)
{
    if(dataWidth != 32)
        throw "Unsupported AHB data width";

    this->dataWidth = dataWidth;

    if(addrWidth != 32)
        throw "Unsupported AHB address width";

    this->addrWidth = addrWidth;
}

AHB::AHB(uint32_t dataWidth, uint32_t addrWidth) : BaseAHB(dataWidth, addrWidth)
{
}

void AHB::tick(bool countEnable, uint64_t steps = 1)
{
    for(uint64_t i = 0; i < steps; i++) {
        *clk = 1;
        evaluateModel();
        *clk = 0;
        evaluateModel();
    }

    if(countEnable) {
        tickCounter += steps;
    }
}

void AHB::timeoutTick(uint8_t* signal, uint8_t expectedValue, int timeout = DEFAULT_TIMEOUT)
{
    do
    {
        tick(true);
        timeout--;
    }
    while((*signal != expectedValue) && timeout > 0);

    if(timeout == 0) {
        throw "Operation timeout";
    }
}

void AHB::write(int width, uint64_t addr, uint64_t value)
{
    if(width != 4) {
        char msg[] = "AHB implementation only handles 4-byte accesses, tried %d"; // we sprintf to self, because width is never longer than 2 digits
        sprintf(msg, msg, width);
        throw msg;
    }
    *ahb_htrans = 2;
    *ahb_hsel = 0;
    *ahb_hwrite = 0;
    *ahb_hsize = 0;
    *ahb_hburst = 0;
    *ahb_hprot = 1;
    *ahb_hmastlock = 0;
    *ahb_hnonsec = 0;
    *ahb_hreadyin = 0;

    tick(true);

    *ahb_haddr = addr;
    *ahb_hwdata = value;

    *ahb_hwrite = 1;
    *ahb_hsize = 2;

    timeoutTick(ahb_hreadyout, 1);

    *ahb_hsel = 1;

    tick(true);

    *ahb_hsel = 0;
    *ahb_hwrite = 0;
    *ahb_htrans = 0;
    *ahb_hsize = 0;

    tick(true);
}

uint64_t AHB::read(int width, uint64_t addr)
{
    if(width != 4) {
        char msg[] = "AHB implementation only handles 4-byte accesses, tried %d"; // we sprintf to self, because width is never longer than 2 digits
        sprintf(msg, msg, width);
        throw msg;
    }
 
    *ahb_htrans = 2;
    *ahb_hsel = 0;
    *ahb_hwrite = 0;
    *ahb_hsize = 0;
    *ahb_hburst = 0;
    *ahb_hprot = 1;
    *ahb_hmastlock = 0;
    *ahb_hnonsec = 0;
    *ahb_hreadyin = 0;
    *ahb_haddr = addr;

    tick(true);

    *ahb_hsize = 2;

    timeoutTick(ahb_hreadyout, 1);

    *ahb_hsel = 1;

    timeoutTick(ahb_hreadyout, 0);

    uint64_t result = *ahb_hrdata;
    *ahb_hsel = 0;

    tick(true);

    *ahb_hsel = 0;
    *ahb_hsize = 0;
    *ahb_htrans = 0;

    tick(true);

    return result;
}

void AHB::reset()
{
    *rst_l = 1;
    tick(true);
    *rst_l = 0;
    tick(true);
}