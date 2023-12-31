//
//  BasicTypes.hpp
//  RGAssignement1
//
//  Created by rick gessner on 3/9/21.
//

#ifndef BasicTypes_h
#define BasicTypes_h

#include <map>
#include <vector>
#include <string>
#include <variant>
#include <iostream>

namespace ECE141 {

  enum class DataTypes : char {
     no_type='N',  bool_type='B', datetime_type='D',
      float_type='F', int_type='I', varchar_type='V',
  };

  using IndexKey = std::variant<uint32_t, std::string>;

  using NamedIndex = std::map<std::string, uint32_t>;
  using StringList = std::vector<std::string>;
  using StringMap = std::map<std::string, std::string>;
  using StringOpt = std::optional<std::string>;
  using IntOpt    = std::optional<uint32_t>;

  using Value = std::variant<bool, int, double, std::string>;
  using KeyValue = std::pair<const std::string, Value>;
  using KeyValues = std::map<const std::string, Value>;
  using Values = std::vector<Value>;

  //Overloaded shift operator to use stream on variant
  std::ostream& operator<<(std::ostream &out, const Value &aValue);
  std::istream& operator>>(std::istream &in, Value &aValue);

}
#endif /* BasicTypes_h */
