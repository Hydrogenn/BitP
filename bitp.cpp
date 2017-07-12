#include <iostream> //this allows more advanced input/output
#include <fstream> //allows access to files
#include <bitset> //stores the bits
#include <string.h> //contains memset.
using namespace std;

int const l = 4096;
string setScript();
void runScript(bitset<l> *script, unsigned long long pointer);
short at(bitset<l> data, int i);
unsigned long range(bitset<l> data, int i, short length);
bitset<l> setRange(bitset<l> data, unsigned long value, int i, short length);
unsigned long extern_function(unsigned long input, string &previous);
unsigned short r(unsigned short value);
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
			case '#':
				mask = 0;
				break;
			case ',':
				mask = 1;
				break;
			case '@':
				mask = 2;
				break;
			case '=':
				mask = 3;
				break;
			case '[':
				mask = 4;
				break;
			case ']':
				mask = 5;
				break;
			case '{':
				mask = 6;
				break;
			case '}':
				mask = 7;
				break;
			case '~':
				mask = 8;
				break;
			case '^':
				mask = 9;
				break;
			case '&':
				mask = 10;
				break;
			case '/':
				mask = 11;
				break;
			case '<':
				mask = 12;
				break;
			case '>':
				mask = 13;
				break;
			case ':':
				mask = 14;
				break;
			case '%':
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
	unsigned long long variable[8] = {0}; //values
	unsigned char v = 0; //points to the value being currently used
	bool value = false; //tells whether the pointer is looking at a command or a value, for the '$' VALUE command
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
	
		/*<debug> this code is commented out in the default compilation.
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
		//</debug>*/
	
		if (value) {
			if (reading) {
				variable[v]<<=4;
			variable[v]|=at(script,pointer);
			}
			value = false;
		}
		else if (reading || at(script,pointer)==0 || at(script,pointer)==5) {
			switch (at(script,pointer)) {
				case 0: //#
					value = true;
					break;
				case 1: //,
					++v;
					if (v==8)
						v=0;
					break;
				case 2: //@
					pointer = variable[v]*4;
					pointer -= 4; //this cancels out the addition later, even if there is overflow.
					break;
				case 3: //= which acts identical to '@' GOTO right now
					//TODO dethread
					pointer = variable[v]*4;
					pointer -= 4;
					break;
				case 4: //[
					if (variable[v] == 0) {
						reading = false;
					}
					break;
				case 5: //]
					reading = true;
					break;
				case 6: //{
					target[0]=variable[v];
					target[1]=variable[r(v)];
					break;
				case 7: //}
					script = setRange(script,variable[v],target[0],target[1]);
					break;
				case 8: //~
					variable[v] = ~variable[v];
					break;
				case 9: //^
					variable[r(v)]^=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 10: //&
					variable[r(v)]&=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 11: ///
					variable[r(v)]|=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 12: //<
					variable[r(v)]<<=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 13: //>
					variable[r(v)]>>=variable[v];
					variable[v] = 0;
					v = r(v);
				case 14: //:
					variable[r(v)] = range(script,variable[v],variable[r(v)]);
					variable[v] = 0;
					v = r(v);
					break;
				case 15: //%
					variable[v] = extern_function(variable[v],previous);
					break;
				default:
					break;
			}
		}
		pointer+=4;
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
}