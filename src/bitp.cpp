#include <iostream> //this allows more advanced input/output
#include <fstream> //allows access to files
#include <bitset> //stores the bits
#include <string> //contains to_string().
#include <string.h> //contains memset.
#include <random> //generating arbitrary commands.
using namespace std;

int const l = 16384; //maximum program size. change if you need to run programs bigger than 2 kb (after compilation).
void runScript(bitset<l> &script, unsigned long long pointer, unsigned long long &length);
short at(bitset<l> data, int i);
unsigned long range(bitset<l> data, int i, short length);
bitset<l> setRange(bitset<l> data, unsigned long value, int i, short length);
unsigned long extern_function(unsigned long input, string &previous);
unsigned short r(unsigned short value);
char packet_input(string &previous);
void packet_output(long long unsigned output);

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		cout << "Usage: bitp <file>" << endl;
		return 1;
	}

	bitset<l> compileScript(string script,unsigned long long &length);
	bitset<l> script;
	unsigned long long length = 0;
<<<<<<< HEAD
	cout << "Enter the name of the script you would like to run." << endl;
	string filename;
	cin >> filename;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
=======
	// cout << "Enter the name of the script you would like to run." << endl;
	string filename = string(argv[1]);
	
>>>>>>> origin/master
	ifstream scriptFile (filename.c_str());
	string scriptString;
	string line;
	if (scriptFile.is_open())
	{
		while ( getline (scriptFile,line) )
		{
			scriptString += line + '\n';
		}
		scriptFile.close();
		try {
			script = compileScript(scriptString,length);
<<<<<<< HEAD
			runScript(script,0,length);
			cout << "Done, hit enter to continue";
		} catch (const std::invalid_argument& e) {
=======
			// Yay?
			unsigned long long zeeeeeero = 0;
			runScript(script, zeeeeeero,length);
		} catch (int e) {
>>>>>>> origin/master
			cerr << "Tried to compile an incomplete program!" << endl;
			cerr << e.what() << endl;
			cout << "Failed to load. Hit enter to continue." << endl;
			cin.ignore();
		}
	}
	else {
		cout << "The file could not be opened. Hit enter to continue" << endl;
	};
}

bitset<l> compileScript(string script,unsigned long long &length) {
	bitset<l> compiled;
	cout << hex << uppercase;
	bool skipping = false;
	int i2 = 0;
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist16(0,15); // distribution in range [0, 15]
	for(int i = 0; i < script.length(); ++i) {
		char c = script.at(i);
		bool masked = false;
		bitset<4> mask;
		switch (c) { //This is a good example of how not to code.
			case '#':
				mask = 0x0;
				break;
			case ',':
				mask = 0x1;
				break;
			case '{':
				mask = 0x2;
				break;
			case '}':
				mask = 0x3;
				break;
			case '~':
				mask = 0x4;
				break;
			case '^':
				mask = 0x5;
				break;
			case '&':
				mask = 0x6;
				break;
			case '/':
				mask = 0x7;
				break;
			case '<':
				mask = 0x8;
				break;
			case '>':
				mask = 0x9;
				break;
			case '@':
				mask = 0xA;
				break;
			case '=':
				mask = 0xB;
				break;
			case ':':
				mask = 0xC;
				break;
			case '%':
				mask = 0xD;
				break;
			case '[':
				mask = 0xE;
				break;
			case ']':
				mask = 0xF;
				break;
			case '_':
				mask = dist16(rng);
				break;
			case '*':
				if (!skipping && !masked) {
					string argument_prefix = "TODO found";
					throw std::invalid_argument( argument_prefix );
				}
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
					compiled[4*(i-i2)+3-o] = mask[o];
				}
			}
			else {
				++i2;
			}
		} else {
			++i2;
		}
		length = 4*(i - i2);
	}
	return compiled;
}

void mapTo(char &c, short i) {
	
	switch (i) {
		case 0x0:
			c = '#';
			break;
		case 0x1:
			c = ',';
			break;
		case 0x2:
			c = '{';
			break;
		case 0x3:
			c = '}';
			break;
		case 0x4:
			c = '~';
			break;
		case 0x5:
			c = '^';
			break;
		case 0x6:
			c = '&';
			break;
		case 0x7:
			c = '/';
			break;
		case 0x8:
			c = '<';
			break;
		case 0x9:
			c = '>';
			break;
		case 0xA:
			c = '@';
			break;
		case 0xB:
			c = '=';
			break;
		case 0xC:
			c = ':';
			break;
		case 0xD:
			c = '%';
			break;
		case 0xE:
			c = '[';
			break;
		case 0xF:
			c = ']';
			break;
	}
	
}

void mapFrom(char c, short &mask) {
	
	switch (c) {
		case '#':
			mask = 0x0;
			break;
		case ',':
			mask = 0x1;
			break;
		case '{':
			mask = 0x2;
			break;
		case '}':
			mask = 0x3;
			break;
		case '~':
			mask = 0x4;
			break;
		case '^':
			mask = 0x5;
			break;
		case '&':
			mask = 0x6;
			break;
		case '/':
			mask = 0x7;
			break;
		case '<':
			mask = 0x8;
			break;
		case '>':
			mask = 0x9;
			break;
		case '@':
			mask = 0xA;
			break;
		case '=':
			mask = 0xB;
			break;
		case ':':
			mask = 0xC;
			break;
		case '%':
			mask = 0xD;
			break;
		case '[':
			mask = 0xE;
			break;
		case ']':
			mask = 0xF;
			break;
	}
	
}

void runScript(bitset<l> &script, unsigned long long pointer, unsigned long long &length) {
	unsigned long long variable[8] = {0}; //values
	unsigned char v = 0; //points to the value being currently used
	bool value = false; //tells whether the pointer is looking at a command or a value, for the '#' VALUE command
	//r(v) gets the value behind v, handling overflow
	unsigned long long ilocation = 0; //stores the values to overwrite when using ']' COMMIT
	unsigned short ilength = 0; //stores the number of values to overflow when using ']' COMMIT
	string previous = "";
	unsigned char port = 0; //points to the port for i/o use
	/*
	0 - cout
	1 - cin
	*/
	
	while(pointer <= length) { // ---------------- BEGIN LOOP
	
		/*short gap = 0; debug
		for (short i=0;i<=7;i++) {
			if (i==v)
				cout << "[" << variable[i] << "] : ";
			else {
				if (i > v && variable[i] == 0 && gap != -1) {
					++gap;
				} else {
					if (gap > 0) {
						cout << "..";
						gap = -1;
					}
					cout << variable[i] << " : ";
				}
			}
		}
		if (pointer<l)
			cout << "*" << pointer/4;
		cout << " ";
		short i = at(script, pointer);
		if (value) {
			cout << "#";
			cout << i;
		}
		else if (pointer<l) {
			char c;
			mapTo(c, i);
			cout << "'" << c << "'";
		}
		
		if (ilocation != 0 || ilength != 0) {
			cout << " [" << ilocation;
			cout << "-" << ilength;
			cout << "=" << range(script,ilocation,ilength);
			cout << "]";
		}
		
		cout << endl;
		
		debug */
	
		if (value) {
			variable[v]<<=4;
			variable[v]|=at(script,pointer);
			value = false;
		}
		else {
			switch (at(script,pointer)) {
				case 0x0: //# VALUE
					value = true;
					break;
				case 0x1: //, NEXT
					++v;
					if (v==8)
						v=0;
					break;
				case 0x2: //{ REMEMBER
					ilocation=variable[r(v)];
					ilength=variable[v];
					break;
				case 0x3: //} COMMIT
					script = setRange(script,variable[v],ilocation,ilength);
					if (ilocation + ilength >= length) {
						length = ilocation + ilength;
					}
					break;
				case 0x4: //~ NOT
					variable[v] = ~variable[v];
					break;
				case 0x5: //^ XOR
					variable[r(v)]^=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 0x6: //& AND
					variable[r(v)]&=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 0x7: /// OR
					variable[r(v)]|=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 0x8: //< LSHIFT
					variable[r(v)]<<=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 0x9: //> RSHIFT
					variable[r(v)]>>=variable[v];
					variable[v] = 0;
					v = r(v);
				case 0xA: //@ IF-GOTO
					if (variable[r(v)] != 0) {
						pointer = variable[v]*4 - 4;
					}
					variable[r(v)] = 0;
					variable[v] = 0;
					v = r(v);
					break;
				case 0xB: //= IF-THREAD (alias)
					if (variable[r(v)] != 0) {
						/*The compiler I use is stubborn and refuses to compile the following:
						thread other(runScript, script, variable[v]*4 - 4);
						other.join();
						  so, multithreading need not provide any kind of performance increase.
						  note that if you want to use the above you must also run this at the top:
						#import <thread>
						  and you should comment out or delete this code below.*/
						unsigned long long newPosition = variable[v] * 4 - 4;
						runScript(script, newPosition, length);
						variable[v] = (newPosition + 4) / 4;
					}
					variable[v] = 0;
					v = r(v);
					break;
				case 0xC: //: READ
					variable[r(v)] = range(script,variable[r(v)],variable[v]);
					variable[v] = 0;
					v = r(v);
					break;
				case 0xD: //% PORT
					port = variable[v];
					break;
				case 0xE: //[ INPUT
					variable[v] = packet_input(previous);
					break;
				case 0xF: //] OUTPUT
					packet_output(variable[v]);
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
		ret |= data[i+3-o]<<o;
	}
	return ret;
}
unsigned long range(bitset<l> data, int i, short length) {
	unsigned long ret = 0;
	for (int o = 0; o <= length; ++ o) {
		ret |= data[i+length-o]<<o;
	}
	return ret;
}
bitset<l> setRange(bitset<l> data, unsigned long value, int i, short length) {
	bitset<64> mask = value;
	for (int o = 0; o <= length; ++ o) {
		data[i+length-o] = mask[o];
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

void packet_output(long long unsigned output) {
	cout << nouppercase;
	cout << (char)(output);
	//cout << endl; debug
	cout << flush;
	cout << uppercase;
}

char packet_input(string &previous) {
	cout << dec << nouppercase;
	if (!previous.length()) {
		getline (cin,previous);
		previous += '\0';
	}
	unsigned long ret = previous.at(0);
	previous = previous.substr(1);
	cout << hex << uppercase;
	return ret;
}