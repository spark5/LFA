/*Oncioiu Raluca
**333CA
**Tema LFA
*/

%{
#include "parsing.h"

using namespace std;

vector<char> alphabet;
vector<symbol> symbols;
vector< pair< string, vector<char> > > symbolList;
Tape tape;
vector<turing_machine> machines;
vector<turing_machine> info;		//informatii despre simbolii valabili pentru orice tranzitie

int depth = 0;						//adancimea unei masini de tip <mt_transition>	
int nr = -1;							//rezultat masina; default, inseamna ca masina a rulat 

%}

DIGIT										[0-9]
LETTER									[a-zA-Z]
OTHER										[#$@!~]|"*"|"%"
IDENTIFIER							[a-zA-Z][a-zA-Z0-9]*
WHITESPACE							[\t\n\r ]
COMMENT 								^";".*\n
SYMBOL									{LETTER}|{DIGIT}|{OTHER}
NAME										({LETTER}|{DIGIT}|"_")*
ALPHABET								^"alphabet"{WHITESPACE}*"::"{WHITESPACE}*({SYMBOL}{WHITESPACE})+";"
NAME_SYMBOL							^{WHITESPACE}*{IDENTIFIER}{WHITESPACE}*"="{WHITESPACE}*{SYMBOL}{WHITESPACE}*";"		
LIST_OF_SYMBOLS					^{WHITESPACE}*{NAME}{WHITESPACE}*":="{WHITESPACE}*"{"{WHITESPACE}*{SYMBOL}{WHITESPACE}*(","{WHITESPACE}*{SYMBOL}{WHITESPACE}*)*"}"{WHITESPACE}*";"
ELEM										{SYMBOL}|("<"{NAME}">")|("&"{NAME})
ELEMENTARY							"L"|"R"|{ELEM}|("L("[!]?{ELEM}")")|("R("[!]?{ELEM}")")
MT_CALL									"["({NAME}|{ELEMENTARY})"]"
MT 											(({NAME}"@")?{MT_CALL})|("&"{NAME})
TRANSITION							{WHITESPACE}*({NAME}"@")?"("{WHITESPACE}*
SET											{WHITESPACE}*({NAME}"@")?[!]?("<"{NAME}">"|"{"{ELEM}(","{WHITESPACE}*{ELEM})*"}"){WHITESPACE}* 	
TURING_MACHINE					^{NAME}{WHITESPACE}*"::="{WHITESPACE}*


%s TURINGC TRANSITIONC SETC
%option stack

%%

{WHITESPACE}					
{COMMENT}																			
{ALPHABET}        														{  
																								makeAlphabet(alphabet, strdup(yytext));
																							}
{NAME_SYMBOL}																	{ 
																								makeSymbols(symbols, strdup(yytext));
																							}
{LIST_OF_SYMBOLS}															{ 
																								makeSymbolList(symbolList, strdup(yytext));}
{TURING_MACHINE}															{
																								yy_push_state(TURINGC);
																								addTopTuringMachine(machines, strdup(yytext));
																								depth = 0;
																							}			
<TURINGC>{MT}{WHITESPACE}*										{ addTuringMachineCall(machines, strdup(yytext));}
<TURINGC>{ELEMENTARY}													

<TURINGC>{TRANSITION}													{	
																								yy_push_state(TRANSITIONC);
																								addTuringMachineTransition(machines, strdup(yytext));	
																							}
<TRANSITIONC>{SET}														{ 
																								yy_push_state(SETC);
																								addTuringMachineSetSymbols(info, strdup(yytext), depth);
																							}
<SETC>{WHITESPACE}*"->"{WHITESPACE}*					
<SETC>{TRANSITION}														{	
																								yy_push_state(TRANSITIONC);	
																								depth++;
																								addTuringMachineSetTransition(machines, info, strdup(yytext), depth);	
																							}
<SETC>{MT}{WHITESPACE}*												{	addTuringMachineSetCall(machines, info, strdup(yytext), depth);}
<SETC>{WHITESPACE}*";"{WHITESPACE}*						{	yy_pop_state(); }															
<TRANSITIONC>{WHITESPACE}*")"{WHITESPACE}*		{
																								yy_pop_state(); 
																								depth--;
																								info.pop_back();
																							}

<TURINGC>{WHITESPACE}*";;"										{ yy_pop_state(); }
%%

int main(int argc, char *argv[])
{
  if(argc == 4)
  {
    FILE *f = fopen(argv[1], "r");

    if(f == NULL)
    {
      fprintf(stderr, "File '%s' can't be opened!\n", argv[1]);
    }

    yyin = f;
  }
  else
  {
    fprintf(stderr, "Usage: ./mtx file.mtx turing_machine_name input\n");
    return 0;
  }
	
  yylex();	
	
	//se creeaza banda pornind de la inputul dat ca argument
	makeTape(tape, argv[3]);
	
	//se executa masina turing data ca input
	nr = executeTopMachine(argv[2], machines, tape, alphabet, symbols, 
		symbolList);	

	//se afiseaza rezultatul 
	if(nr == 0)
	{
		printf("Masina '%s' nu se afla in fisierul '%s'!\n", argv[2], argv[1]);
	}
	
	if(nr == 1)
	{
		printf("Masina '%s' s-a agatat!\n", argv[2]);
	}

	if(nr == -1)
	{
		cutTape(tape);
	
		for(int i = 0; i < (int)tape.sym.size(); i++)
		{		
			if(tape.head == i && i != 0)
			{
				printf("%c", '>');
			}
		
			if(tape.head == i && i == 0)
			{
				printf("%s", "#>");
			}
		
			printf("%c", tape.sym[i]);
		}
	
		printf("\n");
	}
	
	//se sterge memoria
	alphabet.clear();
	symbols.clear();
	tape.sym.clear();
	
	for(int i = 0; i < (int)symbolList.size(); i++)  
	{
		symbolList[i].first.clear();
		symbolList[i].second.clear();
	}
	
	symbolList.clear();
	
	//masini de tip top
	for(int i = 0; i < (int)machines.size(); i++)
	{
		machines[i].name.clear();
		machines[i].character.clear();
		machines[i].temporary_name.clear();
		
		//masini <mt_call> sau <mt_trans>
		for(int j = 0; j < (int)machines[i].mt.size(); j++)
		{
			machines[i].mt[j].name.clear();
			machines[i].mt[j].character.clear();
			machines[i].mt[j].temporary_name.clear();
			
			//daca este de tip <mt_trans>
			if(machines[i].type == TRANSITION)
			{
				//masini de tip <mt_set>, <mt_trans>, <mt_call>
				for(int k = 0; k < (int)machines[i].mt[j].mtset.size(); k++) 
				{
					machines[i].mt[j].mtset[k].name.clear();
					machines[i].mt[j].mtset[k].character.clear();
					machines[i].mt[j].mtset[k].temporary_name.clear();
					machines[i].mt[j].mtset[k].var_name.clear();
					machines[i].mt[j].mtset[k].listName.clear();
					machines[i].mt[j].mtset[k].symbols.clear();
				}
			}
		}
	}
	
	machines.clear();
  return 0;
}

