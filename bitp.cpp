#include <iostream> //this allows more advanced input/output
#include <fstream> //allows access to files
#include <bitset> //stores the bits
#include <conio.h> //this will allow detecting keystrokes instead of key presses
#include <string.h> //contains memset.
#include <ctime> //contains time.
using namespace std;

int const l = 4096;
string setScript();
void runScript(bitset<l> *script, unsigned long long pointer);
short at(bitset<l> data, int i);
unsigned long range(bitset<l> data, int i, short length);
bitset<l> setRange(bitset<l> data, unsigned long value, int i, short length);
unsigned long extern_function(unsigned long input, string &previous);
unsigned short r(unsigned short value);
unsigned long long perform(unsigned long long operand_1, unsigned long long operand_2, unsigned short operation);

void packet_send(unsigned long long int packet[64],unsigned char port);
void packet_get(unsigned long long int packet[64], unsigned char port);

int main() {
	bitset<l> compileScript(string script);
	bitset<l> script;
	ifstream scriptFile ("script.bp");
	string scriptString;
	string line;
	if (scriptFile.is_open())
	{
		while ( getline (scriptFile,line) )
		{
			scriptString += line + '\n';
		}
		scriptFile.close();
	}
	else return 1;
	script = compileScript(scriptString);
	runScript(&script,0);
	cout << "\nDone";
	getch();
}

string setScript() {
	cout << "type/paste script\n";
	string script;
	cin >> script;
    return script;
}

bitset<l> compileScript(string script) {
	bitset<l> compiled;
	cout << hex << uppercase;
	bool skipping = false;
	int i2 = 0;
	for(int i = 0; i < script.length(); ++i) {
		char c = script.at(i);
		bool masked = false;
		bitset<4> mask;
		switch (c) {
			case '<':
				mask = 0;
				break;
			case '>':
				mask = 1;
				break;
			case ';':
				mask = 2;
				break;
			case ':':
				mask = 3;
				break;
			case '~':
				mask = 4;
				break;
			case '^':
				mask = 5;
				break;
			case '&':
				mask = 6;
				break;
			case '|':
				mask = 7;
				break;
			case '{':
				mask = 8;
				break;
			case '}':
				mask = 9;
				break;
			case '[':
				mask = 10;
				break;
			case ']':
				mask = 11;
				break;
			case '.':
				mask = 12;
				break;
			case ',':
				mask = 13;
				break;
			case '+':
				mask = 14;
				break;
			case '-':
				mask = 15;
				break;
			case '\'':
				skipping = !skipping;
				masked = true;
				break;
			default:
				masked = true;
				break;
		}
		if (!skipping) {
			//last resort will convert hexadecimal values into their respective character
			//helps with backwards compatibility, should the commands change internal values
			if (masked) {
				int x = (c-'0');
				if (x>=0 && x<=9) {
					mask = x;
					masked = false;
				}
				else {
					int x = (c-'A'+10);
					if (x>=10 && x<=15) {
						mask = x;
						masked = false;
					}
				}
			}
			if (!masked) {
				for (short o = 0; o < 4; ++ o) {
					compiled[4*(i-i2)+o] = mask[o];
				}
			}
			else {
				++i2;
			}
		} else {
			++i2;
		}
	}
	return compiled;
}

void runScript(bitset<l> *script, unsigned long long pointer) {
	bool cmdsExtend = false; //tells whether the next command is in the first or second command set
	bool cmdsLength = false; //tells whether the pointer is looking at a command or a value's length, for the '>' ASSIGN-LENGTH command
	bool cmdsValue = false; //tells whether the pointer is looking at a command or a value, for the '<' ASSIGN command
	unsigned char cLength = 0; //tells whether or not to continue the cmdsValue chain
	unsigned long long variable[8] = {0}; //values
	unsigned char v = 0; //points to the value being currently used
	//r(v) gets the value behind v, handling overflow
	unsigned long long ilocation = 0; //stores the values to overwrite when using ']' COMMIT
	unsigned char ilength = 0; //stores the number of values to overflow when using ']' COMMIT
	unsigned long long packet[64] = {0}; //contains the packet. half a kb for i/o buffer space.
	unsigned char port = 0; //points to the port for i/o use
	/*
	0 - cout
	1 - cin
	*/
	
	while(pointer < l) { // ---------------- BEGIN LOOP
	
		//<debug> this code is commented out in the default compilation.
		for (short i=0;i<=7;i++) {
			if (i==v)
				cout << "[" << variable[i] << "] : ";
			else
				cout << variable[i] << " : ";
		}
		if (pointer<l)
			cout << "*" << pointer/4;
		cout << " ";
		if (cmdsValue) cout << +(cLength) << "#";
		if (cmdsLength) cout << "#..";
		if (cmdsExtend) cout << "+";
		if (pointer<l) cout << at(*script,pointer);
		cout << " [" << ilocation;
		cout << "-" << +(ilength);
		cout << "=" << range(*script,ilocation,ilength);
		cout << "]";
		cout << endl;
		//</debug>
	
		if (cmdsValue) {
			variable[v]<<=4;
			variable[v]|=at(*script,pointer);
			if (cLength>0) {--cLength;}
			else {cmdsValue=false;}
		}
		else if (cmdsLength) {
			cLength=at(*script,pointer);
			cmdsLength=false;
			cmdsValue=true;
		}
		else if (cmdsExtend) {
			switch (at(*script,pointer)) {
				case 0: //LESSER
					variable[r(v)] = variable[r(v)]<variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 1: //GREATER
					variable[r(v)] = variable[r(v)]>variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 2: //THREAD
					//TODO make this run desynchronized.
					runScript(script,variable[v]*4-4);
					variable[v]=0;
					break;
				case 3: //IF-THREAD
					//TODO make this run desynchronized.
					if (variable[r(v)]) {
						runScript(script,variable[v]*4-4);
					}
					variable[v]=0;
					variable[r(v)]=0;
					v = r(v);
					break;
				case 4: //INVERT
					variable[v] = ~variable[v];
					break;
				case 5: //SEND-PACKET
					packet_send(packet,port);
					break;
				case 6: //CLEAR-ALL
					memset(variable, 0, 8 * sizeof(variable[0]));
					v = 0;
					break;
				case 7: //RECEIVE-PACKET
					packet_get(packet,port);
					break;
				case 8: //LEFT-SHIFT
					variable[r(v)]<<=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 9: //RIGHT-SHIFT
					variable[r(v)]>>=variable[v];
					variable[v] = 0;
					v = r(v);
				case 10: //STORE-PACKET
					for(unsigned short i= sizeof(packet) / sizeof(packet[0]) - 1 ; i>0 ; --i) { //defined as a short in case a memory boost is required
						packet[i] = packet[i-1];
					}
					packet[0] = variable[v];
					variable[v] = 0;
					break;
				case 11: //READ-PACKET
					variable[v] = packet[0];
					for(unsigned short i= sizeof(packet) / sizeof(packet[0]) - 1; i>0 ; --i) { //defined as a short in case a memory boost is required
						packet[i-1] = packet[i];
					}
					break;
				case 12: //JETTISON
					memset(packet, 0, sizeof(packet) * sizeof(packet[0]));
					break;
				case 13: //DOCK
					port = variable[v];
					break;
				case 14: //TIME
					variable[v] = time(0);
					break;
				case 15: //TERMINATE
					return;
			}
			cmdsExtend = false;
		}
		else {
			switch (at(*script,pointer)) {
				case 0: //ASSIGN-LENGTH
					cmdsLength = true;
					break;
				case 1: //ASSIGN
					cmdsValue = true;
					break;
				case 2: //GOTO
					pointer = variable[v]*4;
					pointer -= 4; //this cancels out the addition later, even if there is overflow.
					variable[v]=0;
					break;
				case 3: //IF-GOTO
					if (variable[r(v)]) {
						pointer = variable[v]*4;
						pointer -= 4; //this cancels out the addition later, even if there is overflow.
					}
					variable[v]=0;
					variable[r(v)]=0;
					v = r(v);
					break;
				case 4: //NOT
					variable[v] = !variable[v];
					break;
				case 5: //XOR
					variable[r(v)]^=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 6: //AND
					variable[r(v)]&=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 7: //OR
					variable[r(v)]|=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 8: //LEFT-SHIFTLE
					variable[v]<<=1;
					break;
				case 9: //RIGHT-SHIFTLE
					variable[v]>>=1;
					break;
				case 10: //REMEMBER
					ilocation = variable[v];
					ilength = variable[r(v)];
					break;
				case 11: //COMMIT
					*script =
					setRange(
						*script,
						variable[v],
						ilocation,
						ilength
					);
					break;
				case 12: //READ
					variable[r(v)] =
					range(
						*script,
						variable[v],
						variable[r(v)]
					);
					variable[v] = 0;
					v = r(v);
					break;
				case 13: //NEXT
					++v;
					if (v==8)
						v=0;
					break;
				case 14: //PERFORM
					variable[r(r(v))] =
					perform(
						variable[r(r(v))],
						variable[r(v)],
						variable[v]
					);
					variable[v] = 0;
					variable[r(v)] = 0;
					v = r(r(v));
					break;
				case 15: //EXTEND
					cmdsExtend = true;
					break;
			}
		}
		pointer+=4;
	}
}

unsigned long long perform(unsigned long long operand_1, unsigned long long operand_2, unsigned short operation) {
	switch(operation) {
		case 0:
			return operand_1 + operand_2;
		case 1:
			return operand_1 - operand_2;
		case 2:
			return operand_1 * operand_2;
		case 3:
			return operand_1 / operand_2;
		case 4:
			return operand_1 % operand_2;
		default:
			return 0;
	}
}

short at(bitset<l> data, int i) {
	unsigned short ret = 0;
	for (short o = 0; o <= 3; ++ o) {
		ret |= data[i+o]<<o;
	}
	return ret;
}
unsigned long range(bitset<l> data, int i, short length) {
	unsigned long ret = 0;
	for (int o = 0; o <= length; ++ o) {
		ret |= data[i+o]<<o;
	}
	return ret;
}
bitset<l> setRange(bitset<l> data, unsigned long value, int i, short length) {
	bitset<64> mask = value;
	for (int o = 0; o <= length; ++ o) {
		data[i+o] = mask[o];
	}
	return data;
}
unsigned long extern_function(unsigned long input, string &previous) {
	cout << dec << nouppercase;
	unsigned long output = input;
	if (input) { //if there is input
		cout << (char)(input);
	}
	else { //if there isn't input
		if (!previous.length()) { //if there is no length for previous
			getline (cin,previous);
			previous += '\0';
		}
		output = previous.at(0);
		previous = previous.substr(1);
	}
	cout << hex << uppercase;
	return output;
}
unsigned short r(unsigned short value) {
	if (value == 0)
		return 7;
	else
		return value-1;
}

void packet_send(unsigned long long int packet[64], unsigned char port) {
	cout << dec << nouppercase;
	for (unsigned short i=63; i<64; --i) {
		if (packet[i]!=0)
			cout << (char)(packet[i]);
	}
	cout << flush;
	cout << hex << uppercase;
}

void packet_get(unsigned long long int packet[64], unsigned char port) {
	memset(packet, 0, 64 * sizeof(packet[0]));
	//unsigned char packetPointer = 0;
	packet[0] = getch();
	/*for (unsigned short i=0;i<0x100;++i) {
		if( GetAsyncKeyState( i ) & 0x8000 )
		{
			packet[packetPointer] = i;
			++packetPointer;
		}
	}*/ //this is old code for the windows virtual machine.
}