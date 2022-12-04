#include <VirtualScreen.h>
#include <SFML/Graphics.hpp>

namespace sn
{
    void VirtualScreen::create()
    {
        m_vertrics.resize(256 * 240 * 4);
        m_vertrics.setPrimitiveType(sf::Quads);

        for(int h = 0; h < 240; h++)
        {
            for(int w = 0; w < 256; w++)
            {
                std::size_t index = (h * 256 + w) * 4;

                m_vertrics[index].position = sf::Vector2f(w * 2., h * 2.);
                m_vertrics[index].color = sf::Color::White;

                m_vertrics[index + 1].position = sf::Vector2f(w * 2. + 2., h * 2.);
                m_vertrics[index + 1].color = sf::Color::White;                
                
                m_vertrics[index + 2].position = sf::Vector2f(w * 2. + 2., h * 2. + 2.);
                m_vertrics[index + 2].color = sf::Color::White;

                m_vertrics[index + 3].position = sf::Vector2f(w * 2., h * 2. + 2.);
                m_vertrics[index + 3].color = sf::Color::White;
            }
        }
    }

    void VirtualScreen::setPixel(int w, int h, sf::Color color)
    {
        std::size_t index = (h * 256 + w) * 4;

        m_vertrics[index].color = color;

        m_vertrics[index + 1].color = color;                
        
        m_vertrics[index + 2].color = color;

        m_vertrics[index + 3].color = color;
    }
};