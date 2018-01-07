// Author: Sean Davis

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <climits>

using namespace std;

// Assumes a format of Opcode Src1Reg Src2Reg DestReg ImmValue
const int OPCODE_BITS = 5;
const int NUM_OPCODES = 14;  
const int HALT = 13;  // HALT must be last in the array for end of memory loop to work
const int MAX_REG = 7;
const int REG_BITS = 3;
const int MAX_ADDRESS = 31;
const int MAX_IMMEDIATE = 511;
const int MIN_IMMEDIATE = -512;
const int IMMEDIATE_BITS = 10;
const int IMMEDIATE_MASK = 0x3FF;  // Bits set to mask only immediate of instruction
const int ERROR = -1;


typedef enum
{IMM1, IMM2, REG0, REG1, REG2, REG3} FormatType;      
    
typedef struct
{
  const char opCodeStr[10];
  int opCode;
  FormatType format;
} InstructionFormat;


class Instruction
{
public:
  int instructionFormatIndex;
  int sourceRegA;
  int sourceRegB;
  int destReg;
  int immediateValue;
  char instructionComment[80];
  char comment[80];
  Instruction(): sourceRegA(0), sourceRegB(0), destReg(0), immediateValue(0)
    {comment[0] = '\0';}
}; // class Instruction;


const InstructionFormat instructions[NUM_OPCODES] =
{
  {"AND", 0, REG3},
  {"OR", 1, REG3},
  {"NOT", 2, REG2},
  {"ADD", 3, REG3},
  {"MOV", 4, REG2},
  {"SLL", 5, REG2},
  {"SRL", 6, REG2},
  {"SUB", 7, REG3},
  {"ADDI", 8, IMM2},
  {"SUBI", 9, IMM2},
  {"MOVI", 10, IMM1},
  {"BNEZ", 11, IMM1},
  {"NOP", 12, REG0},
  {"HALT", 31, REG0}};  // HALT must be last in the array for end of memory loop to work

void displayIntro()
{
  cout << "This program assembles a program for 154A CPU projects using mnemonics.\n";
  cout << "The assembler is not case sensitive, and white space is ignored.\n";
  cout << "Register numbers may be prefaced with 'R' or 'r', but it is not necessary.\n";
  cout << "When immediate values are prefaced with 0x they must be unsigned.\n";
  cout << "Immediate values must not be prefaced with '#' since that indicates a comment.\n";
  cout << "The legal instructions with their formats are:\n\n";
  
  for(int i = 0; i < NUM_OPCODES; i++)
  {
    cout << left << setw(4) << instructions[i].opCodeStr << ' ';
  
    switch (instructions[i].format)
    {
     
      case IMM1: cout << "RegDest,    integer"; break;
      case IMM2: cout << "RegDest, RegSourceA,  integer"; break;
      case REG0: break;
      case REG1: cout << "RegDest"; break;
      case REG2: cout << "RegDest, RegSourceA"; break;
      case REG3: cout << "RegDest, RegSourceA, RegSourceB"; break;
    } // switch
    
    cout << endl;
  } // for each instruction
  
  cout << "\nThe last instruction in your program must always be HALT.\n";
} // displayIntro()

int findOpCode(char *line)
{
  char *opcodeStr = strtok(line, " ");
  
  if(opcodeStr == NULL)
    return ERROR;
  
  for(char *ptr = opcodeStr; *ptr; ptr++)
    *ptr = toupper(*ptr);
  
  for(int i = 0; i < NUM_OPCODES; i++)
    if(strcmp(instructions[i].opCodeStr, opcodeStr) == 0)
      return i;

  return ERROR;
} // findOpCode;

int myAtoi(char *ptr)
{
  int num = 0;
  bool negative = false;
 
  if(!ptr)
    return INT_MAX;
  
  while(isspace(*ptr))
    ptr++;
  
  if(*ptr == 'r' || *ptr == 'R')
    ptr++;
  
  if(*ptr == '-')
  {
    negative = true;
    ++ptr;
  } // if starts with a minus sign
  
   
  if(strstr(ptr, "0x") == ptr) // if hexadecimal;
  {
    ptr += 2;
    if(*ptr == '\0' || isspace(*ptr))
      return INT_MAX;
    while(isxdigit(*ptr))
    {
      if(isdigit(*ptr))
        num = num * 16 + *ptr - '0';
      else
        num = num * 16 + tolower(*ptr) - 'a' + 10;
      
      ptr++;
    } // while more digits
    
    if(num & (1 << (IMMEDIATE_BITS - 1)))
      num |= (0xFFFFFFFF << IMMEDIATE_BITS);
  } // if hexadecimal
  else // decimal number
  {
    if(*ptr == '\0' || isspace(*ptr))
      return INT_MAX;
    
    while(isdigit(*ptr))
    {
      num = num * 10 + *ptr - '0';
      ptr++;
    } // while more digits
    
    if(negative)
      num = -num;
  } //else a decimal number;

  if(*ptr != '\0')
    return INT_MAX; // error
  
  return num;
}  // myAtoi()

int processImmediateOperands(int *index, Instruction program[], int count, 
  char *ptr )
{
  int immediateValue, num2, num1 = myAtoi(ptr);
  
  if(num1 < 0 || num1 > MAX_REG)
  {
    cout << "First register number out of range, or missing.\n";
    *index = -1;
    return count;
  }  // first register is wrong

  program[count].destReg = num1;  
  
  if(instructions[*index].format != IMM1)
  {
    ptr = strtok(NULL, " ,");
    num2 = myAtoi(ptr);
    if(num2 < 0 || num2 > MAX_REG)
    {
      cout << "Second register number out of range, or missing.\n";
      *index = -1;
      return count;
    } // if second register is wrong
    
    program[count].sourceRegA = num2;
  } // if more than one register
  
  ptr = strtok(NULL, " ,");
  immediateValue = myAtoi(ptr);
  ptr = strtok(NULL, " ,");
  
  if(immediateValue < MIN_IMMEDIATE || immediateValue > MAX_IMMEDIATE || ptr)
  {
    if(ptr)
      cout << "Extra characters on line.\n";
    else
      cout << "Immediate value out of range, or missing.\n";
      
    *index = -1;
    return count;
  }  // if error
    
  program[count].immediateValue = immediateValue;
  return count + 1;  // OK!
} // processImmediateOperands()


int processRegisterOperands(int *index, Instruction program[], int count, 
  char *ptr)
{
  int num3, num2, num1 = myAtoi(ptr);
  
  if(num1 < 0 || num1 > MAX_REG)
  {
    cout << "First register number out of range, or missing.\n";
    *index = -1;
    return count;
  }  // first register is wrong
  
  if(instructions[*index].format != REG1)
  {
    ptr = strtok(NULL, " ,");
    num2 = myAtoi(ptr);
    if(num2 < 0 || num2 > MAX_REG)
    {
      cout << "Second register number out of range, or missing.\n";
      *index = -1;
      return count;
    } // if second register is wrong
  } // if more than one register
  
  if(instructions[*index].format == REG3)
  {
    ptr = strtok(NULL, " ,");
    num3 = myAtoi(ptr);
    if(num3 < 0 || num3 > MAX_REG)
    {
      cout << "Third register number out of range, or missing.\n";
      *index = -1;
      return count;
    } // if second register is wrong
  } // if more than three registers

  ptr = strtok(NULL, " ,");
  
  if(ptr)
  {
    cout << "Extra characters on line.\n";
    *index = -1;
    return count;
  }  // if more characters on line
    
  program[count].destReg = num1;
  
  if(instructions[*index].format == REG2 || instructions[*index].format == REG3)
  {
     program[count].sourceRegA = num2;
     
     if(instructions[*index].format == REG3)
       program[count].sourceRegB = num3;
  } // if REG2 or REG3
    
  return count + 1;  // OK!
} // processRegisterOperands()


int processOperands(int *index, Instruction program[], int count)
{
  char *ptr = strtok(NULL, ", ");
  program[count].instructionFormatIndex = *index;
  
  if(instructions[*index].format == REG0)
  {
    if(ptr)
    {
      cout << "Invalid format for operate instruction.\n";
      *index = -1;
      return count;
    } //if something more on line
  } // if REG0 type
  else // not REG0
  {
    if(!ptr)
    {
      cout << "Missing operand(s) for this type of instruction.\n";
      *index = -1;
      return count;
    }  // if only mnemonic

    if(instructions[*index].format == IMM1 || instructions[*index].format == IMM2)
      return processImmediateOperands(index, program, count, ptr);
    else
      return processRegisterOperands(index, program, count, ptr);
  }  // else not REG0
  
  return count + 1; // if valid
}  // processOperands()


void setComment(char line[], Instruction program[], int count)
{
  char *ptr = strchr(line, '#');
  
  if(ptr)
  {
    strcpy(program[count].comment, ptr + 1);
    *ptr = '\0'; // eliminate comment from line
  }
  else  // no comment
    program[count].comment[0] = '\0';
} // setComment()


int getProgram(Instruction program[])
{
  int count = 0, index;
  char line[80];
  cout << "Enter your program one instruction per line (HALT ends it).\n";
  cout << "You may add comments by prefacing them with a '#'\n";
  cout << "The file automatically contains your mnemonic in a comment.\n\n";
  
  do
  {
    cout << setw(2) << count << ": ";
    cin.getline(line, 80, '\n');
    setComment(line, program, count);
    
    if((index = findOpCode(line)) == ERROR)
      cout << line << " is not a valid opcode mnemonic.\n";
    else    
      count = processOperands(&index, program, count);
    
    if(count == MAX_ADDRESS && index != HALT)
    {
      index = program[count++].instructionFormatIndex = HALT; 
      cout << "End of memory so last instruction will be a HALT.\n";
      break;
    }  // if at end of memory
  } while(index == ERROR || index != HALT);
  
  return count;
} // getProgram()


void writeFile(Instruction program[], int count)
{
  char filename[80], answer[80];
  int index;
  
  while(1)
  {
    cout << "Filename: ";
    cin.getline(filename, 80, '\n');
    ifstream inf(filename);
    if(inf)
    {
      inf.close();
      cout << filename << " exists.  Do you wish to overwrite it (y or n)? ";
      cin.getline(answer, 80, '\n');
      if(answer[0] == 'y')
        break;
    }
    else
      break;
  } // while(1);
  
  ofstream outf(filename);
  outf << "v2.0 raw\n";
  
  for(int i = 0; i < count; i++)
  {
    index = program[i].instructionFormatIndex;
    
    unsigned num = (program[i].immediateValue & IMMEDIATE_MASK) 
      | (program[i].sourceRegB << IMMEDIATE_BITS )
      | (program[i].sourceRegA << (IMMEDIATE_BITS + REG_BITS))
      | (program[i].destReg << (IMMEDIATE_BITS + 2 * REG_BITS))
      | (instructions[index].opCode << (IMMEDIATE_BITS + 3 * REG_BITS));
    
    outf.fill('0');
    outf << hex << setw(6) << num << " # " << instructions[index].opCodeStr << ' ';
   
    if(instructions[index].format != REG0)
      outf << 'R' << program[i].destReg;
    
    if(instructions[index].format == IMM2 || instructions[index].format == REG2 
      || instructions[index].format == REG3)
      outf << ", " << 'R' << program[i].sourceRegA ;
    
    if(instructions[index].format == REG3)
      outf << ", " <<  'R' << program[i].sourceRegB;
    
    if(instructions[index].format == IMM1 || instructions[index].format == IMM2)
      outf << ", " <<  hex << showbase << (program[i].immediateValue & IMMEDIATE_MASK) << noshowbase;
    
    outf << ' ' << program[i].comment << endl;
  } // for each line

  outf.close();
}  // writeFile()

int main(int argc, char** argv)
{
  int count;
  Instruction program[MAX_ADDRESS + 1];
  displayIntro();
  count = getProgram(program);
  writeFile(program, count);
  
  
  return 0;
} // main()

