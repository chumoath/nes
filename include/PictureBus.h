#ifndef PICTUREBUS_H
#define PICTUREBUS_H

#include <iostream>

namespace sn
{
    using Byte = std::uint8_t;
    using Address = std::uint16_t;

    class PictureBus
    {
        public:
            // CHR_ROM
            // image palette  sprites palette
            // 每个 16 B，一种颜色 1 B
            Byte * m_Palettes;
            
            // 1byte 是一个 tile 的索引，即是 pattern tables 的 索引
            Byte * m_VRAM;  // NameTables Attribute Tables
            
            // CHR_ROM
            // 一个 tile 为 16B，前 8 个byte 是 一个 tile 的 最低 1 bit
            //                  后 8 个 byte 是 一个 tile 的 高 1 bit
            // 最高的两位在 属性表中
            Byte * m_PatternTables;

            PictureBus(Byte * CHR_ROM);

            Byte read(Address addr);

            void write(Address addr, Byte value);
    };
};

#endif