// Jason Brillante "Damdoshi" / RED Spice
#include "W65C02.hpp"

using hbs::W65C02;

const W65C02::Instruction W65C02::instructions[256] = {
  {BRK,IMP}, {ORA,INX_ZP}, {NOP,IMM}, {NOP,NOP1}, {TSB,ZP}, {ORA,ZP}, {ASL,ZP}, {RMB,ZP},
  {PHP,IMP}, {ORA,IMM}, {ASL,ACC}, {NOP,NOP1}, {TSB,ABS}, {ORA,ABS}, {ASL,ABS}, {BBR,ZPREL},
  {BPL,REL}, {ORA,INY_ZP}, {ORA,IND_ZP}, {NOP,NOP1}, {TRB,ZP}, {ORA,ZPX}, {ASL,ZPX}, {RMB,ZP},
  {CLC,IMP}, {ORA,ABY}, {INC,ACC}, {NOP,NOP1}, {TRB,ABS}, {ORA,ABX}, {ASL,ABX}, {BBR,ZPREL},
  {JSR,ABS}, {AND,INX_ZP}, {NOP,IMM}, {NOP,NOP1}, {BIT,ZP}, {AND,ZP}, {ROL,ZP}, {RMB,ZP},
  {PLP,IMP}, {AND,IMM}, {ROL,ACC}, {NOP,NOP1}, {BIT,ABS}, {AND,ABS}, {ROL,ABS}, {BBR,ZPREL},
  {BMI,REL}, {AND,INY_ZP}, {AND,IND_ZP}, {NOP,NOP1}, {BIT,ZPX}, {AND,ZPX}, {ROL,ZPX}, {RMB,ZP},
  {SEC,IMP}, {AND,ABY}, {DEC,ACC}, {NOP,NOP1}, {BIT,ABX}, {AND,ABX}, {ROL,ABX}, {BBR,ZPREL},
  {RTI,IMP}, {EOR,INX_ZP}, {NOP,IMM}, {NOP,NOP1}, {NOP,ZP}, {EOR,ZP}, {LSR,ZP}, {RMB,ZP},
  {PHA,IMP}, {EOR,IMM}, {LSR,ACC}, {NOP,NOP1}, {JMP,ABS}, {EOR,ABS}, {LSR,ABS}, {BBR,ZPREL},
  {BVC,REL}, {EOR,INY_ZP}, {EOR,IND_ZP}, {NOP,NOP1}, {NOP,ZPX}, {EOR,ZPX}, {LSR,ZPX}, {RMB,ZP},
  {CLI,IMP}, {EOR,ABY}, {PHY,IMP}, {NOP,NOP1}, {NOP,NOP8}, {EOR,ABX}, {LSR,ABX}, {BBR,ZPREL},
  {RTS,IMP}, {ADC,INX_ZP}, {NOP,IMM}, {NOP,NOP1}, {STZ,ZP}, {ADC,ZP}, {ROR,ZP}, {RMB,ZP},
  {PLA,IMP}, {ADC,IMM}, {ROR,ACC}, {NOP,NOP1}, {JMP,IND_ABS}, {ADC,ABS}, {ROR,ABS}, {BBR,ZPREL},
  {BVS,REL}, {ADC,INY_ZP}, {ADC,IND_ZP}, {NOP,NOP1}, {STZ,ZPX}, {ADC,ZPX}, {ROR,ZPX}, {RMB,ZP},
  {SEI,IMP}, {ADC,ABY}, {PLY,IMP}, {NOP,NOP1}, {JMP,IND_ABX}, {ADC,ABX}, {ROR,ABX}, {BBR,ZPREL},
  {BRA,REL}, {STA,INX_ZP}, {NOP,IMM}, {NOP,NOP1}, {STY,ZP}, {STA,ZP}, {STX,ZP}, {SMB,ZP},
  {DEY,IMP}, {BITI,IMM}, {TXA,IMP}, {NOP,NOP1}, {STY,ABS}, {STA,ABS}, {STX,ABS}, {BBS,ZPREL},
  {BCC,REL}, {STA,INY_ZP}, {STA,IND_ZP}, {NOP,NOP1}, {STY,ZPX}, {STA,ZPX}, {STX,ZPY}, {SMB,ZP},
  {TYA,IMP}, {STA,ABY}, {TXS,IMP}, {NOP,NOP1}, {STZ,ABS}, {STA,ABX}, {STZ,ABX}, {BBS,ZPREL},
  {LDY,IMM}, {LDA,INX_ZP}, {LDX,IMM}, {NOP,NOP1}, {LDY,ZP}, {LDA,ZP}, {LDX,ZP}, {SMB,ZP},
  {TAY,IMP}, {LDA,IMM}, {TAX,IMP}, {NOP,NOP1}, {LDY,ABS}, {LDA,ABS}, {LDX,ABS}, {BBS,ZPREL},
  {BCS,REL}, {LDA,INY_ZP}, {LDA,IND_ZP}, {NOP,NOP1}, {LDY,ZPX}, {LDA,ZPX}, {LDX,ZPY}, {SMB,ZP},
  {CLV,IMP}, {LDA,ABY}, {TSX,IMP}, {NOP,NOP1}, {LDY,ABX}, {LDA,ABX}, {LDX,ABY}, {BBS,ZPREL},
  {CPY,IMM}, {CMP,INX_ZP}, {NOP,IMM}, {NOP,NOP1}, {CPY,ZP}, {CMP,ZP}, {DEC,ZP}, {SMB,ZP},
  {INY,IMP}, {CMP,IMM}, {DEX,IMP}, {WAI,IMP}, {CPY,ABS}, {CMP,ABS}, {DEC,ABS}, {BBS,ZPREL},
  {BNE,REL}, {CMP,INY_ZP}, {CMP,IND_ZP}, {NOP,NOP1}, {NOP,ZPX}, {CMP,ZPX}, {DEC,ZPX}, {SMB,ZP},
  {CLD,IMP}, {CMP,ABY}, {PHX,IMP}, {STP,IMP}, {NOP,NOP4}, {CMP,ABX}, {DEC,ABX}, {BBS,ZPREL},
  {CPX,IMM}, {SBC,INX_ZP}, {NOP,IMM}, {NOP,NOP1}, {CPX,ZP}, {SBC,ZP}, {INC,ZP}, {SMB,ZP},
  {INX,IMP}, {SBC,IMM}, {NOP,IMP}, {NOP,NOP1}, {CPX,ABS}, {SBC,ABS}, {INC,ABS}, {BBS,ZPREL},
  {BEQ,REL}, {SBC,INY_ZP}, {SBC,IND_ZP}, {NOP,NOP1}, {NOP,ZPX}, {SBC,ZPX}, {INC,ZPX}, {SMB,ZP},
  {SED,IMP}, {SBC,ABY}, {PLX,IMP}, {NOP,NOP1}, {NOP,NOP4}, {SBC,ABX}, {INC,ABX}, {BBS,ZPREL}
};

W65C02::W65C02() { Reset(); }

void W65C02::Read(uint16_t address, State next, bool locked)
{
  bus = {address, 0, true, false, false, locked};
  state = next;
}

void W65C02::Write(uint16_t address, uint8_t data, State next, bool locked)
{
  bus = {address, data, false, false, false, locked};
  state = next;
}

void W65C02::Fetch()
{
  Read(r.pc, FETCH);
  bus.sync = true;
}

void W65C02::SetRegisters(const Registers &registers)
{
  r = registers;
  r.p |= U;
  waiting = stopped = nmi_pending = irq_pending = false;
  irq = nmi = false;
  Fetch();
}

void W65C02::Reset()
{
  waiting = stopped = nmi_pending = irq_pending = false;
  r.p = (r.p | I | U) & ~D;
  Read(r.pc, RESET_FIRST);
  bus.sync = true; // first discarded opcode fetch of the reset sequence
}

bool W65C02::IsResetting() const
{ return state >= RESET_FIRST && state <= RESET_HIGH; }

void W65C02::SetInterrupts(bool irq_asserted, bool nmi_asserted)
{
  irq = irq_asserted;
  if (nmi_asserted && !nmi && !stopped && !IsResetting())
    nmi_pending = true;
  nmi = nmi_asserted;
}

void W65C02::Finish()
{
  irq_pending = irq && !(r.p & I);
  Fetch();
}

void W65C02::FlagValue(uint8_t mask, bool set)
{ r.p = set ? r.p | mask : r.p & ~mask; }

void W65C02::SetNZ(uint8_t data)
{
  FlagValue(Z, data == 0);
  FlagValue(N, (data & 0x80) != 0);
}

void W65C02::Compare(uint8_t reg, uint8_t data)
{
  FlagValue(C, reg >= data);
  SetNZ(uint8_t(reg - data));
}

void W65C02::Add(uint8_t data)
{
  unsigned carry = (r.p & C) != 0;
  unsigned sum = r.a + data + carry;
  if (r.p & D)
    {
      unsigned lo = (r.a & 15) + (data & 15) + carry;
      if (lo > 9) lo = ((lo + 6) & 15) + 16;
      unsigned hi = (r.a & 0xf0) + (data & 0xf0) + lo;
      FlagValue(V, (~(r.a ^ data) & (r.a ^ hi) & 0x80) != 0);
      if (hi > 0x9f) hi += 0x60;
      FlagValue(C, hi > 255);
      r.a = uint8_t(hi);
    }
  else
    {
      FlagValue(V, (~(r.a ^ data) & (r.a ^ sum) & 0x80) != 0);
      FlagValue(C, sum > 255);
      r.a = uint8_t(sum);
    }
  SetNZ(r.a);
}

void W65C02::Subtract(uint8_t data)
{
  int borrow = (r.p & C) ? 0 : 1;
  int difference = int(r.a) - data - borrow;
  FlagValue(V, ((r.a ^ data) & (r.a ^ difference) & 0x80) != 0);
  FlagValue(C, difference >= 0);
  if (r.p & D)
    {
      int lo = (r.a & 15) - (data & 15) - borrow;
      bool high_borrow = difference < 0;
      if (lo < 0) difference -= 6;
      if (high_borrow) difference -= 0x60;
    }
  r.a = uint8_t(difference);
  SetNZ(r.a);
}

bool W65C02::IsWrite() const
{ return instruction.op == STA || instruction.op == STX || instruction.op == STY || instruction.op == STZ; }

bool W65C02::IsModify() const
{
  switch (instruction.op)
    {
    case ASL: case ROL: case LSR: case ROR: case INC: case DEC:
    case TSB: case TRB: case RMB: case SMB: return true;
    default: return false;
    }
}

uint8_t W65C02::StoreValue() const
{
  switch (instruction.op)
    {
    case STA: case PHA: return r.a;
    case STX: case PHX: return r.x;
    case STY: case PHY: return r.y;
    case PHP: return r.p | B | U;
    default: return 0;
    }
}

void W65C02::ReadOperation(uint8_t data)
{
  switch (instruction.op)
    {
    case ORA: r.a |= data; SetNZ(r.a); break;
    case AND: r.a &= data; SetNZ(r.a); break;
    case EOR: r.a ^= data; SetNZ(r.a); break;
    case ADC: Add(data); break;
    case SBC: Subtract(data); break;
    case LDA: r.a = data; SetNZ(r.a); break;
    case LDX: r.x = data; SetNZ(r.x); break;
    case LDY: r.y = data; SetNZ(r.y); break;
    case CMP: Compare(r.a, data); break;
    case CPX: Compare(r.x, data); break;
    case CPY: Compare(r.y, data); break;
    case BIT:
      r.p = (r.p & ~(N | V)) | (data & (N | V));
      [[fallthrough]];
    case BITI: FlagValue(Z, (r.a & data) == 0); break;
    default: break;
    }
}

uint8_t W65C02::ModifyOperation(uint8_t data)
{
  bool carry = (r.p & C) != 0;
  switch (instruction.op)
    {
    case ASL: FlagValue(C, data & 0x80); data <<= 1; break;
    case ROL: FlagValue(C, data & 0x80); data = uint8_t((data << 1) | carry); break;
    case LSR: FlagValue(C, data & 1); data >>= 1; break;
    case ROR: FlagValue(C, data & 1); data = uint8_t((data >> 1) | (carry ? 0x80 : 0)); break;
    case INC: ++data; break;
    case DEC: --data; break;
    case TSB: FlagValue(Z, !(r.a & data)); return data | r.a;
    case TRB: FlagValue(Z, !(r.a & data)); return data & ~r.a;
    case RMB: return data & ~(1u << ((opcode >> 4) & 7));
    case SMB: return data | (1u << ((opcode >> 4) & 7));
    default: return data;
    }
  SetNZ(data);
  return data;
}

void W65C02::ImpliedOperation()
{
  switch (instruction.op)
    {
    case CLC: r.p &= ~C; break;
    case SEC: r.p |= C; break;
    case CLI: r.p &= ~I; break;
    case SEI: r.p |= I; break;
    case CLV: r.p &= ~V; break;
    case CLD: r.p &= ~D; break;
    case SED: r.p |= D; break;
    case DEY: --r.y; SetNZ(r.y); break;
    case INY: ++r.y; SetNZ(r.y); break;
    case DEX: --r.x; SetNZ(r.x); break;
    case INX: ++r.x; SetNZ(r.x); break;
    case TXA: r.a = r.x; SetNZ(r.a); break;
    case TYA: r.a = r.y; SetNZ(r.a); break;
    case TAX: r.x = r.a; SetNZ(r.x); break;
    case TAY: r.y = r.a; SetNZ(r.y); break;
    case TXS: r.s = r.x; break;
    case TSX: r.x = r.s; SetNZ(r.x); break;
    default: break;
    }
}

bool W65C02::BranchTaken() const
{
  switch (instruction.op)
    {
    case BPL: return !(r.p & N);
    case BMI: return r.p & N;
    case BVC: return !(r.p & V);
    case BVS: return r.p & V;
    case BCC: return !(r.p & C);
    case BCS: return r.p & C;
    case BNE: return !(r.p & Z);
    case BEQ: return r.p & Z;
    case BRA: return true;
    default: return bit_branch_taken;
    }
}

void W65C02::BeginBranch(uint8_t displacement)
{
  ++r.pc;
  target = uint16_t(r.pc + (displacement < 128 ? int(displacement) : int(displacement) - 256));
  crossed = (target & 0xff00) != (r.pc & 0xff00);
  if (BranchTaken()) Read(r.pc, BRANCH_DUMMY);
  else Finish();
}

void W65C02::Access()
{
  if (IsWrite()) Write(effective, StoreValue(), WRITE);
  else Read(effective, DATA, instruction.op == RMB || instruction.op == SMB);
}

void W65C02::BeginInterrupt(bool software)
{
  software_interrupt = software;
  vector = (!software && nmi_pending) ? 0xfffa : 0xfffe;
  if (vector == 0xfffa) nmi_pending = false;
  irq_pending = false;
  Read(r.pc, software ? BRK_PAD : INT_DUMMY);
}

void W65C02::Decode(uint8_t data)
{
  opcode = data;
  instruction = instructions[opcode];
  ++r.pc;
  switch (instruction.op)
    {
    case BRK: BeginInterrupt(true); return;
    case JSR: Read(r.pc, JSR_LOW); return;
    case RTS: Read(r.pc, RTS_DUMMY); return;
    case RTI: Read(r.pc, RTI_DUMMY); return;
    case PHP: case PHA: case PHX: case PHY: Read(r.pc, PUSH_DUMMY); return;
    case PLP: case PLA: case PLX: case PLY: Read(r.pc, PULL_DUMMY); return;
    case WAI: case STP: Read(r.pc, WAIT_SECOND); return;
    default: break;
    }
  switch (instruction.mode)
    {
    case NOP1: Finish(); break;
    case IMP: case ACC: Read(r.pc, IMPLIED); break;
    case REL: Read(r.pc, BRANCH); break;
    default: Read(r.pc, OPERAND); break;
    }
}

void W65C02::Clock(uint8_t data, bool ready)
{
  if (stopped) return;
  if (waiting)
    {
      if (!irq && !nmi_pending) return;
      waiting = false;
      irq_pending = irq && !(r.p & I);
      Fetch();
      return;
    }
  if (!ready) return;
  switch (state)
    {
    case FETCH:
      if (nmi_pending || irq_pending) BeginInterrupt(false);
      else Decode(data);
      break;
    case IMPLIED:
      // I is polled before CLI/SEI change it. Their effect on IRQ recognition
      // is delayed until the following instruction (likewise for PLP).
      irq_pending = irq && !(r.p & I);
      if (instruction.mode == ACC) r.a = ModifyOperation(r.a);
      else ImpliedOperation();
      Fetch();
      break;
    case OPERAND:
      ++r.pc;
      low = data;
      switch (instruction.mode)
        {
        case IMM:
          value = data;
          if ((instruction.op == ADC || instruction.op == SBC) && (r.p & D))
            Read(instruction.op == ADC ? 0x007f : 0x0000, DECIMAL);
          else { ReadOperation(data); Finish(); }
          break;
        case ZP: effective = data; Access(); break;
        case ZPX: case ZPY: case INX_ZP: zp = data; Read(uint16_t(r.pc - 1), ZP_INDEX); break;
        case IND_ZP: case INY_ZP: zp = data; Read(zp, POINTER_LOW); break;
        case ZPREL: effective = data; Read(effective, BIT_BRANCH_READ); break;
        default: Read(r.pc, ABS_HIGH); break;
        }
      break;
    case ZP_INDEX:
      if (instruction.mode == INX_ZP)
        { zp = uint8_t(zp + r.x); Read(zp, POINTER_LOW); }
      else { effective = uint8_t(zp + (instruction.mode == ZPY ? r.y : r.x)); Access(); }
      break;
    case POINTER_LOW: low = data; Read(uint8_t(zp + 1), POINTER_HIGH); break;
    case POINTER_HIGH:
      base = uint16_t((uint16_t(data) << 8) | low);
      effective = uint16_t(base + (instruction.mode == INY_ZP ? r.y : 0));
      if (instruction.mode == INY_ZP && (IsWrite() || (base & 0xff00) != (effective & 0xff00)))
        Read(IsWrite() ? uint8_t(zp + 1) : uint16_t(r.pc - 1), INDEX_DUMMY);
      else Access();
      break;
    case ABS_HIGH:
      ++r.pc;
      base = uint16_t((uint16_t(data) << 8) | low);
      effective = base;
      if (instruction.mode == NOP8)
        { count = 0; Read(uint16_t(0xff00 | low), NOP_LONG); }
      else if (instruction.mode == NOP4) Read(base, NOP_LAST);
      else if (instruction.mode == IND_ABS) Read(uint16_t(r.pc - 1), JMP_INDEX);
      else if (instruction.mode == IND_ABX)
        { effective = uint16_t(base + r.x); Read(uint16_t(r.pc - 1), JMP_INDEX); }
      else if (instruction.op == JMP) { r.pc = base; Finish(); }
      else
        {
          if (instruction.mode == ABX) effective = uint16_t(base + r.x);
          if (instruction.mode == ABY) effective = uint16_t(base + r.y);
          crossed = (base & 0xff00) != (effective & 0xff00);
          if ((instruction.mode == ABX || instruction.mode == ABY) &&
              (IsWrite() || crossed || instruction.op == INC || instruction.op == DEC))
            Read(crossed ? uint16_t(r.pc - 1) : effective, INDEX_DUMMY);
          else Access();
        }
      break;
    case INDEX_DUMMY: Access(); break;
    case DATA:
      value = data;
      if (IsModify()) Read(effective, MODIFY, true);
      else if ((instruction.op == ADC || instruction.op == SBC) && (r.p & D)) Read(effective, DECIMAL);
      else { ReadOperation(data); Finish(); }
      break;
    case DECIMAL: ReadOperation(value); Finish(); break;
    case MODIFY: Write(effective, ModifyOperation(value), WRITE, true); break;
    case WRITE: Finish(); break;
    case BRANCH: BeginBranch(data); break;
    case BRANCH_DUMMY:
      if (crossed)
        Read(r.pc, BRANCH_CROSS);
      else { r.pc = target; Finish(); }
      break;
    case BRANCH_CROSS: r.pc = target; Finish(); break;
    case BIT_BRANCH_READ:
      bit_branch_taken = ((data >> ((opcode >> 4) & 7)) & 1) == (instruction.op == BBS);
      Read(effective, BIT_BRANCH_DUMMY);
      break;
    case BIT_BRANCH_DUMMY: Read(r.pc, BRANCH); break;
    case JMP_INDEX: Read(effective, JMP_LOW); break;
    case JMP_LOW: low = data; Read(uint16_t(effective + 1), JMP_HIGH); break;
    case JMP_HIGH:
      target = uint16_t((uint16_t(data) << 8) | low);
      r.pc = target; Finish();
      break;
    case JMP_LAST: r.pc = target; Finish(); break;
    case PUSH_DUMMY: Write(uint16_t(0x100 | r.s), StoreValue(), PUSH); break;
    case PUSH: --r.s; Finish(); break;
    case PULL_DUMMY: Read(uint16_t(0x100 | r.s), PULL_STACK); break;
    case PULL_STACK: ++r.s; Read(uint16_t(0x100 | r.s), PULL); break;
    case PULL:
      irq_pending = irq && !(r.p & I);
      switch (instruction.op)
        {
        case PLA: r.a = data; SetNZ(r.a); break;
        case PLX: r.x = data; SetNZ(r.x); break;
        case PLY: r.y = data; SetNZ(r.y); break;
        case PLP: r.p = (data & ~B) | U; break;
        default: break;
        }
      Fetch();
      break;
    case JSR_LOW: low = data; ++r.pc; Read(uint16_t(0x100 | r.s), JSR_DUMMY); break;
    case JSR_DUMMY: Write(uint16_t(0x100 | r.s), uint8_t(r.pc >> 8), JSR_HIGH_PUSH); break;
    case JSR_HIGH_PUSH: --r.s; Write(uint16_t(0x100 | r.s), uint8_t(r.pc), JSR_LOW_PUSH); break;
    case JSR_LOW_PUSH: --r.s; Read(r.pc, JSR_HIGH); break;
    case JSR_HIGH: r.pc = uint16_t((uint16_t(data) << 8) | low); Finish(); break;
    case RTS_DUMMY: Read(uint16_t(0x100 | r.s), RTS_STACK); break;
    case RTS_STACK: ++r.s; Read(uint16_t(0x100 | r.s), RTS_LOW); break;
    case RTS_LOW: low = data; ++r.s; Read(uint16_t(0x100 | r.s), RTS_HIGH); break;
    case RTS_HIGH: r.pc = uint16_t((uint16_t(data) << 8) | low); Read(r.pc, RTS_LAST); break;
    case RTS_LAST: ++r.pc; Finish(); break;
    case RTI_DUMMY: Read(uint16_t(0x100 | r.s), RTI_STACK); break;
    case RTI_STACK: ++r.s; Read(uint16_t(0x100 | r.s), RTI_STATUS); break;
    case RTI_STATUS: r.p = (data & ~B) | U; ++r.s; Read(uint16_t(0x100 | r.s), RTI_LOW); break;
    case RTI_LOW: low = data; ++r.s; Read(uint16_t(0x100 | r.s), RTI_HIGH); break;
    case RTI_HIGH: r.pc = uint16_t((uint16_t(data) << 8) | low); Finish(); break;
    case BRK_PAD: ++r.pc; [[fallthrough]];
    case INT_DUMMY: Write(uint16_t(0x100 | r.s), uint8_t(r.pc >> 8), INT_HIGH); break;
    case INT_HIGH: --r.s; Write(uint16_t(0x100 | r.s), uint8_t(r.pc), INT_LOW); break;
    case INT_LOW:
      --r.s;
      Write(uint16_t(0x100 | r.s), uint8_t((r.p | U) & ~B) | (software_interrupt ? B : 0), INT_STATUS);
      break;
    case INT_STATUS:
      --r.s; r.p = (r.p | I) & ~D;
      Read(vector, VECTOR_LOW); bus.vector = true;
      break;
    case VECTOR_LOW: low = data; Read(uint16_t(vector + 1), VECTOR_HIGH); bus.vector = true; break;
    case VECTOR_HIGH: r.pc = uint16_t((uint16_t(data) << 8) | low); Finish(); break;
    case RESET_FIRST: Read(r.pc, RESET_SECOND); break;
    case RESET_SECOND: Read(uint16_t(0x100 | r.s), RESET_STACK1); break;
    case RESET_STACK1: --r.s; Read(uint16_t(0x100 | r.s), RESET_STACK2); break;
    case RESET_STACK2: --r.s; Read(uint16_t(0x100 | r.s), RESET_STACK3); break;
    case RESET_STACK3: --r.s; Read(0xfffc, RESET_LOW); bus.vector = true; break;
    case RESET_LOW: low = data; Read(0xfffd, RESET_HIGH); bus.vector = true; break;
    case RESET_HIGH: r.pc = uint16_t((uint16_t(data) << 8) | low); Finish(); break;
    case NOP_LAST: Finish(); break;
    case NOP_LONG:
      if (++count == 5) Finish();
      else Read(0xffff, NOP_LONG);
      break;
    case WAIT_SECOND: Read(r.pc, WAIT_LAST); break;
    case WAIT_LAST:
      waiting = instruction.op == WAI;
      stopped = instruction.op == STP;
      break;
    }
}
