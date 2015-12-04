/*
 * Expression_Tree.h
 */
#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H
#include <iosfwd>
#include <string>
#include <stdexcept>
#include <iostream>

class Expression_Tree
{
 public:
  virtual ~Expression_Tree() noexcept
    {
    }
    
  virtual std::string           get_postfix()   const = 0;
  virtual std::string           get_infix()     const = 0;    
  virtual std::string           str()           const = 0;  
  virtual Expression_Tree *     clone()         const = 0;  
  virtual long double           evaluate()      const = 0;        

  virtual void print(std::ostream& os, const unsigned width = 0) const = 0;   


 protected: 
  Expression_Tree() = default;
  Expression_Tree & operator= ( const Expression_Tree & ) = delete;
  Expression_Tree ( const Expression_Tree & ) = default;
  Expression_Tree ( Expression_Tree && ) = default;

};

class Binary_Operator : public Expression_Tree
{
 public:
  ~Binary_Operator() 
    {
      delete operator_child_left_;
      delete operator_child_right_;
    }

  Binary_Operator & operator= ( const Binary_Operator & ) = delete;
  Binary_Operator() = delete;

  std::string      get_postfix()  const override;
  std::string      get_infix() const override;   
  void             print(std::ostream &os, const unsigned width) const override;

 protected:

 Binary_Operator(const Binary_Operator& b ) 
   : operator_child_left_(b.operator_child_left_->clone()), operator_child_right_(b.operator_child_right_->clone()) { }
 Binary_Operator( Expression_Tree* left,  Expression_Tree* right)
   :  operator_child_left_ ( left) , 
    operator_child_right_( right) 
    {}

// move konstruktor
Binary_Operator(Binary_Operator&& other)
{
  std::swap(operator_child_left_, other.operator_child_left_);
  std::swap(operator_child_right_, other.operator_child_right_);
}

  Expression_Tree     * operator_child_left_;
  Expression_Tree     * operator_child_right_;  
  
};

class Operand : public Expression_Tree
{
 public:
  std::string   get_postfix() const override;
  std::string   get_infix() const override;  
  void          print(std::ostream & os, const unsigned width) const override;
  ~ Operand () = default;
  Operand (Operand &&) = default;
  Operand & operator= ( const Operand & ) = delete;


 protected:
  Operand() = default;
  Operand ( const Operand & ) = default;

};

class Integer final: public Operand
{
 public:
  ~Integer() = default;
  explicit Integer(int value)
    :   value_(value)
  {}

  std::string   str()       const override;
  Integer *     clone()     const override;
  long double   evaluate()  const override;
    
 private:
  Integer & operator=(const Integer & ) = delete;
  Integer( Integer && )                 = default; 


  Integer(const Integer & )             = default;

  const int value_;
};

class Real final: public Operand
{
 public:
  ~Real() = default;
  explicit Real(long double value)
    :   value_(value)
  {}

  std::string   str()       const override;
  Real *        clone()     const override;
  long double   evaluate()  const  override;

 private:
  Real & operator=(const Real & ) = delete;

  Real( Real && )                 = default; 
  Real(const Real & )              = default;

  const long double value_;
};

class Variable final: public Operand
{
 public:
  ~Variable() = default;
  explicit Variable(const std::string & variable, long double value = 0.0)
    : variable_(variable), value_(value)
  {}

  std::string   str()       const override;
  Variable *    clone()     const override;
  long double   evaluate()  const  override;

  void        set_value(long double);
  long double get_value() const;
  std::string get_name()  const;

 private:
  Variable & operator=(const Variable & ) = delete;

  Variable( Variable && )                 = default; 
  Variable(const Variable & )             = default;

  const std::string variable_;
  long double value_;
};

class Plus final: public Binary_Operator
{ 
 public:
  ~Plus() = default;
 Plus(Expression_Tree * left, Expression_Tree * right)
   : Binary_Operator(left,right)
    {} 

  std::string   str()       const override;
  Plus*         clone()     const override;
  long double   evaluate()  const override;

 protected:
 Plus(const Plus & other) : Binary_Operator(other) {}
 Plus(Plus &&other) : Binary_Operator(other){}
};  

class Minus final: public Binary_Operator 
{
 public:
  ~Minus() = default;
 Minus(Expression_Tree * left, Expression_Tree * right)
   : Binary_Operator(left,right)
    {}

  std::string   str()       const override; 
  Minus*        clone()     const override;
  long double   evaluate()  const override;

 protected:
 Minus(const Minus & other) : Binary_Operator(other){}
 Minus(Minus &&other) : Binary_Operator(other){}
};

class Times final: public Binary_Operator
{
 public:
  ~Times() = default;
 Times(Expression_Tree * left, Expression_Tree * right)
   : Binary_Operator(left,right)
    {}

  std::string   str()       const override; 
  Times*        clone()     const override;
  long double   evaluate()  const override; 

 protected:
 Times(const Times & other) : Binary_Operator(other){}
 Times(Times &&other) : Binary_Operator(other){}

};

class Divide final: public Binary_Operator
{
 public:
  ~Divide() = default;
 Divide(Expression_Tree * left, Expression_Tree * right)
   : Binary_Operator(left,right)
    {}

  std::string   str()      const override;
  Divide*       clone()    const override;
  long double   evaluate() const override;
  Divide  & operator= ( const Divide  & ) = delete;

 protected:
 Divide(const Divide & other) : Binary_Operator(other){}
 Divide(Divide &&other) : Binary_Operator(other){}
 
};
class Assign final: public Binary_Operator
{ 
 public:
  ~Assign() = default;
 Assign(Expression_Tree * left, Expression_Tree * right)
   : Binary_Operator(left,right)
    {}

  std::string   str()      const override;
  std::string get_infix()const override;
  Assign*       clone()    const override;
  long double   evaluate() const override;
  Assign  &  operator= ( const Assign& ) = delete;

 protected:
 Assign(const Assign & other) : Binary_Operator(other){}
 Assign(Assign &&other) : Binary_Operator(other){}
};


class Power final: public Binary_Operator
{
 public:
  ~Power() = default;
 Power(Expression_Tree * left, Expression_Tree * right)
   : Binary_Operator(left,right)
    {}

  std::string   str()      const override;
  Power*        clone()    const override;
  long double   evaluate() const override;

 protected:
 Power(const Power & other) : Binary_Operator(other){}
 Power(Power &&other) : Binary_Operator(other){}
};


class expression_tree_error : public std::logic_error 
{
 public:
  explicit expression_tree_error(const std::string& what_arg) noexcept
    :   logic_error(what_arg) 
  {}

  explicit expression_tree_error(const char* what_arg) noexcept
    :   logic_error(what_arg) 
  {}
};

#endif
