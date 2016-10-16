#include <iostream>
#include <fstream>
#include <bitset>
using namespace std;

//'#' DESYNC, ';' LINE-BREAK, and '%' EXTERN are all on the chopping block for changes.
//'#' is rarely, if ever used.
//';' can be replicated by simply having clean code.
//'%' does too many things at once, supposedly being able to handle multiple inputs and outputs. There does not seem to be much viable alternative, however.
int const l = 1024;
string setScript();
void runScript(bitset<l> script);
short at(bitset<l> data, int i);
unsigned long range(bitset<l> data, int i, short length);
bitset<l> setRange(bitset<l> data, unsigned long value, int i, short length);
unsigned long extern_function(unsigned long input, string &previous);
unsigned short r(unsigned short value);

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
	cout << script << "\n";
	runScript(script);
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
	int i2 = 0;
	for(int i = 0; i < script.length(); ++i) {
		char c = script.at(i);
		bool masked = false;
		bitset<4> mask;
		switch (c) {
			case '$':
				mask = 0;
				break;
			case '+':
				mask = 1;
				break;
			case '<':
				mask = 2;
				break;
			case '>':
				mask = 3;
				break;
			case ':':
				mask = 4;
				break;
			case '~':
				mask = 5;
				break;
			case '@':
				mask = 6;
				break;
			case '#':
				mask = 7;
				break;
			case ',':
				mask = 8;
				break;
			case '.':
				mask = 9;
				break;
			case '-':
				mask = 10;
				break;
			case '_':
				mask = 11;
				break;
			case '/':
				mask = 12;
				break;
			case '&':
				mask = 13;
				break;
			case '^':
				mask = 14;
				break;
			case '%':
				mask = 15;
				break;
			default:
				masked = true;
				break;
		}
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
	}
	return compiled;
}

void runScript(bitset<l> script) {
	unsigned int pointer = 0; //points to a location in the script to read
	bool reading = true; //tells whether the command should be reading instructions, for the ':' IF command
	bool value = false; //tells whether the pointer is looking at a command or a value, for the '$' VALUE command
	unsigned long variable[8] = {0,0,0,0,0,0,0,0}; //values
	unsigned short v = 0; //points to the value being currently used
	//r(v) gets the value behind v, handling overflow
	unsigned long target[2] = {0,0}; //stores the values to overwrite when using '-' SELECT and '.' COMMIT
	string previous = ""; //used to handle inputs with more than one character
	while(pointer < l) {
		if (value) {
			if (reading) {
				variable[v]<<=4;
			variable[v]|=at(script,pointer);
			}
			value = false;
		}
		else if (reading || at(script,pointer)==0 || at(script,pointer)==4 || at(script,pointer)==5) {
			switch (at(script,pointer)) {
				case 0: //$
					value = true;
					break;
				case 1: //+
					variable[v]|=1;
					break;
				case 2: //<
					variable[v]<<=1;
					break;
				case 3: //>
					variable[v]>>=1;
					break;
				case 4: //:
					if (!reading)
						reading = true;
					else if (variable[v] == 0) {
						reading = false;
					}
					break;
				case 5: //~
					variable[v] = ~variable[v];
				case 6: //@
					pointer = variable[v]*4;
					pointer -= 4; //this cancels out the addition later, even if there is overflow.
					break;
				case 7: //#
					//TODO bitp.desync();
					break;
				case 8: //,
					++v;
					if (v==8)
						v=0;
					break;
				case 9: //.
					script = setRange(script,variable[v],target[0],target[1]);
					break;
				case 10: //-
					target[0]=variable[r(v)];
					target[1]=variable[v];
					variable[r(v)] = range(script,variable[r(v)],variable[v]);
					variable[v] = 0;
					v = r(v);
					break;
				case 11: //_
					variable[r(v)] = range(script,variable[r(v)],variable[v]);
					variable[v] = 0;
					v = r(v);
					break;
				case 12: ///
					variable[r(v)]|=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 13: //&
					variable[r(v)]&=variable[v];
					variable[v] = 0;
					v = r(v);
					break;
				case 14: //^
					variable[r(v)]^=variable[v];
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
		/*//<debug>
		for (short i=0;i<=7;i++) {
			if (i==v)
				cout << "[" << variable[i] << "] : ";
			else
				cout << variable[i] << " : ";
		}
		if (pointer<l)
			cout << "*" << pointer/4;
		cout << (reading?"..":"//");
		cout << (value?"$ ":". ");
		if (pointer<l)
			cout << at(script,pointer);
		cout << " [" << target[0];
		cout << "-" << target[1];
		cout << "=" << range(script,target[0],target[1]);
		cout << "]";
		cout << endl;
		//</debug>*/
		pointer+=4;
	}
}

short at(bitset<l> data, int i) {
	unsigned short ret = 0;
	for (short o = 0; o <= 3; ++ o) {
		ret += data[i+o]<<o;
	}
	return ret;
}
unsigned long range(bitset<l> data, int i, short length) {
	unsigned long ret = 0;
	for (int o = 0; o <= length; ++ o) {
		ret += data[i+o]<<o;
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
			cin >> previous;
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