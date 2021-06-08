//
//  Join.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/05/21.
//

#ifndef Join_h
#define Join_h

#include <string>
#include <vector>
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "keywords.hpp"
#include "Filters.hpp"
#include "DataProvider.hpp"


namespace ECE141 {


  class Join  {
  public:
    Join(const std::string &aTable, Keywords aType, const std::string &aLHS, const std::string &aRHS)
      : table(aTable), joinType(aType), lhs(aLHS), rhs(aRHS) {}
            
    std::string table;
    Keywords    joinType;
    TableField  lhs;
    TableField  rhs;
  };

  using JoinList = std::vector<Join>;

}

#endif /* Join_h */
