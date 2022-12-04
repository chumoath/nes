#ifndef CARTIDGE_H
#define CARTIDGE_H

#include <iostream>

namespace sn
{
    using Byte = std::uint8_t;
    using Address = std::uint16_t;
    class NesHeader
    {
        public:
            Byte m_name[3];
            Byte m_id;
            Byte m_prnum;
            Byte m_crnum;
            Byte m_control1;
            Byte m_control2;
            Byte m_ramnum;
            Byte m_reserve[7];
    };

    class Cartidge
    {
        public:
            NesHeader nesheader;
            Byte * PRG_ROM;
            Byte * CHR_ROM;

            Cartidge();
            bool loadROM(std::string path);
            Byte readPgr(Address addr);
    };
}

#endif