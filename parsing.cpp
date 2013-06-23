#include "parsing.h"

//functie care extrage simbolii dintr-un alfabet precizat intr-un fisier .mt
void makeAlphabet(vector<char> &alphabet, char *text)
{
	int i = 0;
	int length = strlen(text);
	
	//se evita specificatia alfabetului
	while(i < length)
	{
		if(text[i] == ':' && text[i + 1] == ':')
		{
			i += 2;
			break;
		}
		
		i++;
	} 
	
	//se retin simbolii
	for(int j = i; j < length; j++)
	{
		if(text[j] != ' ' && text[j] != ';')
		{
			alphabet.push_back(text[j]);
		}
	}
}

//functie care retine asocieri de tipul: nume si simbol
void makeSymbols(vector<symbol> &symbols, char *text)
{
	char sir[SIZE];
	char *p = strtok(text, " =;\n"); 
	symbol s;
	
	s.name.append(p);
	
	if(p != NULL)
	{
		p = strtok(NULL, " =;");
		strcpy(sir, p);
		s.sym = sir[0]; 
	}
	
	symbols.push_back(s);
}

//functie care retine asocieri de tipul: nume si lista de simboluri
void makeSymbolList(vector< pair< string, vector<char> > > &symbolList, 
										char *text)
{
	char sir[SIZE];
	char *p = strtok(text, " :=\n");
	pair< string, vector<char> > sList;
	
	sList.first.append(p);
	
	p = strtok(NULL, " ,;:={}\n");
	
	while(p != NULL)
	{
		strcpy(sir, p);
		sList.second.push_back(sir[0]);
		
		p = strtok(NULL, " ,;:={}\n");
	}

	symbolList.push_back(sList);
}

//functie care retine inputul dat ca argument in banda
void makeTape(Tape &tape, char *text)
{
	int length = strlen(text);
	
	for(int i = 0; i < length; i++)
	{		
		if(text[i] == '>')
		{
			tape.head = i;
		}
		else
		{
			tape.sym.push_back(text[i]);	
		}
	}
}

//functie care elimina #-urile in plus sau pune un # la sfarsit daca nu exista
void cutTape(Tape &tape)
{
	int i, k = 0;
	string new_tape;
	
	//# de la inceput
	for(i = 0; i < tape.sym.size(); i++)
	{
		if(tape.sym[i] == '#' && tape.head == i + 1)
		{
			k = i;
			break;
		}
	}

	//#de la sfarsit
	for(i = tape.sym.size() - 1; i >= k; i--)
	{
		if(tape.sym[i] != '#')
		{
			break;
		}
	}
	
	if(i != tape.sym.size() - 1)
	{
		for(int j = i + 1; j >= k; j--)
		{	
			new_tape.insert(0, 1, tape.sym[j]);
		}
	}
	else
	{
		tape.sym.push_back('#');
		return;
	}
		
	if(new_tape.size() == 0)
	{
		new_tape.append("##");
		tape.head = 0;
	}
	else if(new_tape.size() == 1)
	{
		new_tape.push_back('#');
	}
	
	//se salveaza noua banda
	tape.sym = new_tape;
	tape.head -= k;
}

//functie care adauga numele unei masini turing de top dintr-un fisier .mt
void addTopTuringMachine(vector<turing_machine> &machines, char *text)
{
	char *p = strtok(text, " :=\n");
	turing_machine mt;
	
	mt.name.append(p);
	mt.type = 0;
	machines.push_back(mt);
}

//functie care adauga o masina turing de tip <mt-call> la o masina de top
void addTuringMachineCall(vector<turing_machine> &machines, char *text)
{

	int no = 0, k = 0;
	int size = machines.size();
	char sir[SIZE], *p;
	string s;
	turing_machine mtc;

	if(size > 0)
	{
		size--;
	}
	
	memset(sir, 0, sizeof(sir));
	
	//este masina elementara cu/fara nume
	if(text[0] != '&')
	{
		for(int i = 0; i < strlen(text); i++)
		{
			if(text[i] == '[')
			{
				break;
			}
		
			while(text[i] != '@')
			{	
				mtc.temporary_name.push_back(text[i]);
				i++; 
			}
		
			if(mtc.temporary_name.size() != 0)
			{				
				break;
			}
		}
	}
	
	if(mtc.temporary_name.size() == 0 && text[k] == '[')
	{
		strncpy(sir, text + 1, strlen(text) - 1);
	}
	else if(mtc.temporary_name.size() != 0)
	{
		int ssize = strlen(text) - mtc.temporary_name.size();
		strncpy(sir, text + mtc.temporary_name.size() + 2, ssize);
	}
	else
	{
		strcpy(sir, text);
	}
	
	//masina elementara		
	if(sir[k + 1] == ']' && sir[k] != 'L' && sir[k] != 'R')
	{
		mtc.type = WRITE;
		mtc.character.push_back(sir[k]);
	}
	else if(sir[k] == '<')
	{
		mtc.type = WRITE_NAME;
		
		for(int i = 1; i < strlen(sir); i++)
		{
			if(sir[i] == '>')
			{
				break;
			}	
			
			mtc.character.push_back(sir[i]);
		}
	}
	else if(sir[k] == '&')
	{
		mtc.type = REFERENCE;
		
		for(int i = 1; i < strlen(sir) - 1; i++)
		{
			if(sir[i] == ' ' || sir[i] == ']')
			{
				break;
			}
			
			s.push_back(sir[i]);
		}
		
		if(s.size() > 1)
		{
			mtc.name = s;
		}
		else
		{
			mtc.var_name = s;
			mtc.type = WRITE_REF;
		}
	}
	else if(strncmp(sir, "L]", 2) == 0)
	{
		mtc.type = LEFT;	
	}
	else if(sir[k] == 'L')
	{
		if(sir[k + 2] == '!')
		{
			no = 1;
		}
	
		if(sir[k + 1] == '(')
		{
			if(no == 1)
			{
				k++;
			}
		
			if(sir[k + 2] == '&')
			{
				if(no == 1)
				{
					mtc.type = LEFT_NOT_UNTIL_REF;
				}
				else
				{
					mtc.type = LEFT_UNTIL_REF;
				}
			
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == ')')
					{
						break;
					}
					
					mtc.character.push_back(sir[i]);
				}
			}
			else if(sir[k + 2] == '<')
			{
				if(sir[2] == '!')
				{
					mtc.type = LEFT_NOT_UNTIL_NAME;
				}
				else
				{
					mtc.type = LEFT_UNTIL_NAME;
				}
				
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == '>')
					{
						break;
					}
					
					if(sir[i] != '<')
						mtc.character.push_back(sir[i]);
				}
			}
			else
			{
				if(no == 1)
				{
					mtc.type = LEFT_NOT_UNTIL;
				}
				else
				{
					mtc.type = LEFT_UNTIL;
				}
				
				mtc.character.push_back(sir[k + 2]);
			}
		}
	}
	else if(strncmp(sir, "R]", 2) == 0)
	{
		mtc.type = RIGHT;	
	}
	else if(sir[k] == 'R')
	{
		if(sir[k + 2] == '!')
		{
			no = 1;
		}
	
		if(sir[k + 1] == '(')
		{
			if(no == 1)
			{
				k++;
			}
		
			if(sir[k + 2] == '&')
			{
				if(no == 1)
				{
					mtc.type = RIGHT_NOT_UNTIL_REF;
				}
				else
				{
					mtc.type = RIGHT_UNTIL_REF;
				}
			
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == ')')
					{
						break;
					}
					
					mtc.character.push_back(sir[i]);
				}
			}
			else if(sir[k + 2] == '<')
			{
				if(sir[2] == '!')
				{
					mtc.type = RIGHT_NOT_UNTIL_NAME;
				}
				else
				{
					mtc.type = RIGHT_UNTIL_NAME;
				}
				
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == '>')
					{
						break;
					}
					
					if(sir[i] != '<')
						mtc.character.push_back(sir[i]);
				}
			}
			else
			{
				if(no == 1)
				{
					mtc.type = RIGHT_NOT_UNTIL;
				}
				else
				{
					mtc.type = RIGHT_UNTIL;
				}
				
				mtc.character.push_back(sir[k + 2]);
			}
		}
	}
	//masina specificata prin nume
	else
	{
		mtc.type = NAME;
		
		for(int i = 0; i < strlen(sir); i++)
		{
			if(sir[i] == ']')
			{
				break;
			}
		
			mtc.name.push_back(sir[i]);
		}
	}
	
	machines[size].mt.push_back(mtc);
}

//functie care adauga informatii pentru o masina de top de tip <mt-trans>
void addTuringMachineTransition(vector<turing_machine> &machines, char *text)
{	
	int size = machines.size();
	turing_machine mtc;
	
	if(size > 0)
	{
		size--;
	}
	
	//daca are nume temporar, se retine
	for(int i = 0; i < strlen(text); i++)
	{
		if(text[i] == '(')
		{
			break;
		}
		
		while(text[i] != '@')
		{
			mtc.temporary_name.push_back(text[i]);
			i++;
		}
	}
	
	mtc.type = TRANSITION;
	machines[size].mt.push_back(mtc);
}

//functie care adauga informatii <set> pentru o masina de tip <mt-trans>
void addTuringMachineSetSymbols(vector<turing_machine> &info, char *text, 
																int depth)
{
	int i, j, found = 0;
	char *p, sir[SIZE];
	string s, ss;
	turing_machine mtc;

	//nu are tip
	mtc.type = 22;
	
	//adancimea unei masini intr-un set dintr-o tranzitie
	mtc.depth = depth;
	mtc.no = 0;
	
	//numele variabilei care retine simbolul curent pentru o tranzitie
	for(i = 0; i < strlen(text); i++)
	{
		if(text[0] == '<')
		{
			found = 0;
			i--;
			break;
		}
		
		if(text[i] == '{')
		{
			found = 1;
			break;
		}
		
		if(text[i] == '!')
		{
			mtc.no = 1;
			break;
		}
				
		while(text[i] != '@')
		{
			mtc.var_name.push_back(text[i]);
			i++;
		}
		
		if(text[i] == '@')
		{
			break;
		}
	}
	
	if(text[i + 1] == '!')
	{
		mtc.no = 1;
		i++;
	}
	
	//contine lista de simboluri
	if(text[i + 1] == '<' && found == 0)
	{	
		i += 2;
	
		while(text[i] != '>')
		{
			mtc.listName.push_back(text[i]);
			i++;
		}
		
		info.push_back(mtc);
		
		return;
	}
	
	//simbolii valabili pentru o tranzitie
	for(j = 0; j < strlen(text); j++)
	{
		if(text[j] == '{')
		{
			break;
		}
	}
	
	for(int k = j + 1; k < strlen(text); k++)
	{
		if(text[k] == '}')
		{
			break;
		}
		
		while(text[k] != ' ' && text[k] != ',' && text[k] != '}')
		{
			s.push_back(text[k]);
			k++;
		}
		
		if(s.size() != 0)
		{
			for(int t = 0; t < s.size(); t++)
			{
				if(s[t] != '<' && s[t] != '>')
				{
					ss.push_back(s[t]);
				}
			}
			
			mtc.symbols.push_back(ss);
			ss.clear();
			s.clear();
		}
	}
	
	info.push_back(mtc);
}

//functie care adauga o masina de tip <mt-trans> 
//la o tranzitie a unei masini <mt-trans>
void addTuringMachineSetTransition(vector<turing_machine> &machines, 
																	 vector<turing_machine> info, char *text, 
																	 int depth)
{
	int size = machines.size();
	turing_machine mtc;
	
	if(size > 0)
	{
		size--;
	}
	
	int m_size = machines[size].mt.size();
	
	if(m_size > 0)
	{
		m_size--;
	}
	
	//daca are nume temporar, se retine
	for(int i = 0; i < strlen(text); i++)
	{
		if(text[i] == '(')
		{
			break;
		}
		
		while(text[i] != '@')
		{
			mtc.temporary_name.push_back(text[i]);
			i++;
		}
	}
	
	//se copiaza informatiile legate de setul din care face parte
	int info_size = info.size();
	
	if(info_size > 0)
	{
		info_size--;
	
		mtc.no = info[info_size].no;
		mtc.var_name = info[info_size].var_name;
		mtc.listName = info[info_size].listName;
		mtc.symbols = info[info_size].symbols;
	}
	
	//nu va fi recunoscuta cu mtset in masina turing de top
	//practic e ca o masina "goala" doar cu informatii
	//despre setul din care face parte
	mtc.type = SET_TRANSITION;
	mtc.depth = depth;
	
	machines[size].mt[m_size].mtset.push_back(mtc);
}

//functie care adauga o masina de tip <mt_call> 
//la un <set> dintr-o masina de tip <mt_trans> 
void addTuringMachineSetCall(vector<turing_machine> &machines, 
														 vector<turing_machine> info, char *text, int depth)
{
	int no = 0, k = 0;
	int size = machines.size();
	char sir[SIZE], *p;
	string s;
	turing_machine mtc;
	
	if(size > 0)
	{
		size--;
	}
	
	int m_size = machines[size].mt.size();
	
	if(m_size > 0)
	{
		m_size--;
	}
	
	int set_size = machines[size].mt[m_size].mtset.size();
	
	if(set_size > 0)
	{
		set_size--;
	}
	
	//se copiaza informatiile legate de setul din care face parte
	int info_size = info.size();
	
	if(info_size > 0)
	{
		info_size--;
	
		mtc.no = info[info_size].no;
		mtc.var_name = info[info_size].var_name;
		mtc.listName = info[info_size].listName;
		mtc.symbols = info[info_size].symbols;
	}
	
	//adancimea set-ului fata de o tranzitie din care face parte masina turing 
	mtc.depth = depth;
	
	memset(sir, 0, sizeof(sir));
	
	//este masina elementara cu/fara nume
	if(text[0] != '&')
	{
		for(int i = 0; i < strlen(text); i++)
		{
			if(text[i] == '[')
			{
				break;
			}
		
			while(text[i] != '@')
			{	
				mtc.temporary_name.push_back(text[i]);
				i++; 
			}
		
			if(mtc.temporary_name.size() != 0)
			{			
				break;
			}
		}
	}
	
	if(mtc.temporary_name.size() == 0 && text[k] == '[')
	{
		strncpy(sir, text + 1, strlen(text) - 1);
	}
	else if(mtc.temporary_name.size() != 0)
	{
		int ssize = strlen(text) - mtc.temporary_name.size();
		strncpy(sir, text + mtc.temporary_name.size() + 2, ssize);
	}
	else
	{
		strcpy(sir, text);
	}
	
	//masina elementara		
	if(sir[k + 1] == ']' && sir[k] != 'L' && sir[k] != 'R')
	{
		mtc.type = WRITE;
		mtc.character.push_back(sir[k]);
	}
	else if(sir[k] == '<')
	{
		mtc.type = WRITE_NAME;
		
		for(int i = 1; i < strlen(sir); i++)
		{
			if(sir[i] == '>')
			{
				break;
			}	
			
			mtc.character.push_back(sir[i]);
		}
	}
	else if(sir[k] == '&')
	{
		mtc.type = REFERENCE;
		
		for(int i = 1; i < strlen(sir) - 1; i++)
		{
			if(sir[i] == ' ' || sir[i] == ']')
			{
				break;
			}
			
			s.push_back(sir[i]);
		}
		
		if(s.size() > 1)
		{
			mtc.name = s;
		}
		else
		{
			mtc.var_name = s;
			mtc.type = WRITE_REF;
		}
	}
	else if(strncmp(sir, "L]", 2) == 0)
	{
		mtc.type = LEFT;	
	}
	else if(sir[k] == 'L')
	{
		if(sir[k + 2] == '!')
		{
			no = 1;
		}
	
		if(sir[k + 1] == '(')
		{
			if(no == 1)
			{
				k++;
			}
		
			if(sir[k + 2] == '&')
			{
				if(no == 1)
				{
					mtc.type = LEFT_NOT_UNTIL_REF;
				}
				else
				{
					mtc.type = LEFT_UNTIL_REF;
				}
			
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == ')')
					{
						break;
					}
					
					mtc.character.push_back(sir[i]);
				}
			}
			else if(sir[k + 2] == '<')
			{
				if(sir[2] == '!')
				{
					mtc.type = LEFT_NOT_UNTIL_NAME;
				}
				else
				{
					mtc.type = LEFT_UNTIL_NAME;
				}
				
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == '>')
					{
						break;
					}
					
					if(sir[i] != '<')
						mtc.character.push_back(sir[i]);
				}
			}
			else
			{
				if(no == 1)
				{
					mtc.type = LEFT_NOT_UNTIL;
				}
				else
				{
					mtc.type = LEFT_UNTIL;
				}
				
				mtc.character.push_back(sir[k + 2]);
			}
		}
	}
	else if(strncmp(sir, "R]", 2) == 0)
	{
		mtc.type = RIGHT;	
	}
	else if(sir[k] == 'R')
	{
		if(sir[k + 2] == '!')
		{
			no = 1;
		}
	
		if(sir[k + 1] == '(')
		{
			if(no == 1)
			{
				k++;
			}
		
			if(sir[k + 2] == '&')
			{
				if(no == 1)
				{
					mtc.type = RIGHT_NOT_UNTIL_REF;
				}
				else
				{
					mtc.type = RIGHT_UNTIL_REF;
				}
			
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == ')')
					{
						break;
					}
					
					mtc.character.push_back(sir[i]);
				}
			}
			else if(sir[k + 2] == '<')
			{
				if(sir[2] == '!')
				{
					mtc.type = RIGHT_NOT_UNTIL_NAME;
				}
				else
				{
					mtc.type = RIGHT_UNTIL_NAME;
				}
				
				for(int i = 3; i < strlen(sir); i++)
				{
					if(sir[i] == '>')
					{
						break;
					}
					
					if(sir[i] != '<')
						mtc.character.push_back(sir[i]);
				}
			}
			else
			{
				if(no == 1)
				{
					mtc.type = RIGHT_NOT_UNTIL;
				}
				else
				{
					mtc.type = RIGHT_UNTIL;
				}
				
				mtc.character.push_back(sir[k + 2]);
			}
		}
	}
	//masina specificata prin nume
	else
	{
		mtc.type = NAME;
		
		for(int i = 0; i < strlen(sir); i++)
		{
			if(sir[i] == ']')
			{
				break;
			}
		
			mtc.name.push_back(sir[i]);
		}
	}
	
	machines[size].mt[m_size].mtset.push_back(mtc);
}

//functie care executa o masina elementara 
void executeElementaryMachine(Tape &tape, turing_machine machine, 
															vector<symbol> symbols)
{
	char sym;
	
	switch(machine.type)
	{ 
		case WRITE:
		{
			//se suprascrie caracterul curent
			tape.sym[tape.head] = machine.character[0]; 
			break;
		}
		case WRITE_NAME:
		{
			//se cauta numele simbolului in lista cu nume de simboli
			for(int k = 0; k < (int)symbols.size(); k++)
			{
				if(symbols[k].name.compare(machine.character) == 0)
				{
					sym = symbols[k].sym;		
				}
			}
				
			//se suprascrie caracterul curent
			tape.sym[tape.head] = sym;
		
			break;
		}
		case WRITE_REF:
		{
			//se suprascrie caracterul curent
			tape.sym[tape.head] = machine.var_value;
			
			break;
		}
		case LEFT:
		{
			if(tape.head == 0)
			{
				tape.sym.insert(0, 1, '#');
				tape.head++;
			}	
	
			tape.head--;
			
			break;
		}
		case RIGHT:
		{
			if(tape.head == tape.sym.size() - 1)
			{
				tape.sym.push_back('#');
			}
			
			tape.head++; 
			
			break;
		}
		case LEFT_UNTIL:
		{
			if(tape.head == 0)
			{
				tape.sym.insert(0, 1, '#');
				tape.head++;
			}
			
			tape.head--;
		
			while(tape.sym[tape.head] != machine.character[0])
			{	
				if(tape.head == 0)
				{
					tape.sym.insert(0, 1, '#');
					tape.head++;
				}
			
				tape.head--;
			}
			
			break;
		}
		case RIGHT_UNTIL:
		{
			if(tape.head == tape.sym.size() - 1)
			{
				tape.sym.push_back('#');
			}
			
			tape.head++;
			
			while(tape.sym[tape.head] != machine.character[0])
			{	
				if(tape.head == tape.sym.size() - 1)
				{
					tape.sym.push_back('#');
				}
			
				tape.head++;
			}
			
			break;
		}
		case LEFT_NOT_UNTIL:
		{
			if(tape.head == 0)
			{
				tape.sym.insert(0, 1, '#');
				tape.head++;
			}
			
			tape.head--;
		
			while(tape.sym[tape.head] == machine.character[0])
			{	
				if(tape.head == 0)
				{
					tape.sym.insert(0, 1, '#');
					tape.head++;
				}
			
				tape.head--;
			}
			
			break;
		}
		case RIGHT_NOT_UNTIL:
		{
			if(tape.head == tape.sym.size() - 1)
			{
				tape.sym.push_back('#');
			}
		
			tape.head++;
			
			while(tape.sym[tape.head] == machine.character[0])
			{	
				if(tape.head == tape.sym.size() - 1)
				{
					tape.sym.push_back('#');
				}
			
				tape.head++;
			}
			
			break;
		}
		case LEFT_UNTIL_REF:
		{
			if(tape.head == 0)
			{
				tape.sym.insert(0, 1, '#');
				tape.head++;
			}
			
			tape.head--;
		
			while(tape.sym[tape.head] != machine.var_value)
			{	
				if(tape.head == 0)
				{
					tape.sym.insert(0, 1, '#');
					tape.head++;
				}
			
				tape.head--;
			}
			
			break;
		}
		case RIGHT_UNTIL_REF:
		{
			if(tape.head == tape.sym.size() - 1)
			{
				tape.sym.push_back('#');
			}
			
			tape.head++;
			
			while(tape.sym[tape.head] != machine.var_value)
			{	
				if(tape.head == tape.sym.size() - 1)
				{
					tape.sym.push_back('#');
				}
			
				tape.head++;
			}
			
			break;
		}
		case LEFT_NOT_UNTIL_REF:
		{
			if(tape.head == 0)
			{
				tape.sym.insert(0, 1, '#');
				tape.head++;
			}
			
			tape.head--;
		
			while(tape.sym[tape.head] == machine.var_value)
			{	
				if(tape.head == 0)
				{
					tape.sym.insert(0, 1, '#');
					tape.head++;
				}
			
				tape.head--;
			}
		
			break;
		}
		case RIGHT_NOT_UNTIL_REF:
		{
			if(tape.head == tape.sym.size() - 1)
			{
				tape.sym.push_back('#');
			}
			
			tape.head++;
			
			while(tape.sym[tape.head] == machine.var_value)
			{	
				if(tape.head == tape.sym.size() - 1)
				{
					tape.sym.push_back('#');
				}
			
				tape.head++;
			}

			break;
		}
		case LEFT_UNTIL_NAME:
		{
			if(tape.head == 0)
			{
				tape.sym.insert(0, 1, '#');
				tape.head++;
			}
			
			tape.head--;
		
			//se cauta numele simbolului in lista cu nume de simboli
			for(int k = 0; k < (int)symbols.size(); k++)
			{
				if(symbols[k].name.compare(machine.character) == 0)
				{
					sym = symbols[k].sym;		
				}
			}
		
			while(tape.sym[tape.head] != sym)
			{	
				if(tape.head == 0)
				{
					tape.sym.insert(0, 1, '#');
					tape.head++;
				}
			
				tape.head--;
			}
			
			break;
		}
		case RIGHT_UNTIL_NAME:
		{
			if(tape.head == tape.sym.size() - 1)
			{
				tape.sym.push_back('#');
			}
			
			tape.head++;
		
			//se cauta numele simbolului in lista cu nume de simboli
			for(int k = 0; k < (int)symbols.size(); k++)
			{
				if(symbols[k].name.compare(machine.character) == 0)
				{
					sym = symbols[k].sym;		
				}
			}
	
			while(tape.sym[tape.head] != sym)
			{	
				if(tape.head == tape.sym.size() - 1)
				{
					tape.sym.push_back('#');
				}
			
				tape.head++;
			}
			
			break;
		}
		case LEFT_NOT_UNTIL_NAME:
		{
			if(tape.head == 0)
			{
				tape.sym.insert(0, 1, '#');
				tape.head++;
			}
			
			tape.head--;
		
			//se cauta numele simbolului in lista cu nume de simboli
			for(int k = 0; k < (int)symbols.size(); k++)
			{
				if(symbols[k].name.compare(machine.character) == 0)
				{
					sym = symbols[k].sym;		
				}
			}
		
			while(tape.sym[tape.head] == sym)
			{	
				if(tape.head == 0)
				{
					tape.sym.insert(0, 1, '#');
					tape.head++;
				}
			
				tape.head--;
			}
			
			break;
		}
		case RIGHT_NOT_UNTIL_NAME:
		{
			if(tape.head == tape.sym.size() - 1)
			{
				tape.sym.push_back('#');
			}
			
			tape.head++;
		
			//se cauta numele simbolului in lista cu nume de simboli
			for(int k = 0; k < (int)symbols.size(); k++)
			{
				if(symbols[k].name.compare(machine.character) == 0)
				{
					sym = symbols[k].sym;		
				}
			}
	
			while(tape.sym[tape.head] == sym)
			{	
				if(tape.head == tape.sym.size() - 1)
				{
					tape.sym.push_back('#');
				}
			
				tape.head++;
			}
			
			break;
		}
	}
}

//functie ce verifica daca se face match pe un set de simboli dintr-o tranzitie
int infoMatched(char input, turing_machine mtset, vector<symbol> symbols, 
								vector< pair< string, vector<char> > > symbolList, char &sym)
{
	int j, k, count = 0, found = 0;

	//nume de lista de simboluri
	if(mtset.listName.size() != 0)
	{	
		//se cauta in vectorul cu liste de simboluri
		for(j = 0; j < symbolList.size(); j++)
		{
			if(symbolList[j].first.compare(mtset.listName) == 0)
			{
				found = 1;
				break;
			}
		}
		
		if(found == 1)
		{
			//se verifica simbolii din lista de simboluri
			for(k = 0; k < symbolList[j].second.size(); k++)
			{	
				if(mtset.no == 0)
				{
					if(input == symbolList[j].second[k])
					{
						sym = input;
						return 1;
					}
				}
				//simboli negati
				else
				{
					if(input != symbolList[j].second[k])
					{
						count++;
					}
				}
			}
		}
		
		//se respecta regula 
		if((mtset.no == 1) && (count == mtset.symbols.size()))
		{
			sym = input;
			return 1;
		}
	}
	//set de simboli
	else
	{
		for(j = 0; j < mtset.symbols.size(); j++)
		{
			if(mtset.no == 0)
			{
				//simbol 
				if(mtset.symbols[j].size() == 1)
				{
					if(input == mtset.symbols[j][0])
					{
						sym = input;
						return 1;
					}
				}
				//nume de simbol
				else
				{
					//se cauta in asocierile de tip nume variabila, simbol
					for(k = 0; k < symbols.size(); k++)
					{
						if((symbols[k].name.compare(mtset.symbols[j]) == 0) &&
							 (input == symbols[k].sym))
						{
							sym = input;
							return 1;
						}
					}
				}
			}
			//simboli negati
			else
			{
				//simbol 
				if(mtset.symbols[j].size() == 1)
				{
					if(input != mtset.symbols[j][0])
					{
						count++;
					}
				}
				//nume de simbol
				else
				{
					//se cauta in asocierile de tip nume variabila, simbol
					for(k = 0; k < symbols.size(); k++)
					{
						if((symbols[k].name.compare(mtset.symbols[j]) == 0) &&
							 (input == symbols[k].sym))
						{
							count++;
						}
					}
				}
			}
		}
		
		//se respecta regula 
		if((mtset.no == 1) && (count == mtset.symbols.size()))
		{
			sym = input;
			return 1;
		}
	}
	
	return 0;
}

//functie care executa o masina de tip tranzitie
void executeTransitionMachine(Tape &tape, vector<turing_machine> mtset, 
															turing_machine machine,
															vector<turing_machine> machines,	
															vector<char> alphabet,
															vector<checkpoint> &labels, vector<symbol> symbols, 
															vector< pair< string, vector<char> > > symbolList,
															int current_depth, int index)
{
	int t, j, found1 = 0, found2 = 0;
	char sym, ssym;
	bool finished = false;
	checkpoint label;
  
	for(int i = index; i < mtset.size(); i++)
	{
		found1 = 0;
		
		//se executa masini de pe nivelul curent
		if(mtset[i].depth == current_depth)
		{
			//avand conditiile pe care se face match 
			if(mtset[i].type != SET_TRANSITION)
			{
				found1 = infoMatched(tape.sym[tape.head], mtset[i], symbols, symbolList, sym);
			}
			else
			{
				found1 = 1;
			}
			
			//s-a gasit setul pe care s-a facut match
			if(found1 == 1)
			{	
				//se executa toate masinile de pe acel set
				for(j = i; j < mtset.size(); j++)
				{
					found2 = 0;
					
					//se termina setul 
					if((mtset[j].depth != current_depth) && (mtset[j].type != SET_TRANSITION))
					{
						finished = true;
						break;
					}
					
					//avand conditiile pe care se face match
					found2 = infoMatched(sym, mtset[j], symbols, symbolList, ssym);
					
					if(found2 == 1)
					{
						//masina elementara
						if(mtset[j].type <= 17)
						{
							if(mtset[j].temporary_name.size() != 0)
							{
								label.index = j;
								label.depth = mtset[j].depth;
								label.name = mtset[j].temporary_name;
								
								labels.push_back(label);
							}
						
							if(mtset[j].var_name.size() != 0)
							{
								mtset[j].var_value = sym;
							}
						
							executeElementaryMachine(tape, mtset[j], symbols);
						}
						//masina data prin nume
						else if(mtset[j].type == NAME)
						{
							executeTopMachine(mtset[j].name, machines, tape, alphabet, 
								symbols, symbolList);
						}
						//masina referinta
						else if(mtset[j].type == REFERENCE)
						{
							for(t = labels.size() - 1; t >= 0; t--)
							{	
								if(labels[t].name.compare(mtset[j].name) == 0)
								{
									current_depth = labels[t].depth;
									break;
								}
							}
		
							//referinta la o masina din cadrul masinii mt_trans
							if(labels[t].depth != -1)
							{
								executeTransitionMachine(tape, mtset, machine, machines, alphabet, 
									labels, symbols, symbolList, current_depth, labels[t].index);	
							}
							//referinta la o masina de pe un nivel mai jos				
							else 
							{
								executeMachine(labels[t].index, machine, machines, tape, alphabet, 
									symbols, symbolList);
							}
						}
						//masina de tip tranzitie
						else 
						{
							current_depth = mtset[j].depth;
							
							label.index = j;
							label.depth = current_depth;
							label.name = mtset[j].temporary_name;
							
							labels.push_back(label);
							i = j;
				
							break;
						}
					}
					else 
					{	
						if(labels.size() != 0)
						{
							for(int k = 0; k < labels.size(); k++)
							{
								if(labels[k].depth == current_depth)
								{
									labels.erase(labels.begin() + k);	
								}
								else
								{
									break;
								}
							}
						}
					
						finished = true;
						break;
					}
				}
				
				//in caz ca s-a terminat setul, se termina masina de executat
				if((j == mtset.size()) || (finished == true))
				{
					break;
				}
			}
			else
			{
				continue;
			}
		}
	}
}

//functie care executa masina Turing 'machine' specificata in vectorul de masini
//turing 'machines', tinand cont de inputul de pe banda si specificatiile
//despre alfabet si simboli din fisierul cu masini turing 
int executeMachine(int index, turing_machine machine, 
									 vector<turing_machine> machines, 	
									 Tape &tape, vector<char> alphabet, 
									 vector<symbol> symbols, 
									 vector< pair< string, vector<char> > > symbolList)
{
	int t, current_depth = 0;
	char sym;
	checkpoint label;
	vector<checkpoint> labels;

	//se ia specificatia masinii si se executa
	for(int i = index; i < (int)machine.mt.size(); i++)
	{
		//este masina elementara
		if(machine.mt[i].type <= 17)
		{
			if(machine.mt[i].temporary_name.size() != 0)
			{
				label.index = i;
				label.depth = -1;
				label.name = machine.mt[i].temporary_name;
		
				labels.push_back(label);
			}
			
			executeElementaryMachine(tape, machine.mt[i], symbols); 
		}
		
		//masina specificata prin nume
		if(machine.mt[i].type == NAME)
		{
			executeTopMachine(machine.mt[i].name, machines, tape, alphabet, symbols, 
				symbolList);
		}
		
		//masina specificata prin referinta
		if(machine.mt[i].type == REFERENCE)
		{
			//se cauta indexul in vectorul de masini din specificatie
			for(t = labels.size() - 1; t >= 0; t--)
			{	
				if(labels[t].name.compare(machine.mt[i].name) == 0)
				{
					current_depth = labels[t].depth;
					break;
				}
			}

			i = labels[t].index;
		}
		
		//masina cu tranzitii
		if(machine.mt[i].type == TRANSITION)
		{	
			if(machine.mt[i].temporary_name.size() != 0)
			{
				label.index = i;
				label.depth = current_depth;
				label.name = machine.mt[i].temporary_name;
				
				labels.push_back(label);
			}
			
			executeTransitionMachine(tape, machine.mt[i].mtset, machine, machines, 
				alphabet, labels, symbols, symbolList, current_depth, 0);
		}
	}
}		

//functie care identifica masina de top cu numele dat de stringul 'machine'
int executeTopMachine(string machine, vector<turing_machine> machines, 
											Tape &tape, vector<char> alphabet, 
											vector<symbol> symbols, 
											vector< pair< string, vector<char> > > symbolList)
{
	//se cauta specificatia masinii turing de top in vectorul 'machines'
	for(int k = 0; k < (int)machines.size(); k++)
	{
		if(machines[k].name.compare(machine) == 0)
		{
			executeMachine(0, machines[k], machines, tape, alphabet, symbols, symbolList);
			
			return -1;
		}
	}
	
	//masina nu exista in fisier
	return 0;
}								

