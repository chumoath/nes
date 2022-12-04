#ifndef CPU_H
#define CPU_H

#include <MainBus.h>
#include <Opcodes.h>
#include <PPU.h>

namespace sn
{
    class CPU
    {
        public:
            // step
            void step();
            CPU(MainBus & mainbus);
            void RESET();
        //private:
            void pushf();
            void popf();
            // interrput
            void interrupt(InterruptType type);

            // stack
            void push(Byte value);
            Byte pop();

            // r/w
            Byte read(Address addr);
            void write(Address addr, Byte value);
            Address readAddress(Address addr);


            // fetch
            Byte fetch();
            // decode
            void decode(Byte opcode);
            // operand
            void getOperand();
            void getData();
            // execute
            void execute();
            void branch();
            void setZN(Byte data);
            void setPageCrossing(Address addr1, Address addr2, size_t inc = 1);

            // debug
            void disassemble();
            void debug();
            void printStack(size_t size);
            Byte r_A;
            Byte r_X;
            Byte r_Y;
            Address r_PC;
            Byte r_SP;

            // sub false 表示借位
            //     true  表示未借位
            bool f_C;
            bool f_Z;
            bool f_I;
            bool f_D;
            bool f_B;
            bool f_V;
            bool f_N;

            std::string m_opname;
            OPCODE m_op;
            AddressingMode m_mode;
            
            // 操作数
            Byte m_operand;
            Address m_operandaddr;

            // 实际数据
            Byte m_data;
            // 实际数据的地址
            Address m_addr;

            size_t m_skipCycles;
            size_t m_cycles;

            MainBus & m_mainbus;
    };
};

#endif