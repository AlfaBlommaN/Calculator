/*
 * Calculator.cc
 */
#include "Calculator.h"
#include "Expression.h"
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

const string Calculator::valid_command_("?HUBPTSRANIL");


/**
 * run() är huvudfunktionen för kalkylatorn. Skriver först ut hur man använder
 * kalkylatorn och läser sedan sedan in ett kommando i taget och utför det.
 */
void
Calculator::
run()
{
  cout << "Välkommen till Kalkylatorn!\n\n";
  print_help();

  do 
    {
      try 
	{
	  get_command();
	  if (valid_command()) execute_command();
	}
      catch (const exception& e) 
	{
	  cout << e.what() << '\n';
	}
      // Undantag som inte tillhör exception avbryter programkörningen!
    }
  while (command_ != 'S');
}

/**
 * print_help() skriver ut kommandorepertoaren.
 */
void
Calculator::
print_help() const
{
  cout << "  H, ?  Skriv ut denna information\n";
  cout << "  A n   Gör utryck n till aktuellt uttryck\n";
  cout << "  U     Mata in ett nytt uttryck\n";
  cout << "  B     Beräkna aktuellt uttryck\n";
  cout << "  B n   Beräkna uttryck n\n";
  cout << "  I     Visa aktuellt uttryck som infix\n";
  cout << "  I n   Visa uttryck n som infix\n";
  cout << "  L     Lista alla uttryck som infix\n";
  cout << "  N     Visa antal lagrade uttryck\n"; 
  cout << "  P     Visa aktuellt uttryck som postfix\n";
  cout << "  P n   Visa uttryck n som postfix\n";
  cout << "  R     Radera aktuellt uttryck\n";
  cout << "  R n   Radera uttryck n\n";
  cout << "  T     Visa aktuellt uttryck som träd\n";
  cout << "  T n   Visa uttryck n som ett träd\n";
  cout << "  S     Avsluta kalkylatorn\n";
}

/**
 * get_command() läser ett kommando (ett tecken), gör om till versal och lagrar
 * kommandot i medlemmen command_ för vidare behandling av andra operationer. 
 * Ingen kontroll görs om det skrivits mer, i så fall skräp, på kommandoraden.
 */
void
Calculator::
get_command()
{
  cout << ">> ";
  cin >> command_;
  command_ = toupper(command_);
  const string no_argz("?HLNSU");
  if(no_argz.find(command_) != string::npos) argz = false;
  else
    if(cin.peek() != '\n')
      {
	cin >> ws;
	if(isdigit(cin.peek()))
	  {
	    cin >> num;
	    argz = true;
	  }
      } 

    else argz = false;
  cin.ignore(1000, '\n');        
}
 
 
/**
 * valid_command() kontrollerar om kommandot som finns i medlemmen command_
 * tillhör den tillåtna kommandorepertoraren och returnerar antingen true
 * (giltigt kommando) eller false (ogiltigt kommando).
 */
bool
Calculator::
valid_command() const
{
  if (valid_command_.find(command_) == string::npos)
    {
      cout << "Otillåtet kommando: " << command_ << endl;
      return false;
    }
  const string yes_argz{"ABILPRT"};
  if(expression_.empty() && yes_argz.find(command_) != string::npos)
    {
      cout << command_ << " Vectorn är tom, var god och lägg in värden" << endl;
      return false;
    }
  if(argz){
    if(!(num <= expression_.size()))
      {
	cout << "Kalkylatorn har endast: " << expression_.size() << " st sparade uttryck" << endl;
	return false;
      }
  }
  return true;
}

/**
 * execute_command() utför kommandot som finns i medlemmen command_. Kommandot
 * förutsätts ha kontrollerats med valid_command() och alltså är ett giltigt 
 * kommando.
 */

void
Calculator::
execute_command()
{
  int counter = 1;
  int index = curr;
  if(argz)
    index = num-1;


  switch(command_){

  case 'H' || '?': print_help();
    break;
                       
  case 'U' : read_expression(cin);
    break;
                       
  case 'B' : cout << expression_.at(index).evaluate() << endl;
    break;
                       
  case 'P' : cout << expression_.at(index).get_postfix() << endl;
    break;
                       
  case 'I' : cout << expression_.at(index).get_infix() << endl;
    break;
                       
  case 'N' : cout <<" Det finns " << expression_.size()
		  <<" sparade utryck\n";
    break;
  case 'A' : curr = index;                      
    break;

  case 'L' : 
    for(const auto & i: expression_)
      cout << counter++ << ":  " << i.get_infix() << endl;
    break;
                       
  case 'R' :
    if(!expression_.empty()){
      if(!not_last_spot){
	expression_.pop_back();
	curr = expression_.size() - 1;
      } else {
	expression_.erase(begin(expression_) + index);
	curr--;
      }                       
    }
    break;
                       
  case 'T' : expression_.at(index).print_tree(cout);
    break;
                       
  case 'S' : cout << "Kalkylatorn avlutas, välkommen åter!\n";
    break;
                
  default: cout << "Skall inte hända!"<< endl;
    break;                  
  }
}
/**
 * read_expression() läser ett infixuttryck från inströmmen is och ger detta 
 * till funktionen make_expression() som returnerar ett objekt av typen 
 * Expression, vilket lagras som "aktuellt uttryck" i curr_expression_.
 */
void
Calculator::
read_expression(istream& is)
{
  string infix;

  is >> ws;

  if (getline(is, infix))
    {
      expression_.push_back( make_expression(infix));
      curr=expression_.size()-1;
      not_last_spot=false;
    }
  else
    {
      cout << "Felaktig inmatning!\n";
    }
}



