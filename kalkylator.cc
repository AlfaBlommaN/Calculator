#include "Calculator.h"
#include <iostream>
using namespace std;

int main()
{
  Calculator calc;

  try
    {
      calc.run();
    }
  catch (const exception& e) // ska inte kunna hända här egentligen
    {
      cout << e.what() << '\n';
    }
  catch (...) // "catch all"
    {
      cout << "Ett okänt fel har inträffat.\n";
    }

  return 0;
}
