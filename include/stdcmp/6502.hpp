// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_6502_HPP__
# define                        __HBS_6502_HPP__
# include                       <stdint.h>
# include                       <string>
# include                       "AComponent.hpp"

namespace                       hbs
{
  class                         CMP_6502 : public hbs::AComponent<40>
  {
  private:
    enum                        Phase
      {
        RESET_VECTOR_LOW,
        RESET_VECTOR_HIGH,
        FETCH_OPCODE,
        READ_IMMEDIATE,
        READ_ZERO_PAGE_ADDRESS,
        READ_ZERO_PAGE_INDEXED_ADDRESS,
        READ_ZERO_PAGE_POINTER_LOW,
        READ_ZERO_PAGE_POINTER_HIGH,
        READ_ZERO_PAGE_DATA,
        WRITE_ZERO_PAGE_DATA,
        READ_ABSOLUTE_LOW,
        READ_ABSOLUTE_HIGH,
        READ_ABSOLUTE_DATA,
        WRITE_ABSOLUTE_DATA,
        READ_MODIFY_DATA,
        WRITE_MODIFY_DATA,
        READ_INDIRECT_LOW,
        READ_INDIRECT_HIGH,
        READ_RELATIVE,
        STACK_PUSH_DATA,
        STACK_PULL_DATA,
        JSR_PUSH_HIGH,
        JSR_PUSH_LOW,
        RTS_PULL_LOW,
        RTS_PULL_HIGH,
        INTERRUPT_PUSH_PCH,
        INTERRUPT_PUSH_PCL,
        INTERRUPT_PUSH_STATUS,
        INTERRUPT_VECTOR_LOW,
        INTERRUPT_VECTOR_HIGH,
        RTI_PULL_STATUS,
        RTI_PULL_LOW,
        RTI_PULL_HIGH,
        HALTED
      };

    enum                        PendingOperation
      {
        OP_NONE,
        OP_LDA,
        OP_LDX,
        OP_LDY,
        OP_STA,
        OP_STX,
        OP_STY,
        OP_ADC,
        OP_SBC,
        OP_AND,
        OP_ORA,
        OP_EOR,
        OP_CMP,
        OP_CPX,
        OP_CPY,
        OP_BIT,
        OP_JMP,
        OP_JSR,
        OP_PHA,
        OP_PHP,
        OP_PLA,
        OP_PLP,
        OP_INC,
        OP_DEC,
        OP_ASL,
        OP_LSR,
        OP_ROL,
        OP_ROR,
        OP_JMP_INDIRECT,
        OP_RTI
      };

    enum                        IndexRegister
      {
        IX_NONE,
        IX_X,
        IX_Y
      };

    enum                        BranchCondition
      {
        BR_NONE,
        BR_BPL,
        BR_BMI,
        BR_BVC,
        BR_BVS,
        BR_BCC,
        BR_BCS,
        BR_BNE,
        BR_BEQ
      };

    const std::string           &GetType(void) const;

    uint16_t                    address;
    uint16_t                    pc;
    uint16_t                    effective;
    uint16_t                    return_address;
    uint8_t                     scratch;
    uint8_t                     a;
    uint8_t                     x;
    uint8_t                     y;
    uint8_t                     sp;
    uint8_t                     status;
    uint8_t                     opcode;
    uint8_t                     data_out;
    uint8_t                     interrupt_status;
    uint16_t                    interrupt_vector;
    bool                        rw;
    bool                        sync;
    bool                        drive_data;
    Phase                       phase;
    PendingOperation            pending;
    IndexRegister               index_register;
    BranchCondition             branch;
    bool                        indirect_add_y;
    bool                        indexed_indirect;
    size_t                      last_tick;
    bool                        reset_seen_high;

    void                        ResetCore(void);
    void                        Tick(void);
    void                        ExecuteOpcode(uint8_t op);
    void                        ExecuteImplied(uint8_t op);
    void                        CompleteImmediate(uint8_t value);
    void                        CompleteZeroPageAddress(uint8_t value);
    void                        CompleteZeroPageIndexedAddress(uint8_t value);
    void                        CompleteZeroPagePointerLow(uint8_t value);
    void                        CompleteZeroPagePointerHigh(uint8_t value);
    void                        CompleteZeroPageRead(uint8_t value);
    void                        CompleteZeroPageWrite(void);
    void                        CompleteAbsoluteHigh(uint8_t value);
    void                        CompleteAbsoluteRead(uint8_t value);
    void                        CompleteAbsoluteWrite(void);
    void                        CompleteReadModify(uint8_t value);
    void                        CompleteModifyWrite(void);
    void                        CompleteIndirectLow(uint8_t value);
    void                        CompleteIndirectHigh(uint8_t value);
    void                        CompleteRelative(uint8_t value);
    void                        CompleteStackPush(void);
    void                        CompleteStackPull(uint8_t value);
    void                        CompleteJsrPushHigh(void);
    void                        CompleteJsrPushLow(void);
    void                        CompleteRtsPullLow(uint8_t value);
    void                        CompleteRtsPullHigh(uint8_t value);
    void                        BeginInterrupt(uint16_t vector, bool software);
    bool                        ExternalInterruptPending(void);
    void                        CompleteInterruptPushPch(void);
    void                        CompleteInterruptPushPcl(void);
    void                        CompleteInterruptPushStatus(void);
    void                        CompleteInterruptVectorLow(uint8_t value);
    void                        CompleteInterruptVectorHigh(uint8_t value);
    void                        BeginRti(void);
    void                        CompleteRtiStatus(uint8_t value);
    void                        CompleteRtiLow(uint8_t value);
    void                        CompleteRtiHigh(uint8_t value);
    void                        PrepareFetch(void);
    void                        BeginZeroPage(PendingOperation op);
    void                        BeginZeroPageIndexed(PendingOperation op, IndexRegister idx);
    void                        BeginIndexedIndirect(PendingOperation op);
    void                        BeginIndirectIndexed(PendingOperation op);
    void                        BeginAbsolute(PendingOperation op);
    void                        BeginAbsoluteIndexed(PendingOperation op, IndexRegister idx);
    void                        BeginReadModifyZeroPage(PendingOperation op, IndexRegister idx);
    void                        BeginReadModifyAbsolute(PendingOperation op, IndexRegister idx);
    void                        BeginIndirectJump(void);
    void                        BeginImmediate(PendingOperation op);
    void                        BeginRelative(BranchCondition cond);
    void                        BeginStackPush(PendingOperation op, uint8_t value);
    void                        BeginStackPull(PendingOperation op);
    void                        ApplyReadOperation(uint8_t value);
    uint8_t                     ApplyModifyOperation(uint8_t value);
    void                        ExecuteAccumulator(PendingOperation op);
    bool                        IsWriteOperation(void) const;
    bool                        IsModifyOperation(void) const;
    uint8_t                     IndexValue(void) const;
    void                        ContinueWithEffectiveAddress(void);
    void                        SetNZ(uint8_t value);
    void                        SetFlag(uint8_t mask, bool value);
    bool                        GetFlag(uint8_t mask) const;
    void                        AddWithCarry(uint8_t value);
    void                        SubWithCarry(uint8_t value);
    void                        Compare(uint8_t reg, uint8_t value);
    bool                        BranchTaken(void) const;
    uint8_t                     ReadDataBus(void);
    hbs::Tristate               DataBusBit(size_t pin) const;
    hbs::Tristate               AddressBusBit(size_t pin) const;
    static bool                 IsAddressPin(size_t pin);
    static bool                 IsDataPin(size_t pin);

  public:
    static bool                 TypeMatches(const std::string &type);
    static hbs::IComponent      *Create(hbs::Timer        &timer,
                                        const std::string &type,
                                        const std::string &name,
                                        const std::string &value,
                                        const std::string &position);
    virtual std::string         GetDisplayType(void) const;
    virtual std::string         GetDefinitionValue(void) const;
    virtual bool                DrivesPin(size_t          n) const;

  public:
    virtual hbs::Tristate       Compute(size_t            n);

    CMP_6502(const hbs::Timer   &timer,
             const std::string  &name,
             const std::string  &pos);
    virtual ~CMP_6502(void);
  };
}

#endif //                       __HBS_6502_HPP__
