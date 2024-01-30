//
// Copyright (c) 2010-2022 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
#ifndef AHB_H
#define AHB_H
#include "bus.h"
#include <src/renode_bus.h>

struct BaseAHB
{
    BaseAHB(uint32_t dataWidth, uint32_t addrWidth);

    uint32_t dataWidth;
    uint32_t addrWidth;

    uint8_t *hclk;
    uint8_t *hresetn;
    uint8_t *hsel;          // IN
    uint32_t *haddr;        // IN
    uint8_t *htrans;        // IN
    uint8_t *hwrite;        // IN
    uint8_t *hsize;         // IN
    uint8_t *hburst;        // IN
    uint8_t *hprot;         // IN
    uint32_t *hwdata;       // IN
    uint8_t *error;         // IN
    //
    uint32_t *hrdata;       // OUT
    uint8_t *hready;        // OUT
    uint8_t *hresp;         // OUT

};

struct AHB : public BaseAHB, public BaseTargetBus
{
    AHB(uint32_t dataWidth, uint32_t addrWidth);
    virtual void tick(bool countEnable, uint64_t steps);
    virtual void write(int width, uint64_t addr, uint64_t value);
    virtual uint64_t read(int width, uint64_t addr);
    virtual void reset();
    void timeoutTick(uint8_t* signal, uint8_t expectedValue, int timeout);
};
#endif
