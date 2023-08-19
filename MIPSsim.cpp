/* On my honor, I have neither given nor received unauthorized aid on this assignment */
#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<string>
#include <stdio.h>
#include <math.h>

using namespace std;

const int memoryLength = 30;
const int registerSize = 32;

string M[memoryLength];											//memory
int R[32] = { 0 };										//register

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

int binary2Decimal(string binary) {
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
string decimal2Binary(int decimal) {
	int A[5] = {0};
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

	string sign = line.substr(16,1);				//negative number
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
void category1CommonProcess2(string line, string s,int i) {
	M[i] = s;
	M[i].append(to_string(d_rs));
	M[i].append(", R");
	M[i].append(to_string(d_rt));
	M[i].append(", #");
	M[i].append(to_string(d_offset));
}
void category1CommonProcess3 (string line, string s, int i) {
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
	string result0 = line.substr(17,4);
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
	else{
		d_immediate = binary2Decimal(b_immediate);
	}
	M[i] = s;
	M[i].append(to_string(d_dest));
	M[i].append(", R");
	M[i].append(to_string(d_src1));
	M[i].append(", #");
	M[i].append(to_string(d_immediate));

}

void ANDADD(int PC) {
	if (M[PC].substr(6, 1) == ",") {
		dest = atoi(M[PC].substr(5, 1).c_str());
		if (M[PC].substr(10, 1) != ",") {
			op1 = atoi(M[PC].substr(9, 2).c_str());
			op2 = atoi(M[PC].substr(14, M[PC].length() - 14).c_str());
		}
		else {
			op1 = atoi(M[PC].substr(9, 1).c_str());
			op2 = atoi(M[PC].substr(13, M[PC].length() - 13).c_str());
		}
	}
	else {
		dest = atoi(M[PC].substr(5, 2).c_str());
		if (M[PC].substr(11, 1) != ",") {
			op1 = atoi(M[PC].substr(10, 2).c_str());
			op2 = atoi(M[PC].substr(15, M[PC].length() - 15).c_str());
		}
		else {
			op1 = atoi(M[PC].substr(10, 1).c_str());
			op2 = atoi(M[PC].substr(14, M[PC].length() - 14).c_str());
		}
	}
}

void ANDIADDI(int PC) {
	if (M[PC].substr(7, 1) == ",") {
		dest = atoi(M[PC].substr(6, 1).c_str());
		if (M[PC].substr(11, 1) != ",") {
			op1 = atoi(M[PC].substr(10, 2).c_str());
			op2 = atoi(M[PC].substr(15, M[PC].length() - 15).c_str());
		}
		else {
			op1 = atoi(M[PC].substr(10, 1).c_str());
			op2 = atoi(M[PC].substr(14, M[PC].length() - 14).c_str());
		}
	}
	else {
		dest = atoi(M[PC].substr(6, 2).c_str());
		if (M[PC].substr(12, 1) != ",") {
			op1 = atoi(M[PC].substr(11, 2).c_str());
			op2 = atoi(M[PC].substr(16, M[PC].length() - 16).c_str());
		}
		else {
			op1 = atoi(M[PC].substr(11, 1).c_str());
			op2 = atoi(M[PC].substr(15, M[PC].length() - 15).c_str());
		}
	}
}
void SWLW(int PC) {
	if (M[PC].substr(5, 1) == ",") {
		dest = atoi(M[PC].substr(4, 1).c_str());
		op1 = atoi(M[PC].substr(7, 3).c_str());
		op2 = atoi(M[PC].substr(12, M[PC].length() - 13).c_str());
	}
	else {
		dest = atoi(M[PC].substr(4, 2).c_str());
		op1 = atoi(M[PC].substr(8, 3).c_str());
		op2 = atoi(M[PC].substr(13, M[PC].length() - 14).c_str());
	}
}

int findMemoryNum(int number) {
	if (number >= 260) {
		return (number - 260) / 4;
	}
	else return -1;
}
int findMemoryAddr(int number) {
	if (number !=-1) {
		return number*4+260;
	}
	else return -1;
}



int main(int argc, char* argv[]) {

	string fileNameParameter = argv[1];
	string sourcefile = fileNameParameter;
	ifstream infile;
	infile.open(sourcefile.data());
	if (infile.is_open()) {

		string line;
		int i = 0;
		string categorycode;
		string opcode;

		ofstream outfile("./disassembly.txt");
		if (outfile.is_open()) {
			while (getline(infile, line)) {

				int memoryNum = findMemoryAddr(i);

				categorycode = line.substr(0, 3);
				opcode = line.substr(3, 3);

				if(memoryNum<=312){								//instrutions
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
								location = binary2Decimal(newOP)+1;
								location *= -1;
							}
							else {
								location = binary2Decimal(instr_index);
							}
							string l = to_string(location*4);
							M[i] = "J #";
							M[i].append(l);
						}
						else if (opcode == "001"){					//BEQ
							category1CommonProcess1(line);
							string ss = "BEQ R"; 
							category1CommonProcess2(line, ss, i);
							
						}
						else if (opcode == "010"){					//BNE
							category1CommonProcess1(line);
							string ss = "BNE R";
							category1CommonProcess2(line, ss, i);

						}
						else if (opcode == "011"){					//BGTZ
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
						}
					}
					else if (categorycode == "001") {		//category2
						if (opcode == "000") {						//ADD
							
							string ss = "ADD R";

							category2CommonProcess1(line,ss,i);

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

							category3CommonProcess1(line,ss,i);

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
						string result1="0";
						if (result0.substr(0,1) == "0") { result1 = "1"; }
						for (int i = 1; i < 31;i++) {
							if (result0.substr(i,1) == "0") { result1.append("1"); }
							else if(result0.substr(i,1) == "1") { result1.append("0"); }
						}
						
						int value = binary2Decimal(result1)+1;
						
						value *= -1;
						
						M[i] = to_string(value);

					}
					else {											//positive number

						int value = binary2Decimal(line);

						M[i] = to_string(value);

					}

				}
				outfile << line << "\t" << memoryNum <<"\t"<<M[i]<<"\n";

				i++;
			}
			outfile.close();
		}
	}
	infile.close();


	ofstream outfile("./simulation.txt");
	if (outfile.is_open()) {
		int PC = 0;
		int cycleNumber = 1;
		while (PC < memoryLength)
		{
			
			int memoryNumber0;
			int memoryAddr = findMemoryAddr(PC);

			outfile << "--------------------" << "\n";
			outfile << "Cycle " << cycleNumber << ":" << "\t" << memoryAddr << "\t" << M[PC] << "\n";
			outfile << "\n";
			
			string substring1 = M[PC].substr(0,1);
			if (substring1 == "J") {							//JUMP
				memoryNumber0= atoi(M[PC].substr(3, M[PC].length()-3).c_str());
				PC = findMemoryNum(memoryNumber0);
			}
			else if (substring1 == "A") {
				string substring2 = M[PC].substr(1,3);
				if (substring2 == "DD ") {					//ADD
					ANDADD(PC);
					R[dest] = R[op1] + R[op2];
					PC++;
				}
				else if (substring2 == "DDI") {				//ADDI
					ANDIADDI(PC);
					R[dest] = R[op1] + op2;
					PC++;
				}
				else if (substring2 == "ND ") {				//AND
					ANDADD(PC);
					R[dest] = R[op1] & R[op2];
					PC++;
				}
				else if (substring2 == "NDI") {				//ANDI
					ANDIADDI(PC);
					R[dest] = R[op1] & op2;
					PC++;
				}
			}
			else if (substring1 == "B") {
				string substring2 = M[PC].substr(1, 3);
				if (substring2 == "EQ ") {					//BEQ	
					ANDADD(PC);
					PC++;
					if (R[dest] == R[op1]) {
						PC = PC + op2/4;
					}
				}
				else if (substring2 == "NE ") {				//BNE
					ANDADD(PC);
					PC++;
					if (R[dest] != R[op1]) {
						PC = PC + op2 / 4;
					}
				}
				else if (substring2 == "GTZ") {			//BGTZ
					if (M[PC].substr(7, 1) == ",") {
						op1 = atoi(M[PC].substr(6, 1).c_str());
						op2= atoi(M[PC].substr(10, M[PC].length()-10).c_str());
					}
					else {
						op1 = atoi(M[PC].substr(6, 2).c_str());
						op2 = atoi(M[PC].substr(11, M[PC].length() - 11).c_str());
					}
					
					

					if (R[op1] > 0) { PC++; PC = PC + (op2/4); }
					else { PC++; }
					
				}
				else if (substring2 == "REA") {			//BREAK
					outfile << "Registers" << "\n";
					outfile << "R00:";
					for (int a0 = 0; a0 < 8; a0++) { outfile << "\t" << R[a0]; }
					outfile << "\n";
					outfile << "R08:";
					for (int a1 = 8; a1 < 16; a1++) { outfile << "\t" << R[a1]; }
					outfile << "\n";
					outfile << "R16:";
					for (int a2 = 16; a2 < 24; a2++) { outfile << "\t" << R[a2]; }
					outfile << "\n";
					outfile << "R24:";
					for (int a3 = 24; a3 < 32; a3++) { outfile << "\t" << R[a3]; }
					outfile << "\n";
					outfile << "\n";

					outfile << "Data" << "\n";
					outfile << "316:";
					for (int b0 = 14; b0 < 22; b0++) { outfile << "\t" << M[b0]; }
					outfile << "\n";
					outfile << "348:";
					for (int b1 = 22; b1 < 30; b1++) { outfile << "\t" << M[b1]; }
					outfile << "\n";
					break;
				}
			}
			else if (substring1 == "L") {			//LW
				SWLW(PC);

				R[dest] = atoi(M[findMemoryNum(op1 +R[op2])].c_str());
				PC++;
			}
			else if (substring1 == "O") {
				string substring2 = M[PC].substr(1, 2);
				if (substring2 == "R ") {			//OR
					if (M[PC].substr(5, 1) == ",") {
						dest = atoi(M[PC].substr(4, 1).c_str());
						if (M[PC].substr(9, 1) != ",") {
							op1 = atoi(M[PC].substr(8, 2).c_str());
							op2 = atoi(M[PC].substr(13, M[PC].length() - 13).c_str());
						}
						else {
							op1 = atoi(M[PC].substr(8, 1).c_str());
							op2 = atoi(M[PC].substr(12, M[PC].length() - 12).c_str());
						}
					}
					else {
						dest = atoi(M[PC].substr(4, 2).c_str());
						if (M[PC].substr(10, 1) != ",") {
							op1 = atoi(M[PC].substr(9, 2).c_str());
							op2 = atoi(M[PC].substr(14, M[PC].length() - 14).c_str());
						}
						else {
							op1 = atoi(M[PC].substr(9, 1).c_str());
							op2 = atoi(M[PC].substr(13, M[PC].length() - 13).c_str());
						}
					}
					R[dest] = R[op1] | R[op2];
					PC++;

				}
				else if (substring2 == "RI") {		//ORI
					ANDADD(PC);
					R[dest] = R[op1] | op2;
					PC++;
				}
			}
			else if (substring1 == "M")	{			//MUL
					ANDADD(PC);
					R[dest] = R[op1] * R[op2];
					PC++;
			}
			else if (substring1 == "S") {
				string substring2 = M[PC].substr(1, 2);
				if (substring2 == "W ") {			//SW
					SWLW(PC);
					M[findMemoryNum(op1 + R[op2])] = to_string(R[dest]);
					PC++;
				}
				else if (substring2 == "UB") {		//SUB
					ANDADD(PC);
					R[dest] = R[op1] - R[op2];
					PC++;
				}
				else if (substring2 == "RL") {		//SRL
					ANDADD(PC);
					if (op2 >= 5) { R[dest] = 0; }
					else{
						string operant99 = decimal2Binary(op1);
						string operant00;
						for (int a = 0; a < op2; a++) { operant00.append("0"); }
						operant00.append(operant99.substr(0,5-op2));

						R[dest] = binary2Decimal(operant00);
					}
					PC++;
				}
				else if (substring2 == "RA") {		//SRA
					ANDADD(PC);
					R[dest] = R[op1] >> R[op2];
					PC++;
				}
			}

			

			outfile <<"Registers"<< "\n";
			outfile << "R00:";
			for (int a0 = 0; a0 < 8; a0++) { outfile << "\t"<<R[a0]; }
			outfile<< "\n";
			outfile << "R08:";
			for (int a1 = 8; a1 < 16; a1++) { outfile << "\t" << R[a1]; }
			outfile << "\n";
			outfile << "R16:";
			for (int a2 = 16; a2 < 24; a2++) { outfile << "\t" << R[a2]; }
			outfile << "\n";
			outfile << "R24:";
			for (int a3 = 24; a3 < 32; a3++) { outfile << "\t" << R[a3]; }
			outfile << "\n";
			outfile << "\n";

			outfile << "Data" << "\n";
			outfile << "316:";
			for (int b0 = 14; b0 < 22; b0++) { outfile << "\t" << M[b0]; }
			outfile << "\n";
			outfile << "348:";
			for (int b1 = 22; b1 < 30; b1++) { outfile << "\t" << M[b1]; }
			outfile << "\n";
			outfile << "\n";

			cycleNumber++;
		}


		outfile.close();
	}


	return 0;
}
