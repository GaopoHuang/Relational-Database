//
//  Filters.cpp
//  Datatabase5
//
//  Created by rick gessner on 3/5/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include <string>
#include <limits>
#include <algorithm>
#include "keywords.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Compare.hpp"

namespace ECE141 {
  
  using Comparitor = bool (*)(Value &aLHS, Value &aRHS);


  Expression& Expression::setLogicalOp(Operators anOp) {
    static std::map<Operators, Logical> theLogicalOps {
      {Operators::and_op, Logical::and_op},
      {Operators::or_op, Logical::or_op},
      {Operators::not_op, Logical::not_op}
    };
    logic = theLogicalOps.at(anOp);
    return *this;    
  }




  static std::map<Operators, Comparitor> comparitors {
    {Operators::equal_op, equals},
    {Operators::lte_op, lessThanEquals},
    {Operators::lt_op, lessThan},
    {Operators::gt_op, greaterThan},
    {Operators::gte_op, greaterThanEquals}
  };

  bool Expression::operator()(KeyValues &aList) {
    Value theLHS{lhs.value};
    Value theRHS{rhs.value};

    if(TokenType::identifier==lhs.ttype) {
      theLHS=aList[lhs.name]; //get row value
    }

    if(TokenType::identifier==rhs.ttype) {
      theRHS=aList[rhs.name]; //get row value
    }

    return comparitors.count(op)
      ? comparitors[op](theLHS, theRHS) : false;
  }
  
  //--------------------------------------------------------------
  
  Filters::Filters()  {}
  
  Filters::Filters(const Filters &aCopy)  {
    // for(auto& theExpr : aCopy.expressions) {
    //   expressions.push_back(theExpr);
    // }
  }
  
  Filters::~Filters() {
    //no need to delete expressions, they're unique_ptrs!
  }

  Filters& Filters::add(Expression *anExpression) {
    expressions.push_back(std::shared_ptr<Expression>(anExpression));
    return *this;
  }
    
  //compare expressions to row; return true if matches
  bool Filters::matches(KeyValues &aList) const {
    
    //STUDENT: You'll need to add code here to deal with
    //         logical combinations (AND, OR, NOT):
    //         like:  WHERE zipcode=92127 AND age>20

    //Using map and closure to do a switch statement
    using logicEval = std::function<bool(bool,bool)>;
    std::map<Logical, logicEval> switched {
      {Logical::and_op, [](bool lhs, bool rhs) {return lhs && rhs;} },
      {Logical::not_op, [](bool lhs, bool rhs) {return !rhs;}        },
      {Logical::or_op, [](bool lhs, bool rhs) {return lhs || rhs;}  },
    };
    //First expression will be evaluated with (AND true), which is equivalent to itself
    bool theResult{true};
    Logical theLogical = Logical::and_op;
    for(auto &theExpr : expressions) {
      bool theCurrResult = (*theExpr)(aList);
      if(theLogical == Logical::no_op) {
        return false;
      }
      theResult = switched[theLogical](theResult, theCurrResult);
      theLogical = theExpr->logic;
    }
    return theResult;
  }
 

  //where operand is field, number, string...
  StatusResult parseOperand(Tokenizer &aTokenizer,
                            Entity &anEntity, Operand &anOperand) {
    StatusResult theResult{noError};
    Token &theToken = aTokenizer.current();
    if(TokenType::identifier==theToken.type) {
      if(auto *theAttr=anEntity.getAttribute(theToken.data)) {
        anOperand.ttype=theToken.type;
        anOperand.name=theToken.data; //hang on to name...
        anOperand.entityId=Entity::hashString(theToken.data);
        anOperand.dtype=theAttr->getType();
        delete theAttr;
      }
      else {
        anOperand.ttype=TokenType::string;
        anOperand.dtype=DataTypes::varchar_type;
        anOperand.value=theToken.data;
      }
    }
    else if(TokenType::number==theToken.type) {
      anOperand.ttype=TokenType::number;
      anOperand.dtype=DataTypes::int_type;
      if (theToken.data.find('.')!=std::string::npos) {
        anOperand.dtype=DataTypes::float_type;
        anOperand.value=std::stof(theToken.data);
      }
      else anOperand.value=std::stoi(theToken.data);
    }
    else theResult.error=syntaxError;
    if(theResult) aTokenizer.next();
    return theResult;
  }
  
  bool typeMatches(DataTypes &aLHS, DataTypes &aRHS) {
    static DataTypes theNumericals[3] = {
      DataTypes::bool_type, DataTypes::float_type, DataTypes::int_type
    };    
    for(auto numTypeL: theNumericals) {
      if(aLHS == numTypeL) {
        for(auto numTypeR: theNumericals) {
          if(aRHS == numTypeR) {return true;}
        }
        return false;
      }
    }

    return aLHS == aRHS;
  }

  //STUDENT: Add validation here...
  bool validateOperands(Operand &aLHS, Operand &aRHS, Entity &anEntity) {
    bool theResult;
    if(TokenType::identifier==aLHS.ttype) { //most common case...
      //STUDENT: Add code for validation as necessary
      if(anEntity.attrExist(aLHS.name) && typeMatches(aLHS.dtype, aRHS.dtype)) {
        theResult = true;
      }
    }
    else if(TokenType::identifier==aRHS.ttype) {
      //STUDENT: Add code for validation as necessary
      if(anEntity.attrExist(aLHS.name) && typeMatches(aLHS.dtype, aRHS.dtype)) {
        theResult = true;
      }
    }
    return theResult;
  }

  //STUDENT: This starting point code may need adaptation...
  StatusResult Filters::parse(Tokenizer &aTokenizer,Entity &anEntity) {
    StatusResult  theResult{noError};

    while(theResult && (2<aTokenizer.remaining())) {
      Operand theLHS,theRHS;
      Token &theToken=aTokenizer.current();
      if(theToken.type!=TokenType::identifier) return theResult;
      if((theResult=parseOperand(aTokenizer,anEntity,theLHS))) {
        Token &theToken=aTokenizer.current();
        if(theToken.type==TokenType::operators) {
          Operators theOp=Helpers::toOperator(theToken.data);
          aTokenizer.next();
          if((theResult=parseOperand(aTokenizer,anEntity,theRHS))) {
            if(validateOperands(theLHS, theRHS, anEntity)) {
              Expression* theExpression = new Expression(theLHS, theOp, theRHS);
              //Check logical operators
              theToken = aTokenizer.current();
              if(theToken.type==TokenType::identifier && Helpers::isLogicalOperator(theToken.keyword)) {
                theExpression->setLogicalOp(Helpers::toOperator(theToken.data));
              }
              add(theExpression);
              if(aTokenizer.skipIf(semicolon)) {
                break;
              }
            }
            else {theResult.error=syntaxError;}
          }
        }
      }
      else theResult.error=syntaxError;
    }
    return theResult;
  }

}

