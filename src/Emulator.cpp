#include <Emulator.h>

namespace sn
{
    void Emulator::run(std::string path)
    {
        Cartidge cartidge;
        cartidge.loadROM(path);
        
        VirtualScreen screen;
        screen.create();

        PictureBus picturebus(cartidge.CHR_ROM);
        PPU ppu(picturebus, screen);

        MainBus mainbus(cartidge.PRG_ROM, ppu);

        CPU cpu(mainbus);

        sf::RenderWindow window;

        //               匿名函数
        ppu.setInterrupt([&](){ cpu.interrupt(NMI); });
        mainbus.setskipDMA([&](){cpu.m_skipCycles += 513; cpu.m_skipCycles += (cpu.m_cycles & 1); });

        cpu.RESET();
        ppu.RESET();

        window.create(sf::VideoMode(256 * 2, 240 * 2), "mario", sf::Style::Default);
        
        sf::Event event;

        while(window.isOpen())
        {
            while(window.pollEvent(event))
            {
                if(event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }

            // 画一帧，必须这样，否则每 4 个cycles 就会 draw，所以费时间
            for(int i = 0; i < 29781; ++i)
            {
                ppu.step();
                ppu.step();
                ppu.step();
                cpu.step();
            }

            window.draw(screen.m_vertrics);
            window.display();
        }
    }
};