//
//  compare.hpp
//  Created by rick gessner on 4/30/21.
//

#ifndef compare_h
#define compare_h

#include <sstream>
#include "BasicTypes.hpp"

namespace ECE141{


  //Equal ------------------------------------------

  template <typename T>
  bool isEqual(const T &arg1, const T &arg2);

  bool isEqual(const std::string &aStr1, const std::string &aStr2);

  template <typename T1, typename T2>
  bool isEqual(const T1 &arg1, const T2 &arg2);

  template <typename T1>
  bool isEqual(const T1 &arg1, const std::string &aStr);

  bool isEqual(const std::string &aStr, const bool &arg2);

  bool isEqual(const std::string &aStr, const int &arg2);

  bool isEqual(const std::string &aStr, const double &arg2);

  // Less than or Equal to ------------------------------------------

  template <typename T>
  bool isLTE(const T &arg1, const T &arg2);

  bool isLTE(const std::string &aStr1, const std::string &aStr2);

  template <typename T1, typename T2>
  bool isLTE(const T1 &arg1, const T2 &arg2);

  template <typename T1>
  bool isLTE(const T1 &arg1, const std::string &aStr);


  // template <typename T1>
  // bool isGTE(const T1 &arg1, const std::string &aStr); //Forward declaration

  bool isLTE(const std::string &aStr, const bool &arg2);

  bool isLTE(const std::string &aStr, const int &arg2);

  bool isLTE(const std::string &aStr, const double &arg2);




  // Less than ------------------------------------------

  template <typename T>
  bool isLessThan(const T &arg1, const T &arg2);

  bool isLessThan(const std::string &aStr1, const std::string &aStr2);

  template <typename T1, typename T2>
  bool isLessThan(const T1 &arg1, const T2 &arg2);

  template <typename T1>
  bool isLessThan(const T1 &arg1, const std::string &aStr);

  // template <typename T1>
  // bool isGreaterThan(const T1 &arg1, const std::string &aStr); //Forward declaration

  bool isLessThan(const std::string &aStr, const bool &arg2);

  bool isLessThan(const std::string &aStr, const int &arg2);

  bool isLessThan(const std::string &aStr, const double &arg2);



  // Greater than or Equal to ------------------------------------------
  template <typename T>
  bool isGTE(const T &arg1, const T &arg2);

  bool isGTE(const std::string &aStr1, const std::string &aStr2);

  template <typename T1, typename T2>
  bool isGTE(const T1 &arg1, const T2 &arg2);

  template <typename T1>
  bool isGTE(const T1 &arg1, const std::string &aStr);

  bool isGTE(const std::string &aStr, const bool &arg2);

  bool isGTE(const std::string &aStr, const int &arg2);

  bool isGTE(const std::string &aStr, const double &arg2);



  // Greater than ------------------------------------------

  template <typename T>
  bool isGreaterThan(const T &arg1, const T &arg2);

  bool isGreaterThan(const std::string &aStr1, const std::string &aStr2);

  template <typename T1, typename T2>
  bool isGreaterThan(const T1 &arg1, const T2 &arg2);

  template <typename T1>
  bool isGreaterThan(const T1 &arg1, const std::string &aStr);

  bool isGreaterThan(const std::string &aStr, const bool &arg2);

  bool isGreaterThan(const std::string &aStr, const int &arg2);

  bool isGreaterThan(const std::string &aStr, const double &arg2);



  //--------- feel free to add more operators... ------

  bool equals(Value &aLHS, Value &aRHS);

  bool lessThanEquals(Value &aLHS, Value &aRHS);

  bool lessThan(Value &aLHS, Value &aRHS);

  bool greaterThan(Value &aLHS, Value &aRHS);

  bool greaterThanEquals(Value &aLHS, Value &aRHS);

}

#endif /* compare_h */
