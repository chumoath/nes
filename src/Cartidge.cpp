#include <Cartidge.h>
#include <iostream>
#include <fstream>
using namespace std;
namespace sn
{
    Cartidge::Cartidge()
    {
        PRG_ROM = nullptr;
        CHR_ROM = nullptr;
    }

    bool Cartidge::loadROM(string path)
    {
        ifstream ifs(path, ios::in | ios::binary);
        ifs.read((char *)&nesheader, sizeof(nesheader));
        cout << "name:     " << nesheader.m_name[0] << nesheader.m_name[1] << nesheader.m_name[2] << endl;
        cout << "id:       " << std::hex << int(nesheader.m_id) << endl;
        cout << "16k prg:  " << std::hex << int(nesheader.m_prnum) << endl;
        cout << "08k chr:  " << std::hex << int(nesheader.m_crnum) << endl;
        // rom ctrl 1
        cout << "ctrl 1:   " << std::hex << int(nesheader.m_control1) << endl;
        // rom ctrl 2
        cout << "ctrl 2:   " << std::hex << int(nesheader.m_control2) << endl;
        // 需要的 8k ram 数
        cout << "08k ram:  " << std::hex << int(nesheader.m_ramnum) << endl;

        PRG_ROM = new Byte[nesheader.m_prnum << 14];
        CHR_ROM = new Byte[nesheader.m_crnum << 13];

        ifs.read((char *)PRG_ROM, nesheader.m_prnum << 14);
        ifs.read((char *)CHR_ROM, nesheader.m_crnum << 13);

        for(int i = 0; i < 512; i++)
            std::cout << std::hex << int(CHR_ROM[i]) << std::endl;
    }
};