#include <PictureBus.h>


namespace sn
{
    PictureBus::PictureBus(Byte * CHR_ROM)
    {
        m_Palettes = new Byte[0x20];
        m_VRAM = new Byte[0x800];
        m_PatternTables = CHR_ROM;
        for(int i = 0; i < 0x800; i++)
            m_VRAM[i] = 0;
        for(int i = 0; i < 0x20; i++)
            m_Palettes[i] = 0;
    }

    Byte PictureBus::read(Address addr)
    {
        // 0x4000 镜像
        addr &= 0x3fff;

        // 0x2000 -> 读 CHR_ROM
        if(addr < 0x2000)
            // 本身就是 从 0 开始
            return m_PatternTables[addr];
        // 0x3f00 -> 读 Name Tables // 垂直
        else if(addr < 0x3f00)
            return m_VRAM[(addr - 0x2000) & 0x7ff];
        // 0x3f00 - 0x4000 Palettes  每0x20镜像一次
        else
        {
            // 只有写需要镜像
            return m_Palettes[addr & 0x1f];
        }
    }
    
    void PictureBus::write(Address addr, Byte value)
    {
        addr &= 0x3fff;

        if(addr < 0x2000)
            std::cout << "write to Pattern Tables" << std::endl;

        else if(addr < 0x3f00)
            m_VRAM[(addr - 0x2000) & 0x7ff] = value;
        
        else
            {
                addr &= 0x001f;
                switch(addr)
                {
                    case 0x00:
                    case 0x04:
                    case 0x08:
                    case 0x0c:
                    case 0x10:
                    case 0x14:
                    case 0x18:
                    case 0x1c:
                        addr = 0x00;
                }
                m_Palettes[addr] = value;
            }
    }
}