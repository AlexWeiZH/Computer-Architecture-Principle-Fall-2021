//On my honor, I have neither given nor received unauthorized aid on this assignment
#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<string>
#include <stdio.h>
#include <math.h>

using namespace std;

const int memoryLength = 64;
const int registerSize = 32;

string M[memoryLength];					//memory
int Register[32] = { 0 };								//register
struct Fucntional_Unit_Status
{
	string Name[6];										//ALU1, MEM, ALU2, MUL1, MUL2, MUL3
	bool Busy[6];										//FU is busy or not
	int Fi[6];											//Destination register of instruction in the FU	
	string Fj[6];										//Source1
	string Fk[6];										//Source2
};

struct Instruction_Status
{
	string Instructions[memoryLength];
	bool Issue[memoryLength];
	bool WriteBack[memoryLength];
};
string b_dest;											//destination in binary
int d_dest;												//destination in decimal

string b_src1;											//source1 in binary
int d_src1;												//source1 in decimal

string b_src2;											//source2 in binary
int d_src2;												//source2 in decimal
string b_immediate;										//immediate in binary
int d_immediate;										//immediate in decimal

string b_rs;
string b_rt;
int d_rs;
int d_rt;

string b_offset;
int d_offset;

int dest;
int op1;
int op2;

string dest_tmp, op1_tmp, op2_tmp;						//SWProcess

bool op1_ready, op2_ready, dest_ready;				//op<->dest check for RAW, dest<->dest check for WAW, dest<->op check for WAR

string Buf1[8];																			//Buffer1
string Buf2[2], Buf3[2], Buf4[2];														//Buffer2,3,4
string Buf5[1], Buf6[1], Buf7[1], Buf8[1], Buf9[1], Buf10[1], Waiting[1], Executed[1], MEMBUF[1], ALU2BUF[1], MUL3BUF[1];	//Buffer5,6,7,8,9,10,Waiting,Executed

Instruction_Status IS;
string instructions[memoryLength];
string nullString = "";
int breakNumber = 0;
int finishNumber = 0;
int mylocations[6] = {-1,-1 ,-1 ,-1 ,-1 ,-1};

int binary2Decimal(string binary) {										//2->10 0010->2
	int decimal = 0;
	int a;

	int j = 0;
	for (int i = binary.length() - 1; i >= 0; i--) {

		a = atoi(binary.substr(i, 1).c_str());

		decimal += a * pow(2, j);
		j++;
	}
	return decimal;
}
string decimal2Binary(int decimal) {								//10->2 2->0010
	int A[5] = { 0 };
	int i = 0;
	while (decimal)
	{
		A[i] = decimal % 2;
		decimal /= 2;
		i++;

	}
	string result;
	for (int b = 4; b >= 0; b--) {
		result.append(to_string(A[b]));
	}
	return result;
}
void category1CommonProcess1(string line) {
	b_rs = line.substr(6, 5);
	b_rt = line.substr(11, 5);
	b_offset = line.substr(16, 16);

	d_rs = binary2Decimal(b_rs);
	d_rt = binary2Decimal(b_rt);

	string sign = line.substr(16, 1);				//negative number
	string result0 = line.substr(17, 15);
	string result1 = "0";
	if (sign == "1") {
		if (result0.substr(0, 1) == "0") { result1 = "1"; }
		for (int i = 1; i < 15; i++) {
			if (result0.substr(i, 1) == "0") { result1.append("1"); }
			else if (result0.substr(i, 1) == "1") { result1.append("0"); }
		}

		d_offset = binary2Decimal(result1) + 1;

		d_offset *= -4;
	}
	else {
		d_offset = 4 * binary2Decimal(b_offset);
	}
}
void category1CommonProcess2(string line, string s, int i) {
	M[i] = s;
	M[i].append(to_string(d_rs));
	M[i].append(", R");
	M[i].append(to_string(d_rt));
	M[i].append(", #");
	M[i].append(to_string(d_offset));
}
void category1CommonProcess3(string line, string s, int i) {
	d_offset /= 4;
	M[i] = s;
	M[i].append(to_string(d_rt));
	M[i].append(", ");
	M[i].append(to_string(d_offset));
	M[i].append("(R");
	M[i].append(to_string(d_rs));
	M[i].append(")");
}
void category2CommonProcess1(string line, string s, int i) {
	b_dest = line.substr(6, 5);
	b_src1 = line.substr(11, 5);
	b_src2 = line.substr(16, 5);

	d_dest = binary2Decimal(b_dest);
	d_src1 = binary2Decimal(b_src1);
	d_src2 = binary2Decimal(b_src2);

	M[i] = s;
	M[i].append(to_string(d_dest));
	M[i].append(", R");
	M[i].append(to_string(d_src1));
	M[i].append(", R");
	M[i].append(to_string(d_src2));
}
void category2CommonProcess2(string line, string s, int i) {
	b_dest = line.substr(6, 5);
	b_src1 = line.substr(11, 5);
	b_src2 = line.substr(16, 5);

	d_dest = binary2Decimal(b_dest);
	d_src1 = binary2Decimal(b_src1);

	string sign = line.substr(16, 1);
	string result0 = line.substr(17, 4);
	string result1 = "0";
	if (sign == "1") {
		if (result0.substr(0, 1) == "0") { result1 = "1"; }
		for (int i = 1; i < 4; i++) {
			if (result0.substr(i, 1) == "0") { result1.append("1"); }
			else if (result0.substr(i, 1) == "1") { result1.append("0"); }
		}

		d_src2 = binary2Decimal(result1) + 1;

		d_src2 *= -1;
	}
	else {
		d_src2 = binary2Decimal(b_src2);
	}


	M[i] = s;
	M[i].append(to_string(d_dest));
	M[i].append(", R");
	M[i].append(to_string(d_src1));
	M[i].append(", #");
	M[i].append(to_string(d_src2));
}
void category3CommonProcess1(string line, string s, int i) {

	b_dest = line.substr(6, 5);
	b_src1 = line.substr(11, 5);
	b_immediate = line.substr(16, 16);

	d_dest = binary2Decimal(b_dest);
	d_src1 = binary2Decimal(b_src1);

	string sign = line.substr(16, 1);
	string result0 = line.substr(17, 15);
	string result1 = "0";
	if (sign == "1") {
		if (result0.substr(0, 1) == "0") { result1 = "1"; }
		for (int i = 1; i < 15; i++) {
			if (result0.substr(i, 1) == "0") { result1.append("1"); }
			else if (result0.substr(i, 1) == "1") { result1.append("0"); }
		}

		d_immediate = binary2Decimal(result1) + 1;

		d_immediate *= -1;
	}
	else {
		d_immediate = binary2Decimal(b_immediate);
	}
	M[i] = s;
	M[i].append(to_string(d_dest));
	M[i].append(", R");
	M[i].append(to_string(d_src1));
	M[i].append(", #");
	M[i].append(to_string(d_immediate));

}
void ANDADD(string instruction) {
	if (instruction.substr(6, 1) == ",") {
		dest = atoi(instruction.substr(5, 1).c_str());
		if (instruction.substr(10, 1) != ",") {
			op1 = atoi(instruction.substr(9, 2).c_str());
			op2 = atoi(instruction.substr(14, instruction.length() - 14).c_str());
		}
		else {
			op1 = atoi(instruction.substr(9, 1).c_str());
			op2 = atoi(instruction.substr(13, instruction.length() - 13).c_str());
		}
	}
	else {
		dest = atoi(instruction.substr(5, 2).c_str());
		if (instruction.substr(11, 1) != ",") {
			op1 = atoi(instruction.substr(10, 2).c_str());
			op2 = atoi(instruction.substr(15, instruction.length() - 15).c_str());
		}
		else {
			op1 = atoi(instruction.substr(10, 1).c_str());
			op2 = atoi(instruction.substr(14, instruction.length() - 14).c_str());
		}
	}
}
void ANDIADDI(string instruction) {
	if (instruction.substr(7, 1) == ",") {
		dest = atoi(instruction.substr(6, 1).c_str());
		if (instruction.substr(11, 1) != ",") {
			op1 = atoi(instruction.substr(10, 2).c_str());
			op2 = atoi(instruction.substr(15, instruction.length() - 15).c_str());
		}
		else {
			op1 = atoi(instruction.substr(10, 1).c_str());
			op2 = atoi(instruction.substr(14, instruction.length() - 14).c_str());
		}
	}
	else {
		dest = atoi(instruction.substr(6, 2).c_str());
		if (instruction.substr(12, 1) != ",") {
			op1 = atoi(instruction.substr(11, 2).c_str());
			op2 = atoi(instruction.substr(16, instruction.length() - 16).c_str());
		}
		else {
			op1 = atoi(instruction.substr(11, 1).c_str());
			op2 = atoi(instruction.substr(15, instruction.length() - 15).c_str());
		}
	}
}
void SWLW(string instruction) {
	if (instruction.substr(5, 1) == ",") {
		dest = atoi(instruction.substr(4, 1).c_str());
		op1 = atoi(instruction.substr(7, 3).c_str());
		op2 = atoi(instruction.substr(12, instruction.length() - 13).c_str());
	}
	else {
		dest = atoi(instruction.substr(4, 2).c_str());
		op1 = atoi(instruction.substr(8, 3).c_str());
		op2 = atoi(instruction.substr(13, instruction.length() - 14).c_str());
	}
}
void BEQBNE(string instruction,bool &op1_ready, bool &op2_ready)
{
	ANDADD(instruction);
	//RAW
	string strObj1 = "R";
	string strObj2 = "R";
	string temp = to_string(dest);
	strObj1.append(temp);

	temp = to_string(op1);
	strObj2.append(temp);

	size_t nLoc1 = string::npos;							//earlier instruction not issued yet && issued but not finished
	size_t nLoc2 = string::npos;

	for (int i = 0; i <= breakNumber; i++)
	{
		nLoc1 = IS.Instructions[i].find(strObj1);
		if (int(nLoc1) > -1 && int(nLoc1) < 6)
		{
			break; 
		}
	}
	if (nLoc1 != string::npos && int(nLoc1) <= 5)
	{
		op1_ready = false;
	}
	for (int i = 0; i <=  breakNumber; i++)
	{
		nLoc2 = IS.Instructions[i].find(strObj2);
		if (int(nLoc2) > -1 && int(nLoc2) <= 5) { break; }
	}
	if (nLoc2 != string::npos && int(nLoc2) <= 5)
	{
		op2_ready = false;
	}
}
void OR_1(string instruction)
{
	if (instruction.substr(5, 1) == ",")
	{
		dest = atoi(instruction.substr(4, 1).c_str());
		if (instruction.substr(9, 1) != ",")
		{
			op1 = atoi(instruction.substr(8, 2).c_str());
			op2 = atoi(instruction.substr(13, instruction.length() - 13).c_str());
		}
		else 
		{
			op1 = atoi(instruction.substr(8, 1).c_str());
			op2 = atoi(instruction.substr(12, instruction.length() - 12).c_str());
		}
	}
	else 
	{
		dest = atoi(instruction.substr(4, 2).c_str());
		if (instruction.substr(10, 1) != ",")
		{
			op1 = atoi(instruction.substr(9, 2).c_str());
			op2 = atoi(instruction.substr(14, instruction.length() - 14).c_str());
		}
		else
		{
			op1 = atoi(instruction.substr(9, 1).c_str());
			op2 = atoi(instruction.substr(13, instruction.length() - 13).c_str());
		}
	}
}
int findMemoryNum(int number) {							//	260->0
	if (number >= 260) {
		return (number - 260) / 4;
	}
	else return -1;
}
int findMemoryAddr(int number) {					   //	0->260
	if (number != -1) {
		return number * 4 + 260;
	}
	else return -1;
}
int findFirstMylocationAvai() 
{
	int a = 0;
	for (int i = 0; i < 6; i++) 
	{
		if (mylocations[i] == -1) { a = i; break; }
	}
	return a;
}
int findFirstISAvail()
{
	int a = -1;
	for (int i = 0; i < memoryLength; i++) 
	{
		if (IS.Instructions[i] == nullString) { a = i; break;}
	}
	return a;
}
int findFirstBuf1Avail()
{
	int a = -1;
	for (int i = 0; i < 8; i++)
	{
		if (Buf1[i] == nullString) { a = i; break; }
	}
	return a;
}
int findFirstBuf2Avail()
{
	int a = -1;
	if (Buf2[0] == nullString) { a = 0; }
	else 
	{
		if (Buf2[1] == nullString) { a = 1; }
	}
	return a;
}
int findFirstBuf3Avail()
{
	int a = -1;
	if (Buf3[0] == nullString) { a = 0; }
	else
	{
		if (Buf3[1] == nullString) { a = 1; }
	}
	return a;
}
int findFirstBuf4Avail()
{
	int a = -1;
	if (Buf4[0] == nullString) { a = 0; }
	else
	{
		if (Buf4[1] == nullString) { a = 1; }
	}
	return a;
}
int findMyselfInIS(string instruction) 
{
	int a = -1;
	for(int i=0;i< memoryLength;i++)
	{
		if (IS.Instructions[i] == instruction) 
		{
			a = i;
			break;
		}
	}
	return a;
}
int getBuf1ElementNum() 
{
	int a = -1;
	for (int i = 0; i < 8; i++) 
	{
		if (Buf1[i] != nullString) { a = i; }
	}
	return a;
}
void IssueInstruction1(int mylocation, int fisrBuf2Avai, int j)
{	//IS.Issue[mylocation] = true;
	int aa = findFirstMylocationAvai();
	mylocations[aa] = mylocation;

	Buf2[fisrBuf2Avai] = Buf1[j];
	Buf1[j] = nullString;
	for (int aaa = j; aaa < 7; aaa++)
	{
		Buf1[aaa] = Buf1[aaa + 1];
	}
	Buf1[7] = nullString;
}
void IssueInstruction2(int mylocation, int fisrBuf4Avai, int j)
{
	int aa = findFirstMylocationAvai();
	mylocations[aa] = mylocation;

	Buf4[fisrBuf4Avai] = Buf1[j];
	Buf1[j] = nullString;
	for (int aaa = j; aaa < 7; aaa++)
	{
		Buf1[aaa] = Buf1[aaa + 1];
	}
	Buf1[7] = nullString;
}
void IssueInstruction3(int mylocation, int fisrBuf3Avai, int j)
{
	int aa = findFirstMylocationAvai();
	mylocations[aa] = mylocation;

	Buf3[fisrBuf3Avai] = Buf1[j];
	Buf1[j] = nullString;
	for (int aaa = j; aaa < 7; aaa++)
	{
		Buf1[aaa] = Buf1[aaa + 1];
	}
	Buf1[7] = nullString;
}
bool SWLW2(int mylocation)
{
	bool f1 = true;
	for (int a = 0; a < mylocation; a++)
	{
		if (IS.Instructions[a].substr(0, 1) == "S")
		{
			if (IS.Issue[a] == false) { f1 = false; }
		}
	}
	return f1;
}

bool RAW(int mylocation, string strObj)
{
	bool nR = true;
	size_t nLoc1 = string::npos;
	for (int i = 0; i <= mylocation; i++)
	{
		if (IS.Instructions[i].substr(0, 2) != "SW" && i != mylocation)
		{
			nLoc1 = IS.Instructions[i].find(strObj);
			if (int(nLoc1) > -1 && int(nLoc1) <= 5) { break; }
		}
	}
	if (nLoc1 != string::npos && int(nLoc1) <= 5)
	{
		nR = false;
	}
	return nR;
}
bool WAW(int mylocation, string strObj)
{
	bool nW = true;
	size_t nLoc2 = string::npos;
	for (int i = 0; i <= mylocation; i++)			//earlier instruction not issued yet && issued but not finished
	{
		if (IS.Instructions[i].substr(0, 2) == "SW" && i != mylocation) { continue; }
		if (IS.Instructions[i].substr(0, 2) != "SW" && i != mylocation)		//Jump SW
		{
			nLoc2 = IS.Instructions[i].find(strObj);
			if (int(nLoc2) > -1 && int(nLoc2) <= 5) { break; }
		}
	}
	if (nLoc2 != string::npos && int(nLoc2) <= 5)
	{
		nW = false;
	}
	return nW;
}
bool WAR(int mylocation, string strObj)
{
	bool nR = true;
	size_t nLoc3 = string::npos;
	for (int i = 0; i <= mylocation; i++)
	{
		if (IS.Issue[i] == false)							//earlier instruction not issued yet
		{
			if (i != mylocation)					
			{
				nLoc3 = IS.Instructions[i].find(strObj);
				if (int(nLoc3) >5) { break; }
			}
		}
	}
	if (nLoc3 != string::npos)
	{
		nR = false;
	}
	return nR;
}

void cleanUp(string sss) 
{
	int is_location = findMyselfInIS(sss);
	if (is_location != -1)
	{
		IS.WriteBack[is_location] = true;
		for (int i = is_location; i < memoryLength - 1; i++)
		{
			IS.Instructions[i] = IS.Instructions[i + 1];
			IS.Issue[i] = IS.Issue[i + 1];
			IS.WriteBack[i] = IS.WriteBack[i + 1];
		}
		IS.Instructions[memoryLength - 1] = nullString;
		IS.Issue[memoryLength - 1] = false;
		IS.WriteBack[memoryLength - 1] = false;
	}
}
void ALU2Common(int d, int result, string &instruction_A2)
{
	string outputV = to_string(result);
	outputV.append(", R");
	outputV.append(to_string(d));
	Buf6[0] = outputV;
	ALU2BUF[0] = instruction_A2;
	instruction_A2 = nullString;
}
int MemoryFinish() 
{
	int a=0;
	for (int i = 0; i < memoryLength; i++) 
	{
		if (M[i] == nullString) 
		{
			a = i;
			break;
		}
	}
	return a;
}
bool Buf2HasEmptyEntry() 
{
	int a = true;
	if (Buf2[1] != nullString) { a = false; }
	return a;
}
bool Buf3HasEmptyEntry()
{
	int a = true;
	if (Buf3[1] != nullString) { a = false; }
	return a;
}
bool Buf4HasEmptyEntry()
{
	int a = true;
	if (Buf4[1] != nullString) { a = false; }
	return a;
}
bool Buf1HasEmptyEntry()
{
	int a = true;
	if (Buf1[7] != nullString) { a = false; }
	return a;
}
void writefile(int cycleNumber)
{	
	ofstream outfile;
	outfile.open("./simulation.txt", ofstream::app);
	if (outfile.is_open()) 
	{
		outfile << "--------------------" << "\n";
		outfile << "Cycle " << cycleNumber << ":" << "\n";
		outfile << "\n";
		outfile << "IF:" << "\n";
		if (Waiting[0] == nullString) 
		{
			outfile << "\t" << "Waiting:"<< "\n";
		}
		else 
		{
			outfile << "\t" << "Waiting: ["<<Waiting[0]<<"]"<< "\n";
		}
		if (Executed[0] == nullString)
		{
			outfile << "\t" << "Executed:" << "\n";
		}
		else
		{
			outfile << "\t" << "Executed: [" << Executed[0] << "]" << "\n";
		}
		outfile << "Buf1:" << "\n";
		for (int i = 0; i < 8; i++) 
		{
			if (Buf1[i] == nullString) 
			{
				outfile << "\t" << "Entry " << i<<":"<< "\n";
			}
			else 
			{
				outfile << "\t" << "Entry " << i << ": ["<< Buf1[i]<<"]"<< "\n";
			}	
		}
		outfile << "Buf2:" << "\n";
		for (int i = 0; i < 2; i++)
		{
			if (Buf2[i] == nullString)
			{
				outfile << "\t" << "Entry " << i << ":" << "\n";
			}
			else
			{
				outfile << "\t" << "Entry " << i << ": [" << Buf2[i] << "]" << "\n";
			}
		}
		outfile << "Buf3:" << "\n";
		for (int i = 0; i < 2; i++)
		{
			if (Buf3[i] == nullString)
			{
				outfile << "\t" << "Entry " << i << ":" << "\n";
			}
			else
			{
				outfile << "\t" << "Entry " << i << ": [" << Buf3[i] << "]" << "\n";
			}
		}
		outfile << "Buf4:" << "\n";
		for (int i = 0; i < 2; i++)
		{
			if (Buf4[i] == nullString)
			{
				outfile << "\t" << "Entry " << i << ":" << "\n";
			}
			else
			{
				outfile << "\t" << "Entry " << i << ": [" << Buf4[i] << "]" << "\n";
			}
		}
		if (Buf5[0] == nullString)
		{
			outfile << "Buf5:" << "\n";
		}
		else
		{
			outfile << "Buf5: ["<<Buf5[0]<<"]" << "\n";
		}
		if (Buf6[0] == nullString)
		{
			outfile << "Buf6:" << "\n";
		}
		else
		{
			outfile << "Buf6: [" << Buf6[0] << "]" << "\n";
		}
		if (Buf7[0] == nullString)
		{
			outfile << "Buf7:" << "\n";
		}
		else
		{
			outfile << "Buf7: [" << Buf7[0] << "]" << "\n";
		}
		if (Buf8[0] == nullString)
		{
			outfile << "Buf8:" << "\n";
		}
		else
		{
			outfile << "Buf8: [" << Buf8[0] << "]" << "\n";
		}
		if (Buf9[0] == nullString)
		{
			outfile << "Buf9:" << "\n";
		}
		else
		{
			outfile << "Buf9: [" << Buf9[0] << "]" << "\n";
		}
		if (Buf10[0] == nullString)
		{
			outfile << "Buf10:" << "\n";
		}
		else
		{
			outfile << "Buf10: [" << Buf10[0] << "]" << "\n";
		}
		outfile << "\n";
		outfile << "Registers" << "\n";
		outfile << "R00:";
		for (int i = 0; i < 8; i++) 
		{
			outfile << "\t"<< Register[i];
		}
		outfile << "\n";
		outfile << "R08:";
		for (int i = 8; i < 16; i++)
		{
			outfile << "\t" << Register[i];
		}
		outfile << "\n";
		outfile << "R16:";
		for (int i = 16; i < 24; i++)
		{
			outfile << "\t" << Register[i];
		}
		outfile << "\n";
		outfile << "R24:";
		for (int i = 24; i < 32; i++)
		{
			outfile << "\t" << Register[i];
		}
		outfile << "\n";
		outfile << "\n";
		outfile <<"Data"<< "\n";
		int start = breakNumber + 1;		//0
		int finish = finishNumber - 1;		//0
		for (int i = start; i <= finish; i += 8) 
		{
			int start1 = findMemoryAddr(i);							//260
			outfile << start1 << ":";
			outfile << "\t" << M[i];								// 0
			if (i + 1 <= finish) { outfile << "\t" << M[i +1]; }	// 1
			if (i + 2 <= finish) { outfile << "\t" << M[i + 2]; }	// 2
			if (i + 3 <= finish) { outfile << "\t" << M[i + 3]; }	// 3
			if (i + 4 <= finish) { outfile << "\t" << M[i + 4]; }	// 4
			if (i + 5 <= finish) { outfile << "\t" << M[i + 5]; }	// 5
			if (i + 6 <= finish) { outfile << "\t" << M[i + 6]; }	// 6
			if (i + 7 <= finish) { outfile << "\t" << M[i + 7]; }	// 7
			outfile << "\n";
		}
		outfile.close();
	}
}


int main(int argc, char* argv[]) {

	for (int i = 0; i < memoryLength; i++) { M[i] = nullString; }	//Initialize M[] = ""
	for (int i = 0; i < memoryLength; i++) { instructions[i] = nullString; }	//Initialize instructions[] = ""

	string fileNameParameter = argv[1];
	string sourcefile = fileNameParameter;
	bool notBreak = true;
	bool oprandsReady = true;
	bool Buf1IsEmpty, Buf2IsEmpty, Buf3IsEmpty, Buf4IsEmpty;
	Buf1IsEmpty = Buf2IsEmpty = Buf3IsEmpty = Buf4IsEmpty = true;


	string peerInstructions[6];

	ifstream infile;
	infile.open(sourcefile.data());
	if (infile.is_open()) {

		string line;
		int i = 0;
		string categorycode;
		string opcode;
			while (getline(infile, line)) {

				int memoryNum = findMemoryAddr(i);

				categorycode = line.substr(0, 3);
				opcode = line.substr(3, 3);

				if (notBreak==true) {								//instrutions
					if (categorycode == "000") {				//category1
						if (opcode == "000") {						//J
							string instr_index = line.substr(6, 26);//jumping location
							string oprant = line.substr(7, 25);
							string newOP = "0";
							int location;
							if (line.substr(6, 1) == "1") {

								if (oprant.substr(0, 1) == "0") { newOP = "1"; }
								for (int i = 1; i < 25; i++) {
									if (oprant.substr(i, 1) == "0") { newOP.append("1"); }
									else if (oprant.substr(i, 1) == "1") { newOP.append("0"); }
								}
								location = binary2Decimal(newOP) + 1;
								location *= -1;
							}
							else {
								location = binary2Decimal(instr_index);
							}
							string l = to_string(location * 4);
							M[i] = "J #";
							M[i].append(l);
						}
						else if (opcode == "001") {					//BEQ
							category1CommonProcess1(line);
							string ss = "BEQ R";
							category1CommonProcess2(line, ss, i);

						}
						else if (opcode == "010") {					//BNE
							category1CommonProcess1(line);
							string ss = "BNE R";
							category1CommonProcess2(line, ss, i);

						}
						else if (opcode == "011") {					//BGTZ
							category1CommonProcess1(line);
							M[i] = "BGTZ R";
							M[i].append(to_string(d_rs));
							M[i].append(", #");
							M[i].append(to_string(d_offset));

						}
						else if (opcode == "100") {					//SW
							category1CommonProcess1(line);
							string ss = "SW R";
							category1CommonProcess3(line, ss, i);

						}
						else if (opcode == "101") {					//LW
							category1CommonProcess1(line);
							string ss = "LW R";
							category1CommonProcess3(line, ss, i);
						}
						else if (opcode == "110") {					//BREAK
							M[i] = "BREAK";
							notBreak = false;
							breakNumber = i;
						}
					}
					else if (categorycode == "001") {		//category2
						if (opcode == "000") {						//ADD

							string ss = "ADD R";

							category2CommonProcess1(line, ss, i);

						}
						else if (opcode == "001") {					//SUB
							string ss = "SUB R";

							category2CommonProcess1(line, ss, i);
						}
						else if (opcode == "010") {					//AND
							string ss = "AND R";

							category2CommonProcess1(line, ss, i);
						}
						else if (opcode == "011") {					//OR
							string ss = "OR R";

							category2CommonProcess1(line, ss, i);
						}
						else if (opcode == "110") {					//MUL
							string ss = "MUL R";

							category2CommonProcess1(line, ss, i);
						}
						else if (opcode == "100") {					//SRL

							string ss = "SRL R";

							category2CommonProcess2(line, ss, i);


						}
						else if (opcode == "101") {					//SRA

							string ss = "SRA R";

							category2CommonProcess2(line, ss, i);

						}
					}
					else if (categorycode == "010") {		//category3
						if (opcode == "000") {						//ADDI

							string ss = "ADDI R";

							category3CommonProcess1(line, ss, i);

						}
						else if (opcode == "001") {					//ANDI

							string ss = "ANDI R";

							category3CommonProcess1(line, ss, i);

						}
						else if (opcode == "010") {					//ORI

							string ss = "ORI R";

							category3CommonProcess1(line, ss, i);

						}
					}
				}
				else {											//2's complementory code

					int sign = atoi(line.substr(0, 1).c_str());
					if (sign == 1) {								//negative number

						string result0 = line.substr(1, 31);
						string result1 = "0";
						if (result0.substr(0, 1) == "0") { result1 = "1"; }
						for (int i = 1; i < 31; i++) {
							if (result0.substr(i, 1) == "0") { result1.append("1"); }
							else if (result0.substr(i, 1) == "1") { result1.append("0"); }
						}

						int value = binary2Decimal(result1) + 1;

						value *= -1;

						M[i] = to_string(value);

					}
					else {											//positive number

						int value = binary2Decimal(line);

						M[i] = to_string(value);
					}
				}
				i++;
			}
		
	}
	infile.close();

	finishNumber = MemoryFinish();
	for (int i = 0; i <= breakNumber; i++) 
	{
		instructions[i] = M[i];
	}
	for (int aa = 0; aa < 8; aa++) //Initialize buffer1
	{
		Buf1[aa] = nullString;
	}
	for (int aa = 0; aa < 2; aa++) //Initialize buffers
	{
		Buf2[aa] = nullString;
		Buf3[aa] = nullString;
		Buf4[aa] = nullString;
	}
	Buf5[0] = Buf6[0] = Buf7[0] = Buf8[0] = Buf9[0] = Buf10[0] = Waiting[0] = Executed[0] = MEMBUF[0] = ALU2BUF[0]= MUL3BUF[0] = nullString;

	for (int i = 0; i < memoryLength; i++) 
	{
		IS.Instructions[i] = nullString;
		IS.Issue[i] = false;
		IS.WriteBack[i] = false;
	}

	int PC = 0;
	int cycleNumber = 1;
	string instruction, instruction_A1, instruction_A2, instruction_M1, instruction_MEM, instruction_M2, instruction_M3;
	instruction_A1 = instruction_A2 = instruction_M1 = instruction_MEM = instruction_M2 = instruction_M3 = nullString;
	bool notbreak = true;
	int targetValue1, target1, targetValue2, target2, targetValue3, target3;
	int firstBuf1Avail, firstISAvail;
	int sw_after, desttnationn;


	while (notbreak)
	{
		op1_ready = op2_ready = dest_ready = true;
		bool sw_1 = false;
		bool clearWaiting = false;
		string sss1, sss2, sss3, sss4;
		sss1 = sss2 = sss3 = sss4 = nullString;
		int memoryAddr = findMemoryAddr(PC);		//0->260
		int memoryNumber0;							//260->0
		bool fetch = true;

		int locationPointer = 0;

		if (Waiting[0] == nullString)				//BREAK
		{
			string instruction_TEMP = instructions[PC];
			if (instruction_TEMP == "BREAK") 
			{
				Executed[0] = instruction_TEMP;
				writefile(cycleNumber);
				notbreak = false;
				break;
			}
		}
		//WB1
		if (Buf8[0] != nullString)
		{
			sss1 = MEMBUF[0];						//Instruction-wirte-back

			size_t nLoc4 = string::npos;
			nLoc4 = Buf8[0].find(",");
			targetValue1 = atoi(Buf8[0].substr(0, int(nLoc4)).c_str());
			nLoc4 = Buf8[0].find("R");
			target1 = atoi(Buf8[0].substr(int(nLoc4) + 1, Buf8[0].length() - int(nLoc4) - 1).c_str());

			MEMBUF[0] = nullString;
			Buf8[0] = nullString;
			//Write back

		}
		if (Buf6[0] != nullString)
		{
			sss2 = ALU2BUF[0];						//Instruction-wirte-back

			size_t nLoc4 = string::npos;
			nLoc4 = Buf6[0].find(",");
			targetValue2 = atoi(Buf6[0].substr(0, int(nLoc4)).c_str());
			nLoc4 = Buf6[0].find("R");
			target2 = atoi(Buf6[0].substr(int(nLoc4) + 1, Buf6[0].length() - int(nLoc4) - 1).c_str());

			ALU2BUF[0] = nullString;
			Buf6[0] = nullString;
			//Write back

		}
		if (Buf10[0] != nullString)
		{
			sss3 = MUL3BUF[0];						//Instruction-wirte-back

			size_t nLoc4 = string::npos;
			nLoc4 = Buf10[0].find(",");
			targetValue3 = atoi(Buf10[0].substr(0, int(nLoc4)).c_str());
			nLoc4 = Buf10[0].find("R");
			target3 = atoi(Buf10[0].substr(int(nLoc4) + 1, Buf10[0].length() - int(nLoc4) - 1).c_str());

			MUL3BUF[0] = nullString;
			Buf10[0] = nullString;
			//Write back

		}
		//WB1

		//MEM
		if (Buf5[0] != nullString)						//MEM
		{
			instruction_MEM = Buf5[0];
			sss4 = instruction_MEM;
			Buf5[0] = nullString;
			SWLW(instruction_MEM);
			int load_value;
			if (instruction_MEM.substr(0, 1) == "L")	//LW
			{
				int a = findMemoryNum(op1 + Register[op2]);		 //260->0
				load_value = atoi(M[a].c_str());
				string outputV = to_string(load_value);
				outputV.append(", R");
				outputV.append(to_string(dest));
				Buf8[0] = outputV;
				MEMBUF[0] = instruction_MEM;
			}
			else										//SW
			{
				sw_1 = true;
				sw_after = findMemoryNum(op1 + Register[op2]);		//260->0
				desttnationn = dest;
			}
			instruction_MEM = nullString;
		}
		//MUL3
		if (Buf9[0] != nullString)
		{
			instruction_M3 = Buf9[0];
			Buf9[0] = nullString;

			ANDADD(instruction_M3);		//get oprands
			int m1 = Register[op1];
			int m2 = Register[op2];
			int result = m1 * m2;

			string outputV1 = to_string(result);
			outputV1.append(", R");
			outputV1.append(to_string(dest));

			Buf10[0] = outputV1;
			MUL3BUF[0] = instruction_M3;
			instruction_M3 = nullString;
		}
		//MUL2
		if (Buf7[0] != nullString)
		{
			Buf9[0] = Buf7[0];
			Buf7[0] = nullString;
		}
		//MEM

		//EX1
		Buf2IsEmpty = Buf2HasEmptyEntry();
		Buf3IsEmpty = Buf3HasEmptyEntry();
		Buf4IsEmpty = Buf4HasEmptyEntry();
		if (Buf2[0] != nullString)						//ALU1
		{
			Buf5[0] = Buf2[0];
			Buf2[0] = Buf2[1];
			Buf2[1] = nullString;
		}
		instruction_A2 = Buf3[0];
		if (Buf3[0] != nullString)						//ALU2
		{
			Buf3[0] = Buf3[1];
			Buf3[1] = nullString;
		}
		if (Buf4[0] != nullString)						//ALU2
		{
			Buf7[0] = Buf4[0];
			Buf4[0] = Buf4[1];
			Buf4[1] = nullString;
		}
		//EX1

		//EX2
		//ALU2 <---> ADD, AND, ADDI, ANDI, SUB, SRL, SRA, OR, ORI,
		if (instruction_A2 != nullString)
		{
			if (instruction_A2.substr(0, 1) == "A")					//ADD, AND, ADDI, ANDI,
			{
				if (instruction_A2.substr(1, 3) == "DD ")			//ADD
				{
					ANDADD(instruction_A2);
					int result = Register[op1] + Register[op2];
					ALU2Common(dest, result, instruction_A2);

				}
				else if (instruction_A2.substr(1, 3) == "ND ")			//AND
				{
					ANDADD(instruction_A2);
					int result = Register[op1] & Register[op2];
					ALU2Common(dest, result, instruction_A2);
				}
				else if (instruction_A2.substr(1, 3) == "DDI")			//ADDI
				{
					ANDIADDI(instruction_A2);
					int result = Register[op1] + op2;
					ALU2Common(dest, result, instruction_A2);
				}
				else		//ANDI
				{
					ANDIADDI(instruction_A2);
					int result = Register[op1] & op2;
					ALU2Common(dest, result, instruction_A2);
				}
			}
			else if (instruction_A2.substr(0, 1) == "S")			//SUB, SRL, SRA
			{
				if (instruction_A2.substr(1, 2) == "UB")			//SUB
				{
					ANDADD(instruction_A2);
					int result = Register[op1] - Register[op2];
					ALU2Common(dest, result, instruction_A2);
				}
				else if (instruction_A2.substr(1, 2) == "RL")			//SRL
				{
					ANDADD(instruction_A2);
					int result;
					if (op2 >= 5) { result = 0; }
					else
					{
						string operant99 = decimal2Binary(Register[op1]);
						string operant00;
						for (int a = 0; a < op2; a++) { operant00.append("0"); }
						operant00.append(operant99.substr(0, 5 - op2));

						result = binary2Decimal(operant00);
					}

					ALU2Common(dest, result, instruction_A2);
				}
				else			//SRA
				{
					ANDADD(instruction_A2);
					int result = Register[op1] >> op2;
					ALU2Common(dest, result, instruction_A2);
				}
			}
			else													//OR, ORI,
			{
				if (instruction_A2.substr(1, 1) == "R" && instruction_A2.substr(1, 2) != "I")			//OR
				{
					OR_1(instruction_A2);
					int result = Register[op1] | Register[op2];
					ALU2Common(dest, result, instruction_A2);
				}
				if (instruction_A2 != nullString)
				{
					if (instruction_A2.substr(1, 2) == "RI")			//ORI
					{
						ANDADD(instruction_A2);
						int result = Register[op1] | op2;
						ALU2Common(dest, result, instruction_A2);
					}
				}
			}
			instruction_A2 = nullString;
		}
		//EX2

		//IS

		int ALU1_count, ALU2_count, MUL1_count;
		ALU1_count = ALU2_count = MUL1_count = 0;

		int elementNum = getBuf1ElementNum();
	
		Buf1IsEmpty = Buf1HasEmptyEntry();

		if (elementNum > -1)
		{
			for (int j = 0; j <= elementNum; j++)
			{
				if (Buf1[j] == nullString) { break; }
				bool flag1, nRAW, nWAW, nWAR;
				int mylocation;
				mylocation = findMyselfInIS(Buf1[j]);

				string fetchedIns = Buf1[j].substr(0, 1);

				int fisrBuf2Avai = findFirstBuf2Avail();
				int fisrBuf3Avai = findFirstBuf3Avail();
				int fisrBuf4Avai = findFirstBuf4Avail();
				//LW
				if (fetchedIns == "L")
				{
					if (fisrBuf2Avai > -1 && ALU1_count < 2 && Buf2IsEmpty)					//Buf2 has free entry
					{
						flag1 = nRAW = nWAW = nWAR = true;

						//All the previous stores must have issued
						flag1 = SWLW2(mylocation);
						if (flag1 == false) { break; }
						//get operands
						SWLW(Buf1[j]);

						string strObj1 = "R";
						string strObj2 = "R";
						string temp = to_string(dest);
						strObj1.append(temp);

						temp = to_string(op2);
						strObj2.append(temp);
						//RAW
						if (mylocation > -1)
						{
							nRAW = RAW(mylocation, strObj2);
							if (nRAW == false) { continue; }
						}
						//WAW
						if (mylocation > -1)
						{
							nWAW = WAW(mylocation, strObj1);
							if (nWAW == false) { continue; }
						}
						//WAR					
						if (mylocation > -1)
						{
							nWAR = WAR(mylocation, strObj1);
							if (nWAR == false) { continue; }
						}
						//Issue instruction
						if (flag1 && nRAW && nWAR && nWAW)
						{
							IssueInstruction1(mylocation, fisrBuf2Avai, j);
							ALU1_count++;
							j = j - 1;
							elementNum--;
						}
					}
				}
				//SW
				else if (fetchedIns == "S"&& Buf1[j].substr(1, 1)== "W")					//SW
				{
					if (fisrBuf2Avai > -1 && ALU1_count < 2 && Buf2IsEmpty)
					{
						flag1 = nRAW = nWAW = nWAR = true;
						//Stores must be issued in order
						flag1 = SWLW2(mylocation);
						if (flag1 == false) { continue; }
						//get operands
						SWLW(Buf1[j]);

						string strObj1 = "R";
						string strObj2 = "R";
						string temp = to_string(dest);
						strObj1.append(temp);

						temp = to_string(op2);
						strObj2.append(temp);

						//RAW
						if (mylocation > -1)
						{
							nRAW = RAW(mylocation, strObj1);
							if (nRAW == false) { continue; }
						}
						//Issue instruction
						if (flag1 && nRAW)
						{
							IssueInstruction1(mylocation, fisrBuf2Avai, j);
							ALU1_count++;
							j = j - 1;
							elementNum--;
						}
					}
				}
				//MUL
				else if (fetchedIns == "M")
				{
					//MUL
					if (fisrBuf4Avai > -1 && MUL1_count < 2 && Buf4IsEmpty)							//Buf4 has free entry
					{
						flag1 = nRAW = nWAW = nWAR = true;
						//get operands
						ANDADD(Buf1[j]);

						string strObj1 = "R";
						string strObj2 = "R";
						string strObj3 = "R";

						string temp = to_string(dest);
						strObj1.append(temp);

						temp = to_string(op1);
						strObj2.append(temp);

						temp = to_string(op2);
						strObj3.append(temp);
						//RAW
						if (mylocation > -1)
						{
							flag1 = RAW(mylocation, strObj2);
							nRAW = RAW(mylocation, strObj3);

							if (!flag1 || !nRAW) { continue; }
						}
						//WAW
						if (mylocation > -1)
						{
							nWAW = WAW(mylocation, strObj1);
							if (!nWAW) { continue; }
						}
						//WAR
						if (mylocation > -1)
						{
							nWAR = WAR(mylocation, strObj1);
							if (!nWAR) { continue; }
						}
						//Issue instruction
						if (flag1 && nRAW && nWAW && nWAR)
						{
							IssueInstruction2(mylocation, fisrBuf4Avai, j);
							MUL1_count++;
							j = j - 1;
							elementNum--;
						}
					}
				}
				//ADD, AND, ADDI, ANDI, SUB, SRL, SRA, OR, ORI,
				else
				{
					if (fisrBuf3Avai > -1 && ALU2_count < 2 && Buf3IsEmpty)		//Buf3 has free entry
					{
						flag1 = nRAW = nWAW = nWAR = true;
						string fetchedIns2 = Buf1[j].substr(0, 3);
						string fetchedIns3 = Buf1[j].substr(0, 4);

						string strObj1 = "R";
						string strObj2 = "R";
						string strObj3 = "R";
						//get operands
						if (fetchedIns3 == "ADDI" || fetchedIns3 == "ANDI")		//ADDI, ANDI
						{
							ANDIADDI(Buf1[j]);

							string temp = to_string(dest);
							strObj1.append(temp);

							temp = to_string(op1);
							strObj2.append(temp);
							//RAW
							if (mylocation > -1)
							{
								nRAW = RAW(mylocation, strObj2);
								if (!nRAW) { continue; }
							}
							//WAW
							if (mylocation > -1)
							{
								nWAW = WAW(mylocation, strObj1);
								if (!nWAW) { continue; }
							}
							//WAR
							if (mylocation > -1)
							{
								nWAR = WAR(mylocation, strObj1);
								if (!nWAR) { continue; }
							}
						}
						if (fetchedIns2 == "OR ")			//OR
						{
							OR_1(Buf1[j]);
						}
						//ADD, AND, SUB, SRL, SRA, ORI
						if (fetchedIns2 == "ADD" || fetchedIns2 == "AND" || fetchedIns2 == "SUB" || fetchedIns2 == "SRL" || fetchedIns2 == "SRA" || fetchedIns2 == "ORI")
						{
							if (fetchedIns3 != "ADDI" && fetchedIns3 != "ANDI") { ANDADD(Buf1[j]); }
						}
						if (fetchedIns2 == "ORI")
						{
							string temp = to_string(dest);
							strObj1.append(temp);

							temp = to_string(op1);
							strObj2.append(temp);
							//RAW
							if (mylocation > -1)
							{
								nRAW = RAW(mylocation, strObj2);
								if (!nRAW) { continue; }
							}
							//WAW
							if (mylocation > -1)
							{
								nWAW = WAW(mylocation, strObj1);
								if (!nWAW) { continue; }
							}
							//WAR
							if (mylocation > -1)
							{
								nWAR = WAR(mylocation, strObj1);
								if (!nWAR) { continue; }
							}
						}
						if (fetchedIns2 == "ADD" || fetchedIns2 == "AND" || fetchedIns2 == "SUB" || fetchedIns2 == "SRL" || fetchedIns2 == "SRA" || fetchedIns2 == "OR ")
						{
							if (fetchedIns3 != "ADDI" && fetchedIns3 != "ANDI")
							{
								if (fetchedIns2 != "SRL" && fetchedIns2 != "SRA") 
								{
									string temp = to_string(dest);
									strObj1.append(temp);

									temp = to_string(op1);
									strObj2.append(temp);

									temp = to_string(op2);
									strObj3.append(temp);
									//RAW
									if (mylocation > -1)
									{
										flag1 = RAW(mylocation, strObj2);
										nRAW = RAW(mylocation, strObj3);

										if (!flag1 || !nRAW) { continue; }
									}
									//WAW
									if (mylocation > -1)
									{
										nWAW = WAW(mylocation, strObj1);
										if (!nWAW) { continue; }
									}
									//WAR
									if (mylocation > -1)
									{
										nWAR = WAR(mylocation, strObj1);
										if (!nWAR) { continue; }
									}
								}
								else 
								{
									string temp = to_string(dest);
									strObj1.append(temp);

									temp = to_string(op1);
									strObj2.append(temp);

									//RAW
									if (mylocation > -1)
									{
										flag1 = RAW(mylocation, strObj2);

										if (!flag1) { continue; }
									}
									//WAW
									if (mylocation > -1)
									{
										nWAW = WAW(mylocation, strObj1);
										if (!nWAW) { continue; }
									}
									//WAR
									if (mylocation > -1)
									{
										nWAR = WAR(mylocation, strObj1);
										if (!nWAR) { continue; }
									}
								}
								
							}
						}
						//Issue instruction
						if (flag1 && nRAW && nWAW && nWAR)
						{
							IssueInstruction3(mylocation, fisrBuf3Avai, j);
							ALU2_count++;
							j = j - 1;
							elementNum--;
						} 
					}
				}
			}
		}


			for (int zzz = 0; zzz < 6; zzz++) 
			{
				int sz = mylocations[zzz];		//mylocations[aa] = mylocation;
				if (sz != -1) 
				{
					IS.Issue[sz] = true;
				}
			}

			for (int ii = 0; ii < 6; ii++)			//Initialize every tinme
			{
				mylocations[ii] = -1;
			}
		//IS

		//IF
		for (int i = 0; i < 4; i++)
		{
			if (clearWaiting) { break; }
			//Check if it has brach stalls
			if (Waiting[0] != nullString)
			{
				string ins_waiting = Waiting[0];
				string subss = ins_waiting.substr(0, 3);
				if (subss == "BEQ")							//BEQ
				{
					//RAW
					BEQBNE(ins_waiting, op1_ready, op2_ready);

					if (op1_ready && op2_ready)
					{
						PC++;
						Executed[0] = ins_waiting;
						clearWaiting = true;
						if (Register[dest] == Register[op1])
						{
							PC = PC + op2 / 4;
						}
					}
					else { break; }
				}
				else if (subss == "BNE")					//BNE
				{
					BEQBNE(ins_waiting, op1_ready, op2_ready);

					if (op1_ready && op2_ready)
					{
						clearWaiting = true;
						PC++;
						Executed[0] = ins_waiting;

						if (Register[dest] != Register[op1])
						{
							PC = PC + op2 / 4;
						}
					}
					else { break; }
				}
				else										//BGTZ
				{
					if (ins_waiting.substr(7, 1) == ",")
					{
						op1 = atoi(ins_waiting.substr(6, 1).c_str());
						op2 = atoi(ins_waiting.substr(10, ins_waiting.length() - 10).c_str());
					}
					else
					{
						op1 = atoi(ins_waiting.substr(6, 2).c_str());
						op2 = atoi(ins_waiting.substr(11, ins_waiting.length() - 11).c_str());
					}

					//RAW
					string strObj1 = "R";
					string temp = to_string(op1);
					strObj1.append(temp);

					size_t nLoc;							//earlier instruction not issued yet && issued but not finished
					for (int i = 0; i < memoryLength; i++)
					{
						nLoc = IS.Instructions[i].find(strObj1);
						if (int(nLoc) > -1 && int(nLoc) <= 5) { break; }
					}
					if (nLoc != string::npos && int(nLoc) <= 5)
					{
						op1_ready = false;
					}

					if (op1_ready)
					{
						clearWaiting = true;
						PC++;
						Executed[0] = ins_waiting;

						if (Register[op1] > 0) { PC = PC + (op2 / 4); }
					}
					else { break; }
				}
			}
			if (clearWaiting) { break; }
			if ( fetch && Buf1IsEmpty)					//Buf1 has empty slot
			{
				instruction = instructions[PC];
				string substring1 = instruction.substr(0, 1);

				if (substring1 == "J")					//JUMP
				{
					string substring22 = instruction.substr(3, 3);
					memoryNumber0 = atoi(substring22.c_str());
					PC = findMemoryNum(memoryNumber0);	//260->0

					Executed[0] = instruction;

					if (i < 3)
					{
						fetch = false;
						continue;
					}

				}
				else if (substring1 == "B")
				{
					string substring2 = instruction.substr(1, 2);
					if (substring2 == "EQ")				//BEQ
					{
						//RAW
						BEQBNE(instruction, op1_ready, op2_ready);

						if (op1_ready && op2_ready)
						{
							PC++;
							Executed[0] = instruction;
							Waiting[0] = nullString;

							if (Register[dest] == Register[op1])
							{
								PC = PC + op2 / 4;
							}
						}
						else { Waiting[0] = instruction; }

					}
					if (substring2 == "NE")				//BNE
					{
						//RAW
						BEQBNE(instruction, op1_ready, op2_ready);

						if (op1_ready && op2_ready)
						{
							PC++;
							Executed[0] = instruction;
							Waiting[0] = nullString;

							if (Register[dest] != Register[op1])
							{
								PC = PC + op2 / 4;
							}
						}
						else { Waiting[0] = instruction; }
					}
					if (substring2 == "GT")				//BGTZ
					{
						if (instruction.substr(7, 1) == ",")
						{
							op1 = atoi(instruction.substr(6, 1).c_str());
							op2 = atoi(instruction.substr(10, instruction.length() - 10).c_str());
						}
						else
						{
							op1 = atoi(instruction.substr(6, 2).c_str());
							op2 = atoi(instruction.substr(11, instruction.length() - 11).c_str());
						}

						//RAW
						string strObj1 = "R";
						string temp = to_string(op1);
						strObj1.append(temp);

						size_t nLoc;							//earlier instruction not issued yet && issued but not finished
						for (int i = 0; i < memoryLength; i++)
						{
							nLoc = IS.Instructions[i].find(strObj1);
							if (int(nLoc) > -1 && int(nLoc) <= 5) { break; }
						}
						if (nLoc != string::npos && int(nLoc) <= 5)
						{
							op1_ready = false;
						}

						if (op1_ready)
						{
							PC++;
							Executed[0] = instruction;
							Waiting[0] = nullString;

							if (Register[op1] > 0) { PC = PC + (op2 / 4); }
						}
						else { Waiting[0] = instruction; }
					}
					if (substring2 == "RE")				//BREAK
					{
						Executed[0] = instruction;
						writefile(cycleNumber);
						notbreak = false;
						break;
					}

					if (i < 3)
					{
						fetch = false;
						continue;
					}
				}
				else									//LW, SW, ADD, AND, ADDI, ANDI, SUB, SRL, SRA, OR, ORI, MUL
				{
					firstBuf1Avail = findFirstBuf1Avail();
					firstISAvail = findFirstISAvail();
					if (firstBuf1Avail > -1 && firstBuf1Avail < 8)
					{
						Buf1[firstBuf1Avail] = instruction;
					}
					if (firstISAvail > -1 && firstISAvail < memoryLength)
					{
						IS.Instructions[firstISAvail] = instruction;
					}
					PC++;
				}
			}
		}
		//IF

		if (!notbreak) { break; }			//BREAK is fetched


		//WB2
		if (sw_1 == true) { M[sw_after] = to_string(Register[desttnationn]); cleanUp(sss4);}			//MEM
		if (sss1 != nullString) { Register[target1] = targetValue1; cleanUp(sss1); }		//WB1
		if (sss2 != nullString) { Register[target2] = targetValue2; cleanUp(sss2); }		//WB1
		if (sss3 != nullString) { Register[target3] = targetValue3; cleanUp(sss3); }		//WB1
		if (clearWaiting) { Waiting[0] = nullString; }
		writefile(cycleNumber);
		Executed[0] = nullString;
		//WB2

		cycleNumber++;
	}
	return 0;
}