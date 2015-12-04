/*
 * Expression.cc
 */
#include "Expression.h"
#include "Expression_Tree.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

class expression_error : public std::logic_error 
{
public:
  explicit expression_error(const std::string& what_arg) 
    :   logic_error(what_arg) 
  {}

  explicit expression_error(const char* what_arg) noexcept
    :   logic_error(what_arg) 
  {}
};
/*
 *kopieringskonstruktor 
 */
Expression::Expression(const Expression & other)
{
  if (!other.empty()) {
    root_ = other.root_->clone();
  }
}
/*kopieringstilldelning */
Expression & Expression::operator=(const Expression & right) & 
{
  if(this != &right) 
  {
  	delete root_;
    root_ = right.root_->clone();         
  }
  return *this;
}
/*flyttkonstruktor */
Expression::Expression(Expression && other) noexcept
{
  swap(other);
}

/*flytttilldelning */
Expression & Expression::operator=( Expression && right) & noexcept
{
  clear();
  swap(right);
  return *this;
}

Expression::~Expression()
{
  delete root_;
}

void Expression:: clear() & noexcept
{
  delete root_;
  root_ = nullptr;
}

/*
 * evaluate()
 */
long double Expression::evaluate() const
{
   
  if (empty()) {
    throw expression_error("Kan inte evaluera ett tomt uttryck");
  }

  return root_->evaluate();
}

/*
 * get_postfix()
 */
string Expression::get_postfix() const
{
  if (empty()) {
    throw expression_error("Kan inte hämta postfix för ett tomt uttryck");
  }

  return root_->get_postfix();
}

string Expression::get_infix() const
{
  if (empty()) {
    throw expression_error("Kan inte hämta infix för ett tomt uttryck");
  }

  return root_->get_infix();
}

/*
 * empty()
 */
bool Expression::empty() const
{
  return root_ == nullptr;
}

/*
 * print_tree()
 */
void Expression::print_tree(std::ostream& os) const 
{
  if (empty())
    throw expression_error("Trädet är tomt");
  else
    root_->print(os);
}

/*
 * swap(other)
 */
void Expression::swap(Expression& other) noexcept
{
  std::swap(root_, other.root_);
}

/*
 * swap(x, y)
 */
void swap(Expression& left, Expression& right) noexcept
{
  left.swap(right);
}

// make_expression() definieras efter namnrymden nedan.
Expression make_expression(const string& infix);

// Namrymden nedan innehaller intern kod for infix-till-postfix-omvandling
// och generering av uttryckstrad. En anonym namnrymd begransar anvandningen
// av medlemmarna till denna fil.
namespace
{
  using std::vector;
  using std::map;
  using std::make_pair;
  using std::string;

  // Teckenuppsattningar for operander. Anvands av make_expression_tree().
  const string letters{"abcdefghijklmnopqrstuvwxyz"};
  const string digits{"0123456789"};
  const string integer_chars{digits};
  const string real_chars{digits + '.'};
  const string variable_chars{letters};
  const string operand_chars{letters + digits + '.'};

  // Tillatna operatorer. Anvands av make_postfix() och make_expression_tree().
  // Prioritetstabeller, en for inkommandeprioritet och en for stackprioritet. 
  // Hogre varde inom input_prio respektive stack_prio anger inbordes prioritetsordning.
  // Hogre varde i input_prio jamfört med motsvarande position i stack_prio innebar
  // högerassociativitet, det motsatta vansterassociativitet. Anvands av make_postfix(). 
  using priority_table = map<string, int>;

  const vector<string> operators{ "^", "*", "/", "+", "-", "=" };
  const priority_table input_priority{ {"^", 8}, {"*", 5}, {"/", 5}, {"+", 3}, {"-", 3}, {"=", 2} };
  const priority_table stack_priority{ {"^", 7}, {"*", 6}, {"/", 6}, {"+", 4}, {"-", 4}, {"=", 1} };

  // Hjalpfunktioner för att kategorisera lexikala element.
  bool is_operator(char token)
  {
    return find(begin(operators), end(operators), string{token}) != end(operators);
  }

  bool is_operator(const string& token)
  {
    return find(begin(operators), end(operators), token) != end(operators);
  }

  bool is_operand(const string& token)
  {
    return token.find_first_not_of(operand_chars) == string::npos;
  }

  bool is_integer(const string& token)
  {
    return token.find_first_not_of(integer_chars) == string::npos;
  }

  bool is_real(const string& token)
  {
    return token.find_first_not_of(real_chars) == string::npos;
  }

  bool is_identifier(const string& token)
  {
    return token.find_first_not_of(letters) == string::npos;
  }

  // format_infix tar en strang med ett infixuttryck och formaterar med ett
  // mellanrum mellan varje symbol; underlattar vid bearbetningen i make_postfix.
  std::string format_infix(const std::string& infix)
  {
    auto bos = begin(infix);
    auto eos = end(infix);
    string formated;

    for (auto it = bos; it != eos; ++it)
      {
	if (is_operator(*it) || *it == '(' || *it == ')')
	  {
            // Se till att det ar ett mellanrum före en operator eller parentes
            if (it != bos && *(it - 1) != ' ' && *(formated.end() - 1) != ' ')
	      formated.append(1, ' ');
            formated.append(1, *it);
            // Se till att det ar ett mellanrum efter en operator eller parentes
            if ((it + 1) != eos && *(it + 1) != ' ')
	      formated.append(1, ' ');
	  }
	else
	  {
            if (*it != ' ')
	      formated.append(1, *it);
            else if (it != bos && *(it - 1) != ' ')
	      formated.append(1, *it);
	  }
      }
    return formated;
  }

  // make_postfix() tar en infixstrang och returnerar motsvarande postfixstrang.

  std::string make_postfix(const std::string& infix)
  {
    using std::stack;
    using std::string;
    using std::istringstream;
    using std::find;

    stack<string> operator_stack;
    string        token;
    string        previous_token{ "" };
    bool          last_was_operand{ false };
    bool          assignment{false};
    int           paren_count{ 0 };

    istringstream is{ format_infix(infix) };
    string        postfix;

    while (is >> token)
      {
	if (is_operator(token))
	  {
            if (!last_was_operand || postfix.empty() || previous_token == "(")
	      {
		throw expression_error("operator dar operand forvantades\n");
	      }

            if (token == "=")
	      {
		if (assignment)
		  {
		    throw expression_error("multipel tilldelning\n");
		  }
		else
                  assignment = true;
	      }

            while (!operator_stack.empty() && operator_stack.top() != "(" &&
                   input_priority.find(token)->second <=
                   stack_priority.find(operator_stack.top())->second)
	      {
		postfix += operator_stack.top() + ' ';
		operator_stack.pop();
	      }
            operator_stack.push(token);
            last_was_operand = false;
	  }
	else if (token == "(")
	  {
            operator_stack.push(token);
            ++paren_count;
	  }
	else if (token == ")")
	  {
            if (paren_count == 0)
	      {
		throw expression_error("vansterparentes saknas\n");
	      }

            if (previous_token == "(" && !postfix.empty())
	      {
		throw expression_error("tom parentes\n");
	      }

            while (!operator_stack.empty() && operator_stack.top() != "(")
	      {
		postfix += operator_stack.top() + ' ';
		operator_stack.pop();
	      }

            if (operator_stack.empty())
	      {
		throw expression_error("hogerparentes saknar matchande vansterparentes\n");
	      }
            // Det finns en vansterparentes pa stacken
            operator_stack.pop();
            --paren_count;
	  }
	else if (is_operand(token))
	  {
            if (last_was_operand || previous_token == ")")
	      {
		throw expression_error("operand dar operator forvantades\n");
	      }

            postfix += token + ' ';
            last_was_operand = true;
	  }
	else
	  {
            throw expression_error ("otillaten symbol\n");
	  }

	previous_token = token;
      }

    if (postfix == "")
      {
	throw expression_error("tomt infixuttryck!\n");
      }

    if ((!last_was_operand) && (!postfix.empty()))
      {
	throw expression_error("operator avslutar\n");
      }

    if (paren_count > 0)
      {
	throw expression_error ("hogerparentes saknas\n");
      }

    while (!operator_stack.empty())
      {
	postfix += operator_stack.top() + ' ';
	operator_stack.pop();
      }

    if (!postfix.empty())
      {
	postfix.erase(postfix.size() - 1);
      }

    return postfix;
  }

  // make_expression_tree() tar en postfixstrang och returnerar ett motsvarande 
  // lankat trad av Expression_Tree-noder. Tank pa minneslackage...

  Expression_Tree* make_expression_tree(const std::string& postfix)
  {
    using std::stack;
    using std::string;
    using std::istringstream;

    stack<Expression_Tree*> tree_stack;
    string                  token;
    istringstream           ps{ postfix };

    try {
      while (ps >> token)
        {
	  if (is_operator(token))
	    {
              if (tree_stack.empty()) 
		{
		  throw expression_error("felaktig postfix\n");
		}
              Expression_Tree* rhs = tree_stack.top();
              tree_stack.pop();

              if (tree_stack.empty()) 
		{
		  throw expression_error("felaktig postfix\n");

		}
              Expression_Tree* lhs = tree_stack.top();
              tree_stack.pop();

              if (token == "^")
		{
		  tree_stack.push(new Power{lhs, rhs});
		}
              else if (token == "*")
		{
		  tree_stack.push(new Times{lhs, rhs});
		}
              else if (token == "/")
		{
		  tree_stack.push(new Divide{lhs, rhs});
		}
              else if (token == "+")
		{
		  tree_stack.push(new Plus{lhs, rhs});
		}
              else if (token == "-")
		{
		  tree_stack.push(new Minus{lhs, rhs});
		}
              else if (token == "=")
		{
		  tree_stack.push(new Assign{lhs, rhs});
		}
	    }
	  else if (is_integer(token))
	    {
              tree_stack.push(new Integer{std::stoi(token)});
	    }
	  else if (is_real(token))
	    {
              tree_stack.push(new Real{std::stold(token)});
	    }
	  else if (is_identifier(token))
	    {
              tree_stack.push(new Variable{token});
	    }
	  else
	    {
              throw expression_error("felaktig postfix\n");
	    }
        }
    } catch (...) {
      while (!tree_stack.empty())
	{
	  delete tree_stack.top();
	  tree_stack.pop();
	}
      throw expression_tree_error("Error ..");
    }
    // Det ska bara finnas ett trad pa stacken om postfixen ar korrekt.

    if (tree_stack.empty())
      {
	throw expression_error ("ingen postfix given\n");
      }
         
    if (tree_stack.size() > 1)
      {
	while (!tree_stack.empty())
	  {
            delete tree_stack.top();
            tree_stack.pop();
	  }
	throw expression_error( "felaktig postfix\n");
      }
    return tree_stack.top();
  }
  // namespace
}
/*
 * make_expression()
 */
Expression make_expression(const string& infix)
{
  return Expression(make_expression_tree(make_postfix(infix)));
}
