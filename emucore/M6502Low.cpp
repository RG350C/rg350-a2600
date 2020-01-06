//============================================================================
//
// MM     MM  6666  555555  0000   2222
// MMMM MMMM 66  66 55     00  00 22  22
// MM MMM MM 66     55     00  00     22
// MM  M  MM 66666  55555  00  00  22222  --  "A 6502 Microprocessor Emulator"
// MM     MM 66  66     55 00  00 22
// MM     MM 66  66 55  55 00  00 22
// MM     MM  6666   5555   0000  222222
//
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: M6502Low.cxx,v 1.8 2006/02/05 02:49:47 stephena Exp $
//============================================================================

#include "M6502Low.hxx"
#include "Serializer.hxx"
#include "Deserializer.hxx"

#ifdef DEVELOPER_SUPPORT
  #include "Debugger.hxx"
#endif

#define debugStream cout

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502Low::M6502Low(uInt32 systemCyclesPerProcessorCycle)
    : M6502(systemCyclesPerProcessorCycle)
{
#ifdef DEVELOPER_SUPPORT
  myJustHitTrapFlag = false;
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502Low::~M6502Low()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline uInt8 M6502Low::peek(uInt16 address)
{
#ifdef DEVELOPER_SUPPORT
  if(myReadTraps != NULL && myReadTraps->isSet(address))
  {
    myJustHitTrapFlag = true;
    myHitTrapInfo.message = "Read trap: ";
    myHitTrapInfo.address = address;
  }
#endif

  return mySystem->peek(address);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline void M6502Low::poke(uInt16 address, uInt8 value)
{
#ifdef DEVELOPER_SUPPORT
  if(myWriteTraps != NULL && myWriteTraps->isSet(address))
  {
    myJustHitTrapFlag = true;
    myHitTrapInfo.message = "Write trap: ";
    myHitTrapInfo.address = address;
  }
#endif

  mySystem->poke(address, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6502Low::execute(uInt32 number)
{
  __label__ 
lab_0x00, lab_0x01, lab_0x02, lab_0x03, lab_0x04, lab_0x05, lab_0x06, lab_0x07,
lab_0x08, lab_0x09, lab_0x0a, lab_0x0b, lab_0x0c, lab_0x0d, lab_0x0e, lab_0x0f,
lab_0x10, lab_0x11, lab_0x12, lab_0x13, lab_0x14, lab_0x15, lab_0x16, lab_0x17,
lab_0x18, lab_0x19, lab_0x1a, lab_0x1b, lab_0x1c, lab_0x1d, lab_0x1e, lab_0x1f,
lab_0x20, lab_0x21, lab_0x22, lab_0x23, lab_0x24, lab_0x25, lab_0x26, lab_0x27,
lab_0x28, lab_0x29, lab_0x2a, lab_0x2b, lab_0x2c, lab_0x2d, lab_0x2e, lab_0x2f,
lab_0x30, lab_0x31, lab_0x32, lab_0x33, lab_0x34, lab_0x35, lab_0x36, lab_0x37,
lab_0x38, lab_0x39, lab_0x3a, lab_0x3b, lab_0x3c, lab_0x3d, lab_0x3e, lab_0x3f,
lab_0x40, lab_0x41, lab_0x42, lab_0x43, lab_0x44, lab_0x45, lab_0x46, lab_0x47,
lab_0x48, lab_0x49, lab_0x4a, lab_0x4b, lab_0x4c, lab_0x4d, lab_0x4e, lab_0x4f,
lab_0x50, lab_0x51, lab_0x52, lab_0x53, lab_0x54, lab_0x55, lab_0x56, lab_0x57,
lab_0x58, lab_0x59, lab_0x5a, lab_0x5b, lab_0x5c, lab_0x5d, lab_0x5e, lab_0x5f,
lab_0x60, lab_0x61, lab_0x62, lab_0x63, lab_0x64, lab_0x65, lab_0x66, lab_0x67,
lab_0x68, lab_0x69, lab_0x6a, lab_0x6b, lab_0x6c, lab_0x6d, lab_0x6e, lab_0x6f,
lab_0x70, lab_0x71, lab_0x72, lab_0x73, lab_0x74, lab_0x75, lab_0x76, lab_0x77,
lab_0x78, lab_0x79, lab_0x7a, lab_0x7b, lab_0x7c, lab_0x7d, lab_0x7e, lab_0x7f,
lab_0x80, lab_0x81, lab_0x82, lab_0x83, lab_0x84, lab_0x85, lab_0x86, lab_0x87,
lab_0x88, lab_0x89, lab_0x8a, lab_0x8b, lab_0x8c, lab_0x8d, lab_0x8e, lab_0x8f,
lab_0x90, lab_0x91, lab_0x92, lab_0x93, lab_0x94, lab_0x95, lab_0x96, lab_0x97,
lab_0x98, lab_0x99, lab_0x9a, lab_0x9b, lab_0x9c, lab_0x9d, lab_0x9e, lab_0x9f,
lab_0xa0, lab_0xa1, lab_0xa2, lab_0xa3, lab_0xa4, lab_0xa5, lab_0xa6, lab_0xa7,
lab_0xa8, lab_0xa9, lab_0xaa, lab_0xab, lab_0xac, lab_0xad, lab_0xae, lab_0xaf,
lab_0xb0, lab_0xb1, lab_0xb2, lab_0xb3, lab_0xb4, lab_0xb5, lab_0xb6, lab_0xb7,
lab_0xb8, lab_0xb9, lab_0xba, lab_0xbb, lab_0xbc, lab_0xbd, lab_0xbe, lab_0xbf,
lab_0xc0, lab_0xc1, lab_0xc2, lab_0xc3, lab_0xc4, lab_0xc5, lab_0xc6, lab_0xc7,
lab_0xc8, lab_0xc9, lab_0xca, lab_0xcb, lab_0xcc, lab_0xcd, lab_0xce, lab_0xcf,
lab_0xd0, lab_0xd1, lab_0xd2, lab_0xd3, lab_0xd4, lab_0xd5, lab_0xd6, lab_0xd7,
lab_0xd8, lab_0xd9, lab_0xda, lab_0xdb, lab_0xdc, lab_0xdd, lab_0xde, lab_0xdf,
lab_0xe0, lab_0xe1, lab_0xe2, lab_0xe3, lab_0xe4, lab_0xe5, lab_0xe6, lab_0xe7,
lab_0xe8, lab_0xe9, lab_0xea, lab_0xeb, lab_0xec, lab_0xed, lab_0xee, lab_0xef,
lab_0xf0, lab_0xf1, lab_0xf2, lab_0xf3, lab_0xf4, lab_0xf5, lab_0xf6, lab_0xf7,
lab_0xf8, lab_0xf9, lab_0xfa, lab_0xfb, lab_0xfc, lab_0xfd, lab_0xfe, lab_0xff;
    static const void* const a_jump_table[256] = { &&
lab_0x00, && lab_0x01, && lab_0x02, && lab_0x03, && lab_0x04, && lab_0x05, && lab_0x06, && lab_0x07, &&
lab_0x08, && lab_0x09, && lab_0x0a, && lab_0x0b, && lab_0x0c, && lab_0x0d, && lab_0x0e, && lab_0x0f, &&
lab_0x10, && lab_0x11, && lab_0x12, && lab_0x13, && lab_0x14, && lab_0x15, && lab_0x16, && lab_0x17, &&
lab_0x18, && lab_0x19, && lab_0x1a, && lab_0x1b, && lab_0x1c, && lab_0x1d, && lab_0x1e, && lab_0x1f, &&
lab_0x20, && lab_0x21, && lab_0x22, && lab_0x23, && lab_0x24, && lab_0x25, && lab_0x26, && lab_0x27, &&
lab_0x28, && lab_0x29, && lab_0x2a, && lab_0x2b, && lab_0x2c, && lab_0x2d, && lab_0x2e, && lab_0x2f, &&
lab_0x30, && lab_0x31, && lab_0x32, && lab_0x33, && lab_0x34, && lab_0x35, && lab_0x36, && lab_0x37, &&
lab_0x38, && lab_0x39, && lab_0x3a, && lab_0x3b, && lab_0x3c, && lab_0x3d, && lab_0x3e, && lab_0x3f, &&
lab_0x40, && lab_0x41, && lab_0x42, && lab_0x43, && lab_0x44, && lab_0x45, && lab_0x46, && lab_0x47, &&
lab_0x48, && lab_0x49, && lab_0x4a, && lab_0x4b, && lab_0x4c, && lab_0x4d, && lab_0x4e, && lab_0x4f, &&
lab_0x50, && lab_0x51, && lab_0x52, && lab_0x53, && lab_0x54, && lab_0x55, && lab_0x56, && lab_0x57, &&
lab_0x58, && lab_0x59, && lab_0x5a, && lab_0x5b, && lab_0x5c, && lab_0x5d, && lab_0x5e, && lab_0x5f, &&
lab_0x60, && lab_0x61, && lab_0x62, && lab_0x63, && lab_0x64, && lab_0x65, && lab_0x66, && lab_0x67, &&
lab_0x68, && lab_0x69, && lab_0x6a, && lab_0x6b, && lab_0x6c, && lab_0x6d, && lab_0x6e, && lab_0x6f, &&
lab_0x70, && lab_0x71, && lab_0x72, && lab_0x73, && lab_0x74, && lab_0x75, && lab_0x76, && lab_0x77, &&
lab_0x78, && lab_0x79, && lab_0x7a, && lab_0x7b, && lab_0x7c, && lab_0x7d, && lab_0x7e, && lab_0x7f, &&
lab_0x80, && lab_0x81, && lab_0x82, && lab_0x83, && lab_0x84, && lab_0x85, && lab_0x86, && lab_0x87, &&
lab_0x88, && lab_0x89, && lab_0x8a, && lab_0x8b, && lab_0x8c, && lab_0x8d, && lab_0x8e, && lab_0x8f, &&
lab_0x90, && lab_0x91, && lab_0x92, && lab_0x93, && lab_0x94, && lab_0x95, && lab_0x96, && lab_0x97, &&
lab_0x98, && lab_0x99, && lab_0x9a, && lab_0x9b, && lab_0x9c, && lab_0x9d, && lab_0x9e, && lab_0x9f, &&
lab_0xa0, && lab_0xa1, && lab_0xa2, && lab_0xa3, && lab_0xa4, && lab_0xa5, && lab_0xa6, && lab_0xa7, &&
lab_0xa8, && lab_0xa9, && lab_0xaa, && lab_0xab, && lab_0xac, && lab_0xad, && lab_0xae, && lab_0xaf, &&
lab_0xb0, && lab_0xb1, && lab_0xb2, && lab_0xb3, && lab_0xb4, && lab_0xb5, && lab_0xb6, && lab_0xb7, &&
lab_0xb8, && lab_0xb9, && lab_0xba, && lab_0xbb, && lab_0xbc, && lab_0xbd, && lab_0xbe, && lab_0xbf, &&
lab_0xc0, && lab_0xc1, && lab_0xc2, && lab_0xc3, && lab_0xc4, && lab_0xc5, && lab_0xc6, && lab_0xc7, &&
lab_0xc8, && lab_0xc9, && lab_0xca, && lab_0xcb, && lab_0xcc, && lab_0xcd, && lab_0xce, && lab_0xcf, &&
lab_0xd0, && lab_0xd1, && lab_0xd2, && lab_0xd3, && lab_0xd4, && lab_0xd5, && lab_0xd6, && lab_0xd7, &&
lab_0xd8, && lab_0xd9, && lab_0xda, && lab_0xdb, && lab_0xdc, && lab_0xdd, && lab_0xde, && lab_0xdf, &&
lab_0xe0, && lab_0xe1, && lab_0xe2, && lab_0xe3, && lab_0xe4, && lab_0xe5, && lab_0xe6, && lab_0xe7, &&
lab_0xe8, && lab_0xe9, && lab_0xea, && lab_0xeb, && lab_0xec, && lab_0xed, && lab_0xee, && lab_0xef, &&
lab_0xf0, && lab_0xf1, && lab_0xf2, && lab_0xf3, && lab_0xf4, && lab_0xf5, && lab_0xf6, && lab_0xf7, &&
lab_0xf8, && lab_0xf9, && lab_0xfa, && lab_0xfb, && lab_0xfc, && lab_0xfd, && lab_0xfe, && lab_0xff
    };

  // Clear all of the execution status bits except for the fatal error bit
  myExecutionStatus &= FatalErrorBit;

  // Loop until execution is stopped or a fatal error occurs
  for(;;)
  {
    uInt16 operandAddress = 0;
    uInt8 operand = 0;

    goto begin;

end:
      --number;
begin:
      if (myExecutionStatus || (number <= 0) ) goto final;

#ifdef DEVELOPER_SUPPORT
      if(myJustHitTrapFlag)
      {
        if(myDebugger->start(myHitTrapInfo.message, myHitTrapInfo.address))
        {
          myJustHitTrapFlag = false;
          return true;
        }
      }

      if(myBreakPoints != NULL)
      {
        if(myBreakPoints->isSet(PC))
        {
          if(myDebugger->start("Breakpoint hit: ", PC))
            return true;
        }
      }

      int cond = evalCondBreaks();
      if(cond > -1)
      {
        string buf = "CBP: " + myBreakCondNames[cond];
        if(myDebugger->start(buf))
          return true;
      }
#endif

#ifdef DEBUG
      debugStream << "PC=" << hex << setw(4) << PC << " ";
#endif

      // Fetch instruction at the program counter
      IR = peek(PC++);

#ifdef DEBUG
      debugStream << "IR=" << hex << setw(2) << (int)IR << " ";
      debugStream << "<" << ourAddressingModeTable[IR] << " ";
#endif

      // Update system cycles
      mySystem->incrementCycles(myInstructionSystemCycleTable[IR]); 

      // Call code to execute the instruction
	    goto *a_jump_table[IR];
      // 6502 instruction emulation is generated by an M4 macro file
      #include "M6502Low.ins"

final:

    // See if we need to handle an interrupt
    if((myExecutionStatus & MaskableInterruptBit) || 
        (myExecutionStatus & NonmaskableInterruptBit))
    {
      // Yes, so handle the interrupt
      interruptHandler();
    }

    // See if execution has been stopped
    if(myExecutionStatus & StopExecutionBit)
    {
      // Yes, so answer that everything finished fine
      return true;
    }

    // See if a fatal error has occured
    if(myExecutionStatus & FatalErrorBit)
    {
      // Yes, so answer that something when wrong
      return false;
    }

    // See if we've executed the specified number of instructions
    if(number == 0)
    {
      // Yes, so answer that everything finished fine
      return true;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502Low::interruptHandler()
{
  // Handle the interrupt
  if((myExecutionStatus & MaskableInterruptBit) && !I)
  {
    mySystem->incrementCycles(7 * mySystemCyclesPerProcessorCycle);
    mySystem->poke(0x0100 + SP--, (PC - 1) >> 8);
    mySystem->poke(0x0100 + SP--, (PC - 1) & 0x00ff);
    mySystem->poke(0x0100 + SP--, PS() & (~0x10));
    D = false;
    I = true;
    PC = (uInt16)mySystem->peek(0xFFFE) | ((uInt16)mySystem->peek(0xFFFF) << 8);
  }
  else if(myExecutionStatus & NonmaskableInterruptBit)
  {
    mySystem->incrementCycles(7 * mySystemCyclesPerProcessorCycle);
    mySystem->poke(0x0100 + SP--, (PC - 1) >> 8);
    mySystem->poke(0x0100 + SP--, (PC - 1) & 0x00ff);
    mySystem->poke(0x0100 + SP--, PS() & (~0x10));
    D = false;
    PC = (uInt16)mySystem->peek(0xFFFA) | ((uInt16)mySystem->peek(0xFFFB) << 8);
  }

  // Clear the interrupt bits in myExecutionStatus
  myExecutionStatus &= ~(MaskableInterruptBit | NonmaskableInterruptBit);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6502Low::save(Serializer& out)
{
  string CPU = name();

  try
  {
    out.putString(CPU);

    out.putInt(A);    // Accumulator
    out.putInt(X);    // X index register
    out.putInt(Y);    // Y index register
    out.putInt(SP);   // Stack Pointer
    out.putInt(IR);   // Instruction register
    out.putInt(PC);   // Program Counter

    out.putBool(N);     // N flag for processor status register
    out.putBool(V);     // V flag for processor status register
    out.putBool(B);     // B flag for processor status register
    out.putBool(D);     // D flag for processor status register
    out.putBool(I);     // I flag for processor status register
    out.putBool(notZ);  // Z flag complement for processor status register
    out.putBool(C);     // C flag for processor status register

    out.putInt(myExecutionStatus);
  }
# if 0 //LUDO:
  catch(char *msg)
  {
    cerr << msg << endl;
    return false;
  }
# endif
  catch(...)
  {
    cerr << "Unknown error in save state for " << CPU << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6502Low::load(Deserializer& in)
{
  string CPU = name();

  try
  {
    if(in.getString() != CPU)
      return false;

    A = (uInt8) in.getInt();    // Accumulator
    X = (uInt8) in.getInt();    // X index register
    Y = (uInt8) in.getInt();    // Y index register
    SP = (uInt8) in.getInt();   // Stack Pointer
    IR = (uInt8) in.getInt();   // Instruction register
    PC = (uInt16) in.getInt();  // Program Counter

    N = in.getBool();     // N flag for processor status register
    V = in.getBool();     // V flag for processor status register
    B = in.getBool();     // B flag for processor status register
    D = in.getBool();     // D flag for processor status register
    I = in.getBool();     // I flag for processor status register
    notZ = in.getBool();  // Z flag complement for processor status register
    C = in.getBool();     // C flag for processor status register

    myExecutionStatus = (uInt8) in.getInt();
  }
# if 0 //LUDO:
  catch(char *msg)
  {
    cerr << msg << endl;
    return false;
  }
# endif
  catch(...)
  {
    cerr << "Unknown error in load state for " << CPU << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* M6502Low::name() const
{
  return "M6502Low";
}
