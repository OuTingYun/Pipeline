#include <math.h>

#include <fstream>
#include <iostream>
using namespace std;
fstream file;
//  NoIns = false, NoID = false, NoEX = false, NoMEM = false,
bool NOINS[4] = {false, false, false, false}, PCSrc[2] = {false, false},
     HaD[3] = {false, false, false};
int cc = 0, ReadData1 = 0, ReadData2 = 0, Rs = 0, Rt = 0, Rd = 0, PC = 0,
    sign_ext = 0, EXMEM_ALUout = 0, WriteData = 0, EXEMEM_Rt_Rd = 0,
    WB_Rt_RD = 0, MEMWB_Rt_Rd = 0, MEMWB_ALUout = 0, ReadData = 0,
    reg[10] = {0, 9, 5, 7, 1, 2, 3, 4, 5, 6}, mem[5] = {5, 9, 4, 8, 7};
string IDEX_control_signals = "000000000", EXMEM_control_signals = "00000",
       MEMWB_control_signals = "00", function, FarwardA, FarwardB;
char ins[33],
    create_path[4][30] = {{"./Output/loadResult.txt"},
                          {"./Output/dataResult.txt"},
                          {"./Output/branchResult.txt"},
                          {"./Output/genResult.txt"}},
    input_path[4][30] = {{"./Input/Lwhazard.txt"},
                         {"./Input/Datahazard.txt"},
                         {"./Input/Branchhazard.txt"},
                         {"./Input/General.txt"}};
string Split(int start, int end,
             string inp) {  // split the input string form start to end
  string n = "";
  for (int i = start; i <= end; i++) n += inp[i];
  return n;
}
int input = 0;
string LogicalOperator(string left, string right,
                       string op) {  // Doing and/or operator on every bits
  string out;
  if (op == "or")  // or
    for (int i = 0; i < left.length(); i++) out += left[i] | right[i];
  else if (op == "and")  // and
    for (int i = 0; i < left.length(); i++) out += left[i] & right[i];
  return out;
}
int bintodec(string inp, int start) {  // binary to decimal
  int len = inp.length();
  int total = 0;
  bool minus = false;
  if (inp[0] == '1') {  // determine weather a minus number or not
    minus = true;
    for (int i = 0; i < len; i++)
      inp[i] = (inp[i] == '1') ? '0' : '1';  // change bit 0to1 and 1to0
    for (int i = len - 1; i >= 0; i--) {     // plus one
      if (inp[i] == '1')
        inp[i] = '0';
      else {
        inp[i] = '1';
        break;
      }
    }
  }
  for (int i = 0; i < len; i++) total += (inp[i] - '0') * pow(2, (start - i));
  if (minus) total = total * (-1);
  return total;
}
string dectobin(int inp) {  // decimal to binary
  bool minus = false;
  if (inp < 0) {  // determind it is weather a minus number or not
    minus = true;
    inp = inp * (-1);  // let minus be positive
  }
  string result = "";
  while (inp > 0) {  // decinaal to binary
    char n = (inp % 2 + '0');
    result = n + result;
    inp = inp / 2;
  }
  string zero = "";
  for (int i = result.size(); i < 5; i++) {
    zero += "0";
  }
  result = zero + result;  // extend
  if (minus) {             // if input is a minus number.
    for (int i = 0; i < result.size(); i++)
      result[i] = (result[i] == '1') ? '0' : '1';   // change every bit
    for (int i = result.size() - 1; i >= 0; i--) {  // plus one
      if (result[i] == '1')
        result[i] = '0';
      else {
        result[i] = '1';
        break;
      }
    }
  }
  return result;
}
void setID_IE(int rs, int rt, int rd, int sign,
              string control) {  // set every things in ID/EXE reg
  sign_ext = sign;
  Rs = rs;
  Rt = rt;
  Rd = rd;
  ReadData1 = reg[Rs];
  ReadData2 = reg[Rt];
  IDEX_control_signals = control;
}
void createfile(int input) {  // create a file
  ofstream file(create_path[input]);
  if (!file) cout << "fail to Create file" << endl;
  file.close();
}
void writefile(int input) {  // print out every things in file
  fstream file;
  file.open(create_path[input], ios::out | ios::app);
  if (!file)
    cout << "fail to write " << endl;
  else {
    file << "CC " << cc << ":"
         << "\n\nRegisters:\n";
    for (int i = 0; i < 10; i++) {
      file << "$" << i << ": " << reg[i] << "\n";
    }
    int memindex = 0;
    file
        << "\nData memory:"
        << "\n0x00: " << mem[memindex++] << "\n0x04: " << mem[memindex++]
        << "\n0x08: " << mem[memindex++] << "\n0x0C: " << mem[memindex++]
        << "\n0x10: " << mem[memindex] << "\n\nIF/ID :\n"
        << "PC\t\t" << PC << "\nInstruction\t" << ins << "\n\nID/EX :\n"
        << "ReadData1\t" << ReadData1 << "\nReadData2\t" << ReadData2
        << "\nsign_ext\t" << sign_ext << "\nRs\t\t" << Rs << "\nRt\t\t" << Rt
        << "\nRd\t\t" << Rd << "\nControl signals\t" << IDEX_control_signals
        << "\n\nEX/MEM :\nALUout\t\t" << EXMEM_ALUout << "\nWriteData\t"
        << WriteData << "\nRt/Rd\t\t" << EXEMEM_Rt_Rd << "\nControl signals\t"
        << EXMEM_control_signals << "\n\nMEM/WB :\nReadData\t" << ReadData
        << "\nALUout\t\t" << MEMWB_ALUout << "\nRt/Rd\t\t" << MEMWB_Rt_Rd
        << "\nControl signals\t" << MEMWB_control_signals
        << "\n================================================================="
        << "\n";
    file.close();
  }
}
void forward() {  // determind it is forward or not
  if (EXMEM_control_signals[3] == '1' && EXEMEM_Rt_Rd && (EXEMEM_Rt_Rd == Rs)) {
    FarwardA = "10";
  }

  if (MEMWB_control_signals[0] == '1' && MEMWB_Rt_Rd && (EXEMEM_Rt_Rd != Rs) &&
      (MEMWB_Rt_Rd == Rs)) {
    FarwardA = "01";
  }
  if (EXMEM_control_signals[3] == '1' && EXEMEM_Rt_Rd && (EXEMEM_Rt_Rd == Rt)) {
    FarwardB = "10";
  }

  if (MEMWB_control_signals[0] == '1' && MEMWB_Rt_Rd && (EXEMEM_Rt_Rd != Rt) &&
      (MEMWB_Rt_Rd == Rt)) {
    FarwardB = "01";
  }
}
void HazardDection() {  // determine it is hazard or not
  if (IDEX_control_signals[5] == '1' &&
      ((Rt == bintodec(Split(6, 10, ins), 4)) ||
       (Rt == bintodec(Split(11, 15, ins), 4)))) {
    HaD[2] = true;  // if find the Hazard then  HD be set
  }
}
void stall(bool IF, bool ID, bool EX, bool MEM, bool WB,
           bool other) {  // stall the things what we want
  if (IF)                 // clear input
    for (int i = 0; i < 32; i++) ins[i] = '0';
  if (ID) {  // clear ID/EXE reg
    IDEX_control_signals = "000000000";
    sign_ext = 0;
    ReadData1 = 0;
    ReadData2 = 0;
    Rs = 0;
    Rt = 0;
    Rd = 0;
  }
  if (EX) {  // clear EXE/MEM reg
    EXMEM_ALUout = 0;
    WriteData = 0;
    EXEMEM_Rt_Rd = 0;
    EXMEM_control_signals = "00000";
  }
  if (MEM) {  // claer MEM/WB reg
    ReadData = 0;
    MEMWB_control_signals = "00";
    MEMWB_Rt_Rd = 0;
    MEMWB_ALUout = 0;
  }
  if (other) {  // clear every things.
    for (int i = 0; i < 33; i++) ins[i] = ' ';
    NOINS[0] = false;
    NOINS[1] = false;
    NOINS[2] = false;
    NOINS[3] = false;
    HaD[2] = false;
    PCSrc[0] = false;
    cc = 0;
    WB_Rt_RD = 0;
    reg[0] = 0;
    reg[1] = 9;
    reg[2] = 5;
    reg[3] = 7;
    reg[4] = 1;
    reg[5] = 2;
    reg[6] = 3;
    reg[7] = 4;
    reg[8] = 5;
    reg[9] = 6;
    mem[0] = 5;
    mem[1] = 9;
    mem[2] = 4;
    mem[3] = 8;
    mem[4] = 7;
    function = "";
    FarwardA = "";
    FarwardB = "";
    PC = 0;
  }
}
int beq_sign = 0;
// when  we find branch,the need to set PC to correct ins.But in my code the
// first time get into IF() doesn't neeed to change PC's value until the second
// time call IF(),so we use two bit to check.
void IF() {      // insruction fetch stage
  if (HaD[2]) {  // HD[1]set when we call IF() second times after data haazard
    HaD[2] = false;
    return;
  }
  PC += 4;
  if (PCSrc[1] && PCSrc[0]) {
    // it is the second time we call IF() after we found branch
    stall(1, 1, 0, 0, 0, 0);     // clear the ID/EXE reg
    PC = PC - 4 + 4 * beq_sign;  // set PC to correct ins.
    if (beq_sign > 0)  // check we branch to the ins that after this ins.
      for (int i = 1; i < beq_sign; i++) file.getline(ins, sizeof(ins));
    else {
      // branch to the ins which before we found ins
      file.close();
      file.open(input_path[input], ios::in);
      for (int i = 1; i < PC / 4; i++) file.getline(ins, sizeof(ins));
    }
    beq_sign = 0;
    PCSrc[0] = false;
    PCSrc[1] = false;  // clear the label that we finish the branch.
  } else if (PCSrc[0]) {
    // this means that we find branch,but it is the first time we call IF().
    beq_sign = sign_ext;
    PCSrc[1] = true;
  }

  file.getline(ins, sizeof(ins));        // fetch the correct ins.
  if (ins[0] != '0' && ins[0] != '1') {  // there is no ins we can fetch.
    stall(1, 0, 0, 0, 0, 0);             // clear the ins.
    NOINS[0] = true;  // when NoIns on means that there are no ins we can get.
  }
}
string opcode;
void ID() {
  if (!NOINS[0]) {  // there has ins to get
    opcode = Split(0, 5, ins);
    function = "";
    if (opcode == "100011")  // lw
      setID_IE(bintodec(Split(6, 10, ins), 4), bintodec(Split(11, 15, ins), 4),
               0, bintodec(Split(16, 31, ins), 15), "000101011");

    else if (opcode == "101011")  // sw
      setID_IE(bintodec(Split(6, 10, ins), 4), bintodec(Split(11, 15, ins), 4),
               0, bintodec(Split(16, 31, ins), 15), "000100100");
    else if (opcode == "000100") {  // beq  =rs rt label
      setID_IE(bintodec(Split(6, 10, ins), 4), bintodec(Split(11, 15, ins), 4),
               0, bintodec(Split(16, 31, ins), 15), "001010000");
      if (ReadData1 == ReadData2)
        PCSrc[0] = true;  // find beq inorder to let IF() know that find beq

    } else if (opcode == "001000")  // addi
      setID_IE(bintodec(Split(6, 10, ins), 4), bintodec(Split(11, 15, ins), 4),
               0, bintodec(Split(16, 31, ins), 15), "011100010");
    else if (opcode == "001100")  // andi
      setID_IE(bintodec(Split(6, 10, ins), 4), bintodec(Split(11, 15, ins), 4),
               0, bintodec(Split(16, 31, ins), 15), "011100010");
    else if (opcode == "000000") {  // R
      function = Split(26, 31, ins);
      setID_IE(bintodec(Split(6, 10, ins), 4), bintodec(Split(11, 15, ins), 4),
               bintodec(Split(16, 20, ins), 4),
               bintodec(Split(16, 31, ins), 15), "110000010");
    }
  } else {
    NOINS[1] = true;  // because no IF so no ID
    stall(0, 1, 0, 0, 0, 0);
  }
  if (HaD[2]) {
    HaD[0] = true;  // when first time call ID() after datahazard
    IDEX_control_signals = "000000000";
  }
}
void EX() {              // EX stage in pipeline
  if (!NOINS[1]) {       // no ins in ID()
    int adda, addb = 0;  // two value to execute

    if (FarwardA == "01")
      // put the correct vlue in adda. if there is datahazard and so one.
      adda = MEMWB_ALUout;
    else if (FarwardA == "10") {
      adda = EXMEM_ALUout;

    } else
      adda = ReadData1;
    if (HaD[0]) {
      adda = ReadData1;
      // EXMEM_ALUout = 0;
    }
    if (FarwardB == "01")
      // put the correct vlue in addb. if there is datahazard and so one.
      addb = MEMWB_ALUout;
    else if (FarwardB == "10") {
      addb = EXMEM_ALUout;
    }

    else
      addb = ReadData2;
    WriteData = addb;  // set WriteData in correct addb
    if (IDEX_control_signals[3] == '1') addb = sign_ext;  // addb for sw lw
    string ALUOP = Split(1, 2, IDEX_control_signals);

    if (IDEX_control_signals[0] == '1')  // set Rd/Rt
      EXEMEM_Rt_Rd = Rd;
    else
      EXEMEM_Rt_Rd = Rt;
    if (ALUOP == "00" && function == "") {  // lw sw
      EXMEM_ALUout = adda + addb;
    } else if (ALUOP == "10") {  // R
      if (function == "100000")  // add
        EXMEM_ALUout = adda + addb;
      else if (function == "100010")  // sub
        EXMEM_ALUout = adda - addb;
      else if (function == "101010")  // slt
        EXMEM_ALUout = ((adda - addb) < 0) ? 1 : 0;
      else {  // and or

        if (function == "100101") {
          EXMEM_ALUout = bintodec(
              LogicalOperator(dectobin(adda), dectobin(addb), "or"), 4);

        } else if (function == "100100") {
          EXMEM_ALUout = bintodec(
              LogicalOperator(dectobin(adda), dectobin(addb), "and"), 4);
        }
      }
    } else if (function == "") {  // I
      if (ALUOP == "01")
        EXMEM_ALUout = adda - addb;
      else if (ALUOP == "11") {
        if (opcode == "001000")  // addi
          EXMEM_ALUout = adda + addb;
        else if (opcode == "001100")  // andi
          EXMEM_ALUout = bintodec(
              LogicalOperator(dectobin(adda), dectobin(addb), "and"), 4);
      }
    }

    EXMEM_control_signals = Split(4, 8, IDEX_control_signals);
  } else {
    stall(0, 0, 1, 0, 0, 0);
    NOINS[2] = true;
  }
  if (HaD[0]) {
    EXMEM_ALUout = 0;
    HaD[0] = false;
  }
  opcode = "";
  FarwardA = "";
  FarwardB = "";
}
void MEM() {
  if (!NOINS[2]) {  // if there is no ins
    if (EXMEM_control_signals[1] == '1')
      ReadData = mem[EXMEM_ALUout / 4];
    else
      ReadData = 0;
    if (EXMEM_control_signals[2] == '1') mem[EXMEM_ALUout / 4] = WriteData;

    MEMWB_ALUout = EXMEM_ALUout;
    MEMWB_Rt_Rd = EXEMEM_Rt_Rd;
    MEMWB_control_signals = Split(3, 4, EXMEM_control_signals);
  } else {
    stall(0, 0, 0, 1, 0, 0);
    NOINS[3] = true;
  }
}
bool WB() {
  if (!NOINS[3]) {  // if there is on ins
    WB_Rt_RD = MEMWB_Rt_Rd;
    int write;
    if (MEMWB_control_signals[0] == '1') {
      if (WB_Rt_RD == 0) return true;
      if (MEMWB_control_signals[1] == '0')
        reg[MEMWB_Rt_Rd] = MEMWB_ALUout;
      else
        reg[MEMWB_Rt_Rd] = ReadData;
    }
  } else
    return true;
  return false;  // there is no ins.
}
int main() {
  for (input = 0; input <= 3; input++) {    // run for files' number
    stall(1, 1, 1, 1, 1, 1);                // clear all things
    file.open(input_path[input], ios::in);  // open files
    if (!file) cout << "can't open file" << endl;
    createfile(input);  // create output file
    while (true) {
      cc++;
      if (WB()) break;
      MEM();
      EX();
      ID();
      forward();
      IF();
      HazardDection();
      writefile(input);
    }
    file.close();
  }
  // system("pause");
  return 0;
}
// sw 2 4 0X03 ： 10101100100000100000000000000011
// addi 2 4 0X0C：00100000100000100000000000001100
// andi 2 4 0x05：00110000100000100000000000000101
// slt 2 4 2：    00000000100000100001000000101010
// slt 2 2 4：    00000000010001000001000000101010
// sw 4 6 0X03 ：10101100110001001111111111111101
// andi 2 4(-1) -1:00110000100000101111111111111111
