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
        *hclk = 1;
        evaluateModel();
        *hclk = 0;
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
    *htrans = 2;
    *hwrite = 1;
    *hsize = 2;
    *hburst = 0;
    *hprot = 0;
    *error = 1;

    *haddr = addr;
    *hwdata = value;

    timeoutTick(hready, 0);

    *hsel = 1;

    tick(true);

    *hsel = 0;

    tick(true);
}

uint64_t AHB::read(int width, uint64_t addr)
{
    if(width != 4) {
        char msg[] = "AHB implementation only handles 4-byte accesses, tried %d"; // we sprintf to self, because width is never longer than 2 digits
        sprintf(msg, msg, width);
        throw msg;
    }
    *hsel = 1;
    *hwrite = 0;
    *haddr = addr;

    timeoutTick(hready, 0);

    *hsel = 1;

    tick(true);

    uint64_t result = *hrdata;

    *hsel = 0;

    tick(true);

    return result;
}

void AHB::reset()
{
    *hresetn = 0;
    tick(true);
    *hresetn = 1;
    tick(true);
}