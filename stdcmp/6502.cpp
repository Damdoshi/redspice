// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                "6502.hpp"

static const uint8_t    FLAG_C = 0x01;
static const uint8_t    FLAG_Z = 0x02;
static const uint8_t    FLAG_I = 0x04;
static const uint8_t    FLAG_D = 0x08;
static const uint8_t    FLAG_B = 0x10;
static const uint8_t    FLAG_U = 0x20;
static const uint8_t    FLAG_V = 0x40;
static const uint8_t    FLAG_N = 0x80;

const std::string       &hbs::CMP_6502::GetType(void) const
{
  return (type = "6502");
}

static size_t           data_pin_for_bit(size_t bit)
{
  return (33 - bit); // D0=33 ... D7=26 on a NMOS 6502 DIP-40
}

static int              address_bit_for_pin(size_t pin)
{
  if (pin >= 9 && pin <= 20)
    return ((int)pin - 9);
  if (pin >= 22 && pin <= 25)
    return ((int)pin - 10);
  return (-1);
}

bool                    hbs::CMP_6502::IsAddressPin(size_t pin)
{
  return (address_bit_for_pin(pin) >= 0);
}

bool                    hbs::CMP_6502::IsDataPin(size_t pin)
{
  return (pin >= 26 && pin <= 33);
}

bool                    hbs::CMP_6502::DrivesPin(size_t n) const
{
  if (IsAddressPin(n))
    return (true);
  if (IsDataPin(n))
    return (true);
  if (n == 3 || n == 7 || n == 34 || n == 39)
    return (true);
  return (false);
}

void                    hbs::CMP_6502::ResetCore(void)
{
  address = 0xFFFC;
  pc = 0;
  effective = 0;
  return_address = 0;
  scratch = 0;
  a = 0;
  x = 0;
  y = 0;
  sp = 0xFD;
  status = FLAG_U | FLAG_I;
  opcode = 0;
  data_out = 0;
  interrupt_status = 0;
  interrupt_vector = 0xFFFE;
  rw = true;
  sync = false;
  drive_data = false;
  phase = RESET_VECTOR_LOW;
  pending = OP_NONE;
  index_register = IX_NONE;
  branch = BR_NONE;
  indirect_add_y = false;
  indexed_indirect = false;
}

void                    hbs::CMP_6502::SetFlag(uint8_t mask, bool value)
{
  if (value)
    status |= mask;
  else
    status &= ~mask;
  status |= FLAG_U;
}

bool                    hbs::CMP_6502::GetFlag(uint8_t mask) const
{
  return ((status & mask) != 0);
}

void                    hbs::CMP_6502::SetNZ(uint8_t value)
{
  SetFlag(FLAG_Z, value == 0);
  SetFlag(FLAG_N, (value & 0x80) != 0);
}

void                    hbs::CMP_6502::AddWithCarry(uint8_t value)
{
  uint16_t              sum = (uint16_t)a + value + (GetFlag(FLAG_C) ? 1 : 0);
  uint8_t               result = (uint8_t)sum;

  SetFlag(FLAG_C, sum > 0xFF);
  SetFlag(FLAG_V, (~(a ^ value) & (a ^ result) & 0x80) != 0);
  a = result;
  SetNZ(a);
}

void                    hbs::CMP_6502::SubWithCarry(uint8_t value)
{
  uint16_t              borrow = GetFlag(FLAG_C) ? 0 : 1;
  uint16_t              diff = (uint16_t)a - value - borrow;
  uint8_t               result = (uint8_t)diff;

  SetFlag(FLAG_C, (uint16_t)a >= (uint16_t)value + borrow);
  SetFlag(FLAG_V, ((a ^ result) & (a ^ value) & 0x80) != 0);
  a = result;
  SetNZ(a);
}

void                    hbs::CMP_6502::Compare(uint8_t reg, uint8_t value)
{
  uint8_t               result = (uint8_t)(reg - value);

  SetFlag(FLAG_C, reg >= value);
  SetNZ(result);
}

uint8_t                 hbs::CMP_6502::IndexValue(void) const
{
  if (index_register == IX_X)
    return (x);
  if (index_register == IX_Y)
    return (y);
  return (0);
}

bool                    hbs::CMP_6502::IsWriteOperation(void) const
{
  return (pending == OP_STA || pending == OP_STX || pending == OP_STY);
}

bool                    hbs::CMP_6502::IsModifyOperation(void) const
{
  return (pending == OP_INC || pending == OP_DEC || pending == OP_ASL ||
          pending == OP_LSR || pending == OP_ROL || pending == OP_ROR);
}

uint8_t                 hbs::CMP_6502::ApplyModifyOperation(uint8_t value)
{
  uint8_t               result;
  bool                  old_carry;

  switch (pending)
    {
    case OP_INC:
      result = (uint8_t)(value + 1);
      SetNZ(result);
      return (result);
    case OP_DEC:
      result = (uint8_t)(value - 1);
      SetNZ(result);
      return (result);
    case OP_ASL:
      SetFlag(FLAG_C, (value & 0x80) != 0);
      result = (uint8_t)(value << 1);
      SetNZ(result);
      return (result);
    case OP_LSR:
      SetFlag(FLAG_C, (value & 0x01) != 0);
      result = (uint8_t)(value >> 1);
      SetNZ(result);
      return (result);
    case OP_ROL:
      old_carry = GetFlag(FLAG_C);
      SetFlag(FLAG_C, (value & 0x80) != 0);
      result = (uint8_t)((value << 1) | (old_carry ? 1 : 0));
      SetNZ(result);
      return (result);
    case OP_ROR:
      old_carry = GetFlag(FLAG_C);
      SetFlag(FLAG_C, (value & 0x01) != 0);
      result = (uint8_t)((value >> 1) | (old_carry ? 0x80 : 0));
      SetNZ(result);
      return (result);
    default:
      return (value);
    }
}

void                    hbs::CMP_6502::ExecuteAccumulator(PendingOperation op)
{
  pending = op;
  a = ApplyModifyOperation(a);
  pc++;
  PrepareFetch();
}

uint8_t                 hbs::CMP_6502::ReadDataBus(void)
{
  uint8_t               value = 0;
  hbs::Tristate         bit;

  for (size_t i = 0; i < 8; ++i)
    {
      bit = GetPin(data_pin_for_bit(i));
      if (bit == hbs::TRUE)
        value |= (uint8_t)(1 << i);
    }
  return (value);
}

void                    hbs::CMP_6502::PrepareFetch(void)
{
  address = pc;
  rw = true;
  drive_data = false;
  sync = true;
  phase = FETCH_OPCODE;
  pending = OP_NONE;
  index_register = IX_NONE;
  branch = BR_NONE;
  indirect_add_y = false;
  indexed_indirect = false;
}

void                    hbs::CMP_6502::BeginImmediate(PendingOperation op)
{
  pending = op;
  address = ++pc;
  phase = READ_IMMEDIATE;
}

void                    hbs::CMP_6502::BeginZeroPage(PendingOperation op)
{
  pending = op;
  index_register = IX_NONE;
  address = ++pc;
  phase = READ_ZERO_PAGE_ADDRESS;
}

void                    hbs::CMP_6502::BeginZeroPageIndexed(PendingOperation op, IndexRegister idx)
{
  pending = op;
  index_register = idx;
  indirect_add_y = false;
  indexed_indirect = false;
  address = ++pc;
  phase = READ_ZERO_PAGE_INDEXED_ADDRESS;
}

void                    hbs::CMP_6502::BeginIndexedIndirect(PendingOperation op)
{
  pending = op;
  index_register = IX_X;
  indirect_add_y = false;
  indexed_indirect = true;
  address = ++pc;
  phase = READ_ZERO_PAGE_INDEXED_ADDRESS;
}

void                    hbs::CMP_6502::BeginIndirectIndexed(PendingOperation op)
{
  pending = op;
  index_register = IX_NONE;
  indirect_add_y = true;
  indexed_indirect = false;
  address = ++pc;
  phase = READ_ZERO_PAGE_ADDRESS;
}

void                    hbs::CMP_6502::BeginAbsolute(PendingOperation op)
{
  BeginAbsoluteIndexed(op, IX_NONE);
}

void                    hbs::CMP_6502::BeginAbsoluteIndexed(PendingOperation op, IndexRegister idx)
{
  pending = op;
  index_register = idx;
  address = ++pc;
  phase = READ_ABSOLUTE_LOW;
}

void                    hbs::CMP_6502::BeginReadModifyZeroPage(PendingOperation op, IndexRegister idx)
{
  pending = op;
  index_register = idx;
  address = ++pc;
  phase = idx == IX_NONE ? READ_ZERO_PAGE_ADDRESS : READ_ZERO_PAGE_INDEXED_ADDRESS;
}

void                    hbs::CMP_6502::BeginReadModifyAbsolute(PendingOperation op, IndexRegister idx)
{
  BeginAbsoluteIndexed(op, idx);
}

void                    hbs::CMP_6502::BeginIndirectJump(void)
{
  pending = OP_JMP_INDIRECT;
  index_register = IX_NONE;
  indirect_add_y = false;
  indexed_indirect = false;
  address = ++pc;
  phase = READ_ABSOLUTE_LOW;
}

void                    hbs::CMP_6502::BeginRelative(BranchCondition cond)
{
  branch = cond;
  address = ++pc;
  phase = READ_RELATIVE;
}

void                    hbs::CMP_6502::BeginStackPush(PendingOperation op, uint8_t value)
{
  pending = op;
  address = 0x0100 | sp;
  data_out = value;
  rw = false;
  drive_data = true;
  phase = STACK_PUSH_DATA;
}

void                    hbs::CMP_6502::BeginStackPull(PendingOperation op)
{
  pending = op;
  sp++;
  address = 0x0100 | sp;
  rw = true;
  drive_data = false;
  phase = STACK_PULL_DATA;
}

void                    hbs::CMP_6502::ExecuteImplied(uint8_t op)
{
  switch (op)
    {
    case 0x18: SetFlag(FLAG_C, false); break ; // CLC
    case 0x38: SetFlag(FLAG_C, true); break ;  // SEC
    case 0x58: SetFlag(FLAG_I, false); break ; // CLI
    case 0x78: SetFlag(FLAG_I, true); break ;  // SEI
    case 0xB8: SetFlag(FLAG_V, false); break ; // CLV
    case 0xD8: SetFlag(FLAG_D, false); break ; // CLD
    case 0xF8: SetFlag(FLAG_D, true); break ;  // SED
    case 0xAA: x = a; SetNZ(x); break ;        // TAX
    case 0xA8: y = a; SetNZ(y); break ;        // TAY
    case 0x8A: a = x; SetNZ(a); break ;        // TXA
    case 0x98: a = y; SetNZ(a); break ;        // TYA
    case 0xBA: x = sp; SetNZ(x); break ;       // TSX
    case 0x9A: sp = x; break ;                 // TXS
    case 0xE8: x++; SetNZ(x); break ;          // INX
    case 0xC8: y++; SetNZ(y); break ;          // INY
    case 0xCA: x--; SetNZ(x); break ;          // DEX
    case 0x88: y--; SetNZ(y); break ;          // DEY
    default: break ;
    }
  pc++;
  PrepareFetch();
}

void                    hbs::CMP_6502::ExecuteOpcode(uint8_t op)
{
  opcode = op;
  sync = false;
  drive_data = false;
  rw = true;

  switch (op)
    {
    case 0xEA: // NOP
      pc++;
      PrepareFetch();
      break ;
    case 0xA9: BeginImmediate(OP_LDA); break ;
    case 0xA2: BeginImmediate(OP_LDX); break ;
    case 0xA0: BeginImmediate(OP_LDY); break ;
    case 0x69: BeginImmediate(OP_ADC); break ;
    case 0xE9: BeginImmediate(OP_SBC); break ;
    case 0x29: BeginImmediate(OP_AND); break ;
    case 0x09: BeginImmediate(OP_ORA); break ;
    case 0x49: BeginImmediate(OP_EOR); break ;
    case 0xC9: BeginImmediate(OP_CMP); break ;
    case 0xE0: BeginImmediate(OP_CPX); break ;
    case 0xC0: BeginImmediate(OP_CPY); break ;
    case 0xA5: BeginZeroPage(OP_LDA); break ;
    case 0xA6: BeginZeroPage(OP_LDX); break ;
    case 0xA4: BeginZeroPage(OP_LDY); break ;
    case 0x85: BeginZeroPage(OP_STA); break ;
    case 0x86: BeginZeroPage(OP_STX); break ;
    case 0x84: BeginZeroPage(OP_STY); break ;
    case 0x65: BeginZeroPage(OP_ADC); break ;
    case 0xE5: BeginZeroPage(OP_SBC); break ;
    case 0x25: BeginZeroPage(OP_AND); break ;
    case 0x05: BeginZeroPage(OP_ORA); break ;
    case 0x45: BeginZeroPage(OP_EOR); break ;
    case 0xC5: BeginZeroPage(OP_CMP); break ;
    case 0xE4: BeginZeroPage(OP_CPX); break ;
    case 0xC4: BeginZeroPage(OP_CPY); break ;
    case 0x24: BeginZeroPage(OP_BIT); break ;
    case 0xB5: BeginZeroPageIndexed(OP_LDA, IX_X); break ;
    case 0xB6: BeginZeroPageIndexed(OP_LDX, IX_Y); break ;
    case 0xB4: BeginZeroPageIndexed(OP_LDY, IX_X); break ;
    case 0x95: BeginZeroPageIndexed(OP_STA, IX_X); break ;
    case 0x96: BeginZeroPageIndexed(OP_STX, IX_Y); break ;
    case 0x94: BeginZeroPageIndexed(OP_STY, IX_X); break ;
    case 0x75: BeginZeroPageIndexed(OP_ADC, IX_X); break ;
    case 0xF5: BeginZeroPageIndexed(OP_SBC, IX_X); break ;
    case 0x35: BeginZeroPageIndexed(OP_AND, IX_X); break ;
    case 0x15: BeginZeroPageIndexed(OP_ORA, IX_X); break ;
    case 0x55: BeginZeroPageIndexed(OP_EOR, IX_X); break ;
    case 0xD5: BeginZeroPageIndexed(OP_CMP, IX_X); break ;
    case 0xA1: BeginIndexedIndirect(OP_LDA); break ;
    case 0x81: BeginIndexedIndirect(OP_STA); break ;
    case 0x61: BeginIndexedIndirect(OP_ADC); break ;
    case 0xE1: BeginIndexedIndirect(OP_SBC); break ;
    case 0x21: BeginIndexedIndirect(OP_AND); break ;
    case 0x01: BeginIndexedIndirect(OP_ORA); break ;
    case 0x41: BeginIndexedIndirect(OP_EOR); break ;
    case 0xC1: BeginIndexedIndirect(OP_CMP); break ;
    case 0xB1: BeginIndirectIndexed(OP_LDA); break ;
    case 0x91: BeginIndirectIndexed(OP_STA); break ;
    case 0x71: BeginIndirectIndexed(OP_ADC); break ;
    case 0xF1: BeginIndirectIndexed(OP_SBC); break ;
    case 0x31: BeginIndirectIndexed(OP_AND); break ;
    case 0x11: BeginIndirectIndexed(OP_ORA); break ;
    case 0x51: BeginIndirectIndexed(OP_EOR); break ;
    case 0xD1: BeginIndirectIndexed(OP_CMP); break ;
    case 0xAD: BeginAbsolute(OP_LDA); break ;
    case 0xAE: BeginAbsolute(OP_LDX); break ;
    case 0xAC: BeginAbsolute(OP_LDY); break ;
    case 0x8D: BeginAbsolute(OP_STA); break ;
    case 0x8E: BeginAbsolute(OP_STX); break ;
    case 0x8C: BeginAbsolute(OP_STY); break ;
    case 0x6D: BeginAbsolute(OP_ADC); break ;
    case 0xED: BeginAbsolute(OP_SBC); break ;
    case 0x2D: BeginAbsolute(OP_AND); break ;
    case 0x0D: BeginAbsolute(OP_ORA); break ;
    case 0x4D: BeginAbsolute(OP_EOR); break ;
    case 0xCD: BeginAbsolute(OP_CMP); break ;
    case 0xEC: BeginAbsolute(OP_CPX); break ;
    case 0xCC: BeginAbsolute(OP_CPY); break ;
    case 0x2C: BeginAbsolute(OP_BIT); break ;
    case 0xBD: BeginAbsoluteIndexed(OP_LDA, IX_X); break ;
    case 0xB9: BeginAbsoluteIndexed(OP_LDA, IX_Y); break ;
    case 0xBE: BeginAbsoluteIndexed(OP_LDX, IX_Y); break ;
    case 0xBC: BeginAbsoluteIndexed(OP_LDY, IX_X); break ;
    case 0x9D: BeginAbsoluteIndexed(OP_STA, IX_X); break ;
    case 0x99: BeginAbsoluteIndexed(OP_STA, IX_Y); break ;
    case 0x7D: BeginAbsoluteIndexed(OP_ADC, IX_X); break ;
    case 0x79: BeginAbsoluteIndexed(OP_ADC, IX_Y); break ;
    case 0xFD: BeginAbsoluteIndexed(OP_SBC, IX_X); break ;
    case 0xF9: BeginAbsoluteIndexed(OP_SBC, IX_Y); break ;
    case 0x3D: BeginAbsoluteIndexed(OP_AND, IX_X); break ;
    case 0x39: BeginAbsoluteIndexed(OP_AND, IX_Y); break ;
    case 0x1D: BeginAbsoluteIndexed(OP_ORA, IX_X); break ;
    case 0x19: BeginAbsoluteIndexed(OP_ORA, IX_Y); break ;
    case 0x5D: BeginAbsoluteIndexed(OP_EOR, IX_X); break ;
    case 0x59: BeginAbsoluteIndexed(OP_EOR, IX_Y); break ;
    case 0xDD: BeginAbsoluteIndexed(OP_CMP, IX_X); break ;
    case 0xD9: BeginAbsoluteIndexed(OP_CMP, IX_Y); break ;
    case 0x4C: BeginAbsolute(OP_JMP); break ;
    case 0x6C: BeginIndirectJump(); break ;
    case 0x20: BeginAbsolute(OP_JSR); break ;
    case 0xE6: BeginReadModifyZeroPage(OP_INC, IX_NONE); break ;
    case 0xF6: BeginReadModifyZeroPage(OP_INC, IX_X); break ;
    case 0xEE: BeginReadModifyAbsolute(OP_INC, IX_NONE); break ;
    case 0xFE: BeginReadModifyAbsolute(OP_INC, IX_X); break ;
    case 0xC6: BeginReadModifyZeroPage(OP_DEC, IX_NONE); break ;
    case 0xD6: BeginReadModifyZeroPage(OP_DEC, IX_X); break ;
    case 0xCE: BeginReadModifyAbsolute(OP_DEC, IX_NONE); break ;
    case 0xDE: BeginReadModifyAbsolute(OP_DEC, IX_X); break ;
    case 0x0A: ExecuteAccumulator(OP_ASL); break ;
    case 0x06: BeginReadModifyZeroPage(OP_ASL, IX_NONE); break ;
    case 0x16: BeginReadModifyZeroPage(OP_ASL, IX_X); break ;
    case 0x0E: BeginReadModifyAbsolute(OP_ASL, IX_NONE); break ;
    case 0x1E: BeginReadModifyAbsolute(OP_ASL, IX_X); break ;
    case 0x4A: ExecuteAccumulator(OP_LSR); break ;
    case 0x46: BeginReadModifyZeroPage(OP_LSR, IX_NONE); break ;
    case 0x56: BeginReadModifyZeroPage(OP_LSR, IX_X); break ;
    case 0x4E: BeginReadModifyAbsolute(OP_LSR, IX_NONE); break ;
    case 0x5E: BeginReadModifyAbsolute(OP_LSR, IX_X); break ;
    case 0x2A: ExecuteAccumulator(OP_ROL); break ;
    case 0x26: BeginReadModifyZeroPage(OP_ROL, IX_NONE); break ;
    case 0x36: BeginReadModifyZeroPage(OP_ROL, IX_X); break ;
    case 0x2E: BeginReadModifyAbsolute(OP_ROL, IX_NONE); break ;
    case 0x3E: BeginReadModifyAbsolute(OP_ROL, IX_X); break ;
    case 0x6A: ExecuteAccumulator(OP_ROR); break ;
    case 0x66: BeginReadModifyZeroPage(OP_ROR, IX_NONE); break ;
    case 0x76: BeginReadModifyZeroPage(OP_ROR, IX_X); break ;
    case 0x6E: BeginReadModifyAbsolute(OP_ROR, IX_NONE); break ;
    case 0x7E: BeginReadModifyAbsolute(OP_ROR, IX_X); break ;
    case 0x10: BeginRelative(BR_BPL); break ;
    case 0x30: BeginRelative(BR_BMI); break ;
    case 0x50: BeginRelative(BR_BVC); break ;
    case 0x70: BeginRelative(BR_BVS); break ;
    case 0x90: BeginRelative(BR_BCC); break ;
    case 0xB0: BeginRelative(BR_BCS); break ;
    case 0xD0: BeginRelative(BR_BNE); break ;
    case 0xF0: BeginRelative(BR_BEQ); break ;
    case 0x48: BeginStackPush(OP_PHA, a); break ;
    case 0x08: BeginStackPush(OP_PHP, status | FLAG_B | FLAG_U); break ;
    case 0x68: BeginStackPull(OP_PLA); break ;
    case 0x28: BeginStackPull(OP_PLP); break ;
    case 0x40: BeginRti(); break ;
    case 0x60: // RTS
      sp++;
      address = 0x0100 | sp;
      phase = RTS_PULL_LOW;
      break ;
    case 0x00: // BRK
      BeginInterrupt(0xFFFE, true);
      break ;
    case 0x18: case 0x38: case 0x58: case 0x78:
    case 0xB8: case 0xD8: case 0xF8:
    case 0xAA: case 0xA8: case 0x8A: case 0x98:
    case 0xBA: case 0x9A: case 0xE8: case 0xC8:
    case 0xCA: case 0x88:
      ExecuteImplied(op);
      break ;
    default:
      // Unsupported opcode: keep the simulator alive by treating it as a NOP.
      pc++;
      PrepareFetch();
      break ;
    }
}

void                    hbs::CMP_6502::ApplyReadOperation(uint8_t value)
{
  switch (pending)
    {
    case OP_LDA: a = value; SetNZ(a); break ;
    case OP_LDX: x = value; SetNZ(x); break ;
    case OP_LDY: y = value; SetNZ(y); break ;
    case OP_ADC: AddWithCarry(value); break ;
    case OP_SBC: SubWithCarry(value); break ;
    case OP_AND: a &= value; SetNZ(a); break ;
    case OP_ORA: a |= value; SetNZ(a); break ;
    case OP_EOR: a ^= value; SetNZ(a); break ;
    case OP_CMP: Compare(a, value); break ;
    case OP_CPX: Compare(x, value); break ;
    case OP_CPY: Compare(y, value); break ;
    case OP_BIT:
      SetFlag(FLAG_Z, (a & value) == 0);
      SetFlag(FLAG_N, (value & 0x80) != 0);
      SetFlag(FLAG_V, (value & 0x40) != 0);
      break ;
    default: break ;
    }
}

void                    hbs::CMP_6502::CompleteImmediate(uint8_t value)
{
  ApplyReadOperation(value);
  pc++;
  PrepareFetch();
}

void                    hbs::CMP_6502::ContinueWithEffectiveAddress(void)
{
  address = effective;
  if (IsWriteOperation())
    {
      data_out = (pending == OP_STA ? a : (pending == OP_STX ? x : y));
      rw = false;
      drive_data = true;
      phase = WRITE_ABSOLUTE_DATA;
      return ;
    }
  if (IsModifyOperation())
    {
      rw = true;
      drive_data = false;
      phase = READ_MODIFY_DATA;
      return ;
    }
  rw = true;
  drive_data = false;
  phase = READ_ABSOLUTE_DATA;
}

void                    hbs::CMP_6502::CompleteZeroPageAddress(uint8_t value)
{
  effective = value;
  if (indirect_add_y)
    {
      address = effective;
      phase = READ_ZERO_PAGE_POINTER_LOW;
      return ;
    }
  pc++;
  if (IsWriteOperation())
    {
      address = effective;
      data_out = (pending == OP_STA ? a : (pending == OP_STX ? x : y));
      rw = false;
      drive_data = true;
      phase = WRITE_ZERO_PAGE_DATA;
    }
  else if (IsModifyOperation())
    {
      address = effective;
      rw = true;
      drive_data = false;
      phase = READ_MODIFY_DATA;
    }
  else
    {
      address = effective;
      rw = true;
      drive_data = false;
      phase = READ_ZERO_PAGE_DATA;
    }
}

void                    hbs::CMP_6502::CompleteZeroPageIndexedAddress(uint8_t value)
{
  effective = (uint8_t)(value + IndexValue());
  if (indexed_indirect)
    {
      address = effective;
      phase = READ_ZERO_PAGE_POINTER_LOW;
      return ;
    }
  pc++;
  if (IsWriteOperation())
    {
      address = effective;
      data_out = (pending == OP_STA ? a : (pending == OP_STX ? x : y));
      rw = false;
      drive_data = true;
      phase = WRITE_ZERO_PAGE_DATA;
    }
  else if (IsModifyOperation())
    {
      address = effective;
      rw = true;
      drive_data = false;
      phase = READ_MODIFY_DATA;
    }
  else
    {
      address = effective;
      rw = true;
      drive_data = false;
      phase = READ_ZERO_PAGE_DATA;
    }
}

void                    hbs::CMP_6502::CompleteZeroPagePointerLow(uint8_t value)
{
  scratch = value;
  address = (uint8_t)(effective + 1);
  phase = READ_ZERO_PAGE_POINTER_HIGH;
}

void                    hbs::CMP_6502::CompleteZeroPagePointerHigh(uint8_t value)
{
  effective = (uint16_t)scratch | ((uint16_t)value << 8);
  if (indirect_add_y)
    effective += y;
  pc++;
  ContinueWithEffectiveAddress();
}

void                    hbs::CMP_6502::CompleteZeroPageRead(uint8_t value)
{
  ApplyReadOperation(value);
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteZeroPageWrite(void)
{
  rw = true;
  drive_data = false;
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteAbsoluteHigh(uint8_t value)
{
  effective |= (uint16_t)value << 8;
  if (pending == OP_JSR)
    {
      return_address = pc;
      address = 0x0100 | sp;
      data_out = (uint8_t)(return_address >> 8);
      rw = false;
      drive_data = true;
      phase = JSR_PUSH_HIGH;
      return ;
    }
  if (pending == OP_JMP_INDIRECT)
    {
      address = effective;
      phase = READ_INDIRECT_LOW;
      return ;
    }
  pc++;
  if (pending == OP_JMP)
    {
      pc = effective;
      PrepareFetch();
      return ;
    }
  effective += IndexValue();
  ContinueWithEffectiveAddress();
}

void                    hbs::CMP_6502::CompleteAbsoluteRead(uint8_t value)
{
  ApplyReadOperation(value);
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteAbsoluteWrite(void)
{
  rw = true;
  drive_data = false;
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteReadModify(uint8_t value)
{
  data_out = ApplyModifyOperation(value);
  rw = false;
  drive_data = true;
  phase = WRITE_MODIFY_DATA;
}

void                    hbs::CMP_6502::CompleteModifyWrite(void)
{
  rw = true;
  drive_data = false;
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteIndirectLow(uint8_t value)
{
  scratch = value;
  address = (effective & 0xFF00) | ((effective + 1) & 0x00FF); // NMOS page wrap
  phase = READ_INDIRECT_HIGH;
}

void                    hbs::CMP_6502::CompleteIndirectHigh(uint8_t value)
{
  pc = (uint16_t)scratch | ((uint16_t)value << 8);
  PrepareFetch();
}

bool                    hbs::CMP_6502::BranchTaken(void) const
{
  switch (branch)
    {
    case BR_BPL: return (!GetFlag(FLAG_N));
    case BR_BMI: return (GetFlag(FLAG_N));
    case BR_BVC: return (!GetFlag(FLAG_V));
    case BR_BVS: return (GetFlag(FLAG_V));
    case BR_BCC: return (!GetFlag(FLAG_C));
    case BR_BCS: return (GetFlag(FLAG_C));
    case BR_BNE: return (!GetFlag(FLAG_Z));
    case BR_BEQ: return (GetFlag(FLAG_Z));
    default: return (false);
    }
}

void                    hbs::CMP_6502::CompleteRelative(uint8_t value)
{
  int8_t                offset = (int8_t)value;

  pc++;
  if (BranchTaken())
    pc = (uint16_t)(pc + offset);
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteStackPush(void)
{
  sp--;
  rw = true;
  drive_data = false;
  pc++;
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteStackPull(uint8_t value)
{
  if (pending == OP_PLA)
    {
      a = value;
      SetNZ(a);
    }
  else if (pending == OP_PLP)
    status = (value | FLAG_U) & ~FLAG_B;
  pc++;
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteJsrPushHigh(void)
{
  sp--;
  address = 0x0100 | sp;
  data_out = (uint8_t)(return_address & 0xFF);
  rw = false;
  drive_data = true;
  phase = JSR_PUSH_LOW;
}

void                    hbs::CMP_6502::CompleteJsrPushLow(void)
{
  sp--;
  rw = true;
  drive_data = false;
  pc = effective;
  PrepareFetch();
}

void                    hbs::CMP_6502::CompleteRtsPullLow(uint8_t value)
{
  effective = value;
  sp++;
  address = 0x0100 | sp;
  phase = RTS_PULL_HIGH;
}

void                    hbs::CMP_6502::CompleteRtsPullHigh(uint8_t value)
{
  effective |= (uint16_t)value << 8;
  pc = effective + 1;
  PrepareFetch();
}

bool                    hbs::CMP_6502::ExternalInterruptPending(void)
{
  if (GetPrevious(6) == hbs::TRUE && GetPin(6) == hbs::FALSE)
    return (true);
  if (GetPin(4) == hbs::FALSE && !GetFlag(FLAG_I))
    return (true);
  return (false);
}

void                    hbs::CMP_6502::BeginInterrupt(uint16_t vector, bool software)
{
  if (software)
    return_address = (uint16_t)(pc + 2);
  else
    return_address = pc;
  interrupt_vector = vector;
  interrupt_status = status | FLAG_U | (software ? FLAG_B : 0);
  SetFlag(FLAG_I, true);
  address = 0x0100 | sp;
  data_out = (uint8_t)(return_address >> 8);
  rw = false;
  drive_data = true;
  sync = false;
  phase = INTERRUPT_PUSH_PCH;
}

void                    hbs::CMP_6502::CompleteInterruptPushPch(void)
{
  sp--;
  address = 0x0100 | sp;
  data_out = (uint8_t)(return_address & 0xFF);
  rw = false;
  drive_data = true;
  phase = INTERRUPT_PUSH_PCL;
}

void                    hbs::CMP_6502::CompleteInterruptPushPcl(void)
{
  sp--;
  address = 0x0100 | sp;
  data_out = interrupt_status;
  rw = false;
  drive_data = true;
  phase = INTERRUPT_PUSH_STATUS;
}

void                    hbs::CMP_6502::CompleteInterruptPushStatus(void)
{
  sp--;
  rw = true;
  drive_data = false;
  address = interrupt_vector;
  phase = INTERRUPT_VECTOR_LOW;
}

void                    hbs::CMP_6502::CompleteInterruptVectorLow(uint8_t value)
{
  pc = value;
  address = interrupt_vector + 1;
  phase = INTERRUPT_VECTOR_HIGH;
}

void                    hbs::CMP_6502::CompleteInterruptVectorHigh(uint8_t value)
{
  pc |= (uint16_t)value << 8;
  PrepareFetch();
}

void                    hbs::CMP_6502::BeginRti(void)
{
  sp++;
  address = 0x0100 | sp;
  rw = true;
  drive_data = false;
  sync = false;
  phase = RTI_PULL_STATUS;
}

void                    hbs::CMP_6502::CompleteRtiStatus(uint8_t value)
{
  status = (value | FLAG_U) & ~FLAG_B;
  sp++;
  address = 0x0100 | sp;
  phase = RTI_PULL_LOW;
}

void                    hbs::CMP_6502::CompleteRtiLow(uint8_t value)
{
  effective = value;
  sp++;
  address = 0x0100 | sp;
  phase = RTI_PULL_HIGH;
}

void                    hbs::CMP_6502::CompleteRtiHigh(uint8_t value)
{
  pc = effective | ((uint16_t)value << 8);
  PrepareFetch();
}

void                    hbs::CMP_6502::Tick(void)
{
  uint8_t               data;

  if (GetPin(40) == hbs::FALSE)
    {
      ResetCore();
      reset_seen_high = false;
      return ;
    }
  if (last_tick == timer.GetTime())
    return ;
  if (GetPin(2) == hbs::FALSE) // RDY
    return ;
  if (GetPrevious(37) != hbs::FALSE || GetPin(37) != hbs::TRUE)
    return ;
  last_tick = timer.GetTime();
  reset_seen_high = true;
  if (GetPrevious(38) == hbs::TRUE && GetPin(38) == hbs::FALSE)
    SetFlag(FLAG_V, true);
  data = ReadDataBus();

  switch (phase)
    {
    case RESET_VECTOR_LOW:
      pc = data;
      address = 0xFFFD;
      rw = true;
      drive_data = false;
      sync = false;
      phase = RESET_VECTOR_HIGH;
      break ;
    case RESET_VECTOR_HIGH:
      pc |= (uint16_t)data << 8;
      PrepareFetch();
      break ;
    case FETCH_OPCODE:
      if (GetPrevious(6) == hbs::TRUE && GetPin(6) == hbs::FALSE)
        BeginInterrupt(0xFFFA, false);
      else if (GetPin(4) == hbs::FALSE && !GetFlag(FLAG_I))
        BeginInterrupt(0xFFFE, false);
      else
        ExecuteOpcode(data);
      break ;
    case READ_IMMEDIATE:
      CompleteImmediate(data);
      break ;
    case READ_ZERO_PAGE_ADDRESS:
      CompleteZeroPageAddress(data);
      break ;
    case READ_ZERO_PAGE_INDEXED_ADDRESS:
      CompleteZeroPageIndexedAddress(data);
      break ;
    case READ_ZERO_PAGE_POINTER_LOW:
      CompleteZeroPagePointerLow(data);
      break ;
    case READ_ZERO_PAGE_POINTER_HIGH:
      CompleteZeroPagePointerHigh(data);
      break ;
    case READ_ZERO_PAGE_DATA:
      CompleteZeroPageRead(data);
      break ;
    case WRITE_ZERO_PAGE_DATA:
      CompleteZeroPageWrite();
      break ;
    case READ_ABSOLUTE_LOW:
      effective = data;
      address = ++pc;
      phase = READ_ABSOLUTE_HIGH;
      break ;
    case READ_ABSOLUTE_HIGH:
      CompleteAbsoluteHigh(data);
      break ;
    case READ_ABSOLUTE_DATA:
      CompleteAbsoluteRead(data);
      break ;
    case WRITE_ABSOLUTE_DATA:
      CompleteAbsoluteWrite();
      break ;
    case READ_MODIFY_DATA:
      CompleteReadModify(data);
      break ;
    case WRITE_MODIFY_DATA:
      CompleteModifyWrite();
      break ;
    case READ_INDIRECT_LOW:
      CompleteIndirectLow(data);
      break ;
    case READ_INDIRECT_HIGH:
      CompleteIndirectHigh(data);
      break ;
    case READ_RELATIVE:
      CompleteRelative(data);
      break ;
    case STACK_PUSH_DATA:
      CompleteStackPush();
      break ;
    case STACK_PULL_DATA:
      CompleteStackPull(data);
      break ;
    case JSR_PUSH_HIGH:
      CompleteJsrPushHigh();
      break ;
    case JSR_PUSH_LOW:
      CompleteJsrPushLow();
      break ;
    case RTS_PULL_LOW:
      CompleteRtsPullLow(data);
      break ;
    case RTS_PULL_HIGH:
      CompleteRtsPullHigh(data);
      break ;
    case INTERRUPT_PUSH_PCH:
      CompleteInterruptPushPch();
      break ;
    case INTERRUPT_PUSH_PCL:
      CompleteInterruptPushPcl();
      break ;
    case INTERRUPT_PUSH_STATUS:
      CompleteInterruptPushStatus();
      break ;
    case INTERRUPT_VECTOR_LOW:
      CompleteInterruptVectorLow(data);
      break ;
    case INTERRUPT_VECTOR_HIGH:
      CompleteInterruptVectorHigh(data);
      break ;
    case RTI_PULL_STATUS:
      CompleteRtiStatus(data);
      break ;
    case RTI_PULL_LOW:
      CompleteRtiLow(data);
      break ;
    case RTI_PULL_HIGH:
      CompleteRtiHigh(data);
      break ;
    case HALTED:
      rw = true;
      drive_data = false;
      sync = true;
      break ;
    }
}

hbs::Tristate           hbs::CMP_6502::DataBusBit(size_t pin) const
{
  size_t                bit = 33 - pin;

  if (!drive_data)
    return (hbs::UNDEFINED);
  return ((data_out & (1 << bit)) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_6502::AddressBusBit(size_t pin) const
{
  int                   bit = address_bit_for_pin(pin);

  if (bit < 0)
    return (hbs::UNDEFINED);
  return ((address & (1 << bit)) ? hbs::TRUE : hbs::FALSE);
}

hbs::Tristate           hbs::CMP_6502::Compute(size_t n)
{
  hbs::Tristate         tri;
  Preset                p;

  if (AlreadyComputed(n, tri))
    return (tri);
  p = PresetOutput(n);
  if (DrivesPin(n))
    {
      Tick();
      if (IsAddressPin(n))
        return (p->second = AddressBusBit(n));
      if (IsDataPin(n))
        return (p->second = DataBusBit(n));
      if (n == 3)
        return (p->second = GetPin(37) == hbs::TRUE ? hbs::FALSE : hbs::TRUE); // PHI1O
      if (n == 39)
        return (p->second = GetPin(37)); // PHI2O
      if (n == 7)
        return (p->second = sync ? hbs::TRUE : hbs::FALSE);
      if (n == 34)
        return (p->second = rw ? hbs::TRUE : hbs::FALSE);
    }
  if (n == 1 || n == 8 || n == 21)
    return (hbs::UNDEFINED);
  return (GetPin(n));
}

hbs::CMP_6502::CMP_6502(const hbs::Timer        &time,
                        const std::string       &name,
                        const std::string       &pos)
  : AComponent(time, name, pos),
    last_tick(0),
    reset_seen_high(false)
{
  ResetCore();
}

hbs::CMP_6502::~CMP_6502(void)
{}

bool                    hbs::CMP_6502::TypeMatches(const std::string &type)
{
  if (type == "6502" || type == "MOS6502" || type == "NMOS6502")
    return (true);
  return (false);
}

hbs::IComponent         *hbs::CMP_6502::Create(hbs::Timer        &timer,
                                               const std::string &type,
                                               const std::string &name,
                                               const std::string &value,
                                               const std::string &position)
{
  (void)value;
  if (!TypeMatches(type))
    return (NULL);
  return (new hbs::CMP_6502(timer, name, position));
}

std::string             hbs::CMP_6502::GetDisplayType(void) const
{
  return ("6502 NMOS CPU, bus-level educational core");
}

std::string             hbs::CMP_6502::GetDefinitionValue(void) const
{
  return ("subset: reset,bus,RDY,SYNC,NOP,loads/stores,ADC,SBC,logic,CMP/CPX/CPY,BIT,branches,stack,JSR/RTS,BRK/RTI,IRQ/NMI");
}
