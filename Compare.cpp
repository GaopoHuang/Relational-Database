#include "Compare.hpp"


namespace ECE141 {
      //Equal ------------------------------------------

  template <typename T>
  bool isEqual(const T &arg1, const T &arg2) {
    return arg1==arg2;
  }

  bool isEqual(const std::string &aStr1, const std::string &aStr2) {
    return aStr1==aStr2;
  }

  template <typename T1, typename T2>
  bool isEqual(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)==arg1;
  }

  template <typename T1>
  bool isEqual(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr==aStr;
    return theResult;
  }

  bool isEqual(const std::string &aStr, const bool &arg2) {
    return isEqual(arg2,aStr);
  }

  bool isEqual(const std::string &aStr, const int &arg2) {
    return isEqual(arg2,aStr);
  }

  bool isEqual(const std::string &aStr, const double &arg2) {
    return isEqual(arg2,aStr);
  }

  // Less than or Equal to ------------------------------------------

  template <typename T>
  bool isLTE(const T &arg1, const T &arg2) {
    return arg1<=arg2;
  }

  bool isLTE(const std::string &aStr1, const std::string &aStr2) {
    return aStr1<=aStr2;
  }

  template <typename T1, typename T2>
  bool isLTE(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)>=arg1;
  }

  template <typename T1>
  bool isLTE(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr<=aStr;
    return theResult;
  }


  template <typename T1>
  bool isGTE(const T1 &arg1, const std::string &aStr); //Forward declaration

  bool isLTE(const std::string &aStr, const bool &arg2) {
    return isGTE(arg2,aStr);
  }

  bool isLTE(const std::string &aStr, const int &arg2) {
    return isGTE(arg2,aStr);
  }

  bool isLTE(const std::string &aStr, const double &arg2) {
    return isGTE(arg2,aStr);
  }




  // Less than ------------------------------------------

  template <typename T>
  bool isLessThan(const T &arg1, const T &arg2) {
    return arg1<arg2;
  }

  bool isLessThan(const std::string &aStr1, const std::string &aStr2) {
    return aStr1<aStr2;
  }

  template <typename T1, typename T2>
  bool isLessThan(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)>arg1;
  }

  template <typename T1>
  bool isLessThan(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr<aStr;
    return theResult;
  }

  template <typename T1>
  bool isGreaterThan(const T1 &arg1, const std::string &aStr); //Forward declaration

  bool isLessThan(const std::string &aStr, const bool &arg2) {
    return isGreaterThan(arg2,aStr);
  }

  bool isLessThan(const std::string &aStr, const int &arg2) {
    return isGreaterThan(arg2,aStr);
  }

  bool isLessThan(const std::string &aStr, const double &arg2) {
    return isGreaterThan(arg2,aStr);
  }



  // Greater than or Equal to ------------------------------------------
  template <typename T>
  bool isGTE(const T &arg1, const T &arg2) {
    return arg1>=arg2;
  }

  bool isGTE(const std::string &aStr1, const std::string &aStr2) {
    return aStr1>=aStr2;
  }

  template <typename T1, typename T2>
  bool isGTE(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)<=arg1;
  }

  template <typename T1>
  bool isGTE(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr>=aStr;
    return theResult;
  }

  bool isGTE(const std::string &aStr, const bool &arg2) {
    return isLTE(arg2,aStr);
  }

  bool isGTE(const std::string &aStr, const int &arg2) {
    return isLTE(arg2,aStr);
  }

  bool isGTE(const std::string &aStr, const double &arg2) {
    return isLTE(arg2,aStr);
  }



  // Greater than ------------------------------------------

  template <typename T>
  bool isGreaterThan(const T &arg1, const T &arg2) {
    return arg1>arg2;
  }

  bool isGreaterThan(const std::string &aStr1, const std::string &aStr2) {
    return aStr1>aStr2;
  }

  template <typename T1, typename T2>
  bool isGreaterThan(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)<arg1;
  }

  template <typename T1>
  bool isGreaterThan(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr>aStr;
    return theResult;
  }

  bool isGreaterThan(const std::string &aStr, const bool &arg2) {
    return isLessThan(arg2,aStr);
  }

  bool isGreaterThan(const std::string &aStr, const int &arg2) {
    return isLessThan(arg2,aStr);
  }

  bool isGreaterThan(const std::string &aStr, const double &arg2) {
    return isLessThan(arg2,aStr);
  }



  //--------- feel free to add more operators... ------

bool equals(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isEqual(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }


  bool lessThanEquals(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isLTE(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool lessThan(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isLessThan(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool greaterThan(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isGreaterThan(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

    
  bool greaterThanEquals(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isGTE(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }



}