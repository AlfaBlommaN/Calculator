/*
 * Expression_Tree.cc
 */
#include <iostream>
#include "Expression_Tree.h"
#include <iomanip>
#include <cmath>
#include <sstream>
#include <string>
#include <typeinfo>

using namespace std;

std::string Binary_Operator::get_postfix() const
{
  return operator_child_left_->get_postfix() + " " + operator_child_right_->get_postfix() + " " + str();
}


string Binary_Operator::get_infix() const
{
  Binary_Operator * pointer_right;
  Binary_Operator * pointer_left;
  pointer_right = dynamic_cast<Binary_Operator*>(operator_child_right_);
  pointer_left = dynamic_cast<Binary_Operator*>(operator_child_left_);
  string str_operator_right = operator_child_right_->get_infix();
  string str_operator_left = operator_child_left_->get_infix();
  if(pointer_right)
    {
      str_operator_right = '(' + str_operator_right + ')';
    }
  if(pointer_left)
    {
      str_operator_left = '(' + str_operator_left + ')';
    }
  return str_operator_left + ' ' +str() + ' ' + str_operator_right;
}


void Binary_Operator::print(std::ostream &os, const unsigned width) const 
{
  operator_child_right_-> print(os, width+3);
  os  << setw(width+2) << '/'     << endl
      << setw(width+1) << str()       << endl
      << setw(width+2) << '\\'        << endl ;
  operator_child_left_-> print(os, width+3);
}

string Operand::get_postfix() const
{
  return str();
}

string Operand:: get_infix() const
{
  return str();
}

void Operand::print(ostream &os, const unsigned width) const 
{
  os <<setw(width-1)<<right<<" "<< str() <<endl;
}

std::string Plus::str() const 
{
  return "+";
}

Plus* Plus::clone() const 
{
  try
    {
      return new Plus(*this);
    }
  catch (const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch (...)
    {
      throw expression_tree_error("Plus::clone() something went wrong here");
    }       
}

long double Plus::evaluate() const
{
  if (!operator_child_left_ || !operator_child_right_)
    throw expression_tree_error("Plus::evaluate()   saknar operander");

  return operator_child_right_->evaluate() + operator_child_left_-> evaluate();
}

std::string Minus::str() const
{
  return "-";
}

Minus* Minus::clone() const 
{
  try
    {
      return new Minus(*this);
    }
  catch (const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch (...)
    {
      throw expression_tree_error("Plus::clone() något gick fel här!");
    }
}

long double Minus::evaluate() const
{
  if(!operator_child_left_ || !operator_child_right_)
    throw expression_tree_error("Minus::evaluate() saknar operand(er)");
  return operator_child_right_->evaluate() - operator_child_left_->evaluate();
}
std::string Times::str() const
{
  return "*";
}

Times* Times::clone() const
{
  try
    {
      return new Times(*this);
    }
  catch (const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch (...)
    {
      throw expression_tree_error("Times::clone() något gick fel här");
    }       
}

long double Times::evaluate() const
{
  if(!operator_child_left_ || !operator_child_right_)
    throw expression_tree_error("Minus::evaluate() saknar operand(er)");
  return operator_child_right_->evaluate() * operator_child_left_->evaluate();
}

std::string Divide::str() const 
{
  return "/";
}

Divide* Divide::clone() const
{
  try
    {
      return new Divide(*this);
    }
  catch (const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch(...)
    {
      throw expression_tree_error("Divide::clone() något gick fel här");
    }
}

long double Divide::evaluate()  const
{

  long double right = 0;
  right = operator_child_right_ -> evaluate(); 
  if(!operator_child_left_ || !operator_child_right_)
    {
      throw expression_tree_error("Divde::clone() saknar operand(er)");
    }
  else if (right == 0)
    {
      throw expression_tree_error("Division med 0");
    }
  else
    {
      return operator_child_left_->evaluate() / right; 
    }       
}

std::string Power::str() const
{
  return "^";
}

Power* Power::clone() const
{
  try
    {
      return new Power(*this);
    }
  catch (const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch(...)
    {
      throw expression_tree_error("Power::clone() något gick fel här!");
    }
}

long double Power::evaluate()  const
{
  if(!operator_child_left_ || !operator_child_right_)
    throw expression_tree_error("Power::evaluate()  saknar operander");
  return pow(operator_child_left_->evaluate(), operator_child_right_->evaluate());
}

std::string Assign::str() const
{
  return "=";
}

Assign* Assign::clone() const
{
  try
    {
      return new Assign(*this);
    }
  catch (const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch(...)
    {
      throw expression_tree_error("Assign::clone() något gick fel här!");
    }
}

string Assign::get_infix() const
{
  return operator_child_left_->get_infix()+' '+ str()+' '+ operator_child_right_->get_infix();
}

long double Assign::evaluate() const
{       
  Variable *pleft{dynamic_cast<Variable*>(operator_child_left_) };
  if(pleft == nullptr)
    throw expression_tree_error("Assign::evaluate() något gick fel här!");

  pleft->set_value(operator_child_right_->evaluate());
  return pleft->get_value();
}

std::string Integer::str() const 
{
  return std::to_string(value_);
}

Integer* Integer::clone() const 
{
  try
    {
      return new Integer(*this);
    }
  catch ( const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch(...)
    {
      throw expression_tree_error("Integer::evaluate() något gick fel här!");
    }
}

long double Integer::evaluate() const 
{

  return static_cast <long double> (value_);
}

std::string Real::str() const
{  
  stringstream remove_deci;
  remove_deci << setprecision(2) << value_;
  return remove_deci.str();        
}

Real* Real::clone() const 
{
  try
    {
      return new Real(*this);
    }
  catch ( const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch(...)
    {
      throw expression_tree_error("Real::evaluate() något gick fel!");
    }
}

long double Real::evaluate() const
{
  return value_;
}

std::string Variable::str() const 
{
  return variable_;
}

Variable* Variable::clone() const 
{
  try
    {
      return new Variable(*this);
    }
  catch ( const bad_alloc& e)
    {
      throw expression_tree_error{e.what()};
    }
  catch(...)
    {
      throw expression_tree_error("Variable::evaluate() något gick fel!");
    }
}

long double Variable::evaluate() const
{
  return value_;
}

long double Variable::get_value() const
{
  return value_;
}

void Variable::set_value(long double value)
{
  value_ = value;
}
