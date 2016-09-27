/*
 * File:   lexicalAnalyzer.cpp
 * Author: Derek S. Prijatelj
 *
 * Assignment #1: Lexical Analyzer
 */
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <stdio.h>
#include <regex>
#include <execinfo.h>


using namespace std;

/*  The tasks of the Lexical Analyzer:
 *  1) Read in file in order to take in the source code for process
 *  2) Distinguish between different types of tokens
 *  3) Use Hash function to hash the (chaining method for collisions)
 *     ("Array" of vectors) tokens to a Symbol Table.
 *      ¿Hash and ST may be separate?
 *  4)
 *
 *
 *  Read in the file and as it reads it in it will hash the tokens into the Symbol Table.
 */
class Token {
private:
	string name;
	int type, value, size, elType;

public:
	Token(string name);
	Token(string name, int type, int value);
	Token(string name, int type, int value, int size, int elType);
	void setHashVal(int hashVal);
	void setType(int type);
	void setVal(int value);
	void setSize(int size);
	void setElType(int elType);
	int getHashVal();
	string getName();
	int getType();
	int getVal();
	int getSize();
	int getElType();
};
Token::Token(string name) {
	this->name = name;
	this->type = 0;
	this->value = 0;
	size = 0;
	elType = 0;
}
Token::Token(string name, int type, int value) {
	this->name = name;
	this->type = type;
	this->value = value;
	size = 0;
	elType = 0;
}
Token::Token(string name, int type, int value, int size, int elType) {
	this->name = name;
	this->type = type;
	this->value = value;
	this->size = size;
	this->elType = elType;
}
void Token::setType(int type) {
	this->type = type;
}
void Token::setVal(int value) {
	this->value = value;
}
void Token::setSize(int size) {
	this->size = size;
}
void Token::setElType(int elType) {
	this->elType = elType;
}
string Token::getName() {
	return this->name;
}
int Token::getType() {
	return this->type;
}
int Token::getVal() {
	return this->value;
}
int Token::getSize() {
	return this->size;
}
int Token::getElType() {
	return this->elType;
}

//	TYPE
const int INT = 1001; 	// integer
const int INC = 1002; 	// integer constant
const int INV = 1003; 	// integer variable
const int INCa = 1004;		// integer constant array
const int INVa = 1005;		// integer variable array

const int CHAR = 1006; 	// character
const int CHAC = 1007; 	// char const
const int CHAV = 1008;		// char var
const int CHACa = 1009;		// char constant array
const int CHAVa = 1010;		// char variable array

const int RES = 1011; 	// Reserved Words
const int OP = 1012; 	// Operators
const int PUN = 1013;	// PUNctuation
//const int RELOP = 1014; // Relative Operators
const int ID = 1000;	// ID

const int STR = 2010;		// String

//	VALUE
const int NE = 2000; 	// !=
const int LE = 2001; 	// <=
const int GE = 2002; 	// >=
const int EE = 2003; 	// ==
const int OR = 2004; 	// ||
const int AND = 2005; 	// &&
const int OUT = 2006; 	// <<
const int IN = 2007; 	// >>
const int PP = 2008; 	// ++
const int DEC = 2009; 	// --


typedef unordered_map<string, vector<Token> > hashmap;
//vector<string> order;	// I used an unordered map, which means I cannot determine order
// & cannot determine type for IDs w/o another data structure.(nvm: ST.size)
std::vector<std::vector<Token> *> identify;
std::vector<std::vector<std::vector <Token> > > history;
int strCount = 0;
hashmap Chronicles;
string findName(int * type, int * value);
void exp(int * type, int * value, ifstream * fIn, hashmap * SymbolTable, string * result,  ofstream * inter);
void term(int * type, int * value, ifstream * fIn, hashmap * SymbolTable, string * result,  ofstream * inter);
void factor(int * type, int * value, ifstream * fIn, hashmap * SymbolTable, string * result,  ofstream * inter);
void ioAssign(int * ioT, int * ioV, bool output, ifstream * fIn, hashmap * SymbolTable,  ofstream * inter);
void condition(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * falsel, string * skipL,  ofstream * inter);
void WhileLoop(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable,  ofstream * inter);
void IfSeries(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable,  ofstream * inter);
void contentBlock(int * type, int * value, ifstream * fIn, hashmap * SymbolTable,string * result,  ofstream * inter);
void semiConditionPrep(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter);
void RelOp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult, int RoP, string * save,  ofstream * inter);
void orExp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter);
void andExp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter);
void notExp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter);

/*
 * 	The only way this will work for instant access of ID's (Orig: type = index and value = vector index), then I need
 * 	to make it so that the type for ids is a pointer that points to either the hashvalue (impossible) or the index in the
 * 	SymbolTable that the element is located in, which should be doable, because then the pointer would change with the rehash.
 *
 * 	This raises multiple questions:
 * 	How do I point to the index of the element? Is that even possible/will it change with the element or only point to that index?
 * 	How can I change the token object to allow for pointers (large hexadecimal numbers) rather than just an integer?(generic type)
 *
 *	A round about way of doing this: I could simply start from 0 and increment as I add ID's and save that as the ID type. Then
 *	use that type value in an array that points directly to the vector of tokens in which it is stored.
 *			ID type = identify vector index
 *			identify.at(idtype) = ptr to vector of exact id;
 *			When given type and value and type is < 1000, then its an ID and can access identify.at(type) for ptr to vector.
 * 	OR
 * 	I could change the data structure used and simply use a vector of vectors, this would involve having to include the hash
 * 	function into the token object. And this would include changing the way the data struct is called in the Entire Lexer.
 *	So we're not doing the latter for the sake of time and sanity.
 */

// This has to be the inefficient route and it is definitely disgusting and I hate it. I will look into making it so reading
//	from the file can establish these values... This is just... uuugh.... & kills the flexibility of reading in from file.
//	very limiting
const int Const = 3000; 	//
const int If = 3001; 	//
const int Else = 3002; 	//
const int While = 3003; 	//
const int For = 3004; 	//
const int Void = 3005; 	//
const int Main = 3006; 	//
const int Include = 3007; 	//
const int Iostream = 3008; 	//
const int Endl = 3009; 	//
const int Cin = 3010; 	//
const int Cout = 3011; 	//
const int Goto = 3012;
const int Return = 3013;
const int Using = 3014;
const int Namespace = 3015;
const int Std = 3019;
const int Int = 3020;
const int Char = 3021;

void hashID(hashmap * SymbolTable, string * name, int * type, int * value) {
	hashmap::const_iterator got = SymbolTable->find(*name); // SHOULD I DELETE THIS AND ANY OTHER THATS CREATED IN THIS METHOD?
	if (got == SymbolTable->end()) { //Item Not Found in Symbol Table: Add ID to symbol table
		vector<Token> newRow;
		pair<string, vector<Token> > newP(*name, newRow); // The Pair is the key and then a new vector ROW!
		SymbolTable->insert(newP);
		*type = identify.size();
		*value = SymbolTable->at(*name).size();
		Token newToken = Token(*name, *type, *value); // value = 1 cuz its the first in vector
		SymbolTable->at(*name).push_back(newToken);
		identify.push_back(&(SymbolTable->at(*name)));
	} else { //Item exists in Symbol Table: Chain method for collisions. (add to end of vector <Token>)
		if (SymbolTable->at(*name).empty()) {
			//*value = SymbolTable->at(*name).size();
			*type = RES;
			if (name->compare("const") == 0)
				*value = Const;
			else if (name->compare("if") == 0)
				*value = If;
			else if (name->compare("else") == 0)
				*value = Else;
			else if (name->compare("while") == 0)
				*value = While;
			else if (name->compare("for") == 0)
				*value = For;
			else if (name->compare("void") == 0)
				*value = Void;
			else if (name->compare("main") == 0)
				*value = Main;
			else if (name->compare("include") == 0)
				*value = Include;
			else if (name->compare("iostream") == 0)
				*value = Iostream;
			else if (name->compare("endl") == 0)
				*value = Endl;
			else if (name->compare("cin") == 0)
				*value = Cin;
			else if (name->compare("cout") == 0)
				*value = Cout;
			else if (name->compare("goto") == 0)
				*value = Goto;
			else if (name->compare("return") == 0)
				*value = Return;
			else if (name->compare("using") == 0)
				*value = Using;
			else if (name->compare("namespace") == 0)
				*value = Namespace;
			else if (name->compare("std") == 0)
				*value = Std;
			else if (name->compare("int") == 0)
				*value = Int;
			else if (name->compare("char") == 0)
				*value = Char;
			else
				*value = ID;
			Token newToken = Token(*name, RES, *value);
			SymbolTable->at(*name).push_back(newToken);
		} else {
			bool exists = false;
			int index = 0;
			for (int i = 0;
					i < SymbolTable->at(*name).size() && exists == false; i++) {
				if (name->compare(SymbolTable->at(*name).at(i).getName())
						== 0) {
					exists = true;
					index = i;
				}
			}
			if (exists == false) {
				if (SymbolTable->at(*name).at(0).getType() == RES) { // RESword
					*type = RES;
					if (name->compare("const") == 0)
						*value = Const;
					else if (name->compare("if") == 0)
						*value = If;
					else if (name->compare("else") == 0)
						*value = Else;
					else if (name->compare("while") == 0)
						*value = While;
					else if (name->compare("for") == 0)
						*value = For;
					else if (name->compare("void") == 0)
						*value = Void;
					else if (name->compare("main") == 0)
						*value = Main;
					else if (name->compare("include") == 0)
						*value = Include;
					else if (name->compare("iostream") == 0)
						*value = Iostream;
					else if (name->compare("endl") == 0)
						*value = Endl;
					else if (name->compare("cin") == 0)
						*value = Cin;
					else if (name->compare("cout") == 0)
						*value = Cout;
					else if (name->compare("goto") == 0)
						*value = Goto;
					else if (name->compare("return") == 0)
						*value = Return;
					else if (name->compare("using") == 0)
						*value = Using;
					else if (name->compare("namespace") == 0)
						*value = Namespace;
					else if (name->compare("std") == 0)
						*value = Std;
					else if (name->compare("int") == 0)
						*value = Int;
					else if (name->compare("char") == 0)
						*value = Char;
					else
						*value = ID;
					Token newToken = Token(*name, RES, *value);
					SymbolTable->at(*name).push_back(newToken);
				} else { 	//ID
					*type = identify.size();
					*value = SymbolTable->at(*name).size();
					Token newToken = Token(*name, *type, *value);
					SymbolTable->at(*name).push_back(newToken);
					identify.push_back(&(SymbolTable->at(*name)));
				}
			} else { // it does exist, do not store, BUT MUST change the value and type accordingly for output!
				if (SymbolTable->at(*name).at(0).getType() == RES) { // RESword
					*type = RES;
					if (name->compare("const") == 0)
						*value = Const;
					else if (name->compare("if") == 0)
						*value = If;
					else if (name->compare("else") == 0)
						*value = Else;
					else if (name->compare("while") == 0)
						*value = While;
					else if (name->compare("for") == 0)
						*value = For;
					else if (name->compare("void") == 0)
						*value = Void;
					else if (name->compare("main") == 0)
						*value = Main;
					else if (name->compare("include") == 0)
						*value = Include;
					else if (name->compare("iostream") == 0)
						*value = Iostream;
					else if (name->compare("endl") == 0)
						*value = Endl;
					else if (name->compare("cin") == 0)
						*value = Cin;
					else if (name->compare("cout") == 0)
						*value = Cout;
					else if (name->compare("goto") == 0)
						*value = Goto;
					else if (name->compare("return") == 0)
						*value = Return;
					else if (name->compare("using") == 0)
						*value = Using;
					else if (name->compare("namespace") == 0)
						*value = Namespace;
					else if (name->compare("std") == 0)
						*value = Std;
					else if (name->compare("int") == 0)
						*value = Int;
					else if (name->compare("char") == 0)
						*value = Char;
					else
						*value = ID;
					//Token newToken = Token(*name, RES, *value);
					//SymbolTable->at(*name).push_back(newToken);
				} else {					//ID
											//*type = identify.size();
											//*value = SymbolTable->at(*name).size();
					*type = SymbolTable->at(*name).at(index).getType();
					*value = SymbolTable->at(*name).at(index).getVal();
					/*
					 Token newToken = Token(*name, *type, *value);
					 SymbolTable->at(*name).push_back(newToken);
					 identify.push_back(&(SymbolTable->at(*name)));
					 */
				}
			}
		}
	}
	//delete got;//	? 'deletes' when scope is done no?
}
void hashString(hashmap * SymbolTable, string * name, int * type, int * value) {
	hashmap::const_iterator got = SymbolTable->find(*name);
	*type = STR;
	strCount++;
	if (got == SymbolTable->end()) {//Item Not Found in Symbol Table: Add ID to symbol table
			//order.push_back(*name);//save item into order to preserve order.
		vector<Token> newRow;
		*value = strCount;
		Token newToken = Token(*name, *type, *value);// value = 1 cuz its the first in vector
		pair<string, vector<Token> > newP(*name, newRow);// The Pair is the key and then a new vector ROW!
		SymbolTable->insert(newP);
		SymbolTable->at(*name).push_back(newToken);
	} else {//Item exists in Symbol Table: Chain method for collisions. (add to end of vector <Token>)
		bool exists = false;
		for (int i = 0; i < SymbolTable->at(*name).size() && exists == false;
				i++) {
			if (name->compare(SymbolTable->at(*name).at(i).getName()) == 0)
				exists = true;
		}
		if (exists == false) {
			//*value = SymbolTable->at(*name).size() + 1;
			*value = strCount;
			Token newToken = Token(*name, *type, *value);
			SymbolTable->at(*name).push_back(newToken);
		}
	}
	//delete got;//	? 'deletes' when scope is done no?
}

void storeKnown(hashmap * SymbolTable, string * name) {
	/*
	 * Saves reserved word list to symbol table via file that needs
	 * to be in same directory as lexical analyzer that stores the
	 * reserved words 1 per line.
	 */
	ifstream reservedWordList("reservedWords.txt");
	if (reservedWordList.is_open()) {
		while (reservedWordList.good()) {
			getline(reservedWordList, *name);
			//order.push_back(*name);
			hashmap::const_iterator got = SymbolTable->find(*name);	// Need to connect name here to the files through getline()
			if (got == SymbolTable->end()) {//Item Not Found in Symbol Table: Add new blank row.
				vector<Token> newRow;
				pair<string, vector<Token> > newP(*name, newRow);// The Pair is the key and then a new vector ROW!
				SymbolTable->insert(newP);
			} else {//Item exists in Symbol Table: Chain method for collisions. (add to end of vector <Token>)
				bool exists = false;
				for (int i = 0;
						i < SymbolTable->at(*name).size() && exists == false;
						i++) {
					if (name->compare(SymbolTable->at(*name).at(i).getName())
							== 0)
						exists = true;
				}
				if (exists == false) {
					Token newToken = Token(*name);
					SymbolTable->at(*name).push_back(newToken);
				}
			}
			//delete got;//	? 'deletes' when scope is done no?
		}
	} else {
		throw 404;
	}
	reservedWordList.close();
}

bool isAlpha(char c){
	return ('a' <= (int) c && (int) c <= 'z') || ('A' <= (int) c && (int) c <= 'Z');
}
bool isNum(char c){
	return ('0' <= (int) c && (int) c <= '9');
}
bool isAlphaNum(char c){
	return isAlpha(c) || isNum(c);
}

void lexiAna(hashmap * SymbolTable) {
	//Create Symbol Table as a vector of vectors of Token objects:
	//hashmap SymbolTable;
	//SymbolTable.reserve(200);
	string name;

	ofstream lexed("lexed.txt", ofstream::trunc); //file returned from lexer for use with parser

	//	Before reading the file, create hash function. Hash the Reserved Words into the Symbol Table.
	//	When finding sequence of letters, digits and underscores, use hash function to see if there
	//	is already an entry in the SymbolTable. "If so retrieve information needed to generate a token
	//	corresponding to this sequence."
	storeKnown(SymbolTable, &name); //Store all reserved words (known tokens) into the Symbol Table.
	//Now READ
	try {
		ifstream readFile("src.txt");    //this should be a .cpp file
		string line;
		int value = 0, type = 0, count = 0;
		bool dubQ = false, singQ = false, comm = false, id = false; //double Quote, Single Quote, comment
		bool first = true;
		if (!name.empty())
			name.clear();
		if (readFile.is_open()) {
			hashmap::const_iterator got; // Need to connect name here to the files through getline()
			while (readFile.good()) {
				getline(readFile, line); // Get Line from file to read char by char.
				for (int i = 0; i < line.size(); i++) { //readFile char by char and compare through the gauntlet:
					//if 1st char' or string"
					//Comment Blocks
					if (i < line.size() - 1 && line.at(i) == '/'
							&& line.at(i + 1) == '*' && comm == false
							&& i != line.size() - 1) {
						comm = true;
						i++;
						//nameClear(&SymbolTable, &name, &value, &id);
					} else if (i < line.size() - 1 && line.at(i) == '*'
							&& line.at(i + 1) == '/' && comm == true
							&& i != line.size() - 1) {
						comm = false;
						i++;
					}    				// End of Comment Blocks
					else if (comm == false) { // else if the comment is not waiting for '*/', so run this.
						if ((line.at(i) == '$' || line.at(i) == '@'
								|| line.at(i) == '^' || line.at(i) == '`'
								|| line.at(i) == '~') && singQ == false
								&& dubQ == false) {
							//nameClear(&SymbolTable, &name, &value, &id);
							throw 8080;    				//Illegal characters
						}
						//ID
						if ((isAlpha(line.at(i))
								|| (line.at(i) == '_'))
								&& (id == false && singQ == false
										&& dubQ == false)
								&& (i < line.size() - 1)) { // Initial start of ID/word
							id = true;
							name.push_back(line.at(i));
							//cout << " Begin ID ";
						} else if ((isAlphaNum(line.at(i))
								|| (line.at(i) == '_'))
								&& (id == true && singQ == false
										&& dubQ == false)
								&& (i < line.size() - 1)) { // the rest of the ID/word
							name.push_back(line.at(i));
							//cout << " Middle of ID ";
						} else if (( //Special: single character ID at end of line
						(i == line.size() - 1)
								&& (id == false && singQ == false && dubQ == false)
								&& (isAlpha(line.at(i))
										|| (line.at(i) == '_'))) || ( //Special: ID goes to end of line. (i == line.size()-1)
								(id == true && singQ == false && dubQ == false)
										&& (i == line.size() - 1)
										&& (isAlphaNum(line.at(i))
												|| (line.at(i) == '_')))) {
							name.push_back(line.at(i));
							hashID(SymbolTable, &name, &type, &value);
							if (first){
								lexed << left << setw(5) << type << value;
								first = false;
								break;// end of line therefore end of loop.
							}
							else
								lexed << endl << left << setw(5) << type << value;
							//printf("%*i %*i %*s\n",5, type, 15, value, -10, name.c_str() );
							name.clear();
							id = false;
						} else if ( //TERMINAL Condition: Ends if not correct char
						(id == true && singQ == false && dubQ == false) //standard statement
								&& !(isAlphaNum(line.at(i))
										|| (line.at(i) == '_'))) {
							hashID(SymbolTable, &name, &type, &value);
							if (first){
								lexed << left << setw(5) << type << value;
								first = false;
							}
							else
								lexed << endl << left << setw(5) << type << value;
							//printf("%*i %*i %*s\n",5, type, 15, value, -10, name.c_str() );
							name.clear();
							id = false;
						}    				// End of IDs
											//Strings
						if (line.at(i) == '\"' && dubQ == false
								&& i != line.size() - 1) {
							dubQ = true;
							//nameClear(&SymbolTable, &name, &value, &id);
						} else if (dubQ == true && line.at(i) != '\"'
								&& i != line.size() - 1) {
							name.push_back(line.at(i));
						} else if (dubQ == true && line.at(i) == '\"') {
							hashString(SymbolTable, &name, &type, &value);
							if (first){
								lexed << left << setw(5) << type << value;
								first = false;
							}
							else
								lexed << endl << left << setw(5) << type << value;
							//printf("%*i %*i %*s\n",5, type, 15, value, -10, name.c_str() );
							name.clear();
							dubQ = false;
						} else if ((line.at(i) != '\"' && dubQ == true
								&& i == line.size() - 1)
								|| (line.at(i) == '\"' && dubQ == false
										&& i == line.size() - 1)) {
							throw 666;
							dubQ = false;
						}	    				// End of Strings
												//Characters
						if (line.at(i) == '\'' && singQ == false
								&& i != line.size() - 1) {
							singQ = true;
							//nameClear(&SymbolTable, &name, &value, &id);
						} else if (singQ == true & count < 1) {
							count++;
							name.push_back(line.at(i));
							value = line.at(i);
						} else if (singQ == true && count == 1
								&& line.at(i) == '\'') {
							if (first){
								lexed << left << setw(5) << CHAR << value;
								first = false;
							}
							else
							lexed  << endl << left << setw(5) << CHAR << value;
							//printf("%*i %*i %*s\n",5, CHAR, 15, value, -10, name.c_str() );
							name.clear();
							count = 0;
							singQ = false;
						} else if ((singQ == true && count >= 2)
								|| (singQ == true && count == 1
										&& line.at(i) != '\'')
								|| (line.at(i) == '\'' && singQ == false
										&& i == line.size() - 1)) {
							throw 667;
							singQ = false;
							count = 0;
						}	    				// End of Characters
												//OPERATORS
						if ((line.at(i) == '+' || line.at(i) == '-'
								|| line.at(i) == '*' || line.at(i) == '/'
								|| line.at(i) == '%' || line.at(i) == '='
								|| line.at(i) == '>' || line.at(i) == '<'
								|| line.at(i) == '!' || line.at(i) == '&'
								|| line.at(i) == '|') && singQ == false
								&& dubQ == false) {					//Operators
								//nameClear(&SymbolTable, &name, &value, &id);
							if (i < (line.size() - 1) && line.at(i) == '+'
									&& line.at(i + 1) == '+') {	//First handle Doubles
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = PP;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '-'
									&& line.at(i + 1) == '-') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = DEC;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '='
									&& line.at(i + 1) == '=') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = EE;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '>'
									&& line.at(i + 1) == '=') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = GE;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '<'
									&& line.at(i + 1) == '=') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = LE;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '!'
									&& line.at(i + 1) == '=') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = NE;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '&'
									&& line.at(i + 1) == '&') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = AND;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '|'
									&& line.at(i + 1) == '|') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = OR;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '<'
									&& line.at(i + 1) == '<') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = OUT;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '>'
									&& line.at(i + 1) == '>') {
								name.push_back(line.at(i));
								name.push_back(line.at(i + 1));
								value = IN;
								i++;
							} else if (i < (line.size() - 1)
									&& line.at(i) == '/'
									&& line.at(i + 1) == '/') {	// HANDLE THE COMMENT
								line.clear();
								break;
							} else {		// Handle Singles Now, not doubles.
								if (line.at(i) == '&' || line.at(i) == '|') {
									throw 700;// illegal characters when by themselves(not in str or char).
								} else {			// any other legal operator
									name.push_back(line.at(i));
									value = line.at(i);
								}
							}
							if (first){
								lexed << left << setw(5) << OP << value;
								first = false;
							}
							else
								lexed << endl << left << setw(5) << OP << value;
							//printf("%*i %*i %*s\n",5, OP, 15, value, -10, name.c_str() );
							name.clear();
						}							// END OPERATORS.
													//PUNCTUATION & DELIMITERS
						if ((line.at(i) == ';' || line.at(i) == '('
								|| line.at(i) == ')' || line.at(i) == '['
								|| line.at(i) == ']' || line.at(i) == ','
								|| line.at(i) == '.' || line.at(i) == '{'
								|| line.at(i) == '}' || line.at(i) == '#')
								&& singQ == false && dubQ == false) {
							/*if (line.at(i)==';'){
							 name.push_back(line.at(i));
							 value = line.at(i);
							 }
							 else{*/
							name.push_back(line.at(i));
							value = line.at(i);
							//	}
							if (first){
								lexed << left << setw(5) << PUN << value;
								first = false;
							}
							else
							lexed << endl << left << setw(5) << PUN << value;
							//printf("%*i %*i %*s\n",5, PUN, 15, value, -10, name.c_str());
							name.clear();
						}	    				// END of PUNCTUATION...

						//INTEGERS
						if (isNum(line.at(i))
								&& singQ == false && dubQ == false
								&& id == false) {
							if (i < (line.size() - 1)
									&& isNum(line.at(i+1))) {
								name.push_back(line.at(i));
								value = atoi(name.c_str());
							} else {
								name.push_back(line.at(i));
								value = atoi(name.c_str());
								if (first) {
									lexed << left << setw(5) << INT << value;
									first = false;
								} else
									lexed << endl << left << setw(5) << INT
											<< value;
								//printf("%*i %*i\n",5, INT, 15, value);
								name.clear();
							}
						}	    				// END of INTEGERS
					}// End of Else for the Block comments. (else = if comm==false & current & next != '/*')
					 // Reserved Words
					 // Identifiers
					 // Integers
					 // Operators
					 // Characters
					 // Strings
					 // Punctuation
				}
			}
			//delete got;//	? 'deletes' when scope is done no?
		} else {
			throw 404;
		}
		//......................
		readFile.close();    				// Close the file and quit reading.
		if (comm == true) { // if comment block left open throw error because end file and no closing '*/'
			throw 668;
		}
	} catch (int e) {
		if (e == 404)
			cout << "\nError: Unable to open File!\n";
		else if (e == 666)
			cout << "\nError: Unterminated String!\n";
		else if (e == 667)
			cout << "\nError: Unterminated Character!\n";
		else if (e == 668)
			cout << "\nError: Unterminated Comment!\n";
		else
			cout << "\nError: Illegal Character!\n";
		//return SymbolTable;// return the Symbol Table at time of error.
	}
	// Done now for the Lexical Analyzer part!

	lexed.close();
	//delete lexed;//	? 'deletes' when scope is done, no?
	//delete name;//	? 'deletes' when scope is done, no?
	//return SymbolTable;
}

void getLineTV(ifstream * readFile, string * line, string * type,
		string * value) {
	getline(*readFile, *line);    // Get Line from file to read char by char.
	if (!readFile->eof() && line->size() > 5) {
		*type = line->substr(0, (line->find_first_of(" ")));
		*value = line->substr(5);
	} else {
		*type = "-1";
		*value = "-1";
	}
}
void getLineTVi(ifstream * readFile, int * type,
		int * value) {
	string line;
	getline(*readFile, line);    // Get Line from file to read char by char.
	if (!readFile->eof() && line.size() > 5) {
		*type = atoi(line.substr(0, (line.find_first_of(" "))).c_str());
		*value = atoi(line.substr(5).c_str());
	} else {
		*type = -1;
		*value = -1;
	}
}
/*
 * parseDec:
 * 	meant to parse the declarations of the entire source code provided via file. Modifies the existing symbol table.
 */
void parseDec(hashmap * SymbolTable) {
	//open file for input:
	try {
		ifstream readFile("lexed.txt");    //this should be a .cpp file
		string line, type, value;
		int t1, t2, v1, v2, v3, v4;
		bool begin = false, con = false, var = false, id = false, noBracket =
				true, arSize = false, init = false, val = false;
		//vector <string> visitedID;
		//vector <std::vector <Token> *> visitedID;
		if (readFile.is_open()) {
			while (readFile.good()) {
				getLineTV(&readFile, &line, &type, &value);
				t1 = atoi(type.c_str());
				v1 = atoi(value.c_str());
				if (t1 != -1 && v1 != -1) {
					//cout << "type =" << type << " value =" << value << endl;
					/*
					 printf("%*s %*s %*s %*s %*s %*i %*s %*i %*s %*i %*s %*i %*s %*i %*s %*i %*s %*i %*s %*i\n",
					 -5, "type :", -5, type.c_str(), -5, "value :", -5, value.c_str(),
					 -5, "begin :", -5, begin, -5, "con :", -5, con, -5, "var :", -5, var,
					 -5, "id :", -5, id, -5, "noBracket :", -5, noBracket, -5, "arSize :", -5, arSize,
					 -5, "init :", -5, init, -5, "val :", -5, val);
					 */
					//	BEGIN
					if (begin == false) {// If declaration not started, then begin parsing
						if (t1 == RES
								&& (v1 == Const || v1 == Int || v1 == Char)) {// first token = reserved word (const or var), correct
							if (v1 == Const) {
								con = true;
								begin = true;
							} else {
								//var = true;
								begin = true;
								// Must handle if a variable is first.
								if (v1 == Int || v1 == Char) {//if a var then good.
								//cout << "\nInt or Char v1 = \n" << v1 << endl;
									var = true;
									v3 = v1;
								} else {// not Const or correct Var, so ew, error.
									throw 706;
									break;
								}
							}
						} // else. ignore: not potential declaration yet.
					} // END BEGIN

					// VAR
					else if (var == false) { //begun, second token either res word (vartype) or id
						if (t1 == RES) {
							if (v1 == Int || v1 == Char) { //if a var then good.
							//cout << "\nInt or Char v1 = \n" << v1 << endl;
								var = true;
								v3 = v1;
							} else {	// not correct Var, so ew, error.
								throw 706;
								break;
							}
						} else {
							throw 701;
							break;
						}
					}	// END VAR

					// ID
					else if (id == false) {	// we have gotten past the beginning and Var gate, looking for ID.
						//cout << "\nv3 = " << v3 << endl;
						//else{
						if (t1 < 1000) {// ids are the only ones that are below 1000.		When checking ids also check if already changed, if so, then declared twice error.
							if ( /*t1 < identify.size() && v1 < identify.at(t1)->size() &&*/
							(identify.at(t1)->at(v1).getType() == INC
									|| identify.at(t1)->at(v1).getType() == INV
									|| identify.at(t1)->at(v1).getType() == CHAC
									|| identify.at(t1)->at(v1).getType() == CHAV)) {// check if already changed.
								throw 705;
								break;
							} else {
								if (con == true) {
									if (v3 == Int) {
										identify.at(t1)->at(v1).setType(INC);
										//identify.at(t1)->at(v1).setVal(0);// set default value
										t2 = t1;
										v2 = v1;
										//visitedID.push_back(identify.at(t1)->at(v1).getName());
										//visitedID.push_back(identify.at(t1));
										v4 = INC;
									} else if (v3 == Char) {
										identify.at(t1)->at(v1).setType(CHAC);
										//identify.at(t1)->at(v1).setVal(0);// set default value
										t2 = t1;
										v2 = v1;
										//visitedID.push_back(identify.at(t1)->at(v1).getName());
										//visitedID.push_back(identify.at(t1));
										v4 = CHAC;
									} else {
										throw 706;
										break;
									}
								} else {
									if (v3 == Int) {
										identify.at(t1)->at(v1).setType(INV);
										//identify.at(t1)->at(v1).setVal(0);// set default value
										t2 = t1;
										v2 = v1;
										//visitedID.push_back(identify.at(t1)->at(v1).getName());
										//visitedID.push_back(identify.at(t1));
										v4 = INV;
									} else if (v3 == Char) {
										identify.at(t1)->at(v1).setType(CHAV);
										//identify.at(t1)->at(v1).setVal(0);// set default value
										t2 = t1;
										v2 = v1;
										//visitedID.push_back(identify.at(t1)->at(v1).getName());
										//visitedID.push_back(identify.at(t1));
										v4 = CHAV;
									} else {
										throw 706;
										break;
									}
								}
								id = true; //identify.at(t1)->at(v1).setVal(); have to set the value after the op and during
							}

						} else if (t1 == RES) {	// the word after variable is a RES word, but is it an acceptable RES word?
							if (v1 == Main) {
								if (con == true) {
									throw 707;//	Cannot declare constant in a method.
									break;
								}
								break;// end the search for declarations making this only work on declarations outside of method.
								var = false;
								con = false;
								begin = false;
							} else {
								cout << "v1 = " << v1;
								throw 700;	// Res when expecting ID.
								break;
							}
						} else {
							cout << "\nt1 = " << t1 << " v1 = " << v1;
							throw 702;	// Anything but an ID.
							break;
						}

						//}

					}	// END ID

					// INIT
					else if (init == false) {// begin, var, id all = true. Now either endsign';', comma ',', or operator '='
						if (t1 == PUN && v1 == ';') {	// end the declaration.
							if (con) {
								cout << "\n HERE A";
								throw 704;
								break;
							} else {// end of variable declaration. reset all bools
								begin = false;
								var = false;
								id = false;
							}
						} else if (t1 == OP && v1 == '=') {
							init = true;
						} else if (t1 == PUN && v1 == ',') {	// commas
							if (con) {
								cout << "\n HERE B";
								throw 704;
								break;
							} else {// end of variable declaration. reset all bools
								id = false;	// id should be next if comma present
							}
						} else if (t1 == PUN && v1 == '[') {// brackets are read in as : PUN (1013) 91'[' and 93 ']'
						//cout << "\n Bracket begin \n";
							noBracket = false; //open up gate
							init = true; // close init gate
						} else {
							cout << "\n HERE C";
							throw 704;
							break;
						}
					} // END INIT

					// noBracket && arSize
					else if (noBracket == false && arSize == false) { //if the id was an array and we are checking for arSize
						if (t1 == INT) {
							//noBracket = true;
							arSize = true;
							//cout << "\nt2 = " << t2 << " v2 = "<< v2 << " name = " << identify.at(t2)->at(v2).getName();
							identify.at(t2)->at(v2).setSize(v1); // set size to v1 (the value of integer)
							identify.at(t2)->at(v2).setElType(v3); // set ElType to v3 (the variable type for current declaration)
							identify.at(t2)->at(v2).setVal(0); // set value to 0
							if (v4 == INC) {
								identify.at(t2)->at(v2).setType(INCa); // set type to correct array version.
							} else if (v4 == INV) {
								identify.at(t2)->at(v2).setType(INVa); // set type to correct array version.
							} else if (v4 == CHAC) {
								identify.at(t2)->at(v2).setType(CHACa); // set type to correct array version.
							} else if (v4 == CHAV) {
								identify.at(t2)->at(v2).setType(CHAVa); // set type to correct array version.
							} else {
								cout << "\nv3 = " << v3 << " t2 = " << t2
										<< " v2 = " << v2 << " identify type = "
										<< identify.at(t2)->at(v2).getType()
										<< endl;
								throw 703;
								break;
							}
						}
						else if (t1 == PUN && v1 == ']'){
							//noBracket = true;
							//arSize = true;
							//cout << "\nt2 = " << t2 << " v2 = "<< v2 << " name = " << identify.at(t2)->at(v2).getName();
							identify.at(t2)->at(v2).setSize(-1); // set size to v1 (the value of integer)
							identify.at(t2)->at(v2).setElType(v3); // set ElType to v3 (the variable type for current declaration)
							identify.at(t2)->at(v2).setVal(0); // set value to 0
							if (v4 == INC) {
								identify.at(t2)->at(v2).setType(INCa); // set type to correct array version.
							} else if (v4 == INV) {
								identify.at(t2)->at(v2).setType(INVa); // set type to correct array version.
							} else if (v4 == CHAC) {
								identify.at(t2)->at(v2).setType(CHACa); // set type to correct array version.
							} else if (v4 == CHAV) {
								identify.at(t2)->at(v2).setType(CHAVa); // set type to correct array version.
							} else {
								cout << "\nv3 = " << v3 << " t2 = " << t2
										<< " v2 = " << v2 << " identify type = "
										<< identify.at(t2)->at(v2).getType()
										<< endl;
								throw 703;
								break;
							}
							noBracket = true;
							arSize = false;
						}
						else {
							cout << "\n HERE D";
							cout << "\nt1 = "<<t1 << "v1 = "<<v1<< endl;
							throw 704; // no ending bracket.
							break;
						}
					} // end arSize

					//	noBracket
					else if (noBracket == false && arSize == true) { //if the id was an array and we are checking for end BRACKET
						if (t1 == PUN && v1 == ']') {
							noBracket = true;
							arSize = false;
						} else {
							cout << " TEST ME \n";
							throw 704; // no ending bracket.
							break;
						}
					} // END Brackets

					//	VAL
					else if (val == false) { // begin, var, id, init = true; id | # | char
						if (t1 < 1000) { // if value is an id
							val = true;
							identify.at(t2)->at(v2).setVal(
									identify.at(t1)->at(v1).getVal()); // set value to id's value.
						} else if (t1 == INT) { // if value is an INT
							val = true;
							identify.at(t2)->at(v2).setVal(v1); // set value to integer value
						} else if (t1 == CHAR) { // if value is an CHAR
							val = true;
							identify.at(t2)->at(v2).setVal(v1); // set value to char value
						} else if (t1 == PUN && v1 == ';') { // if no value, then there is the end signs.
							begin = false;
							con = false;
							var = false;
							id = false;
							init = false;
							val = false;
						} else if (t1 == PUN && v1 == ',') {
							id = false;
							init = false;
							val = false;
						} else { // incorrect value type;
							throw 706; // or 704. . .
							break;
						}
					} // END VALUE

					// Else (end of declaration)
					else { // else, the end of the declaration or comma, so check for a ';' || ','
						if (t1 == PUN && v1 == ';') { // reset all the bools
							begin = false;
							con = false;
							var = false;
							id = false;
							init = false;
							val = false;
						} else if (t1 == PUN && v1 == ',') { // if comma then an id follows
							id = false;
							init = false;
							val = false;
						} else {
							throw 704;
							break;
						}
					} // END Else (End of Declarations)
				} // end of type -1 val -1 end of file conditional
			}
			//print Error message if . . .
			// Reserved Word is treated as Var or Const.
			// Var or const is declared twice
			// illegal type name is used. (only accept in src programs: const int, const char, int and char)
			/* Syntax Error
			 -missing semicolon after a list of declared var;
			 -missing equal sign in a definition of const;
			 -missing comma between var & const.
			 -for arrays
			 -missing opening/closing bracket;
			 -missing illegal size indicator;
			 */
			readFile.close();
		} else
			throw 404;
	} catch (int e) {
		if (e == 404)
			cout << "\nError " << e << ": Unable to open File!\n";
		else if (e == 700)
			cout << "\nError " << e << ": Expected id, found Reserved Word!\n";
		else if (e == 701)
			cout << "\nError " << e
					<< ": Expected variable type, token found not a variable\n";
		else if (e == 702)
			cout << "\nError " << e
					<< ": Expected id, token found not an id!\n";
		else if (e == 703)
			cout << "\nError " << e
					<< ": Illegal type name used in source program!\n";
		else if (e == 704)
			cout << "\nError " << e << ": Syntax Error!\n";
		else if (e == 705)
			cout << "\nError " << e
					<< ": Variable or constant declared twice!\n";
		else if (e == 706)
			cout << "\nError " << e
					<< ": Illegal value type used in source program!\n";
		else
			cout << "\nError " << e
					<< ": Constants must be initialized in declarations!\n";
	}
}

/*
 *	parseExp:
 *		Parses expressions, assignments, and I/O statements.
 *		Constructs parse tree linearly in quadruples format. Generates new Temporaries using the function newTemp().
 *
 */
// generates new temporaries for use with parse tree. Takes the OP and the values, and does the algebra

bool isVar(int type){
	return type == INV || type == CHAV;
}
bool isCon(int type){
	return type == INC || type == CHAC;
}
bool isVarA(int type){
	return type == INVa || type == CHAVa;
}
bool isConA(int type){
	return type == INCa || type == CHACa;
}

bool isVarST(int type){
	return isVarA(type) || isVar(type);
}
bool isConST(int type){
	return isConA(type) || isCon(type);
}

bool isArrST(int type){
	return isConA(type) || isVarA(type);
}

bool isArrID(hashmap * SymbolTable, int * t, int * v){
	return isArrST((int)SymbolTable->at(findName(t, v)).at(*v).getType());
}

bool isVarID(hashmap * SymbolTable, int * t, int * v){
	return isVarST((int)SymbolTable->at(findName(t, v)).at(*v).getType());
}
bool isConID(hashmap * SymbolTable, int * t, int * v){
	return isConST((int)SymbolTable->at(findName(t, v)).at(*v).getType());
}
bool isDeclaredID(hashmap * SymbolTable, int * t, int * v){
	return isVarID(SymbolTable, t, v) || isConID(SymbolTable, t, v);
}
int tmpNum = 0;
int totalTmp = 0;	//	 store the total number of different temporary variables made by compiler.
string arg1;// the string used to print out the entire quadruple. saves the content
//	in order to relax the necessary order to print, otherwise limited by needing to print out result after setting it.
string newTmp() {
	string tmp = "_t_" + to_string(tmpNum);
	tmpNum++;//increments for next temporary.
	if (tmpNum > totalTmp)
		totalTmp++;
	return tmp;
}
void resetTmp() {
	tmpNum = 1;// resets temporaries
}

//	Checks whether or not the next type and value in file is the desired Type and Value,
//		returns the value and moves forward in stream
int nextIs(int codeType, int codeValue, ifstream * fIn) {
	int whatType, whatValue;
	streampos pos = fIn->tellg();
	getLineTVi(fIn, &whatType, &whatValue);
	if (whatType == codeType && whatValue == codeValue)
		return codeValue;
	if (fIn->eof())
		fIn->clear();
	fIn->seekg(pos, ios::beg);
	return 0;
}
int nextIsT(int codeType, ifstream * fIn) {// for type check
	int whatType, waste;
	streampos pos = fIn->tellg();
	getLineTVi(fIn, &whatType, &waste);
	if (whatType == codeType)
		return codeType;
	if (fIn->eof())
		fIn->clear();
	fIn->seekg(pos, ios::beg);
	return 0;
}

// <exp>:= <term> {+ <term> | - <term>}
void exp(int * type, int * value, ifstream * fIn, hashmap * SymbolTable, string * result,  ofstream * inter) {
	int op;
	//string arg1 = *result, arg2 = *result;
	string arg2 = *result;

	bool negate1 = false, negate2 = false;
	while (*type == OP && *value == '-') {
		getLineTVi(fIn, type, value);// move to next token and continue, but remember to negate the outcome of this result.
		negate1 = !negate1;
	}
	term(type, value, fIn, SymbolTable, result, inter);			// first argument
	if(negate1){
		//printf("%*c", -5, '-');								// Operation Print Out
		*inter << left << setw(7) << '-';
		//printf("%*s", -14, result->c_str());				// First Argument Print Out
		*inter << left << setw(14) << result->c_str();
		*result = newTmp();
		//printf("%*s \n", -5, result->c_str());	// need to figure out how to save the tmp correctly for next quadruple.
		*inter << left << result->c_str() << endl;
	}

	while ((op = nextIs(OP, '+', fIn)) || (op = nextIs(OP, '-', fIn))) {

		getLineTVi(fIn, type, value);						// move forward in stream to next token (the one after the op sign)
		while (*type == OP && *value == '-') {
			getLineTVi(fIn, type, value);// move to next token and continue, but remember to negate the outcome of this result.
			negate2 = !negate2;
		}
		term(type, value, fIn, SymbolTable, &arg2, inter);			// second argument

		if (negate2) {
			//printf("%*c", -5, '-');						// Operation Print Out
			*inter << left << setw(7) << '-';
			//printf("%*s", -14, arg2.c_str());	// First Argument Print Out
			*inter << left << setw(14) << arg2.c_str();
			arg2 = newTmp();
			//printf("%*s \n", -5, arg2.c_str());// need to figure out how to save the tmp correctly for next quadruple.
			*inter << left << arg2.c_str() << endl;
		}

		//print out for exp
		//printf("%*c", -5, op);								// Operation Print Out
		*inter << left << setw(7) << (char)op;
		//printf("%*s", -7, result->c_str());					// First Argument Print Out
		*inter << left << setw(7) << result->c_str();
		//printf("%*s", -7, arg2.c_str());					// Second Argument Print out
		*inter << left << setw(7) << arg2.c_str();
		*result = newTmp();
		//printf("%*s \n", -5, result->c_str());	// need to figure out how to save the tmp correctly for next quadruple.
		*inter << left << result->c_str() << endl;
	}
}
// <term>:= <factor> {* <factor> | / <factor>}
void term(int * type, int * value, ifstream * fIn, hashmap * SymbolTable, string * result,  ofstream * inter){
	int op;
	//string arg1 = *result, arg2 = *result;
	string arg2 = *result;

	bool negate1 = false, negate2 = false;
	while (*type == OP && *value == '-') {
		getLineTVi(fIn, type, value);// move to next token and continue, but remember to negate the outcome of this result.
		negate1 = !negate1;
	}
	factor(type, value, fIn, SymbolTable, result, inter);			// first argument
	if(negate1){
		//printf("%*c", -5, '-');								// Operation Print Out
		*inter << left << setw(7) << '-';
		//printf("%*s", -14, result->c_str());				// First Argument Print Out
		*inter << left << setw(14) << result->c_str();
		*result = newTmp();
		//printf("%*s \n", -5, result->c_str());	// need to figure out how to save the tmp correctly for next quadruple.
		*inter << left << result->c_str() << endl;
	}

	while ((op = nextIs(OP, '*', fIn)) || (op = nextIs(OP, '/', fIn)) || (op = nextIs(OP, '%', fIn))) {

		getLineTVi(fIn, type, value);						// move forward in stream to next token (the one after the op sign)
		while (*type == OP && *value == '-') {
			getLineTVi(fIn, type, value);// move to next token and continue, but remember to negate the outcome of this result.
			negate2 = !negate2;
		}
		factor(type, value, fIn, SymbolTable, &arg2, inter);			// second argument

		if (negate2) {
			//printf("%*c", -5, '-');						// Operation Print Out
			*inter << left << setw(7) << '-';
			//printf("%*s", -14, arg2.c_str());	// First Argument Print Out
			*inter << left << setw(14) << arg2.c_str();
			arg2 = newTmp();
			//printf("%*s \n", -5, arg2.c_str());// need to figure out how to save the tmp correctly for next quadruple.
			*inter << left << arg2.c_str() << endl;
		}

		//printf("%*c", -5, op);								// Operation Print Out
		*inter << left << setw(7) << (char)op;
		//printf("%*s", -7, result->c_str());					// First Argument Print Out
		*inter << left << setw(7) << result->c_str();
		//printf("%*s", -7, arg2.c_str());					// Second Argument Print out
		*inter << left << setw(7) << arg2.c_str();
		*result = newTmp();
		//printf("%*s \n", -5, result->c_str());	// need to figure out how to save the tmp correctly for next quadruple.
		*inter << left << result->c_str() << endl;
	}
}
// <factor>:= id | int | (<exp>)
void factor(int * type, int * value, ifstream * fIn, hashmap * SymbolTable, string * result,  ofstream * inter){
	if((*type == PUN && *value == '(')){
		getLineTVi(fIn, type, value);// move forward in stream to next token (the one after '(' ) and pass to exp
		exp(type, value, fIn, SymbolTable, result, inter);// if ( <exp> )
		// check for end parenthsis here, NOT anywhere else, this will contain the state of parenthesis
		if ( ')' != nextIs(PUN, ')', fIn)){
			cout << "\nIn Factor:\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = "
					<< *value;
			throw 813;
		}// skip this token
	}
	else if (*type < 1000){
		//arg(type, value, SymbolTable);//	if ID | INT | CHAR
		if(isArrID(SymbolTable, type, value)){// if the variable is declared as an array
			if('[' == nextIs(PUN, '[', fIn)){
				// now set onto token after '['
				string arrName = findName(type, value);
				getLineTVi(fIn, type, value);
				exp(type, value, fIn, SymbolTable, result, inter);
				// print out the index quadruple.
				string arr = newTmp();
				//printf("%*c%*s%*s\n", -5, '=', -14, result->c_str(), -5, arr.c_str());
				*inter << left << setw(7) << '=' << setw(14) << result->c_str() << arr.c_str() << endl;
				if (']' != nextIs(PUN, ']', fIn)) {
					cout << "\nInside Factor:\ntype = " << *type << " value = " << *value;
					getLineTVi(fIn, type, value);
					cout << "\nNextIs type = " << *type << " value = "
							<< *value;
					throw 807;
				}
				*result = newTmp();
				//printf("%*s%*s%*s%*s\n", -5, "=[]", -7, arrName.c_str(), -7, arr.c_str(), -5, result->c_str());
				*inter << left << setw(7) << "=[]" << setw(7) << arrName.c_str() << setw(7) << arr.c_str() << result->c_str() << endl;
			}
			else{
				cout << "\ntype = " << *type << " value = " << *value;
				getLineTVi(fIn, type, value);
				cout << "\nNextIs type = " << *type << " value = " << *value;
				throw 808;
			}
		}
		else if(isDeclaredID(SymbolTable, type, value)){
			*result = findName(type, value);
		}
		else{
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = " << *value;
			throw 805;
		}
	}
	else if ( *type == INT || *type == CHAR){
		*result = to_string(*value);
	}

}
void assignment (int * type, int * value, ifstream * fIn, hashmap * SymbolTable,  ofstream * inter){
	string result, assign;
	if ('=' == nextIs(OP, '=', fIn)) { // if next token is an equals sign, therefore assignment
		assign = findName(type, value); // save assignment token for the very last token to be printed.
		result = assign;
		getLineTVi(fIn, type, value); // set onto next token
		exp(type, value, fIn, SymbolTable, &result, inter);
		//printf("%*c%*s%*s\n", -5, '=', -14, result.c_str(), -5, assign.c_str());
		*inter << left << setw(7) << '=' << setw(14) << result.c_str() << assign.c_str() << endl;
		if (';' != nextIs(PUN, ';', fIn)) {
			throw 814; // missing semi-colon at end of assignment.
		}
	}
	// Array Assignment: Checks through whole array and checks for '=', if no '=' then do nothing. This would be inefficient, but works
	//	in order to be more efficient, make a safe way of checking if there is an equals sign at the end of this w/o losing place in
	//	the stream if there is an = sign, because then you must reset to proper location and evaluate the expresion within the array,
	//	but this would get rid of evaluating array when there is no reason to (no '=' sign)
	else if ('[' == nextIs(PUN, '[', fIn)){
		assign = findName(type, value); // save array assignment token for the very last token to be printed.
		result = assign;
		getLineTVi(fIn, type, value); // set onto next token (the begining of the exp in [brackets])
		exp(type, value, fIn, SymbolTable, &result, inter);// set result = to end of exp.
		string arr = newTmp();
		//printf("%*c%*s%*s\n", -5, '=', -14, result.c_str(), -5, arr.c_str());
		*inter << left << setw(7) << '=' << setw(14) << result.c_str() << arr.c_str() << endl;
		if (']' != nextIs(PUN, ']', fIn)) {
			cout << "\nInside Factor:\ntype = " << *type << " value = "
					<< *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = " << *value;
			throw 807;
		}
		// now, Check for '=' in case this is an assignment
		if ('=' == nextIs(OP, '=', fIn)) { // if next token is an equals sign, therefore assignment
				getLineTVi(fIn, type, value); // set onto next token
				exp(type, value, fIn, SymbolTable, &result, inter);
				//printf("%*s%*s%*s%*s\n", -5, "[]=", -7, result.c_str(), -7, arr.c_str(), -5, assign.c_str());
				*inter << left << setw(7) << "[]=" << setw(7) << result.c_str() << setw(7) << arr.c_str() << assign.c_str() << endl;
			}
	}
	else if ('+' == nextIs(OP, '+', fIn)
			|| '-' == nextIs(OP, '-', fIn)) { //if next is '+' || '-' : exp
		cout << "\ntype = " << *type << " value = " << *value;
		getLineTVi(fIn, type, value);
		cout << "\nNextIs type = " << *type << " value = " << *value;
		throw 801;
		//break;
	} else if ('*' == nextIs(OP, '*', fIn)
			|| '/' == nextIs(OP, '/', fIn)
			|| '%' == nextIs(OP, '%', fIn)) { //if next is '*' || '/' : term
		cout << "\ntype = " << *type << " value = " << *value;
		getLineTVi(fIn, type, value);
		cout << "\nNextIs type = " << *type << " value = " << *value;
		throw 802;
		//break;

	}
	else {
		cout << "\ntype = " << *type << " value = " << *value;
		getLineTVi(fIn, type, value);
		cout << "\nNextIs type = " << *type << " value = " << *value;
		throw 810;
		//break;
	}
}
// io:= <exp> | string | id
void ioAssign(int * ioT, int * ioV, bool output, ifstream * fIn, hashmap * SymbolTable,  ofstream * inter) {
	getLineTVi(fIn, ioT, ioV);// grab current token and set ioT & ioV
	string result;
	//cout << "\nioAssign: The current Token Type = "<< *ioT << " Value = " << *ioV << endl;
	if (output) {			// if cout
		if (*ioT == STR) {// current token is string
			string str = "\"";
			str.append(findName(ioT, ioV));
			str.push_back('"');
			if (OUT == nextIs(OP, OUT, fIn)) {
				//printf("%*s %s \n", -5, "cout", str.c_str());
				*inter << left << setw(7) << "cout" << str.c_str() << endl;
				//getLineTVi(fIn, ioT, ioV);
				ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);	// recursive calling~
			} else if (';' == nextIs(PUN, ';', fIn)) {
				//printf("%*s %s \n", -5, "cout", str.c_str());
				*inter << left << setw(7) << "cout" << str.c_str() << endl;
			}
		}
		if (*ioT == CHAR) {// current token is character
			string str = "'";
			str.push_back((char)*ioV);
			str.push_back('\'');
			if (OUT == nextIs(OP, OUT, fIn)) {
				//printf("%*s %s \n", -5, "cout", str.c_str());
				*inter << left << setw(7) << "cout" << str.c_str() << endl;
				//getLineTVi(fIn, ioT, ioV);
				ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);	// recursive calling~
			} else if (';' == nextIs(PUN, ';', fIn)) {
				//printf("%*s %s \n", -5, "cout", str.c_str());
				*inter << left << setw(7) << "cout" << str.c_str() << endl;
			}

		}
		else if (*ioT == INT){
			exp(ioT, ioV, fIn, SymbolTable, &result, inter);
			if (OUT == nextIs(OP, OUT, fIn)) {
				//printf("%*s %s\n", -5, "cout", result.c_str());
				*inter << left << setw(7) << "cout" << result.c_str() << endl;
				ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);// recursive calling~
			} else if (';' == nextIs(PUN, ';', fIn)) {
				//printf("%*s %s\n", -5, "cout", result.c_str());
				*inter << left << setw(7) << "cout" << result.c_str() << endl;
			}
		}
		else if (*ioT < 1000) {//Type less than 1000 = ID
			if (isDeclaredID(SymbolTable, ioT, ioV)) {
				exp(ioT, ioV, fIn, SymbolTable, &result, inter);
				if (OUT == nextIs(OP, OUT, fIn)) {
					//printf("%*s %s\n", -5, "cout", result.c_str());
					*inter << left << setw(7) << "cout" << result.c_str() << endl;
					ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);// recursive calling~
				} else if (';' == nextIs(PUN, ';', fIn)) {
					//printf("%*s %s\n", -5, "cout", result.c_str());
					*inter << left << setw(7) << "cout" << result.c_str() << endl;
				}
			}
			else{
				throw 805;// NOT a defined variable, therefore throw error.
			}
		} else if (*ioT == RES && *ioV == Endl) {// if endl then print out '\n'
			if (OUT == nextIs(OP, OUT, fIn)) {
				//printf("%*s %s \n", -5, "cout", "'\\n'");
				*inter << left << setw(7) << "cout" << "\\n" << endl;
				//getLineTVi(fIn, ioT, ioV);
				ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);	// recursive calling~
			} else if (';' == nextIs(PUN, ';', fIn)) {
				//printf("%*s %s \n", -5, "cout", "'\\n'");
				*inter << left << setw(7) << "cout" << "\\n" << endl;
			}
		}
		else if (*ioT == RES) {// need to throw exception if any other REServed word is here.
			throw 806;
		}
	} else {			// if cin
		if (*ioT == STR) {// current token is string
					string str = "\"";
					str.append(findName(ioT, ioV));
					str.push_back('"');
					if (IN == nextIs(OP, IN, fIn)) {
						//printf("%*s %s \n", -5, "cin", str.c_str());
						*inter << left << setw(7) << "cin" << str.c_str() << endl;
						//getLineTVi(fIn, ioT, ioV);
						ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);	// recursive calling~
					} else if (';' == nextIs(PUN, ';', fIn)) {
						//printf("%*s %s \n", -5, "cin", str.c_str());
						*inter << left << setw(7) << "cin" << str.c_str() << endl;
					}
				}
				if (*ioT == CHAR) {// current token is character
					string str = "'";
					str.push_back((char)*ioV);
					str.push_back('\'');
					if (IN == nextIs(OP, IN, fIn)) {
						//printf("%*s %s \n", -5, "cin", str.c_str());
						*inter << left << setw(7) << "cin" << str.c_str() << endl;
						//getLineTVi(fIn, ioT, ioV);
						ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);	// recursive calling~
					} else if (';' == nextIs(PUN, ';', fIn)) {
						//printf("%*s %s \n", -5, "cin", str.c_str());
						*inter << left << setw(7) << "cin" << str.c_str() << endl;
					}

				}
				else if (*ioT == INT){
					exp(ioT, ioV, fIn, SymbolTable, &result, inter);
					if (IN == nextIs(OP, IN, fIn)) {
						//printf("%*s %s\n", -5, "cin", result.c_str());
						*inter << left << setw(7) << "cin" << result.c_str() << endl;
						ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);// recursive calling~
					} else if (';' == nextIs(PUN, ';', fIn)) {
						//printf("%*s %s\n", -5, "cin", result.c_str());
						*inter << left << setw(7) << "cin" << result.c_str() << endl;
					}
				}
				else if (*ioT < 1000) {//Type less than 1000 = ID
					if (isDeclaredID(SymbolTable, ioT, ioV)) {
						exp(ioT, ioV, fIn, SymbolTable, &result, inter);
						if (IN == nextIs(OP, IN, fIn)) {
							//printf("%*s %s\n", -5, "cin", result.c_str());
							*inter << left << setw(7) << "cin" << result.c_str() << endl;
							ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);// recursive calling~
						} else if (';' == nextIs(PUN, ';', fIn)) {
							//printf("%*s %s\n", -5, "cin", result.c_str());
							*inter << left << setw(7) << "cin" << result.c_str() << endl;
						}
					}
					else{
						throw 805;// NOT a defined variable, therefore throw error.
					}
				} else if (*ioT == RES && *ioV == Endl) {// if endl then print out '\n'
					if (IN == nextIs(OP, IN, fIn)) {
						//printf("%*s %s \n", -5, "cin", "'\\n'");
						*inter << left << setw(7) << "cin" << "\\n" << endl;
						//getLineTVi(fIn, ioT, ioV);
						ioAssign(ioT, ioV, output, fIn, SymbolTable, inter);	// recursive calling~
					} else if (';' == nextIs(PUN, ';', fIn)) {
						//printf("%*s %s \n", -5, "cin", "'\\n'");
						*inter << left << setw(7) << "cin" << "\\n" << endl;
					}
				}
				else if (*ioT == RES) {// need to throw exception if any other REServed word is here.
					throw 806;
				}
	}
}

int LNum = 0;
string newL() {
	string tmpL = "L_" + to_string(LNum);
	LNum++;//increments for next temporary.
	return tmpL;
}
void resetL() {
	LNum = 1;// resets temporaries
}
//string skipL;// set this label when its time to skip the content block.

void checkNext(ifstream * fIn){ // Checks and then moves back to beginning position.
	int whatType, whatValue;
	streampos pos = fIn->tellg();
	getLineTVi(fIn, &whatType, &whatValue);
	cout << "Checking Next:\nNext Type = " << whatType << " Next Value = " << whatValue << endl;
	if (fIn->eof())
		fIn->clear();
	fIn->seekg(pos, ios::beg);
}
void checkNexty(ifstream * fIn){ // Checks and then moves back to beginning position.
	int whatType, whatValue;
	streampos pos = fIn->tellg();
	getLineTVi(fIn, &whatType, &whatValue);
	cout << "Checking Next:\nNext Type = " << whatType << " Next Value = " << whatValue << endl;
	getLineTVi(fIn, &whatType, &whatValue);
	cout << "Checking Next2:\nNext2 Type = " << whatType << " Next Value = " << whatValue << endl;
	getLineTVi(fIn, &whatType, &whatValue);
	cout << "Checking Next3:\nNext3 Type = " << whatType << " Next Value = " << whatValue << endl;
	getLineTVi(fIn, &whatType, &whatValue);
	cout << "Checking Next4:\nNext4 Type = " << whatType << " Next Value = " << whatValue << endl;
	if (fIn->eof())
		fIn->clear();
	fIn->seekg(pos, ios::beg);
}
int checkNextR(int codeType, int codeValue, ifstream * fIn){
	int whatType, whatValue;
	streampos pos = fIn->tellg();
	fIn->seekg(pos, ios::beg);
	getLineTVi(fIn, &whatType, &whatValue);
	if (fIn->eof())
		fIn->clear();
	fIn->seekg(pos, ios::beg);
	if (whatType == codeType && whatValue == codeValue)
			return codeValue;
	else
		return 0;
}


/*
 *
 * Assignment 4: adding functionality for If and While
 *
 */

void orExp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter){
	int op;
	string exp2 = *result;
	andExp(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult, inter);
	while((op = nextIs(OP,OR,fIn))){
		//getLineTVi(fIn, type, value);// move to next token and continue.
		andExp(type, value, isWhile, fIn, SymbolTable, &exp2, skipTrue, boolResult, inter);
		// print out:
		//printf("%*s%*s%*s",-5,"|",-5,result->c_str(),-5,exp2.c_str());
		*inter << left << setw(7) << '|' << setw(7) << result->c_str() << setw(7) << exp2.c_str();
		*result = newTmp();
		//printf("%*s\n",-5,result->c_str());
		*inter << left << result->c_str() << endl;
	}
}
void andExp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter){
	int op;
	string exp2 = *result;
	notExp(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult, inter);
	while((op = nextIs(OP,AND,fIn))){
		//getLineTVi(fIn, type, value);// move to next token and continue.
		notExp(type, value, isWhile, fIn, SymbolTable, &exp2, skipTrue, boolResult, inter);
		// print out:
		//printf("%*s%*s%*s",-5,"&",-5,result->c_str(),-5,exp2.c_str());
		*inter << left << setw(7) << '&' << setw(7) << result->c_str() << setw(7) << exp2.c_str();
		*result = newTmp();
		//printf("%*s\n",-5,result->c_str());
		*inter << left << result->c_str() << endl;
	}
}
void notExp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter){
	if('!' == nextIs(OP,'!',fIn)){
		string save;// only temporary until I fix the parameters!!!!!!
		RelOp(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult, 0, &save, inter);
		//printf("%*c%*s%*c%*s\n",-5,'^',-5,result->c_str(),-5,'1',-5,result->c_str());
		*inter << left << setw(7) << '^' << setw(7) << result->c_str() << setw(7) << '1' << result->c_str() << endl;
	}
	else{
		string save;// only temporary until I fix the parameters!!!!!!
		RelOp(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult, 0, &save, inter);
		//semiConditionPrep(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult);
	}
}

void RelOp(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult, int RoP, string * save,  ofstream * inter){
	if ('(' == nextIs(PUN, '(', fIn)){
		//semiConditionPrep(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult);
		orExp(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult, inter);
		if(')' != nextIs(PUN, ')', fIn)){
			throw 836;// Missing End Parenthesis
			}
	}
	else{
		semiConditionPrep(type, value, isWhile, fIn, SymbolTable, result, skipTrue, boolResult, inter);
	}
}

void contentBlockPrep(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result,  ofstream * inter){
	bool lookForCurly = false;
	if ('{' == nextIs(PUN, '{', fIn)){
		lookForCurly = true;
		//cout << "lookForCurly = " << lookForCurly << endl;
	}
	getLineTVi(fIn, type, value);// get next token for the contentBlock to progress.
	contentBlock(type, value, fIn, SymbolTable, result, inter);
	while(lookForCurly && ('}' != nextIs(PUN,'}',fIn) && !fIn->eof())){
		getLineTVi(fIn, type, value);// get next token for the contentBlock to progress.
		contentBlock(type, value, fIn, SymbolTable, result, inter);
	}
	if (fIn->eof()) {
		if (isWhile)
			throw 824;
		else
			throw 825;
	} else if (!lookForCurly && '}' == nextIs(PUN, '}', fIn)) {
		if (isWhile) {
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = "
					<< *value;
			throw 826;
		} else {
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = "
					<< *value;
			throw 827;
		}

	}
}

void semiCondition(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue,  ofstream * inter){
	string exp2 = *result;// save the result of the first expression
	getLineTVi(fIn, type, value);// get the current line.
	exp(type, value, fIn, SymbolTable, result, inter);

	getLineTVi(fIn, type, value); // set onto next token (The potential REL-OP) for next section
	if(*type == OP){
		if(*value == NE){
			getLineTVi(fIn, type, value);// get the current line.
			exp(type, value, fIn, SymbolTable, &exp2, inter);
			//cout << endl << "exp1 = " << exp1 << endl;
			*skipTrue = newL();
			//printf("%*s%*s%*s%*s%*s\n",-5,"!=",-5,result->c_str(),-5,exp2.c_str(),-5,"goto",-5,skipTrue->c_str());
			*inter << left << setw(7) << "!=" << setw(7) << result->c_str() << setw(7) << exp2.c_str() << setw(7) << "goto" << skipTrue->c_str() << endl;
		}
		else if(*value == '<'){
			getLineTVi(fIn, type, value);// get the current line.
			exp(type, value, fIn, SymbolTable, &exp2, inter);
			//cout << endl << "exp1 = " << exp1 << endl;
			*skipTrue = newL();
			//printf("%*c%*s%*s%*s%*s\n",-5,'<',-5,result->c_str(),-5,exp2.c_str(),-5,"goto",-5,skipTrue->c_str());
			*inter << left << setw(7) << "<" << setw(7) << result->c_str() << setw(7) << exp2.c_str() << setw(7) << "goto" << skipTrue->c_str() << endl;
		}
		else if(*value == LE){
			getLineTVi(fIn, type, value);// get the current line.
			exp(type, value, fIn, SymbolTable, &exp2, inter);
			//cout << endl << "exp1 = " << exp1 << endl;
			*skipTrue = newL();
			//printf("%*s%*s%*s%*s%*s\n",-5,"<=",-5,result->c_str(),-5,exp2.c_str(),-5,"goto",-5,skipTrue->c_str());
			*inter << left << setw(7) << "<=" << setw(7) << result->c_str() << setw(7) << exp2.c_str() << setw(7) << "goto" << skipTrue->c_str() << endl;
		}
		else if(*value == EE){
			getLineTVi(fIn, type, value);// get the current line.
			exp(type, value, fIn, SymbolTable, &exp2, inter);
			//cout << endl << "exp1 = " << exp1 << endl;
			*skipTrue = newL();
			//printf("%*s%*s%*s%*s%*s\n",-5,"==",-5,result->c_str(),-5,exp2.c_str(),-5,"goto",-5,skipTrue->c_str());
			*inter << left << setw(7) << "==" << setw(7) << result->c_str() << setw(7) << exp2.c_str() << setw(7) << "goto" << skipTrue->c_str() << endl;
		}
		else if(*value == GE){
			getLineTVi(fIn, type, value);// get the current line.
			exp(type, value, fIn, SymbolTable, &exp2, inter);
			//cout << endl << "exp1 = " << exp1 << endl;
			*skipTrue = newL();
			//printf("%*s%*s%*s%*s%*s\n",-5,">=",-5,result->c_str(),-5,exp2.c_str(),-5,"goto",-5,skipTrue->c_str());
			*inter << left << setw(7) << ">=" << setw(7) << result->c_str() << setw(7) << exp2.c_str() << setw(7) << "goto" << skipTrue->c_str() << endl;
		}
		else if(*value == '>'){
			getLineTVi(fIn, type, value);// get the current line.
			exp(type, value, fIn, SymbolTable, &exp2, inter);
			*skipTrue = newL();
			//printf("%*c%*s%*s%*s%*s\n",-5,'>',-5,result->c_str(),-5,exp2.c_str(),-5,"goto",-5,skipTrue->c_str());
			*inter << left << setw(7) << ">" << setw(7) << result->c_str() << setw(7) << exp2.c_str() << setw(7) << "goto" << skipTrue->c_str() << endl;
		}
		/*
		else{
			throw 832;// incorrect operator
		}
		*/
	}
	/*
	else if (')' != nextIs(PUN, ')', fIn)){// End of Condition
		if (isWhile){	//	if While
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = " << *value;
			throw 822;
		}
		else{//	if If
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = " << *value;
			throw 823;
		}
	}
	*/
}

void semiConditionPrep(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * result, string * skipTrue, string *boolResult,  ofstream * inter){
		semiCondition(type, value, isWhile, fIn, SymbolTable, result, skipTrue, inter);	// allows to recrusively check for parentheis

		string toTrue = newL();// the label that skips to true

		*result = newTmp();
		//printf("%*s%*s%*s\n",-5,"=",-10,"0",-5,result->c_str());		// false assign
		*inter << left << setw(7) << '=' << setw(14) << '0' << result->c_str() << endl;
		//printf("%*s%s\n",-5,"goto",toTrue.c_str());						// skip true
		*inter << left << setw(7) << "goto" << toTrue.c_str() << endl;
		//printf("%s\n",skipTrue->c_str());								// to true assign
		*inter << left << skipTrue->c_str() << endl;
		//printf("%*s%*s%*s\n",-5,"=",-10,"1",-5,result->c_str());		// true assign
		*inter << left << setw(7) << '=' << setw(14) << '1' << result->c_str() << endl;
		//printf("%s\n",toTrue.c_str());									//skip true
		*inter << left << toTrue.c_str() << endl;

}

void condition(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable, string * falseL, string * skipL,  ofstream * inter){
// condition (<exp> <rel-op> <exp>)
	string result, skipTrue, boolResult;
	string save = boolResult;// save it to save.
	// begin the recursive calling of conditions Relative operators...
	orExp(type, value, isWhile, fIn, SymbolTable, &result, &skipTrue, &boolResult, inter);

	//	FIX PRINT SO IT ONLY RELIES ON WHAT IT NEEDS TO. i.e. RESULT ONLY?!?!
	//printf("%*s%*s%*c%*s%*s\n",-5,"==",-5,result.c_str(),-5,'0',-5,"goto",-5,falseL->c_str());
	*inter << left << setw(7) << "==" << setw(7) << result.c_str() << setw(7) << '0' << setw(7) << "goto" << falseL->c_str() << endl;
	// BELOW IS ALL GOOD!

	if (')' == nextIs(PUN, ')', fIn)) { // end condition (check for excess ')')
		// Begin Content Block of condition: copy & paste from parse2 starting at //begin search
		contentBlockPrep(type, value, isWhile, fIn, SymbolTable, &result, inter);

		string storeSkip;
		if(!isWhile){
			//storeSkip = skipL;
			*skipL = newL();
		}

		//printf("%*s%*s\n",-5,"goto",-5, skipL->c_str());				// skip the rest of the if Series since this was true
		*inter << left << setw(7) << "goto" << skipL->c_str() << endl;
	}
	else{// Excess parenthesis in conditional
		if (isWhile){
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = " << *value;
			throw 830;
		}
		else{
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = " << *value;
			throw 831;
		}
	}
}

void WhileLoop(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable,  ofstream * inter){
	string initialL = newL();// keeps up with increment
	//cout << "\nIn WhileLoop\n";
	//printf("%s\n",initialL.c_str());
	*inter << left << initialL.c_str() << endl;
	string falseL = newL();
	string falseLabel = falseL;
	if ('(' == nextIs(PUN, '(', fIn)) {
	condition(type, value, true, fIn, SymbolTable, &falseL, &initialL, inter);
	//printf("%*s\n", -5, falseLabel.c_str());// the label at the end of the first if content block
	*inter << left << falseL.c_str() << endl;
	}
	else{
		cout << "\ntype = " << *type << " value = " << *value;
		getLineTVi(fIn, type, value);
		cout << "\nNextIs type = " << *type << " value = " << *value;
		throw 820;
	}
}

void IfSeries(int * type, int * value, bool isWhile, ifstream * fIn, hashmap * SymbolTable,  ofstream * inter){
	//string endOfIf = skipL;
	string endOfIf;// set to new L?
	if ('(' == nextIs(PUN, '(', fIn)) { // if there is a parenthesis: then good
		string falseL = newL();
		//cout << endl << "entering first condition" << endl;
		condition(type, value, false, fIn, SymbolTable, &falseL, &endOfIf, inter);
		//printf("%*s\n", -5, falseL.c_str());// the label at the end of the first if content block
		*inter << left << falseL.c_str() << endl;
	}
	else if (Else == nextIs(RES, Else, fIn)){
		throw 833;
	}
	else{//	 else throw error
		cout << "\nFirst If in If Series.";
		cout << "\nElse currently believes it need parenthesises when it does not."
				"\n The real problem is an Else shouldn't be here.";
		cout << "\ntype = " << *type << " value = " << *value;
		getLineTVi(fIn, type, value);
		cout << "\nNextIs type = " << *type << " value = " << *value;
		throw 821;
	}
	//cout << "\nMove onto next token... Necessary? Current stored type and value:"
	//		"\ntype = " << *type << " value = " << *value<< endl;
	//checkNext(fIn);
	// Check for Else If and then Else.

	bool hitElse = false;
	while (Else == nextIs(RES,Else, fIn) && hitElse == false) {	//	if RES Word value == Else
		if (If == nextIs(RES,If, fIn) && hitElse == false) {	//	Else If
			if ('(' == nextIs(PUN, '(', fIn)) { // if there is a parenthesis: then good
				cout << endl << "Else If" << endl;
				string falseL = newL();
				condition(type, value, false, fIn, SymbolTable, &falseL, &endOfIf, inter);
				//printf("%*s\n", -5, falseL.c_str());// the label at the end of the first if content block
				*inter << left << falseL.c_str() << endl;

			} else{//	 else throw error
				cout << "\ntype = " << *type << " value = " << *value;
				getLineTVi(fIn, type, value);
				cout << "\nNextIs type = " << *type << " value = " << *value;
				throw 821;
			}
		}
		else{// Else
			if (hitElse == false){
			hitElse = true;
			string result;
			contentBlockPrep(type, value, isWhile, fIn, SymbolTable, &result, inter);
			}
			//else
				//checkNexty(fIn);
			/*	Causes to throw error when in nested if, ignorant that is it looking at it's container's else not it's own.
			else{
				throw 834;// already hit an else, this means two else's in a row, throw else w/o a beginning if.
			}
			*/
		}
	}
	//printf("%*s\n", -5, endOfIf.c_str());// the label at the end of the If Series
	*inter << left << endOfIf.c_str() << endl;
}
void contentBlock(int * type, int * value, ifstream * fIn, hashmap * SymbolTable,string * result,  ofstream * inter){
	if (*type < 1000 && *type >= 0) {
		assignment(type, value, fIn, SymbolTable, inter);
	} else if (*type == RES) {
		if (*value == Cout) { // if is cout then next : <<
			if (OUT == nextIs(OP, OUT, fIn)) { // if next token is <<, therefore output assignment
				if (*type == -1 && *value == -1) {	// if end of file then incomplete assignment
					throw 800;
				}
				ioAssign(type, value, true, fIn, SymbolTable, inter);
			} else {
				throw 803;
			}
		} else if (*value == Cin) {	// if is cin then next : >>
			if (IN == nextIs(OP, IN, fIn)) {// if next token is >>, therefore input assignment
				if (*type == -1 && *value == -1) {	// if end of file then incomplete assignment
					throw 800;
				}
				ioAssign(type, value, false, fIn, SymbolTable, inter);
			} else {
				throw 804;
			}
		}

		else if (*value == While) {	// if RES Word value == While
				WhileLoop(type, value, true, fIn, SymbolTable, inter);
		} else if (*value == If || *value == Else) {	//	if RES Word value == If
				IfSeries(type, value, false, fIn, SymbolTable, inter);
		}
		else if (*value == Else){
			cout << "\ntype = " << *type << " value = " << *value;
			getLineTVi(fIn, type, value);
			cout << "\nNextIs type = " << *type << " value = " << *value;
			throw 834;
		}
	}
	// else ignore because not beginning an assignment.
}
void parse2(hashmap * SymbolTable) {
	try {
		bool halt = true;
		string result;// argument will be used to carry the value of the token throughout the recursion until it prints.
		ifstream fIn("lexed.txt");    //
		if (fIn.is_open()) {
			int type, value;
			resetL();
			ofstream inter("intermediate.txt", ofstream::trunc);	//	Intermediate Code
			while (fIn.good()) {
				getLineTVi(&fIn, &type, &value);
				if (halt) {
					if (type == RES && value == Main)
						halt = false;
				} else {    // begin the search
					resetTmp();
					contentBlock(&type, &value, &fIn, SymbolTable, &result, &inter);// the content contained by '{' and '}'
				}
			}
			fIn.close();
			inter.close();
		} else
			throw 404;

	} catch (int e) {
		if (e == 404)
			cout << "\nError " << e << ": Unable to open File!\n";
		else if (e == 800)
			cout << "\nError " << e << ": Incomplete assignment!\n";
		else if (e == 801)
			cout << "\nError " << e << ": Expression without assignment!\n";
		else if (e == 802)
			cout << "\nError " << e << ": Term without assignment!\n";
		else if (e == 803)
			cout << "\nError " << e << ": Cout must be followed by \"<<\"!\n";
		else if (e == 804)
			cout << "\nError " << e << ": Cin must be followed by \">>\"!\n";
		else if (e == 805)
			cout << "\nError " << e << ": An undeclared variable in assignment!\n";
		else if (e == 806)
			cout << "\nError " << e << ": A reserved word in assignment!\n";
		else if (e == 807)
			cout << "\nError " << e << ": An unclosed bracket! Missing the right ending bracket ']'.\n";
		else if (e == 808)
			cout << "\nError " << e << ": Variable declared as an Array is missing its following brackets.\n";
		else if (e == 809)
			cout << "\nError " << e << ": Missing '<<' or ';' in cout assignment!\n";
		else if (e == 810)
			cout << "\nError " << e << ": Improper token following in assignment! \n";
		else if (e == 811)
			cout << "\nError " << e << ": Non-Integer or Non-Character Token in assignment!\n";
		else if (e == 812)
			cout << "\nError " << e << ": Missing '<<' or ';' in cout assignment!\n";
		else if (e == 813)
			cout << "\nError " << e << ": Missing end parenthesis in expression!\n";
		else if (e == 814)
			cout << "\nError " << e << ": Missing semi-colon at end of assignment!\n";
		else if (e == 820)
			cout << "\nError " << e << ": Missing beginning parenthesis at start of while condition!\n";
		else if (e == 821)
			cout << "\nError " << e << ": Missing beginning parenthesis at start of if condition!\n";
		else if (e == 822)
			cout << "\nError " << e << ": Missing ending parenthesis at end of while condition!\n";
		else if (e == 823)
			cout << "\nError " << e << ": Missing ending parenthesis at end of if condition!\n";
		else if (e == 824)
			cout << "\nError " << e << ": Missing ending curly bracket at end of while block!\n";
		else if (e == 825)
			cout << "\nError " << e << ": Missing ending curly bracket at end of if block!\n";
		else if (e == 826)
			cout << "\nError " << e << ": Excess ending curly bracket at end of while block!\n";
		else if (e == 827)
			cout << "\nError " << e << ": Excess ending curly bracket at end of if block!\n";
		else if (e == 828)
			cout << "\nError " << e << ": Missing ending parenthesis in while conditional statement!\n";
		else if (e == 829)
			cout << "\nError " << e << ": Missing ending parenthesis in if conditional statement!\n";
		else if (e == 830)
			cout << "\nError " << e << ": Excess ending parenthesis in while conditional statement!\n";
		else if (e == 831)
			cout << "\nError " << e << ": Excess ending parenthesis in if conditional statement!\n";
		else if (e == 832)
			cout << "\nError " << e << ": Incorrect operator in condition's boolean logic!\n";
		else if (e == 833)
			cout << "\nError " << e << ": If Else statement without an an initial If statement!\n";
		else if (e == 834)
			cout << "\nError " << e << ": Else statement without an an initial If statement!\n";
		else if (e == 835)
			cout << "\nError " << e << ": Missing Negation's beginning parenthesis!\n";
		else if (e == 834)
			cout << "\nError " << e << ": Missing Negation's ending parenthesis!\n";
	}
}
int STs=0, Hs=0;
string findName(int * type, int * value){
	for (hashmap::iterator got = Chronicles.begin();
			got != Chronicles.end(); ++got) { //	Iterate through different tokens
		if (!got->second.empty()) {
			for (int i = 0; i < got->second.size(); i++) {
				if(got->second.at(i).getType() == *type && got->second.at(i).getVal() == *value){
					return got->second.at(i).getName();
				}
			}
		}
	}
	return "Item Non-Existent in Symbol Table.";
}

/*
 * 	Assignment 5: Code Generation
 *		Code Generation takes the quadruples from parser output and
 *	generates the actual code. This is the final task of our compiler
 *
 *	Create Declarations of all vars from symbol table.
 *	Translates each line of intermediate code into one line of C++ code.
 *
 * */
bool isInt(int type){
	return (type == INC || type == INV || type == INCa || type == INVa);
}
bool isChar(int type){
	return (type == CHAV || type == CHAC || type == CHACa || type == CHAVa);
}
bool isSetVar(int type){
	return (isInt(type) || isChar(type));
}
void createDecs(hashmap * SymbolTable){	//	Create Declarations
	for (hashmap::iterator got = SymbolTable->begin();
			got != SymbolTable->end(); ++got) {
		if (!got->second.empty()) {
			for (int i = 0; i < got->second.size(); i++) {
				int t = got->second.at(i).getType();
				if (isSetVar(t)){
					if (isConST(t))
						cout << "const ";// print out const
					if (t == INV || t == INC)// if INT
						cout << "int " << got->second.at(i).getName() << " = " << got->second.at(i).getVal() << ';' << endl;// print out id and what it =.
					else if (t == CHAV || t == CHAC)// Char.
						cout << "char " << got->second.at(i).getName() << " = '" << (char)got->second.at(i).getVal() << "';" << endl;
					else if (t == INVa || t == INCa){	// or int array
						cout << "int " << got->second.at(i).getName() << "[";
						if (got->second.at(i).getSize() >= 0)
							cout << got->second.at(i).getSize();
						cout <<"];" << endl;
					}
					else if (t == CHAVa || t == CHACa){	// or char array
						cout << "char " << got->second.at(i).getName() << "[";
						if (got->second.at(i).getSize() >= 0)
							cout << got->second.at(i).getSize();
						cout <<"];" << endl;
					}
					else
						cout << "Error: Problem reading Set Variables when creating declarations.\n";
				}
			}
		}
	}
	for (int i = 0; i < totalTmp; i++){	//	int and char are the same thing, only printed differently.
		if (i == 0)
			cout << "int";
		if(i != totalTmp-1)
		cout << " _t_" << i << ',';
		else
			cout << " _t_" << i << ";\n";
	}

}
/*
vector<string> splitter (string s, char delim){// splits the string based on delim, but also deletes recurring delims.
	vector<string> result;
	//s = regex_replace(s, regex("[' ']{2,}"), " ");
	string res;
	string param = " $2";
	regex_replace(std::back_inserter(res), s.begin(), s.end(), regex("[' ']{2,}"), param.c_str(), regex_constants::match_default);
	stringstream ss(res);
	//stringstream ss(s);
	string partial;
	while(getline(ss, partial, delim)){
		result.push_back(partial);
	}
	return result;
}
vector<string> splice(string line){
	if(line.empty()){
		vector<string> result;
		return result;
	}
	else{
		string check = line.substr(0,7);
		if (check.find("cout") != string::npos || check.find("cin") != string::npos){// if output/input perserve
			vector<string> result;
			//check = regex_replace(check, regex("[' ']{2,}"), "");// remove the space buffer 3
			string res;
			string param = "$2";
			//regex_replace(std::back_inserter(res), check.begin(), check.end(), regex("[' ']{2,}"), param.c_str(), regex_constants::match_default);// 6
			regex_replace(std::back_inserter(res), check.begin(), check.end(), regex("[' ']{2,}"), param.c_str(), regex_constants::match_default)
			//result.push_back(res, check); //4
			result.push_back(res);
			line.erase(0,7);
			result.push_back(line);
			return result;
		}
		else{// if not ouptu/input
			return splitter(line, ' ');
		}
	}
}
*/
vector<string> split(string line){	//	splits the intermediate code into tokens
	vector<string> result;
	if( line.size() <= 7){
		result.push_back(line.substr(0, line.find_first_of(" ")));
	}
	else{// if ( line.size() >7)
		string seg = line.substr(0, line.find_first_of(" "));
		//cout << "first = " << seg << endl;
		result.push_back(seg);// gets first token
		line = line.substr(7);// shifts to next token
		if(seg.compare("cout") == 0 || seg.compare("cin") == 0){
			result.push_back(line);
		}
		else{	//	not output/input, so do standard process
			while(!(line.size() <= 0)){ // loops till empty
				if (line.size()<=7){
					if (line.find(" ") != string::npos){
						line = line.substr(line.find_first_of(" "), line.find_last_of(" "));
					}
					result.push_back(line);// gets the next token
					break;
				}
				else{
					if ((line.substr(0,7)).compare("       ") == 0){
						line = line.substr(7);
					}
					else{
						result.push_back(line.substr(0, line.find_first_of(" ")));// gets the next token
						line = line.substr(7);// shifts to next token
					}
				}
			}
		}
	}
	return result;
}

void translate(){	//	Translate intermediate code into c++ code.
	try {
		ifstream in("intermediate.txt");    //this should be a .cpp file
		if (in.is_open()) {
			string line;
			while(in.good()){
				getline(in,line);
				if(line.empty())
					break;// the file is done, or corrupted/incorrect file
				//vector<string> str = splice(line);// set new vector of tokens from line.
				vector<string> str = split(line);// set new vector of tokens from line.
				/*
				cout << "str size = " << str.size() << endl;
				for(int i = 0; i < str.size(); i++){
					cout << " at"<< i << " " << str.at(i);

				}
				cout << endl;
				*/

				if(str.size() == 1){// Label so printout.
					cout << str.at(0) << ':' << endl;
				}
				else if(str.size() == 2){// goto Label || cout/cin
					if (str.at(0).compare("cout") == 0){
						cout << str.at(0) << " << ";
						if (str.at(1).compare("\\n") == 0)
							cout << "endl";
						else
							cout << str.at(1);
						cout << ';' << endl;
					}
					else if (str.at(0).compare("cin") == 0){
						cout << str.at(0) << " >> " << str.at(1) << ';' << endl;
					}
					else if (str.at(0).compare("goto") == 0){// goto label
						cout << str.at(0) << " " << str.at(1) << ';' << endl;
					}
					else{
						cout << "str.at(0) = " << str.at(0) << endl;
						throw 900;
					}
				}
				else if(str.size() == 3){	//	assignment =
					if (str.at(0).compare("=") == 0){
						cout << str.at(2) << " " << str.at(0) << " " << str.at(1) << ';' << endl;
					}
					else{
						throw 901;
					}
				}
				else if(str.size() == 4){	//	arithmatic or RELOP (&|^)
					if (str.at(0).compare("+") == 0 || str.at(0).compare("-") == 0 ||
							str.at(0).compare("*") == 0 || str.at(0).compare("/") == 0 ||
							str.at(0).compare("%") == 0){	//	arithmetic
						cout << str.at(3) << " = " << str.at(1) << " " << str.at(0) << " " << str.at(2) << ';' << endl;
					}
					//	REL OPs below
					else if (str.at(0).compare("^") == 0){	//	^
						cout << str.at(3) << " = " << str.at(1) << " ^ " << str.at(2) << ';' << endl;
					}
					else if ( str.at(0).compare("&") == 0){	//	&
						cout << str.at(3) << " = " << str.at(1) << " & " << str.at(2) << ';' << endl;
					}
					else if (str.at(0).compare("|") == 0){	//	|
						cout << str.at(3) << " = " << str.at(1) << " | " << str.at(2) << ';' << endl;
					}
					else if (str.at(0).compare("=[]") == 0){
						cout << str.at(3) << " = " << str.at(1) << '[' << str.at(2) << "];" << endl;
					}
					else if (str.at(0).compare("[]=") == 0){
						cout << str.at(3) << "[" << str.at(2) << "] = " << str.at(1) << ";" << endl;
					}
					else
						throw 902;
				}
				else if(str.size() == 5){	//	conditional
					if (str.at(0).compare(">") == 0 || str.at(0).compare(">=") == 0 || str.at(0).compare("==") == 0
							|| str.at(0).compare("<=") == 0 || str.at(0).compare("<") == 0){
						cout << "if ( " << str.at(1) << " " << str.at(0) << " " << str.at(2) << " ){ goto " << str.at(4) << ":}\n";
					}
					else
						throw 903;
				}
				else
					throw 904;


				// equations . . .
				//		arithmatic
				//		assignments
				//
				// if and whiles . . . ???? handling simply by printing out the gotos and labels DONE
				// cout and cin  DONE
			}
			in.close();
		} else
			throw 404;
	} catch (int e) {
		if (e == 404)
			cout << "Error " << e << ": File could not be opened.\n";
		else if (e == 900)
			cout << "Error " << e << ": Invalid data in intermediate code for a two token line.\n";
		else if (e == 901)
					cout << "Error " << e << ": Invalid data in intermediate code for a three token line.\n";
		else if (e == 902)
					cout << "Error " << e << ": Invalid data in intermediate code for a four token line.\n";
		else if (e == 903)
					cout << "Error " << e << ": Invalid data in intermediate code for a five token line.\n";
		else if (e == 904)
					cout << "Error " << e << ": Invalid data in intermediate code: Too many tokens.\n";
	}
}
void codeGen(hashmap * SymbolTable){
	createDecs(SymbolTable);	//	creates declarations of all vars from symbol table
	cout << "\nint main(){\n";
	translate();	//	translates ea. line of intermediate code into 1 line c++ code.
	cout << "return 0;\n}\n";
}



void printST(hashmap *SymbolTable) {
	printf("\n%*s %*s %*s %*s %*s %*s\n", -25, "Hash Value:", -10, "Name:", -7,
			"Type:", -10, "Value:", -7, "Size:", -7, "elType:");
	hashmap::hasher func = SymbolTable->hash_function();
	for (hashmap::iterator got = SymbolTable->begin();
			got != SymbolTable->end(); ++got) {
		if (!got->second.empty()) {
			STs++;
			printf("%*lu %*s %*c \n", -25, func(got->first), -10,
					&got->first.at(0), 0, ':');
			for (int i = 0; i < got->second.size(); i++) {
				printf("%*c %*s %*i %*i %*i %*i \n", -25, ' ', -10,
						got->second.at(i).getName().c_str(), -7,
						got->second.at(i).getType(), -10,
						got->second.at(i).getVal(), -7,
						got->second.at(i).getSize(), -7,
						got->second.at(i).getElType());
			}
			cout << endl;
		}
	}
}
void record1(hashmap * SymbolTable) { //	Record ST into history for first time
	for (hashmap::iterator got = SymbolTable->begin();
			got != SymbolTable->end(); ++got) { //	Iterate through different tokens
		if (!got->second.empty()) {
			Hs++;
			vector<vector<Token> > newRow; // new collision vector.
			for (int i = 0; i < got->second.size(); i++) { //	Iterate through collisions
				vector<Token> newVariation; // new Variation vector for storing changes in states throughout history
				newVariation.push_back(got->second.at(i));
				// stores token from collision into the first Variation vector index
				newRow.push_back(newVariation); // stores new Variation vector into the collisions vector (newRow).
			}
			history.push_back(newRow); //stores the new Row into the main vector of different tokens
		}
	}
}

void printHistory(){
	for(int i = 0; i < history.size(); i++){
		for(int j = 0; j < history.at(i).size(); j++){
			for(int k = 0; k < history.at(i).at(j).size(); k++){
				printf("%*c %*s %*i %*i %*i %*i \n", -25, ' ', -10,
						history.at(i).at(j).at(k).getName().c_str(), -7,
						history.at(i).at(j).at(k).getType(), -10,
						history.at(i).at(j).at(k).getVal(), -7,
						history.at(i).at(j).at(k).getSize(), -7,
						history.at(i).at(j).at(k).getElType());
			}// variants printed
			cout << endl;
		}// collisions printed
		cout << '-' << endl;
	}
}
// This search is disgusting time wise. I want constant & this is worst case O(n) to O(n*collisions). ugh . . .
//	Will optimize in future by switching the id type and string value to be the hashcode value (for immediate access
int main(int argc, char** argv) {
	cout
			<< "The print out of the Symbol Table will show ALL rows in the Symbol Table,"
			<< " including rows that are empty w/ no actual Tokens in the vector:\n\n";
	// Test your Code! Print out Symbol Table same as example.
	hashmap SymbolTable;
	SymbolTable.reserve(200);
	lexiAna(&SymbolTable);
	//printST(&SymbolTable);

	//record1(&SymbolTable);
	Chronicles = SymbolTable;
	/*
	cout << "\nHistory:\n";
	printHistory();

	//if(history.size() == SymbolTable.size())
	if(Hs == STs)
		cout << "\nMatch in Size:\n";
	else
		cout << "\nNot match in Size: ST: " << STs << " History: "<< Hs <<"\n";
	*/
	parseDec(&SymbolTable);	  			// edit the argument given.
	//cout << "\nAdjusted Symbol Table\n";
	//record2(&SymbolTable);
	//printST(&SymbolTable);
	//cout << "\nParse Tree:\n";
	//printf("%*s %*s %*s %s \n", -4, "OP", -6,"arg1" , -6, "arg2", "Result");
	parse2(&SymbolTable);

	// Code Gen
	codeGen(&SymbolTable);

	return 0;
}
