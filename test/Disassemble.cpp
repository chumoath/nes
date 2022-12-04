#include <CPU.h>
#include <iostream>
#include <iomanip>
#include <Cartidge.h>
#include <MainBus.h>
#include <PPU.h>

using namespace std;
int main(int argc, char* argv[])
{
    if(argv[2] == "debug")
    {
        // 手动输入汇编代码，放入内存中
        // 再从 0 开始读指令
        /*
        sn::Cartidge c;
        sn::MainBus m(0x800);
        sn::CPU cpu(m, c);
        cpu.RESET();
        cpu.r_PC = 0;

        sn::Byte buff[1024];
        int size = 0;
        int op_num = 0;

        while(1)
        {   
            cpu.r_PC = 0;
            int b;
            
            // ctrl + d 退出
            size = 0;

            // 读 size 个字节，以 0xfff结尾
            std::cout << "assemble: ";
            while((std::cin >> std::hex >> b) && (b != 0xfff))
            {
                buff[size++] = b;;
            }

            for(int i = 0 ; i < size; i++)
                m.write(cpu.r_PC++, buff[i]);

            cpu.r_PC = 0;
            std::cout << "op_num: ";
            std::cin >> std::dec >> op_num;
            for(int i = 0; i < op_num; i++)
            {
                cpu.step();
                cpu.disassemble();
                cpu.debug();
                cpu.printStack(32);
            }
            std::cout << "--------------------" << std::endl;
        
        }
        */
        // PHP 0x08
        // PLP 0x28
        // 0x38
        // 0xf8
        // 0x78
        // JMP 0x4c
        // jsr 0x20
        // rts 0x60
    }
    else  // disassemble file
    {
        // 用顺序执行的过程来反汇编
        sn::Cartidge cartidge;
        cartidge.loadROM(argv[1]);
        
        sn::PictureBus picturebus(cartidge.CHR_ROM);
        sn::PPU ppu(picturebus);
        
        sn::MainBus mainbus(cartidge.PRG_ROM, ppu);
        sn::CPU cpu(mainbus);

        // 不能直接将匿名对象作参数，因为匿名对象直接就析构了，无法引用

        cpu.RESET();
        cpu.m_disassemble = true;

        while(1)
        {
            int num;
            std::cin >> std::dec >> num;
            for(int i = 0; i < num; i++)
            {
                // 不能直接全部 反汇编，因为PGR本身也是有数据的，指令不是按照从前往后的顺序全都是指令
                // 除非明确知道哪些地方是 指令
                std::cout << setbase(16) << setw(4) << setfill('0');
                std::cout << cpu.r_PC << ": ";
                cpu.step();            
                cpu.disassemble();
                cpu.debug();
                cpu.printStack(16);
                // 不执行也不取实际数据
            }
        }

    }
}