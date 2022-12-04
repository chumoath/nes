#ifndef PPU_H
#define PPU_H

#include <iostream>
#include <PictureBus.h>
#include <functional>
#include <SFML/Graphics.hpp>
#include <VirtualScreen.h>

namespace sn
{
    using Byte = std::uint8_t;
    using Address = std::uint16_t;

    const int ScanLineEndCycles = 340;     // col
    const int ScanLineVisibleCycles = 256; // col
    const int VisibleScanLine = 239;       // row
    const int FrameEndScanLine = 260;      // row

    class PPU
    {
        public:
            PictureBus & m_picture_bus;
            VirtualScreen & m_virtual_screen;
            std::function<void(void)> m_vblank_callback;

            PPU(PictureBus & picturebus, VirtualScreen & virtual_screen);
            void RESET();
            void step();
            void generateInterrupt();
            void setInterrupt(std::function<void(void)>);

            Byte * m_sprite_ram;

            // Registers
            Byte m_spr_ram_addr;
            
            // r vram 时 暂存
            Byte m_ret_vram_buf;

            Address m_vram_addr;
            bool m_first_write;     // $2005 和 $2006 共用
                                    // 所以必须在设置完 vram数据后，才能设置 scroll
                                    // 否则 scroll 会被影响

            Address m_temp_addr;

            // buf
            sf::Color * m_buff_bg;
            sf::Color * m_buff_sprs;

            // ctrl1
            // r/w vram 后 + inc
            Address m_vram_inc;

            bool m_generateNMI;

            // ctrl2
            bool m_showBackground;
            bool m_hideEdgeBackground;
            // status
            bool m_vblank;


            // scroll
            Byte m_x_scroll_fine;



            int m_scanline;
            int m_cycles;
            bool m_evenFrame;

            enum RenderStatus
            {
                PreRender,
                Render,
                PostRender,
                VerticalBlank
            };

            RenderStatus m_ppu_status;

            Address m_spr_page_addr;
            Address m_bg_page_addr;


            // Registers operations
            Byte getVRAM();
            Byte getStatus();
            void setVRAM(Byte value);
            void setSprRAM(Byte value);
            void setCtrl1(Byte value);
            void setCtrl2(Byte value);
            void setVramAddr(Byte value);
            void setScroll(Byte value);
            void setSprRamAddr(Byte value);
    };
};
#endif