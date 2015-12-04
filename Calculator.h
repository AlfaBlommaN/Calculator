/*
 * Calculator.h
 */
#ifndef CALCULATOR_H
#define CALCULATOR_H
#include "Expression.h"
#include <iosfwd>
#include <vector>
/**
 * Calculator är en klass för hantering av enkla aritmetiska uttryck.
 */
class Calculator
{
 public:
  Calculator() = default;

  Calculator(const Calculator&) = delete;
  Calculator& operator=(const Calculator&) = delete;

  void run();

 private:

  static const std::string valid_command_;
  std:: vector <Expression> expression_; 

  bool argz = false;
  bool not_last_spot;
  bool valid_command() const;

  char command_;
  unsigned  curr = 0;
  unsigned num=0;

  void print_help() const;
  void get_command();
  void execute_command();



  void read_expression(std::istream&);
};

#endif
 
