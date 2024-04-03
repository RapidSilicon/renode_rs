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

    uint8_t *clk;
    uint8_t *rst_l;
    //
    uint16_t *ahb_haddr;        // IN
    uint8_t *ahb_hburst;        // IN
    uint8_t *ahb_hmastlock;     // IN
    uint8_t *ahb_hprot;         // IN
    uint8_t *ahb_hsize;         // IN
    uint8_t *ahb_htrans;        // IN
    uint8_t *ahb_hwrite;        // IN
    uint32_t *ahb_hwdata;       // IN
    uint8_t *ahb_hsel;          // IN
    uint8_t *ahb_hreadyin;      // IN
    uint8_t *ahb_hnonsec;       // IN
    //
    uint32_t *ahb_hrdata;       // OUT
    uint8_t *ahb_hreadyout;     // OUT
    uint8_t *ahb_hresp;         // OUT
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
