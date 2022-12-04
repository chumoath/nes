#include <MainBus.h>
#include <iostream>
#include <PPU.h>
#include <string.h>

namespace sn
{
    MainBus::MainBus(Byte * PRG_ROM, PPU & ppu):
    m_PRG_ROM(PRG_ROM),  // 引用赋值引用必须这样初始化
    m_ppu(ppu)
    {
        m_RAM = new Byte[0x800];
        for(int i = 0; i < 0x800; i++)
            m_RAM[i] = 0;
    }

    Byte MainBus::read(Address addr)
    {
        if(addr < 0x2000)
            return m_RAM[addr & 0x7ff];

        else if(addr < 0x4000)
        {
            addr &= 7;
            switch(addr)
            {
                case PPU_STATUS:
                    return m_ppu.getStatus();
                    break;
                case VRAM_IO:
                    return m_ppu.getVRAM();
                    break;
            }
        }
        else if(addr < 0x4020); // 目前不用
        else if(addr >= 0x8000)
            // ROM 都是从 0 开始
            return m_PRG_ROM[addr - 0x8000];
    }
    
    void MainBus::write(Address addr, Byte value)
    {
        if(addr < 0x2000)
            m_RAM[addr & 0x7ff] = value;
        else if(addr < 0x4000)
        {
            addr &= 7;
            switch(addr)
            {
                case PPU_CTRL1:
                    m_ppu.setCtrl1(value);
                    break;
                case PPU_CTRL2:
                    m_ppu.setCtrl2(value);
                    break;
                case SPR_RAM_ADDRESS:
                    m_ppu.setSprRamAddr(value);
                    break;
                case SPR_RAM_IO:
                    m_ppu.setSprRAM(value);
                    break;
                case SCROLL:
                    m_ppu.setScroll(value);
                    break;
                case VRAM_ADDRESS:
                    m_ppu.setVramAddr(value);
                    break;
                case VRAM_IO:
                    m_ppu.setVRAM(value);
                    break;
            }
        }
        else if(addr < 0x4020)
        {
            switch(addr)
            {
                case DMA_:
                    DMA(value);
                    break;
            }
        }
        else if(addr >= 0x8000)
            // PRG_ROM 是一段自己的内存，一般不会写到ROM
            m_PRG_ROM[addr - 0x8000] = value;
    }

    void MainBus::DMA(Byte pgtr)
    {
        /*
        Address addr = pgtr << 8;
        const Byte * virtual_ram_addr;

        if(addr < 0x2000)
        {
            // addr 是 虚拟RAM的地址，不是实际的地址，所以不能直接 (Byte *)addr作为地址
            virtual_ram_addr = &m_RAM[addr & 0x07ff];
        }
        else
        {

        }
        
        // sprite ram 中放的是精灵的属性

        // DMA 放入 精灵 RAM 是从设置的 ram_Addr 开始 循环放的，所以要这样判断
        memcpy(m_ppu.m_sprite_ram + m_ppu.m_spr_ram_addr, virtual_ram_addr, 256 - m_ppu.m_spr_ram_addr);

        if(m_ppu.m_spr_ram_addr)
           memcpy(m_ppu.m_sprite_ram, virtual_ram_addr + (256 - m_ppu.m_spr_ram_addr), m_ppu.m_spr_ram_addr);
    */
        m_skipDMACallback();
    }

    void MainBus::setskipDMA(std::function<void(void)> cb)
    {
        m_skipDMACallback = cb;
    }

};
