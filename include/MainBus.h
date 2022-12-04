#ifndef MAINBUS_H
#define MAINBUS_H
#include <iostream>
#include <vector>
#include <PPU.h>
#include <functional>

namespace sn
{
    using Byte = std::uint8_t;
    using Address = std::uint16_t;

    class MainBus
    {
        public:
            enum Register
            {
                PPU_CTRL1,
                PPU_CTRL2,
                PPU_STATUS,
                SPR_RAM_ADDRESS,
                SPR_RAM_IO,
                SCROLL,
                VRAM_ADDRESS,
                VRAM_IO,
                DMA_ = 0x4014
            };
            
            MainBus(Byte * PRG_ROM, PPU & ppu);
            void DMA(Byte pgtr);
            
            std::function<void(void)> m_skipDMACallback;

            void setskipDMA(std::function<void(void)> cb);
            Byte read(Address addr);
            void write(Address addr, Byte value);
            

            Byte * m_RAM;
            Byte * m_PRG_ROM;
            PPU & m_ppu;
    };
};

#endif