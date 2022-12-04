#include <CPU.h>
#include <iostream>

namespace sn
{
    CPU::CPU(MainBus & mainbus):
    m_mainbus(mainbus)
    {};
    
    void CPU::RESET()
    {
        f_N = f_V = f_B = f_D = f_Z = f_C = false;
        f_I = true;
        r_A = r_X = r_Y = 0;
        r_SP = 0xfd; // 0xff 表示栈底
        // SP 是 回环的  0x00 - 0x01 = 0xff

        r_PC = readAddress(RESETVector);
        m_skipCycles = m_cycles = 0;
    }

    void CPU::step()
    {
        ++m_cycles;
        // 当 m_skipCycles 为0 时，执行指令
        // 要执行指令的话，就设置跳过cycles = 0，这样就防止了 0 -- 后 还是无法执行的问题
        if(m_skipCycles-- > 1) return;
        
        m_skipCycles = 0;

        // std::cout << std::hex << r_PC << ": ";
        Byte opcode = fetch();
        m_skipCycles += ops_cycles[opcode];
        decode(opcode);
        getOperand();
        // disassemble();
        getData();
        execute();
    }

    void CPU::interrupt(InterruptType type)
    {
        // 可屏蔽中断
        if(f_I && type != NMI && type != BRK_)
            return;
        
        // quirk
        if(type == BRK_)
        {
            // 跳过的可能是中断类型
            r_PC++;
        }

        push(r_PC >> 8);
        push(r_PC);
        // if BRK_，需要设置 f_B，一起压栈

        Byte flags = f_N << 7 |
                     f_V << 6 |
                       1 << 5 | //unused bit, supposed to be always 1
          (type == BRK_) << 4 | //B flag set if BRK
                     f_D << 3 |
                     f_I << 2 |
                     f_Z << 1 |
                     f_C;
        push(flags);

        f_I = true;

        switch(type)
        {
            // BRK_导致 IRQ
            // memory mapper 触发
            case IRQ:
            case BRK_:
                r_PC = readAddress(IRQVector);
                break;
            // PPU V-Blank 触发，可由PPU control reg 屏蔽
            case NMI:
                r_PC = readAddress(NMIVector);
                break;
        }

        m_skipCycles += 7;
    }
    // stack - 回环的
    void CPU::push(Byte value)
    {
        // 因为 实际的程序将 0xff 放到 SP，说明0xff是栈顶
        write(0x0100 | r_SP--, value);
    }

    Byte CPU::pop()
    {
        return read(0x0100 | ++r_SP);
    }

    Address CPU::readAddress(Address addr)
    {
        return read(addr) | read(addr + 1) << 8;
    }

    Byte CPU::read(Address addr)
    {   
        m_mainbus.read(addr);
    }

    void CPU::write(Address addr, Byte value)
    {
        m_mainbus.write(addr, value);
    }

    // fetch
    Byte CPU::fetch()
    {
        return read(r_PC++);
    }
    // decode
    void CPU::decode(Byte opcode)
    {
        m_op = ops[opcode];
        m_mode = ops_addrmode[opcode];
        m_opname = ops_name[m_op];
    }
    // operand
    void CPU::getOperand()
    {
        switch(m_mode)
        {
            case IMPL:
                break;
            case D:
            case D_Y:
            case D_X:
            case D_X_PRE:
            case D_Y_POST:
            case BRANCH:
            case IMME:
                m_operand = read(r_PC++);
                break;
            case A:
            case A_X:
            case A_Y:
            case A_INDIR:
                // 间接地址取地址都应该分开
                // r_PC: JMP ($ffff)
                // $ffff 取低4位，从 $ff00取高4位，取操作数时不回环

                // error 下面的错误，应该是取出操作数之后，再根据操作数来实现 bug
                // m_operandaddr = read(r_PC) | read((r_PC + 1 & 0x00ff) | (r_PC & 0xff00)) << 8; 
                // JMP - r_PC += 2; // 虽然是直接跳转，但是还是 +2，方便 反汇编
                m_operandaddr = readAddress(r_PC);
                r_PC += 2;
                break;
        }
    }

    void CPU::getData()
    {
        switch(m_mode)
        {
            case IMPL:
                break;
            case D:
                m_addr = m_operand;
                m_data = read(m_addr);
                break;
            case D_Y:
                m_addr = m_operand + r_Y;
                m_data = read(m_addr);
                break;
            case D_X:
                m_addr = m_operand + r_X;
                m_data = read(m_addr);
                break;
            // 间接X不考虑是否跨页
            case D_X_PRE:
                // 不能用 readm_address，因为 Byte + r_X + 1 必须是回环的
                m_addr = read((m_operand + r_X) & 0x00ff) | read((m_operand + r_X + 1) & 0x00ff) << 8;
                m_data = read(m_addr);
                break;

            case D_Y_POST:
                // operand + 1 也要 回环
                m_addr = read(m_operand) | read((m_operand + 1) & 0x00ff) << 8;
                switch(m_op)
                {
                    case STA:
                        break;
                    default:
                        setPageCrossing(m_addr, m_addr + r_Y);
                }
                m_addr += r_Y;
                m_data = read(m_addr);
                break;

            case BRANCH:
            case IMME:
                m_data = m_operand;
                break;

            case A:
                m_addr = m_operandaddr;
                switch(m_op)
                {
                    case JMP:
                    case JSR:
                    case STA:
                    case STX:
                    case STY:
                        break;
                    default:
                        // 取实际数据都应该在执行时进行，因为这样容易反汇编
                        m_data = read(m_addr);
                }
                break;
            case A_INDIR:
                // 间接地址取地址都应该分开                        + & |
                m_addr = read(m_operandaddr) | read( ((m_operandaddr + 1) & 0x00ff) | (m_operandaddr & 0xff00) ) << 8;
                // 只有JMP，JMP不需要看是否跨页

                // 全都放到 execute 进行
                // r_PC = m_addr;

                // 取数据 和 执行两个部分 是 有先后的，也可以合并在一起，分开 更容易反汇编
                break;

            case A_X:
                m_addr = m_operandaddr;
                switch(m_op)
                {
                    case ASL:
                    case DEC:
                    case INC:
                    case LSR:
                    case ROL:
                    case ROR:
                    case STA:
                        break;
                    default:
                        setPageCrossing(m_addr, m_addr + r_X);
                }
                m_addr += r_X;
                m_data = read(m_addr);
                break;
            case A_Y:
                m_addr = m_operandaddr;
                switch(m_op)
                {
                    case STA:
                        break;
                    default:
                        setPageCrossing(m_addr, m_addr + r_Y);
                }
                m_addr += r_Y;
                m_data = read(m_addr);
                break;
        }
    }


    void CPU::setZN(Byte data)
    {
        f_Z = !data;
        f_N = data & 0x80;
    }
    
    void CPU::setPageCrossing(Address addr1, Address addr2, size_t inc)
    {
        if((addr1 & 0xff00) != (addr2 & 0xff00))
        {
            m_skipCycles += inc;
        }
    }

    void CPU::branch()
    {
        // branch succeed +1
        // to new page    +2
        m_skipCycles++;
        
        // int_8 -> int16_t(前面补符号位) -> uint16_t，无符号加法
        Address new_PC = r_PC + (std::int8_t)m_data;
        setPageCrossing(r_PC, new_PC, 2);
        // 可前可后
        // r_PC += int(m_data); 有符号整数会被转化为无符号整数，出错
        
        // 实际上 一个字节 加上 负的就相当于减去，去除进位的话
        r_PC = new_PC;
    }

    // execute
    void CPU::execute()
    {
        switch(m_op)
        {
            case ADC:
            {
                std::uint16_t sum = r_A + m_data + f_C;
                //Carry forward or UNSIGNED overflow
                f_C = sum & 0x100;
                //SIGNED overflow, would only happen if the sign of sum is
                //different from BOTH the operands
                f_V = (r_A ^ sum) & (m_data ^ sum) & 0x80;
                r_A = static_cast<Byte>(sum);
                setZN(r_A);
            }
            break;
            case AND:
            {
                r_A &= m_data;
                setZN(r_A);
                break;
            }
            case EOR:
            {
                r_A ^= m_data;
                setZN(r_A);
                break;
            }
            case ORA:
            {
                r_A |= m_data;
                setZN(r_A);
                break;
            }
            case ROL:
            case ASL:
                switch(m_mode)
                {
                    case IMPL:
                    {
                        bool prev_C = f_C;
                        f_C = r_A & 0x80;
                        r_A = r_A << 1 | (prev_C && (m_op == ROL));
                        setZN(r_A);
                        break;
                    }
                    default:
                    {
                        bool prev_C = f_C;
                        f_C = m_data & 0x80;
                        m_data = m_data << 1 | (prev_C && (m_op == ROL));
                        write(m_addr, m_data);
                        setZN(m_data);
                        break;
                    }
                }
                break;
            case BCC:
                if(!f_C) branch(); break;
            case BCS:
                if(f_C) branch(); break;
            case BEQ:
                if(f_Z) branch(); break;
            case BNE:
                if(!f_Z) branch(); break;
            case BMI:
                if(f_N) branch(); break;
            case BPL:
                if(!f_N) branch(); break;
            case BVC:
                if(!f_V) branch(); break;
            case BVS:
                if(f_V) branch(); break;
            case CLC:
                f_C = false; break;
            case CLD:
                f_D = false; break;
            case CLI:
                f_I = false; break;
            case CLV:
                f_V = false; break;
            case SEC:
                f_C = true; break;
            case SED:
                f_D = true; break;
            case SEI:
                f_I = true; break;
            case BIT:
                f_Z = !(r_A & m_data);
                f_V = m_data & 0x40;
                f_N = m_data & 0x80;
                break;
            case BRK:
                interrupt(BRK_); break;
            case CMP:
            {
                // 无符号加减法 C 表示是否借位，设置了表示未借位
                // 减不过，相当于借位，所以一定会进位
                // 减过，不会进位，也就是不会借位
                std::uint16_t diff = r_A - m_data;
                f_C = !(diff & 0x100);
                setZN(diff);
            }
            break;
            case CPX:
            {
                std::uint16_t diff = r_X - m_data;
                f_C = !(diff & 0x100);
                setZN(diff);
            }
            break;
            case CPY:
            {
                std::uint16_t diff = r_Y - m_data;
                f_C = !(diff & 0x100);
                setZN(diff);
            }
            break;
            case DEC:
                write(m_addr, --m_data);
                setZN(m_data);
                break;
            case DEX:
                setZN(--r_X);
                break;
            case DEY:
                setZN(--r_Y);
                break;
            case INC:
                write(m_addr, ++m_data);
                setZN(m_data);
                break;
            case INX:
                setZN(++r_X); break;
            case INY:
                setZN(++r_Y); break;
            case JMP:
                // m_addr 是真实的地址，m_operaddr 是 操作数的绝对地址，用于反汇编
                r_PC = m_addr;
                break;
            case JSR:
                push((r_PC - 1) >> 8);
                push(r_PC - 1);

                r_PC = m_addr;
                break;
            case RTS:
                r_PC = pop();
                r_PC |= pop() << 8;
                r_PC++;
                break;
            case LDA:
                r_A = m_data;
                setZN(r_A);
                break;
            case LDX:
                r_X = m_data;
                setZN(r_X);
                break;
            case LDY:
                r_Y = m_data;
                setZN(r_Y);
                break;
            case LSR:
            case ROR:
                switch(m_mode)
                {
                    case IMPL:
                    {    
                        bool prev_C = f_C;
                        f_C = r_A & 1;
                        r_A = r_A >> 1 | (prev_C && (m_op == ROR)) << 7;
                        setZN(r_A);
                        break;
                    }
                    default:
                    {
                        bool prev_C = f_C;
                        f_C = m_data & 1;
                        m_data = m_data >> 1 | (prev_C && (m_op == ROR)) << 7;
                        write(m_addr, m_data);
                        setZN(m_data);
                    }
                }
                break;
            case NOP:
                break;
            case PHA:
                push(r_A); break;
            case PLA:
                r_A = pop(); setZN(r_A); break;
            case PHP:
                {
                    Byte flags = f_N << 7 |
                                 f_V << 6 |
                                   1 << 5 | //supposed to always be 1
                                   1 << 4 | //PHP pushes with the B flag as 1, no matter what
                                 f_D << 3 |
                                 f_I << 2 |
                                 f_Z << 1 |
                                 f_C;
                    push(flags);
                }
                break;
            
            case PLP:
                {
                    Byte flags = pop();
                    f_N = flags & 0x80;
                    f_V = flags & 0x40;
                    f_D = flags & 0x8;
                    f_I = flags & 0x4;
                    f_Z = flags & 0x2;
                    f_C = flags & 0x1;
                }
                break;
            case RTI:
                {
                    Byte flags = pop();
                    f_N = flags & 0x80;
                    f_V = flags & 0x40;
                    f_D = flags & 0x8;
                    f_I = flags & 0x4;
                    f_Z = flags & 0x2;
                    f_C = flags & 0x1;
                }
                r_PC = pop(); 
                r_PC |= pop() << 8;
                break;
            case SBC:
            {
                // f_C false 借  true 未借
                //High carry means "no borrow", thus negate and subtract
                std::uint16_t diff = r_A - m_data - !f_C;
                //if the ninth bit is 1, the resulting number is negative => borrow => low carry
                // 如果结果是负的，表示借位了，也就是第7位溢出了
                // 所以用16位是为了看溢出的那位判断正负
                f_C = !(diff & 0x100);
                //Same as ADC, except instead of the subtrahend,
                //substitute with it's one complement
                f_V = (r_A ^ diff) & (~m_data ^ diff) & 0x80;
                r_A = diff;
                setZN(diff);
            }
            break;
            case STA:
                write(m_addr, r_A); break;
            case STX:
                write(m_addr, r_X); break;
            case STY:
                write(m_addr, r_Y); break;
            case TAX:
                r_X = r_A; setZN(r_X); break;
            case TAY:
                r_Y = r_A; setZN(r_Y); break;
            case TSX:
                r_X = r_SP; setZN(r_X); break;
            case TXA:
                r_A = r_X; setZN(r_A); break;
            case TXS:
                r_SP = r_X; break;
            case TYA:
                r_A = r_Y; setZN(r_A); break;
        }
    }

    void CPU::disassemble()
    {   
        std::cout << m_opname << " ";

        switch(m_mode)
        {
            case IMPL:
                break;
            case D:
                std::cout << "$" << std::hex << int(m_operand) << " ";   
                break; 
            case D_Y:
                std::cout << "$" << std::hex << int(m_operand) << ", " << "Y";
                break;
            case D_X:
                std::cout << "$" << std::hex << int(m_operand) << ", " << "X";
                break;
            case D_X_PRE:
                std::cout << "(" << "$" << std::hex << int(m_operand) << ", X" << ")";
                break;
            case D_Y_POST:
                std::cout << "(" << "$" << std::hex << int(m_operand) << "), Y";
                break;
            case BRANCH:
                std::cout << "$" << std::hex << int(m_operand);
                break;
            case IMME:
                std::cout << "#$" << std::hex << int(m_operand);
                break;
            case A:
                std::cout << "$" << std::hex << m_operandaddr;
                break;
            case A_INDIR:
                std::cout << "($" << std::hex << m_operandaddr << ")";
                break;
            case A_X:
                std::cout << "$" << std::hex << m_operandaddr << ", X";
                break;
            case A_Y:
                std::cout << "$" << std::hex << m_operandaddr << ", Y";
                break;
        }
        std::cout << std::endl;
    }

    void CPU::debug()
    {
        std::cout << "r_X:  " << std::hex << int(r_X) << std::endl;
        std::cout << "r_Y:  " << std::hex << int(r_Y) << std::endl;
        std::cout << "r_A:  " << std::hex << int(r_A) << std::endl;
        std::cout << "r_PC: " << std::hex << int(r_PC) << std::endl;
        std::cout << "r_SP: " << std::hex << int(r_SP) << std::endl;
        std::cout << "r_P:  " << "N V B D I Z C" << std::endl;
        std::cout << "      " 
                              << (f_N?1:0) << " " << (f_V?1:0) << " "
                              << (f_B?1:0) << " " << (f_D?1:0) << " "
                              << (f_I?1:0) << " " << (f_Z?1:0) << " "
                              << (f_C?1:0) << std::endl;
    }

    void CPU::printStack(size_t size)
    {
        if(r_SP == 0xff) return;

        Address addr = 0x0100 | r_SP;

        addr++;

        for(int i = 0; i < size;)
        {
            std::cout << std::hex << int(addr) << "-" << int(addr+16 >= 0x1ff ? 0x1ff:addr+16) << ": ";
            
            for(int j = 0; addr != 0x200 && j < 16; i++, j++, addr++)
                std::cout << std::hex << int(read(addr)) << " ";
            
            std::cout << std::endl;

            if(addr == 0x200) break;
        }
    }
};