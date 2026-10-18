#include "W65C02.hpp"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using hbs::W65C02;
static unsigned checks;
#define CHECK(x) do { ++checks; if (!(x)) throw std::runtime_error(std::string(__func__) + ": " #x); } while (0)

struct Machine
{
  W65C02 cpu;
  std::array<uint8_t,65536> ram{};
  void Cycle(bool ready = true)
  {
    auto b = cpu.GetBus();
    // A memory device may observe a held write more than once under RDY.
    if (!b.read) ram[b.address] = b.data;
    cpu.Clock(ram[b.address], ready);
  }
  unsigned Instruction()
  {
    unsigned cycles = 0;
    do { Cycle(); ++cycles; } while (!cpu.GetBus().sync && cycles < 20);
    return cycles;
  }
  void Start(uint16_t pc = 0x200, uint8_t p = W65C02::U)
  { W65C02::Registers r; r.pc=pc; r.s=0xff; r.p=p; cpu.SetRegisters(r); }
};

static void ResetAndControl()
{
  Machine m;
  W65C02::Registers r;
  r.pc=0x1234; r.s=1; r.a=0x19; r.x=0x83; r.y=0x42; r.p=0xff;
  m.cpu.SetRegisters(r);
  m.ram[0xfffc]=0x00; m.ram[0xfffd]=0x80;
  m.cpu.Reset();
  const uint16_t address[] = {0x1234,0x1234,0x101,0x100,0x1ff,0xfffc,0xfffd};
  for (unsigned i=0;i<7;++i)
    {
      auto b=m.cpu.GetBus();
      CHECK(b.address==address[i] && b.read && b.sync==(i==0) && b.vector==(i>=5));
      m.Cycle();
    }
  auto after=m.cpu.GetRegisters();
  CHECK(after.pc==0x8000 && after.s==0xfe && after.a==r.a && after.x==r.x && after.y==r.y);
  CHECK((after.p & (W65C02::I|W65C02::D))==W65C02::I);
  CHECK(m.cpu.GetBus().sync);

  // RDY freezes read AND write cycles without losing a latched NMI.
  m.Start(); m.ram[0x200]=0x85; m.ram[0x201]=0x12;
  m.Cycle(); m.Cycle();
  CHECK(!m.cpu.GetBus().read);
  auto held=m.cpu.GetBus();
  m.Cycle(false); m.Cycle(false);
  CHECK(m.cpu.GetBus().address==held.address && !m.cpu.GetBus().read);
  m.cpu.SetInterrupts(false,true); m.cpu.SetInterrupts(false,false);
  m.Cycle(); m.Cycle();
  CHECK(!m.cpu.GetBus().sync);
  m.ram[0xfffa]=0; m.ram[0xfffb]=0x90;
  for(unsigned i=0;i<6;++i) m.Cycle();
  CHECK(m.cpu.GetRegisters().pc==0x9000 && m.cpu.GetBus().sync);

  for (bool masked : {false,true})
    {
      m.Start(0x200,W65C02::U|(masked?W65C02::I:0));
      m.ram[0x200]=0xcb;
      m.Cycle(); m.Cycle(); m.Cycle();
      CHECK(m.cpu.IsWaiting() && !m.cpu.GetBus().sync);
      for (unsigned i=0;i<10;++i) m.Cycle();
      CHECK(m.cpu.IsWaiting());
      m.cpu.SetInterrupts(true,false); m.Cycle();
      CHECK(!m.cpu.IsWaiting() && m.cpu.GetBus().sync);
      m.ram[0x201]=0xea; m.Cycle();
      CHECK(m.cpu.GetRegisters().pc==(masked?0x202:0x201));
    }
  m.Start(); m.ram[0x200]=0xdb;
  m.Cycle(); m.Cycle(); m.Cycle();
  CHECK(m.cpu.IsStopped());
  m.cpu.SetInterrupts(true,true); m.Cycle();
  CHECK(m.cpu.IsStopped() && m.cpu.GetRegisters().pc==0x201);
  m.cpu.Reset(); CHECK(!m.cpu.IsStopped());

  // The reserved $5C is eight cycles on WDC silicon (the SST corpus has four).
  m.Start(); m.ram[0x200]=0x5c; m.ram[0x201]=0x34; m.ram[0x202]=0x12;
  const uint16_t nop[] = {0x200,0x201,0x202,0xff34,0xffff,0xffff,0xffff,0xffff};
  for (auto a:nop) { CHECK(m.cpu.GetBus().address==a); m.Cycle(); }
  CHECK(m.cpu.GetBus().sync && m.cpu.GetRegisters().pc==0x203);
}

static void Interrupts()
{
  Machine m;
  m.Start(0x200, W65C02::U|W65C02::D|W65C02::C);
  m.ram[0x200]=0xea; m.ram[0xfffe]=0; m.ram[0xffff]=0x80;
  m.cpu.SetInterrupts(true,false);
  CHECK(m.Instruction()==2);
  const uint16_t addresses[]={0x201,0x201,0x1ff,0x1fe,0x1fd,0xfffe,0xffff};
  for(unsigned i=0;i<7;++i)
    {
      auto b=m.cpu.GetBus();
      CHECK(b.address==addresses[i] && b.read==(i<2||i>=5));
      CHECK(b.vector==(i>=5));
      m.Cycle();
    }
  CHECK(m.ram[0x1ff]==2 && m.ram[0x1fe]==1 && m.ram[0x1fd]==(W65C02::U|W65C02::D|W65C02::C));
  CHECK(m.cpu.GetRegisters().pc==0x8000 && (m.cpu.GetRegisters().p&W65C02::D)==0);
  m.cpu.SetInterrupts(false,false);
  m.ram[0x8000]=0x40;
  CHECK(m.Instruction()==6);
  CHECK(m.cpu.GetRegisters().pc==0x201 && (m.cpu.GetRegisters().p&W65C02::D));

  // NMI during BRK must not replace BRK's vector on CMOS; it follows BRK.
  m.Start(); m.ram[0x200]=0; m.ram[0xfffa]=0; m.ram[0xfffb]=0x90;
  m.Cycle(); m.Cycle(); m.cpu.SetInterrupts(false,true);
  for(unsigned i=0;i<5;++i) m.Cycle();
  CHECK(m.cpu.GetRegisters().pc==0x8000);
  for(unsigned i=0;i<7;++i) m.Cycle();
  CHECK(m.cpu.GetRegisters().pc==0x9000);
  m.ram[0x9000]=0xea;
  CHECK(m.Instruction()==2); // held-low NMI is not a second edge
  CHECK(m.cpu.GetRegisters().pc==0x9001);

  // CLI/SEI/PLP poll the previous I value; RTI uses the restored value.
  m.Start(0x200,W65C02::U|W65C02::I);
  m.ram[0x200]=0x58; m.ram[0x201]=0xea; m.ram[0x202]=0xea;
  m.cpu.SetInterrupts(true,false);
  CHECK(m.Instruction()==2 && m.cpu.GetRegisters().pc==0x201);
  CHECK(m.Instruction()==2 && m.cpu.GetRegisters().pc==0x202);
  m.Cycle(); CHECK(!m.cpu.GetBus().sync && m.cpu.GetRegisters().pc==0x202);
  m.Start(); m.ram[0x200]=0x78;
  m.cpu.SetInterrupts(true,false);
  CHECK(m.Instruction()==2 && (m.cpu.GetRegisters().p&W65C02::I));
  m.Cycle(); CHECK(!m.cpu.GetBus().sync && m.cpu.GetRegisters().pc==0x201);
  m.Start(0x200,W65C02::U|W65C02::I);
  m.ram[0x200]=0x28; m.ram[0x100]=W65C02::U;
  m.cpu.SetInterrupts(true,false);
  CHECK(m.Instruction()==4 && !(m.cpu.GetRegisters().p&W65C02::I));
  CHECK(m.Instruction()==2 && m.cpu.GetRegisters().pc==0x202);
}

static void BusDetails()
{
  Machine m;
  // MLB covers only modify/write on the usual RMW instructions, but all
  // three memory cycles on RMB/SMB (measured WDC behaviour).
  for(unsigned op : {0x06,0x26,0x46,0x66,0xc6,0xe6,0x04,0x14,0x07,0x87})
    {
      m.Start(); m.ram[0x200]=uint8_t(op); m.ram[0x201]=0x10;
      for(unsigned c=0;c<5;++c)
        {
          CHECK(m.cpu.GetBus().locked==(c>=3 || (c==2 && (op==0x07||op==0x87))));
          CHECK(m.cpu.GetBus().read==(c!=4));
          m.Cycle();
        }
      CHECK(!m.cpu.GetBus().locked && m.cpu.GetBus().sync);
    }
  // Zero-page index wraps, and its dummy cycle reads the operand, not I/O.
  m.Start();
  auto r=m.cpu.GetRegisters(); r.x=0x10; m.cpu.SetRegisters(r);
  m.ram[0x200]=0xb5; m.ram[0x201]=0xf8; m.ram[0x08]=0x9a;
  const uint16_t zp[]={0x200,0x201,0x201,0x08};
  for(auto a:zp) { CHECK(m.cpu.GetBus().address==a); m.Cycle(); }
  CHECK(m.cpu.GetRegisters().a==0x9a);
  // The indirect jump pointer really crosses the page; its extra read is
  // the last operand byte, BEFORE the two pointer reads.
  m.Start(); m.ram[0x200]=0x6c; m.ram[0x201]=0xff; m.ram[0x202]=0x12;
  m.ram[0x12ff]=0x78; m.ram[0x1300]=0x56; m.ram[0x1200]=0xba;
  const uint16_t jmp[]={0x200,0x201,0x202,0x202,0x12ff,0x1300};
  for(auto a:jmp) { CHECK(m.cpu.GetBus().address==a); m.Cycle(); }
  CHECK(m.cpu.GetRegisters().pc==0x5678);
  for(unsigned op:{0xdc,0xfc})
    {
      m.Start(); m.ram[0x200]=uint8_t(op); m.ram[0x201]=0x34; m.ram[0x202]=0x12;
      const uint16_t addresses[]={0x200,0x201,0x202,0x1234};
      for(auto a:addresses) { CHECK(m.cpu.GetBus().address==a); m.Cycle(); }
      CHECK(m.cpu.GetRegisters().pc==0x203);
    }
}

static void Arithmetic()
{
  Machine m;
  for(unsigned op:{0x69,0xe9})
    for(unsigned a=0;a<256;++a)
      for(unsigned b=0;b<256;++b)
        for(unsigned carry=0;carry<2;++carry)
          {
            W65C02::Registers r; r.pc=0x200; r.a=uint8_t(a); r.p=W65C02::U|carry;
            m.cpu.SetRegisters(r); m.ram[0x200]=uint8_t(op); m.ram[0x201]=uint8_t(b);
            CHECK(m.Instruction()==2);
            int result=op==0x69?int(a+b+carry):int(a)-int(b)-int(!carry);
            int sa=a<128?int(a):int(a)-256, sb=b<128?int(b):int(b)-256;
            int signed_result=op==0x69?sa+sb+int(carry):sa-sb-int(!carry);
            auto f=m.cpu.GetRegisters();
            unsigned flags=(uint8_t(result)==0?W65C02::Z:0)|(uint8_t(result)&W65C02::N)|
              ((op==0x69?result>255:result>=0)?W65C02::C:0)|
              (signed_result< -128||signed_result>127?W65C02::V:0);
            CHECK(f.a==uint8_t(result) && f.p==(flags|W65C02::U));
          }
  // An independent decimal-integer oracle checks every valid packed BCD pair.
  for(unsigned op:{0x69,0xe9})
    for(unsigned a=0;a<100;++a)
      for(unsigned b=0;b<100;++b)
        for(unsigned carry=0;carry<2;++carry)
          {
            W65C02::Registers r; r.pc=0x200; r.a=uint8_t(a/10*16+a%10);
            r.p=W65C02::U|W65C02::D|carry; m.cpu.SetRegisters(r);
            m.ram[0x200]=uint8_t(op); m.ram[0x201]=uint8_t(b/10*16+b%10);
            CHECK(m.Instruction()==3);
            int n=op==0x69?int(a+b+carry):int(a)-int(b)-int(!carry);
            bool c=op==0x69?n>=100:n>=0;
            unsigned decimal=unsigned((n+100)%100), expected=decimal/10*16+decimal%10;
            auto f=m.cpu.GetRegisters();
            CHECK(f.a==expected && bool(f.p&W65C02::C)==c && bool(f.p&W65C02::Z)==(expected==0));
            CHECK((f.p&W65C02::N)==(expected&128) && (f.p&W65C02::D));
          }
}

static uint8_t Byte(std::istream &in)
{
  int v=in.get();
  if(v==EOF) throw std::runtime_error("Truncated test vector");
  return uint8_t(v);
}
static uint16_t Word(std::istream &in) { uint16_t lo=Byte(in); return lo|(uint16_t(Byte(in))<<8); }
static W65C02::Registers Registers(std::istream &in)
{
  W65C02::Registers r;
  r.pc=Word(in); r.s=Byte(in); r.a=Byte(in); r.x=Byte(in); r.y=Byte(in); r.p=Byte(in);
  return r;
}
static std::vector<std::pair<uint16_t,uint8_t>> Memory(std::istream &in)
{
  std::vector<std::pair<uint16_t,uint8_t>> ram;
  unsigned n=Word(in);
  while(n--) { auto a=Word(in); auto v=Byte(in); ram.emplace_back(a,v); }
  return ram;
}
static bool Same(const W65C02::Registers &a,const W65C02::Registers &b)
{ return a.pc==b.pc && a.s==b.s && a.a==b.a && a.x==b.x && a.y==b.y && a.p==b.p; }

static void Corpus(const char *path)
{
  std::ifstream in(path,std::ios::binary);
  if(!in) throw std::runtime_error("Cannot read corpus");
  unsigned passed=0, failed=0;
  std::array<unsigned,256> failures{};
  while(in.peek()!=EOF)
    {
      std::string name;
      for(unsigned n=Byte(in); n; --n) name+=char(Byte(in));
      Machine m;
      m.cpu.SetRegisters(Registers(in));
      for(auto [a,v]:Memory(in)) m.ram[a]=v;
      bool ok=true;
      std::string reason;
      unsigned op=m.ram[m.cpu.GetRegisters().pc];
      unsigned n=Byte(in);
      for(unsigned c=0;c<n;++c)
        {
          uint16_t a=Word(in); uint8_t d=Byte(in); bool rd=Byte(in)!=0;
          auto b=m.cpu.GetBus();
          if (b.address!=a || b.read!=rd || (!rd && b.data!=d) || (rd && m.ram[b.address]!=d))
            {
              if(ok) reason="cycle "+std::to_string(c)+" address="+std::to_string(b.address)+" expected="+std::to_string(a);
              ok=false;
            }
          m.Cycle();
        }
      auto final=Registers(in);
      if(!Same(final,m.cpu.GetRegisters()))
        {
          auto r=m.cpu.GetRegisters();
          if(ok) reason="registers: PC "+std::to_string(r.pc)+"/"+std::to_string(final.pc)+" A "+std::to_string(r.a)+"/"+std::to_string(final.a)+" P "+std::to_string(r.p)+"/"+std::to_string(final.p);
          ok=false;
        }
      for(auto [a,v]:Memory(in)) if(m.ram[a]!=v) {if(ok)reason="memory"; ok=false;}
      if(!m.cpu.GetBus().sync) { if(ok)reason="not at instruction boundary"; ok=false; }
      if(ok) ++passed;
      else
        {
          ++failed;
          if(failures[op]++<2) std::cerr<<name<<": "<<reason<<'\n';
        }
    }
  std::cout<<"W65C02S bus vectors: "<<passed<<" passed, "<<failed<<" failed\n";
  CHECK(passed>0 && failed==0);
}

static void Klaus(const char *path, uint16_t success)
{
  Machine m;
  std::ifstream in(path,std::ios::binary);
  in.read(reinterpret_cast<char *>(m.ram.data()),m.ram.size());
  CHECK(in.gcount()==65536);
  m.Start(0x400);
  uint16_t previous=0;
  uint64_t cycles=0, instructions=0;
  while(cycles<500000000)
    {
      uint16_t pc=m.cpu.GetRegisters().pc;
      if(pc==previous)
        {
          std::cout<<path<<": loop at $"<<std::hex<<pc<<std::dec<<", "<<instructions<<" instructions, "<<cycles<<" cycles\n";
          CHECK(pc==success); return;
        }
      previous=pc;
      cycles+=m.Instruction(); ++instructions;
    }
  throw std::runtime_error("Klaus test timeout");
}

int main(int argc,char **argv)
{
  try
    {
      if(argc==3 && std::string(argv[1])=="--corpus") Corpus(argv[2]);
      else if(argc==4 && std::string(argv[1])=="--klaus") Klaus(argv[2],uint16_t(std::strtoul(argv[3],nullptr,16)));
      else { ResetAndControl(); Interrupts(); BusDetails(); Arithmetic(); std::cout<<"W65C02S controls/arithmetic: "<<checks<<" checks passed\n"; }
      return 0;
    }
  catch(const std::exception &e) { std::cerr<<e.what()<<'\n'; return 1; }
}
