#include <PPU.h>
#include <PictureBus.h>
#include <VirtualScreen.h>
#include <PaletteColors.h>

namespace sn
{
    int i = 0;
    PPU::PPU(PictureBus & picturebus, VirtualScreen & virtual_screen):
    m_picture_bus(picturebus),
    m_virtual_screen(virtual_screen)
    {
        // 最多 64 个 Sprites，每个 4 bytes
        m_sprite_ram = new Byte[64 * 4];

        // 里面存放的都是颜色
        // 只需要存储一个界面，即一个 NameTable
        m_buff_bg = new sf::Color[256 * 240];

        m_buff_sprs = new sf::Color[8 * 8 * 64];
    }

    void PPU::setInterrupt(std::function<void(void)> cb)
    {
        m_vblank_callback = cb;
    }

    void PPU::RESET()
    {
        m_generateNMI = m_vblank = false;
        m_first_write = m_showBackground = m_evenFrame = true;
        m_vram_addr = m_temp_addr = m_cycles = m_spr_ram_addr = m_x_scroll_fine = m_scanline = 0;
        m_vram_inc = 1;
        m_ppu_status = PreRender;
    }

    // 一次只执行一个 case 中的 if else if 中的一个，一次就是一个周期
    // 除非要增加一行判断是否增加 scanline
    void PPU::step()
    {
    
        bool end = false;
        switch(m_ppu_status)
        {
            case PreRender:
                // m_cycles 是 行
                // m_scanline 是 列
                if(m_cycles == 1)
                {
                    m_vblank = false;
                }
                // at dot of 257
                else if(m_cycles == ScanLineVisibleCycles + 1 && m_showBackground)
                {
                    // 清除
                    m_vram_addr &= ~0x41f;
                    m_vram_addr |= m_temp_addr & 0x41f;
                }
                else if(m_cycles >= 280 && m_cycles <= 304 && m_showBackground)
                {
                    // 清除 
                    m_vram_addr &= ~0x7be0;
                    m_vram_addr |= m_temp_addr & 0x7be0;
                }
                else if(m_cycles == ScanLineEndCycles)
                {
                    m_ppu_status = Render;
                    // 当前是 cycles = 340 的 周期，不是 cycles = 0 的周期
                    // 如果这么写，结束之后会直接进入 cycles = 1 周期
                    // m_cycles = 0;
                    end = true;
                    m_scanline = 0;
                }
                break;
            
            case Render:
                // 绘制背景
                // 一个后期画一个点，其实私以为可以直接全部画好，然后等cycle

                if(m_cycles > 0 && m_cycles <= ScanLineVisibleCycles)
                {
                    // x 随着 m_cycles 更新
                    Byte x = m_cycles - 1;  // cycles   从 1 开始，为了索引，需要 -1
                    Byte y = m_scanline;    // scanline 从 0 开始

                    Byte bg_color = 0;

                    if(m_showBackground)
                    {
                        Byte x_fine = (m_x_scroll_fine + x) % 8;

                        if (!m_hideEdgeBackground || x >= 8)
                        {
                            // fetch tile index from nametable
                            // 索引直接就在 vram_adress 中   nametable_index Y X   X 满了后就进位到 Y
                            Address addr = 0x2000 | (m_vram_addr & 0x0fff);
                            Byte tile = m_picture_bus.read(addr);
                            
                            // fetch pattern
                            Byte y_fine = (m_vram_addr & 0x7000) >> 12; // 获取 y_fine
                            // 一个 tile 占用 16B，一个 y 为 8 pixel，占 1byte
                            // 在 pattern table 中的位置
                            // tile << 4，tile 所在的 16byte
                            // y_fine，该 pixel 在 tile 的 第 y_fine 行，
                            // 也就是 低 1bit 在 第 y_fine 个字节
                            //       高  1bit 在 第 y_fine + 8 个字节
                            // 再通过 x_fine 来确定在 tile 行的位置
                            addr = (tile << 4) + y_fine;
                            addr += m_bg_page_addr;
                            
                            // x_fine ^ 7，在第7位，右移一位，在第0位，右移7位，不用取反是因为高1位
                            bg_color = (m_picture_bus.read(addr) >> (x_fine ^ 7)) & 1;
                            bg_color |= ((m_picture_bus.read(addr + 8) >> (x_fine ^ 7)) & 1) << 1;

                            // fetch attribute
                            addr = 0x23c0 | (m_vram_addr & 0x0c00) | ((m_vram_addr >> 4) & 0x38) | ((m_vram_addr >> 2) & 0x07);
                            Byte attribute = m_picture_bus.read(addr);
                            int shift = ((m_vram_addr >> 4) & 4) | (m_vram_addr & 2);
                            bg_color |= ((attribute >> shift) & 0x3) << 2;
                        }

                        if(x_fine == 7)
                        {
                            if((m_vram_addr & 0x001f) == 31)
                            {
                                m_vram_addr &= ~0x001f;  // X_coarse = 0;
                                m_vram_addr ^= 0x0400;   // 切换 nametable
                            }
                            else
                            {
                                m_vram_addr++; // X_coarse ++;
                            }
                        }

                    }

                    m_buff_bg[y * 256 + x] = sf::Color(colors[m_picture_bus.read(0x3f00 + bg_color)]);

                }
                // 在 VisibleCycles 要干两件事
                // 1、画像素点
                // 2、更新 y
                if(m_cycles == ScanLineVisibleCycles && m_showBackground)
                {
                    if((m_vram_addr & 0x7000) != 0x7000)
                    {
                        m_vram_addr += 0x1000;
                    }
                    else
                    {
                        m_vram_addr &= ~0x7000;
                        int y = (m_vram_addr & 0x03e0) >> 5;
                        if(y == 29)
                        {
                            y = 0;
                            m_vram_addr ^= 0x0800;
                        }
                        else if(y == 31)
                        {
                            y = 0;
                        }
                        else
                        {
                            y += 1;
                        }
                        m_vram_addr = (m_vram_addr & ~0x03e0) | (y << 5);
                    }
                }
                else if(m_cycles == ScanLineVisibleCycles + 1 && m_showBackground)
                {
                    m_vram_addr &= ~0x41f;
                    m_vram_addr |= m_temp_addr & 0x41f;
                }
                else if(m_cycles == ScanLineEndCycles)
                {

                    m_scanline++;
                    end = true;
                    //m_cycles = 0;
                }

                // 目前为当前行，在 ++ 之后，必须比可见的大才能说明到达了下一行
                // cycle 和 scanline是不一样的
                // cycle 是 在最后一个周期设置好，下一个周期就可以跳转了
                // 而 scanline++ 后，到了下一行，但该行还没开始
                
                if(m_scanline == VisibleScanLine + 1)
                {
                    m_ppu_status = PostRender;
                }
                break;

            case PostRender:
                // 跳过一行，并进入下一行
                // 下一行才开启 v_blank
                // 到最后一个 cycle，将像素存到屏幕中
                if(m_cycles == ScanLineEndCycles)
                {
                    m_scanline++;
                    end = true;
                    // m_cycles = 0;

                    // 下一个 cycle 会 进入 VerticalBlank
                    m_ppu_status = VerticalBlank;

                    // 写到 虚拟屏幕
                    for(int y = 0; y < 240; y++)
                    {
                        for(int x = 0; x < 256; x++)
                        {
                            m_virtual_screen.setPixel(x, y, m_buff_bg[y * 256 + x]);
                        }
                    }

                    std::cout << "note: 第 " << i << " 帧" << std::endl;
                    i++;

                }
                break;
            
            case VerticalBlank:
                if(m_cycles == 1 && m_scanline == VisibleScanLine + 2)
                {
                    m_vblank = true;
                    if(m_generateNMI) m_vblank_callback();
                }
                
                else if(m_cycles == ScanLineEndCycles)
                {
                    // m_cycles = 0;
                    end = true;
                    m_scanline++;
                }
                
                if(m_scanline == FrameEndScanLine + 1)
                {
                    m_ppu_status = PreRender;
                    m_scanline = 0;
                    m_evenFrame = !m_evenFrame;
                }
                break;
        }


        if(end)
            m_cycles = 0;
        else
            m_cycles++;
    }

    // $2005
    void PPU::setScroll(Byte value)
    {
        if(m_first_write)
        {
            m_temp_addr &= 0x7fe0;              // 清除 低 5位
            m_temp_addr |= (value & 0xf8) >> 3; // temp_addr 0-4 bit = value 高 5位
            m_x_scroll_fine = value & 0x07;            // 获取低 3 位
            m_first_write = false;
        }
        else
        {
            m_temp_addr &= 0x0c1f;              // 清除 5-9  12-14       0b1000 1100 0001 1111
            m_temp_addr |= (value & 0xf8) << 2; // temp_addr  5- 9 bit = value 高 5位 - 补  2 个 0
                                                // temp_addr 10-11 bit 不应被影响
            m_temp_addr |= (value & 0x07) << 12;// temp_addr 12-14 bit = value 低3位  - 补 12 个 0

            m_first_write = true;
        }
    }

    // $2006
    void PPU::setVramAddr(Byte value)
    {
        std::cout << "set addr: " << int(value) << std::endl;

        if(m_first_write)
        {
            m_temp_addr &= 0x00ff;               // 清除 temp 影响的位
            m_temp_addr |= (value & 0x3f) << 8;
            m_first_write = false;
        }
        else
        {
            m_temp_addr &= 0xff00;                // 清除要设置的位
            m_temp_addr |= value;
            m_vram_addr = m_temp_addr;
            m_first_write = true;
        }
    }

    // $2007
    Byte PPU::getVRAM()
    {
        Byte ret = m_picture_bus.read(m_vram_addr);
        m_vram_addr += m_vram_inc;

        // Pattern Tables  NameTables Attributes 的第一次读的会放在 buf 里
        // Palettes 不受影响
        if(m_vram_addr < 0x3f00)
        {
            // 将结果放进 buf，返回 buf 中的值
            Byte tmp = ret;
            ret = m_ret_vram_buf;
            m_ret_vram_buf = tmp;
        }

        std::cout << "get data: " << std::hex << int(m_vram_addr) << ": " << int(ret) << std::endl;

        return ret;
    }

    // $2007
    void PPU::setVRAM(Byte value)
    {
        std::cout << "set data: " << int(value) << std::endl;

        m_picture_bus.write(m_vram_addr, value);
        m_vram_addr += m_vram_inc;
    }

    // set sprite ram address
    // 该地址是 1byte 
    // $ 2003
    void PPU::setSprRamAddr(Byte value)
    {
        m_spr_ram_addr = value;
    }

    // w sprite ram
    // 只能写 sprite ram
    // 只有寄存器操作才会自增
    // $2004
    void PPU::setSprRAM(Byte value)
    {
        m_sprite_ram[m_spr_ram_addr++] = value;
    }

    // $ 2000
    void PPU::setCtrl1(Byte value)
    {
        m_vram_inc = value & 0x04 ? 32 : 1;
        m_generateNMI = value & 0x80;
        m_spr_page_addr = value & 0x08 ? 0x1000 : 0x0000;
        m_bg_page_addr = value & 0x10 ? 0x1000 : 0x0000;

        // nametables 的 基地址
        // 对 0x2000 的 w 影响 temp 寄存器，存储 名字表索引
        
        // 清除名字表索引
        m_temp_addr &= 0xf3ff;
        m_temp_addr |= (value & 0x3) << 10;
    }

    // $ 2001
    void PPU::setCtrl2(Byte value)
    {
        m_showBackground = value & 0x08;
        m_hideEdgeBackground = !(value & 0x2);

    }
    
    // $2002
    Byte PPU::getStatus()
    {
        Byte status = m_vblank << 7;

        m_vblank = false;

        // 在 读 0x2002 时重置 m_first_write
        m_first_write = true;

        return status;
    }
};