/*Oncioiu Raluca
**333CA
**Tema LFA
*/

#ifndef PARSING_H_
#define PARSING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <string>

using namespace std;

//masini elementare
#define LEFT 1
#define RIGHT 2

#define LEFT_UNTIL 3
#define RIGHT_UNTIL 4

#define LEFT_NOT_UNTIL 5
#define RIGHT_NOT_UNTIL 6

#define LEFT_UNTIL_REF 7
#define RIGHT_UNTIL_REF 8

#define LEFT_NOT_UNTIL_REF 9
#define RIGHT_NOT_UNTIL_REF 10

#define LEFT_UNTIL_NAME 11
#define RIGHT_UNTIL_NAME 12

#define LEFT_NOT_UNTIL_NAME 13
#define RIGHT_NOT_UNTIL_NAME 14

#define WRITE 15
#define WRITE_REF 16
#define WRITE_NAME 17

//masini date prin nume/referinta sau de tip tranzitie/tranzitie in set 
#define NAME 18
#define REFERENCE 19
#define TRANSITION 20
#define SET_TRANSITION 21

#define SIZE 100

typedef struct { string sym; int head; } Tape;

typedef struct { string name; char sym; } symbol;

typedef struct { int depth, index; string name; } checkpoint; 

typedef struct _turing_machine
{
	string name; 							//nume masina 
	int type; 								//poate fi elementara/nume/referinta

	//doar pentru masini elementare
	string character;					//simbol
	string temporary_name;		//precedat de @
														
	//lista de masini turing; doar pentru masini top														
	vector< struct _turing_machine > mt;				 

	//valabil doar pentru masinile de tip mt_set
	int depth;									//adancimea intr-o masina de tip <mt-trans>
	int no;											//echivalent pentru not simboli	
	string var_name;		//variabila ce retine simbolul curent pentru o tranzitie
	char var_value;			//valoare variabila
	string listName;		//numele unei liste de simboli definita anterior in fisier
	vector<string> symbols;			//simbolii valabili per tranzitie
	
	vector<struct _turing_machine> mtset; 
	 																	
} turing_machine;

void makeAlphabet(vector<char> &alphabet, char *text);

void makeSymbols(vector<symbol> &symbols, char *text);
void makeSymbolList(vector< pair< string, vector<char> > > &symbolList, 
										char *text);

void makeTape(Tape &tape, char *text);

void cutTape(Tape &tape);
										
void addTopTuringMachine(vector<turing_machine> &machines, char *text);
void addTuringMachineCall(vector<turing_machine> &machines, char *text);
void addTuringMachineTransition(vector<turing_machine> &machines, char *text);
void addTuringMachineSetSymbols(vector<turing_machine> &info, char *text, 
																int depth);
void addTuringMachineSetTransition(vector<turing_machine> &machines, 
																	 vector<turing_machine> info, char *text, 
																	 int depth);
void addTuringMachineSetCall(vector<turing_machine> &machines, 
														 vector<turing_machine> info, char *text, 
														 int depth);	

void executeElementaryMachine(Tape &tape, turing_machine machine, 
															vector<symbol> symbols);

int infoMatched(char input, turing_machine mtset, vector<symbol> symbols, 
									vector< pair< string, vector<char> > > symbolList, char &sym);

void executeTransitionMachine(Tape &tape, vector<turing_machine> mtset, 
															turing_machine machine,
															vector<turing_machine> machines,	
															vector<char> alphabet,
															vector<checkpoint> &labels, vector<symbol> symbols, 
															vector< pair< string, vector<char> > > symbolList,
															int current_depth, int index);
															
int executeMachine(int index, turing_machine machine, 
										vector<turing_machine> machines, 
										Tape &tape, vector<char> alphabet, 
										vector<symbol> symbols, 
										vector< pair< string, vector<char> > > symbolList);
										
int executeTopMachine(string machine, vector<turing_machine> machines, 
											Tape &tape, vector<char> alphabet, 
											vector<symbol> symbols, 
											vector< pair< string, vector<char> > > symbolList);										

#endif

