#ifndef OPCODES_H
#define OPCODES_H

namespace sn
{
    using Byte = std::uint8_t;
    using Address = std::uint16_t;

    const Address IRQVector = 0xfffe;
    const Address NMIVector = 0xfffa;
    const Address RESETVector = 0xfffc;

    enum InterruptType
    {
        BRK_,
        NMI,
        IRQ
    };

    enum AddressingMode
    {
        IMPL,    // 0
        D,       // 1
        D_Y,     // 1
        D_X,     // 1
        D_X_PRE, // 1
        D_Y_POST,// 1
        BRANCH,  // 1
        IMME,    // 1
        A,       // 2
        A_INDIR, // 2
        A_X,     // 2
        A_Y,     // 2
    };

    enum OPCODE
    {
        ARR, AXS, ANC, ASL, AND, ADC, ALR, AHX, 
        BCC, BMI, BRK, BEQ, BIT, BNE, BPL, BVS, BCS, BVC, 
        CPX, CPY, CLI, CLV, CLC, CLD, CMP, 
        DEC, DEX, DEY, DCP, 
        EOR,
        INY, INX, INC, ISC, 
        JSR, JMP, 
        LDX, LSR, LDA, LAX, LAS, LDY, 
        NOP, 
        ORA, 
        PHP, PLP, PLA, PHA, 
        ROL, RTI, ROR, RRA, RTS, RLA, 
        SAX, SBC, SED, STP, SLO, SRE, SEC, SEI, STY, STA, STX, SHX, SHY, 
        TAS, TXS, TSX, TAY, TYA, TAX, TXA, 
        XAA
    };

    static AddressingMode ops_addrmode[] = {
        IMPL, D_X_PRE, IMPL, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A,       A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_X, D_X, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_X, A_X,
        A,    D_X_PRE, IMPL, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A,       A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_X, D_X, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_X, A_X, 
        IMPL, D_X_PRE, IMPL, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A,       A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_X, D_X, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_X, A_X, 
        IMPL, D_X_PRE, IMPL, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A_INDIR, A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_X, D_X, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_X, A_X, 
        IMME, D_X_PRE, IMME, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A,       A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_Y, D_Y, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_Y, A_Y, 
        IMME, D_X_PRE, IMME, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A,       A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_Y, D_Y, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_Y, A_Y, 
        IMME, D_X_PRE, IMME, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A,       A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_X, D_X, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_X, A_X, 
        IMME, D_X_PRE, IMME, D_X_PRE, D, D, D, D, IMPL, IMME, IMPL, IMME, A,       A, A, A, BRANCH, D_Y_POST, IMPL, D_Y_POST, D_X, D_X, D_X, D_X, IMPL, A_Y, IMPL, A_Y, A_X, A_X, A_X, A_X, 
    };

    static OPCODE ops[] = {
        BRK,ORA,STP,SLO,NOP,ORA,ASL,SLO,PHP,ORA,ASL,ANC,NOP,ORA,ASL,SLO,BPL,ORA,STP,SLO,NOP,ORA,ASL,SLO,CLC,ORA,NOP,SLO,NOP,ORA,ASL,SLO,
        JSR,AND,STP,RLA,BIT,AND,ROL,RLA,PLP,AND,ROL,ANC,BIT,AND,ROL,RLA,BMI,AND,STP,RLA,NOP,AND,ROL,RLA,SEC,AND,NOP,RLA,NOP,AND,ROL,RLA,
        RTI,EOR,STP,SRE,NOP,EOR,LSR,SRE,PHA,EOR,LSR,ALR,JMP,EOR,LSR,SRE,BVC,EOR,STP,SRE,NOP,EOR,LSR,SRE,CLI,EOR,NOP,SRE,NOP,EOR,LSR,SRE,
        RTS,ADC,STP,RRA,NOP,ADC,ROR,RRA,PLA,ADC,ROR,ARR,JMP,ADC,ROR,RRA,BVS,ADC,STP,RRA,NOP,ADC,ROR,RRA,SEI,ADC,NOP,RRA,NOP,ADC,ROR,RRA,
        NOP,STA,NOP,SAX,STY,STA,STX,SAX,DEY,NOP,TXA,XAA,STY,STA,STX,SAX,BCC,STA,STP,AHX,STY,STA,STX,SAX,TYA,STA,TXS,TAS,SHY,STA,SHX,AHX,
        LDY,LDA,LDX,LAX,LDY,LDA,LDX,LAX,TAY,LDA,TAX,LAX,LDY,LDA,LDX,LAX,BCS,LDA,STP,LAX,LDY,LDA,LDX,LAX,CLV,LDA,TSX,LAS,LDY,LDA,LDX,LAX,
        CPY,CMP,NOP,DCP,CPY,CMP,DEC,DCP,INY,CMP,DEX,AXS,CPY,CMP,DEC,DCP,BNE,CMP,STP,DCP,NOP,CMP,DEC,DCP,CLD,CMP,NOP,DCP,NOP,CMP,DEC,DCP,
        CPX,SBC,NOP,ISC,CPX,SBC,INC,ISC,INX,SBC,NOP,SBC,CPX,SBC,INC,ISC,BEQ,SBC,STP,ISC,NOP,SBC,INC,ISC,SED,SBC,NOP,ISC,NOP,SBC,INC,ISC,
    };

    static std::string ops_name[] = {
        "ARR", "AXS", "ANC", "ASL", "AND", "ADC", "ALR", "AHX", 
        "BCC", "BMI", "BRK", "BEQ", "BIT", "BNE", "BPL", "BVS", "BCS", "BVC", 
        "CPX", "CPY", "CLI", "CLV", "CLC", "CLD", "CMP", 
        "DEC", "DEX", "DEY", "DCP", 
        "EOR",
        "INY", "INX", "INC", "ISC", 
        "JSR", "JMP", 
        "LDX", "LSR", "LDA", "LAX", "LAS", "LDY", 
        "NOP", 
        "ORA", 
        "PHP", "PLP", "PLA", "PHA", 
        "ROL", "RTI", "ROR", "RRA", "RTS", "RLA", 
        "SAX", "SBC", "SED", "STP", "SLO", "SRE", "SEC", "SEI", "STY", "STA", "STX", "SHX", "SHY", 
        "TAS", "TXS", "TSX", "TAY", "TYA", "TAX", "TXA", 
        "XAA"
    };
};
    static size_t ops_cycles[] = 
    {// 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
        7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,  //
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,  //
        6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,  //
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,  //
        6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,  //
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,  //
        6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,  //
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,  //
        0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,  //
        2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,  //
        2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,  //
        2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,  //
        2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,  //
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,  //
        2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,  //
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,  //
    };
#endif