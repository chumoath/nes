#ifndef VIRTUALSCREEN_H
#define VIRTUALSCREEN_H
#include <SFML/Graphics.hpp>

namespace sn
{
    class VirtualScreen
    {
        public:
            sf::VertexArray m_vertrics;
            
            void create();
            void setPixel(int x, int y, sf::Color color);
    };
};

#endif