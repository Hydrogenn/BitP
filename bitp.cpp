#include <iostream>
#include <vector>
#include <bitset>
#include <math.h>
using namespace std;

class LineState {
	public:
		LineState() :
			variables({0,0}),
			target({0,0}),
			operation(0),
			reading(true),
			looking(true),
			stacking(false),
			targetting(false)
			{}
		LineState(bool i) :
			variables({0,0}),
			target({0,0}),
			operation(0),
			reading(true),
			looking(i),
			stacking(false),
			targetting(false)
			{looking = i;}
		int variables[2];
		int target[2];
		short operation;
		bool reading;
		bool looking;
		bool stacking;
		bool targetting;
};

int const l = 1024;
void parse(bitset<l>& data, LineState& line);
string setScript();
void runScript(bitset<l> script);
short at(bitset<l> data, int i);
long range(bitset<l> data, int i, short length);
bitset<l> setRange(bitset<l> data, long value, int i, short length);
long extern_function(long input);

int main() {
	main:
	bitset<l> compileScript(string script);
	bitset<l> script;
	string scriptString = setScript();
	script = compileScript(scriptString);
	cout << script << "\n";
	runScript(script);
	goto main;
}

string setScript() {
	cout << "type/paste script\n";
	string script;
	cin >> script;
    return script;
}

void parse(bitset<l>& data, LineState& line) {
	switch(line.operation) {
		case 1:
			line.target[0] = line.variables[0];
			line.target[1] = line.variables[1];
			line.targetting = true;
			line.variables[0] = range(data, line.variables[0],line.variables[1]);
			break;
		case 2:
			line.variables[0] = range(data, line.variables[0],line.variables[1]);
			break;
		case 3:
			line.variables[0] <<= line.variables[1];
			break;
		case 4:
			line.variables[0] >>= line.variables[1];
			break;
		case 5:
			line.variables[0] |= line.variables[1];
			break;
		case 6:
			line.variables[0] &= line.variables[1];
			break;
		case 7:
			line.variables[0] ^= line.variables[1];
			break;
		default:
			break;
	}
	line.variables[1] = 0;
	if (line.targetting)
		data = setRange(data,line.variables[0],line.target[0],line.target[1]);
}

bitset<l> compileScript(string script) {
	bitset<l> compiled;
	for(int i = 0; i < script.length(); ++i) {
		char c = script.at(i);
		bool masked = false;
		bitset<4> mask;
		switch (c) {
			case '0':
				mask = 0;
					break;
			case '1':
				mask = 1;
				break;
			case '(':
				mask = 2;
				break;
			case ')':
				mask = 3;
				break;
			case ';':
				mask = 4;
				break;
			case ':':
				mask = 5;
				break;
			case '@':
				mask = 6;
				break;
			case '#':
				mask = 7;
				break;
			case '%':
				mask = 8;
				break;
			case '-':
				mask = 9;
				break;
			case '_':
				mask = 10;
				break;
			case '<':
				mask = 11;
				break;
			case '>':
				mask = 12;
				break;
			case '/':
				mask = 13;
				break;
			case '&':
				mask = 14;
				break;
			case '^':
				mask = 15;
				break;
			default:
				masked = true;
				break;
		}
		if (!masked) {
			for (short o = 0; o < 4; ++ o) {
		    	compiled[4*i+o] = mask[o];
			}
		}
	}
	return compiled;
}

void runScript(bitset<l> script) {
	int pointer = 0;
	vector<LineState> State (1,LineState());
	LineState *Line;
	int *LineVar;
	Line = &State.back();
	int newVar = 0;
	while(pointer < l) {
		LineVar = &Line->variables[Line->stacking?1:0];
		if ((Line->reading && Line->looking) || at(script,pointer)==2 || at(script,pointer)==3 || at(script,pointer)==4) {
			switch (at(script,pointer)) {
				case 0: //0
					*LineVar<<=1;
					break;
				case 1: //1
					*LineVar<<=1;
					*LineVar+=1;
					break;
				case 2: //(
					if (Line->looking && Line->reading) {
						State.push_back(LineState());
					}
					else {
						State.push_back(LineState(false));
					}
					Line = &State.back();
					break;
				case 3: //)
					if (Line->stacking)
						parse(script,*Line);
					newVar = Line->variables[0];
					State.pop_back();
					Line = &State.back();
					Line->variables[Line->stacking?1:0] = newVar;
					break;
				case 4: //;
					if (Line->looking) {
						if (Line->stacking)
							parse(script,*Line);
						*Line = LineState();
					}
					break;
				case 5: //:
					if (Line->stacking)
						parse(script,*Line);
					if (Line->variables[0]==0) {
						Line->reading=false;
					} else {
						*Line = LineState();
					}
					break;
				case 6: //@
					if (Line->stacking)
						parse(script,*Line);
					pointer = 4*Line->variables[0]-4;
					*Line = LineState();
					break;
				case 7: //#
					//TODO bitp.desync();
					break;
				case 8: //%
					if (Line->stacking)
						parse(script,*Line);
					Line->variables[0] = extern_function(Line->variables[0]);
					break;
				case 9: //-
					if (Line->stacking)
						parse(script,*Line);
					Line->stacking = true;
					Line->operation = 1;
					break;
					break;
				case 10: //_
					if (Line->stacking)
						parse(script,*Line);
					Line->stacking = true;
					Line->operation = 2;
					break;
					break;
				case 11: //<
					if (Line->stacking)
						parse(script,*Line);
					Line->stacking = true;
					Line->operation = 3;
					break;
					break;
				case 12: //>
					if (Line->stacking)
						parse(script,*Line);
					Line->stacking = true;
					Line->operation = 4;
					break;
					break;
				case 13: ///
					if (Line->stacking)
						parse(script,*Line);
					Line->stacking = true;
					Line->operation = 5;
					break;
				case 14: //&
					if (Line->stacking)
						parse(script,*Line);
					Line->stacking = true;
					Line->operation = 6;
					break;
				case 15: //^
					if (Line->stacking)
						parse(script,*Line);
					Line->stacking = true;
					Line->operation = 7;
					break;
					break;
				default:
					break;
			}
		}
		LineVar = &Line->variables[Line->stacking?1:0];
		if (Line->stacking)
			cout << Line->variables[0] << ":" << "(" << Line->variables[1] << ")" << ":" << Line->looking << ":" << Line->reading << "\n";
		else
			cout << "(" << Line->variables[0] << ")" << ":" << Line->variables[1] << ":" << Line->looking << ":" << Line->reading << ":" << State.size() << "\n";
		pointer+=4;
	}
}

short at(bitset<l> data, int i) {
	short ret = 0;
	for (short o = 0; o <= 3; ++ o) {
		ret += data[i+o]*pow(2,o);
	}
	return ret;
}
long range(bitset<l> data, int i, short length) {
	short ret = 0;
	for (int o = 0; o <= length; ++ o) {
		ret += data[i+o]*pow(2,o);
	}
	return ret;
}
bitset<l> setRange(bitset<l> data, long value, int i, short length) {
	bitset<64> mask = value;
	for (int o = 0; o <= length; ++ o) {
		data[i+o] = mask[o];
	}
	return data;
}
long extern_function(long input) {
	long output = 0;
	if (input%2) { //if input ends in a 1
		cin >> output;
	} else { //if input ends in a 0
		cout << input << "\n";
	}
	return output;
}