// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <hw/sdmmc.h>
#include <hwreg/bitfields.h>

namespace sdmmc {

class MsdcCfg : public hwreg::RegisterBase<MsdcCfg, uint32_t> {
public:
    static constexpr uint32_t kCardCkModeNoDiv = 0;
    static constexpr uint32_t kCardCkModeDiv   = 1;
    static constexpr uint32_t kCardCkModeDdr   = 2;
    static constexpr uint32_t kCardCkModeHs400 = 3;

    static auto Get() { return hwreg::RegisterAddr<MsdcCfg>(0x00); }

    DEF_FIELD(21, 20, card_ck_mode);
    DEF_FIELD(19, 8, card_ck_div);
    DEF_BIT(7, card_ck_stable);
    DEF_BIT(3, pio_mode);
};

class MsdcInt : public hwreg::RegisterBase<MsdcInt, uint32_t> {
public:
    static constexpr uint32_t kAllInterruptBits = 0xffffffff;

    static auto Get() { return hwreg::RegisterAddr<MsdcInt>(0x0c); }

    DEF_BIT(15, data_crc_err);
    DEF_BIT(14, data_timeout);
    DEF_BIT(12, transfer_complete);
    DEF_BIT(10, cmd_crc_err);
    DEF_BIT(9, cmd_timeout);
    DEF_BIT(8, cmd_ready);
};

class MsdcFifoCs : public hwreg::RegisterBase<MsdcFifoCs, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<MsdcFifoCs>(0x14); }

    DEF_BIT(31, fifo_clear);
    DEF_FIELD(23, 16, tx_fifo_count);
    DEF_FIELD(7, 0, rx_fifo_count);
};

class MsdcRxData : public hwreg::RegisterBase<MsdcRxData, uint8_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<MsdcRxData>(0x1c); }

    DEF_FIELD(7, 0, data);
};

class SdcCfg : public hwreg::RegisterBase<SdcCfg, uint32_t> {
public:
    static constexpr uint32_t kReadTimeoutMax = 0xff;
    static constexpr uint32_t kWriteTimeoutMax = 0x1fff;

    static constexpr uint32_t kBusWidth1 = 0;
    static constexpr uint32_t kBusWidth4 = 1;
    static constexpr uint32_t kBusWidth8 = 2;

    static auto Get() { return hwreg::RegisterAddr<SdcCfg>(0x30); }

    DEF_FIELD(31, 24, read_timeout);
    DEF_FIELD(17, 16, bus_width);
};

class SdcCmd : public hwreg::RegisterBase<SdcCmd, uint32_t> {
public:
    static constexpr uint32_t kAutoCmd12 = 1;

    static constexpr uint32_t kBlockTypeSingle = 1;
    static constexpr uint32_t kBlockTypeMulti  = 2;

    static constexpr uint32_t kRespTypeR1   = 1;
    static constexpr uint32_t kRespTypeR2   = 2;
    static constexpr uint32_t kRespTypeR3   = 3;
    static constexpr uint32_t kRespTypeR4   = 4;
    static constexpr uint32_t kRespTypeR1b  = 7;

    static auto Get() { return hwreg::RegisterAddr<SdcCmd>(0x34); }

    static SdcCmd FromRequest(const sdmmc_req_t* req) {
        SdcCmd cmd = Get().FromValue(0);

        cmd.set_cmd(req->cmd_idx);
        uint32_t resp_type = req->cmd_flags & (SDMMC_RESP_R1 |
                                               SDMMC_RESP_R2 |
                                               SDMMC_RESP_R3 |
                                               SDMMC_RESP_R1b);
        if (resp_type == SDMMC_RESP_R1) {
            cmd.set_resp_type(kRespTypeR1);
        } else if (resp_type == SDMMC_RESP_R2) {
            cmd.set_resp_type(kRespTypeR2);
        } else if (resp_type == SDMMC_RESP_R3) {
            cmd.set_resp_type(kRespTypeR3);
        } else if (resp_type == SDMMC_RESP_R1b) {
            cmd.set_resp_type(kRespTypeR1b);
        }

        cmd.set_block_size(req->blocksize);

        if (req->cmd_flags & SDMMC_RESP_DATA_PRESENT) {
            if (req->cmd_flags & SDMMC_CMD_MULTI_BLK) {
                cmd.set_block_type(kBlockTypeMulti);
            } else {
                cmd.set_block_type(kBlockTypeSingle);
            }

            if (!(req->cmd_flags & SDMMC_CMD_READ)) {
                cmd.set_write(1);
            }

            if (req->blockcount > 1) {
                cmd.set_auto_cmd(kAutoCmd12);
            }
        }

        return cmd;
    }

    DEF_FIELD(29, 28, auto_cmd);
    DEF_FIELD(27, 16, block_size);
    DEF_BIT(13, write);
    DEF_FIELD(12, 11, block_type);
    DEF_FIELD(9, 7, resp_type);
    DEF_FIELD(5, 0, cmd);
};

class SdcArg : public hwreg::RegisterBase<SdcArg, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<SdcArg>(0x38); }
};

class SdcStatus : public hwreg::RegisterBase<SdcStatus, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<SdcStatus>(0x3c); }

    uint32_t busy() {
        return cmd_busy() || sdc_busy();
    }

    DEF_BIT(1, cmd_busy);
    DEF_BIT(0, sdc_busy);
};

class SdcResponse : public hwreg::RegisterBase<SdcResponse, uint32_t> {
public:
    static auto Get(int index) { return hwreg::RegisterAddr<SdcResponse>(0x40 + (index << 2)); }

    DEF_FIELD(31, 0, response);
};

class SdcBlockNum : public hwreg::RegisterBase<SdcBlockNum, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<SdcStatus>(0x50); }
};

class DmaStartAddrHigh4Bits : public hwreg::RegisterBase<DmaStartAddrHigh4Bits, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<DmaStartAddrHigh4Bits>(0x8c); }

    DEF_FIELD(3, 0, address);

    DmaStartAddrHigh4Bits& set(uint64_t addr) {
        set_address((addr & kAddressMask) >> 32);
        return *this;
    }

private:
    static constexpr uint64_t kAddressMask = 0xf00000000;
};

class DmaStartAddr : public hwreg::RegisterBase<DmaStartAddr, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<DmaStartAddr>(0x90); }

    DEF_FIELD(31, 0, address);

    DmaStartAddr& set(uint64_t addr) {
        set_address(addr & kAddressMask);
        return *this;
    }

private:
    static constexpr uint64_t kAddressMask = 0xffffffff;
};

class DmaCtrl : public hwreg::RegisterBase<DmaCtrl, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<DmaCtrl>(0x98); }

    DEF_BIT(10, last_buffer);
    DEF_BIT(1, dma_stop);
    DEF_BIT(0, dma_start);
};

class DmaCfg : public hwreg::RegisterBase<DmaCfg, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<DmaCfg>(0x9c); }

    DEF_BIT(0, dma_active);
};

class DmaLength : public hwreg::RegisterBase<DmaLength, uint32_t> {
public:
    static auto Get() { return hwreg::RegisterAddr<DmaLength>(0xa8); }
};

}  // namespace sdmmc
