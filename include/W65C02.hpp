// Jason Brillante "Damdoshi" / RED Spice
// WDC W65C02S digital, bus-cycle model. See docs/W65C02S.md.
#ifndef HBS_W65C02_HPP
#define HBS_W65C02_HPP
#include <cstdint>

namespace hbs
{
  class W65C02
  {
  public:
    enum Flag : uint8_t { C=1, Z=2, I=4, D=8, B=16, U=32, V=64, N=128 };
    struct Registers
    {
      uint16_t pc = 0;
      uint8_t s = 0, a = 0, x = 0, y = 0, p = U | I;
    };
    struct Bus
    {
      uint16_t address = 0;
      uint8_t data = 0;
      bool read = true, sync = false, vector = false, locked = false;
    };

    W65C02();
    const Registers &GetRegisters() const { return r; }
    const Bus &GetBus() const { return bus; }
    // Debugger entry point: restore registers at an instruction boundary.
    void SetRegisters(const Registers &registers);
    void Reset();
    // Call on every input transition, even when RDY is low.
    void SetInterrupts(bool irq_asserted, bool nmi_asserted);
    void SetOverflow() { if (!stopped) r.p |= V; }
    // Complete ONE PHI2 cycle (falling edge), then present the next bus cycle.
    void Clock(uint8_t data, bool ready = true);
    bool IsWaiting() const { return waiting; }
    bool IsStopped() const { return stopped; }
    bool IsResetting() const;

  private:
    enum Operation : uint8_t
    {
      NOP, ORA, AND, EOR, ADC, STA, LDA, CMP, SBC, ASL, ROL, LSR, ROR,
      STX, LDX, DEC, INC, STY, LDY, CPY, CPX, BIT, BITI, STZ, TSB, TRB,
      RMB, SMB, BBR, BBS, BPL, BMI, BVC, BVS, BCC, BCS, BNE, BEQ, BRA,
      BRK, JSR, RTI, RTS, JMP, PHP, PLP, PHA, PLA, PHY, PLY, PHX, PLX,
      CLC, SEC, CLI, SEI, CLV, CLD, SED, DEY, INY, DEX, INX,
      TXA, TYA, TAX, TAY, TXS, TSX, WAI, STP
    };
    enum Mode : uint8_t
    { IMP, ACC, IMM, ZP, ZPX, ZPY, ABS, ABX, ABY, INX_ZP, INY_ZP,
      IND_ZP, REL, ZPREL, IND_ABS, IND_ABX, NOP1, NOP8, NOP4 };
    struct Instruction { Operation op; Mode mode; };
    static const Instruction instructions[256];
    enum State : uint8_t
    {
      FETCH, IMPLIED, OPERAND, ZP_INDEX, POINTER_LOW, POINTER_HIGH,
      ABS_HIGH, INDEX_DUMMY, DATA, MODIFY, WRITE, DECIMAL,
      BRANCH, BRANCH_DUMMY, BRANCH_CROSS, BIT_BRANCH_READ, BIT_BRANCH_DUMMY,
      JMP_LOW, JMP_HIGH, JMP_LAST, JMP_INDEX,
      PUSH_DUMMY, PUSH, PULL_DUMMY, PULL_STACK, PULL,
      JSR_LOW, JSR_DUMMY, JSR_HIGH_PUSH, JSR_LOW_PUSH, JSR_HIGH,
      RTS_DUMMY, RTS_STACK, RTS_LOW, RTS_HIGH, RTS_LAST,
      RTI_DUMMY, RTI_STACK, RTI_STATUS, RTI_LOW, RTI_HIGH,
      BRK_PAD, INT_DUMMY, INT_HIGH, INT_LOW, INT_STATUS, VECTOR_LOW, VECTOR_HIGH,
      RESET_FIRST, RESET_SECOND, RESET_STACK1, RESET_STACK2, RESET_STACK3,
      RESET_LOW, RESET_HIGH, NOP_LAST, NOP_LONG, WAIT_SECOND, WAIT_LAST
    };
    Registers r;
    Bus bus;
    State state = FETCH;
    Instruction instruction = {NOP, IMP};
    uint8_t opcode = 0, low = 0, value = 0, zp = 0, count = 0;
    uint16_t effective = 0, base = 0, target = 0, vector = 0;
    bool irq = false, nmi = false, nmi_pending = false, irq_pending = false;
    bool waiting = false, stopped = false, software_interrupt = false;
    bool crossed = false, bit_branch_taken = false;

    void Read(uint16_t address, State next, bool locked = false);
    void Write(uint16_t address, uint8_t data, State next, bool locked = false);
    void Fetch();
    void Finish();
    void Decode(uint8_t data);
    void Access();
    void ReadOperation(uint8_t data);
    uint8_t ModifyOperation(uint8_t data);
    void ImpliedOperation();
    bool IsWrite() const;
    bool IsModify() const;
    uint8_t StoreValue() const;
    bool BranchTaken() const;
    void BeginBranch(uint8_t displacement);
    void BeginInterrupt(bool software);
    void SetNZ(uint8_t data);
    void FlagValue(uint8_t mask, bool set);
    void Compare(uint8_t reg, uint8_t data);
    void Add(uint8_t data);
    void Subtract(uint8_t data);
  };
}
#endif
