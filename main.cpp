// sudo apt-get install libsfml-dev => 安装 sfml
#include <Emulator.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{   
    if(argc != 2)
    {
        printf("Usage: %s <*.nes>\n", argv[0]);
        exit(1);
    }
    sn::Emulator emulator;
    emulator.run(argv[1]);
}