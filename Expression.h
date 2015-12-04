/*
 * Expression.h
 */
#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <iosfwd>
#include <stdexcept>
#include <string>

/**
 * expression_error kastas om fel inträffar i en Expression-operation.
 * Ett diagnostiskt meddelande ska skickas med.
 */

/**
 * Expression är en klass för att representera ett enkelt aritmetiskt uttryck.
 */
class Expression
{
 public:
  
  friend Expression make_expression(const std::string &);

  Expression() = default;
  ~Expression ();
  Expression & operator=(const Expression & right) &;
  Expression & operator =(Expression && other) & noexcept;

  Expression(const Expression & other);  
  Expression(Expression && other) noexcept;
  long double evaluate() const;

  std::string get_postfix() const;
  std::string get_infix() const;

  bool empty() const;
  void clear() & noexcept;
  void print_tree(std::ostream& os ) const;
  void swap(Expression& other) noexcept;

 private:
  class Expression_Tree * root_ {nullptr};
  explicit Expression(class Expression_Tree* p) : root_(p) {}

};

void swap(Expression& left, Expression& right) noexcept;

Expression make_expression(const std::string& infix);


#endif

