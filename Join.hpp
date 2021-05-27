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


namespace ECE141 {

  class Join  {
  public:
    Join(const std::string &aTable, Keywords aType)
      : table(aTable), joinType(aType), expr() {}
            
    std::string table;
    Keywords    joinType;
    Expression  expr;
  };

  using JoinList = std::vector<Join>;

}

#endif /* Join_h */
