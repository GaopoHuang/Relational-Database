#include <map>
#include <vector>
#include <string>
#include <variant>
#include <iostream>
#include "BasicTypes.hpp"

namespace ECE141 {

  std::ostream& operator<<(std::ostream &out, const Value &aValue) {
    static char theTypes[] = {'b', 'i', 'd', 's'};
    char theType = theTypes[aValue.index()];
    std::visit([&theType, &out](auto const &aValue) {
      out << theType << ' ' << aValue;
    }, aValue);
    return out;
  }

  std::istream& operator>>(std::istream &in, Value &aValue) {
    char theType;
    in >> theType;
    std::string theStr;
    switch( theType ){
      case 'b':
        bool theBool;
        in >> theBool;
        aValue = theBool;
        return in;
      case 'i':
        int theInt;
        in >> theInt;
        aValue = theInt;
        return in;
      case 'd':
        double theDouble;
        in >> theDouble;
        aValue = theDouble;
        return in;
      case 's':
        in >> theStr;
        aValue = theStr;
        return in;
      default:
        return in;
    }
  }

}