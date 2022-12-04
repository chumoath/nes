#include <Emulator.h>

int main(int argc, char * argv[])
{   
    sn::Emulator emulator;
    emulator.run(argv[1]);
}