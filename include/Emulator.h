#ifndef EMULATOR_H
#define EMULATOR_H

#include <CPU.h>
#include <PPU.h>
#include <VirtualScreen.h>
#include <Cartidge.h>
#include <SFML/Graphics.hpp>

namespace sn
{
    class Emulator
    {
        public:
            void run(std::string path);
    };

};
#endif