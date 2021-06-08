#include "SQLSelectStatement.hpp"
#include "Tokenizer.hpp"
#include "SQLProcessor.hpp"
#include "Entity.hpp"
#include "Row.hpp"
#include "TokenSequence.hpp"
#include "Helpers.hpp"
#include <map>
#include <functional>


namespace ECE141 {
  //---------------------------------------------------------
  SQLSelectStatement::SQLSelectStatement(SQLProcessor* aProc, Keywords aStatementType) :
    Statement(aStatementType), proc(aProc){}

  SQLSelectStatement::SQLSelectStatement(const SQLSelectStatement &aCopy) :
    Statement(aCopy), proc(aCopy.proc), dbQuery(aCopy.dbQuery){}
  
  SQLSelectStatement::~SQLSelectStatement() {}

  StatusResult SQLSelectStatement::hasProperLen(Tokenizer &aTokenizer, size_t aMin, size_t aMax) {
    if(aTokenizer.remaining() < aMin) {
      return StatusResult{syntaxError};
    }
    else if(aMax != -1 && aTokenizer.remaining() > aMax) {
      return StatusResult{syntaxError};
    }
    return StatusResult{};
  }

//-----------------------------------------------------------------------------------

  StatusResult parseGroupBy(Tokenizer &aTokenizer, Query& aQuery) {
    return StatusResult{};
  }

  StatusResult parseOrderBy(Tokenizer &aTokenizer, Query& aQuery) {
    TokenSequence theSeq{aTokenizer};
    StatusResult theResult{syntaxError};
    if(theSeq.has(Keywords::order_kw).then(Keywords::by_kw)) {
      aTokenizer.next(2);
      std::string theField;
      if(theResult = theSeq.getIdentifier(theField)) {
        aQuery.setOrderBy(theField);
      }    
    }
    return theResult;
  }

  StatusResult parseLimitBy(Tokenizer &aTokenizer, Query& aQuery) {
    StatusResult theResult{syntaxError};
    if(aTokenizer.skipIf(Keywords::limit_kw)) {
      if(aTokenizer.current().type == TokenType::number) {
        aQuery.setLimit(std::stoi(aTokenizer.current().data));
        aTokenizer.next();
        theResult.error = noError;
      }
    }
    return theResult;
  }

  StatusResult parseFilter(Tokenizer &aTokenizer, Query& aQuery) {
    StatusResult theResult{syntaxError};
    if(aTokenizer.skipIf(Keywords::where_kw)) {
      theResult = aQuery.getFilters().parse(aTokenizer, *aQuery.getFrom());
    }
    return theResult;
  }

  using clauseCallback = std::function<StatusResult(Tokenizer&, Query&)>;
  StatusResult SQLSelectStatement::parseClause(Tokenizer &aTokenizer, Query& aQuery) {
    static std::map<Keywords, clauseCallback> theClauseCb {
      {Keywords::order_kw, parseOrderBy},
      {Keywords::group_kw, parseGroupBy},
      {Keywords::limit_kw, parseLimitBy},
      {Keywords::where_kw, parseFilter}
    };
    StatusResult theResult;
    while(theResult && aTokenizer.remaining()>2) {
      Keywords theKw = aTokenizer.current().keyword;
      if(theClauseCb.count(theKw)) {
        theResult = theClauseCb[theKw](aTokenizer, aQuery);
      }
      else {
        theResult.error = unexpectedKeyword;
      }
    }
    return theResult;
  }

  StatusResult SQLSelectStatement::parseTableName(Tokenizer& aTokenizer, std::string& aTable) {
    StatusResult theResult{identifierExpected};
    Token &theToken = aTokenizer.current();
    std::string theEntityName;
    if(TokenType::identifier==theToken.type) {
      theEntityName=theToken.data;
      if(Entity* theEntity = proc->getEntity(theEntityName)) {
        aTokenizer.next();
        delete theEntity;
        aTable = theEntityName;
        theResult.error = noError;
      }
      else {theResult.error = unknownTable;}
    }
    return theResult;
  }

  StatusResult SQLSelectStatement::parseTableField(Tokenizer& aTokenizer, TableField& aField) {
    StatusResult theResult{syntaxError};
    TokenSequence theSeq{aTokenizer};
    std::string theTable, theAttr;
    if(theSeq.getIdentifier(theTable)) {
      if(theResult = theSeq.getOperator(Operators::dot_op)) {
        if(theSeq.getIdentifier(theAttr)) {
          if(Entity* theEntity = proc->getEntity(theTable)) {
            if(theEntity->attrExist(theAttr)) {
              aField.table = theTable;
              aField.attribute = theAttr;
              theResult.error = noError;
              delete theEntity;
            }
          }
        }
      }
    }
    return theResult;
  }

  //jointype JOIN tablename ON table1.field=table2.field
  StatusResult SQLSelectStatement::parseJoin(Tokenizer &aTokenizer) {
    Token &theToken = aTokenizer.current();
    StatusResult theResult{joinTypeExpected}; //add joinTypeExpected to your errors file if missing...

    Keywords theJoinType{Keywords::join_kw}; //could just be a plain join
    if(in_array<Keywords>(gJoinTypes, theToken.keyword)) {
      theJoinType=theToken.keyword;
      aTokenizer.next(1); //yank the 'join-type' token (e.g. left, right)
      if(aTokenizer.skipIf(Keywords::join_kw)) {
        std::string theTable;
        if((theResult=parseTableName(aTokenizer, theTable))) {
          Join theJoin(theTable, theJoinType, std::string(""),std::string(""));
          theResult.value=keywordExpected; //on...
          if(aTokenizer.skipIf(Keywords::on_kw)) { //LHS field = RHS field
            TableField LHS("");
            if((theResult=parseTableField(aTokenizer, theJoin.lhs))) {
              if(aTokenizer.skipIf(Operators::equal_op)) {
                if((theResult=parseTableField(aTokenizer, theJoin.rhs))) {
                  joins.push_back(theJoin);
                }
              }
            }
          }
        }
      }
    }
    return theResult;
  }


  StatusResult SQLSelectStatement::validateQuery(Query& aQuery) {
    StatusResult theResult{};
    Entity* theJoinEntity = nullptr;
    //***Only handle the first join table***
    if(joins.size()>0) {
      theJoinEntity = proc->getEntity(joins[0].table);
    }

    //Validate the field list
    if(!aQuery.selectAll()) {
      for(auto& attrName: aQuery.getFields()) {
        if(!aQuery.getFrom()->attrExist(attrName)) {
          if(! (theJoinEntity && theJoinEntity->attrExist(attrName))) {
            theResult.error = unknownAttribute;
          }
        }
      }
    }

    //validate order field
    if(StringOpt theField = aQuery.getOrder()) {
      if(!aQuery.getFrom()->attrExist(theField.value())) {
        if(theJoinEntity && theJoinEntity->attrExist(theField.value())) {
          theResult.error = unknownAttribute;
        }
      }
    }

    //Filter fields were validated when parsing
    return theResult;
  }


  StatusResult SQLSelectStatement::parse(Tokenizer &aTokenizer) {
    StatusResult theResult{syntaxError};
    if(!hasProperLen(aTokenizer,4)) {return theResult;}
    TokenSequence theSeq(aTokenizer);
    //Parse fields 
    if(theSeq.has(Keywords::select_kw).thenOp('*').then(Keywords::from_kw)) {
      aTokenizer.next(3);
      theResult.error = noError;
    }
    else if(aTokenizer.skipIf(Keywords::select_kw)) {
      StringList theFieldList;
      dbQuery.setSelectAll(false);
      while(aTokenizer.remaining()>2) {
        Token& theToken = aTokenizer.current();
        if(theToken.type == TokenType::identifier) {
          theFieldList.push_back(theToken.data);
          aTokenizer.next();
          if(aTokenizer.skipIf(',')) {
            continue;
          }
          else if(aTokenizer.skipIf(Keywords::from_kw)) {
            dbQuery.setSelect(theFieldList);
            theResult.error = noError;
            break;
          }
        }
      }
    }
    //Get entity name and parse any following clauses
    if(theResult) {
      std::string theEntityName;
      if(theSeq.getIdentifier(theEntityName)) {
        if(Entity* theEntity = proc->getEntity(theEntityName)) {
          dbQuery.setFrom(theEntity);
          theResult = parseJoin(aTokenizer);
          if(theResult = parseClause(aTokenizer, dbQuery)) {
            theResult = validateQuery(dbQuery);
          }
          
        }
        else {theResult.error=unknownTable;}
      }
    }
    return theResult;
  }


  StatusResult SQLSelectStatement::dispatch() {
    if(joins.size() > 0) {
      return proc->showQuerywithJoin(dbQuery, joins);
    }
    return proc->showQuery(dbQuery);
  }

  bool SQLSelectStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::select_kw);
  }





  //---------------------------------------------------------
  UpdateStatement::UpdateStatement(SQLProcessor* aProc, Keywords aStatementType) :
    SQLSelectStatement(aProc,aStatementType){}

  UpdateStatement::UpdateStatement(const UpdateStatement &aCopy) :
    SQLSelectStatement(aCopy), updatequery(aCopy.updatequery) {}
  
  UpdateStatement::~UpdateStatement() {}

  StatusResult parseKVs(Tokenizer &aTokenizer, UpdateQuery& anUpdateQuery) {
    StatusResult theResult;
    while (theResult && (2<aTokenizer.remaining())) {
      Token &theToken = aTokenizer.current();
      if(theToken.type != TokenType::identifier) {return theResult;}
      std::string theId{theToken.data};  
      Value theValue;
      aTokenizer.next();
      if(aTokenizer.skipIf('=')) {
        //Parse the value and store it with matching type
        theToken = aTokenizer.current();
        if(theToken.type == TokenType::number) {
          if(theToken.data.find('.')!=std::string::npos) {
            theValue = std::stof(theToken.data);
          }
          else {theValue = std::stoi(theToken.data);}
        }
        else if(theToken.type == TokenType::string) {theValue = theToken.data; }
        else {theResult.error = unexpectedValue;}
      }
      else {theResult.error = operatorExpected;}
      //Set the update field if parsing succeeds for one pair
      if(theResult) {
        aTokenizer.next();
        anUpdateQuery.setUpdate(theId, theValue);
      }
    }
    return theResult;
  }

  StatusResult UpdateStatement::validateQuery(UpdateQuery& anUpdateQuery) {
    StatusResult theResult{};
    //Validate the update field 
    for(auto& theField: anUpdateQuery.getUpdateFields()) {
      if(auto *theAttr = anUpdateQuery.getFrom()->getAttribute(theField.first)) {
        if(!Helpers::typeMatches(theAttr->getType(), theField.second)) {
          theResult.error = syntaxError;
        }
      } 
    }

    return theResult;
  }

  StatusResult UpdateStatement::parse(Tokenizer &aTokenizer) {
    StatusResult theResult{syntaxError};
    if(!hasProperLen(aTokenizer,6)) {return theResult;}
    TokenSequence theSeq(aTokenizer);
    //Parse fields 
    if(theSeq.has(Keywords::update_kw).thenId(name).then(Keywords::set_kw)) {
      aTokenizer.next(3);
      theResult.error = noError;
      Entity* theEntity = proc->getEntity(name);
      if(theEntity) {
        updatequery.setFrom(theEntity);
        if(theResult = parseKVs(aTokenizer, updatequery)) {
          if(theResult = parseClause(aTokenizer, updatequery)) {
            theResult = validateQuery(updatequery);
          }
        }
      }
      else {theResult.error=unknownTable;}
    }
    return theResult;
  }

  StatusResult UpdateStatement::dispatch() {
    return proc->updateRows(updatequery);
  }

  bool UpdateStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::update_kw);
  }



  //---------------------------------------------------------
  DeleteStatement::DeleteStatement(SQLProcessor* aProc, Keywords aStatementType) :
    SQLSelectStatement(aProc,aStatementType){}

  DeleteStatement::DeleteStatement(const DeleteStatement &aCopy) :
    SQLSelectStatement(aCopy), deletequery(aCopy.deletequery) {}
  
  DeleteStatement::~DeleteStatement() {}

  StatusResult DeleteStatement::parse(Tokenizer &aTokenizer) {
    StatusResult theResult{syntaxError};
    if(!hasProperLen(aTokenizer,3)) {return theResult;}
    TokenSequence theSeq(aTokenizer);
    //Parse fields 
    if(theSeq.has(Keywords::delete_kw).then(Keywords::from_kw).thenId(name)) {
      aTokenizer.next(3);
      theResult.error = noError;
      if(Entity* theEntity = proc->getEntity(name)) {
        deletequery.setFrom(theEntity);
        if(theResult = parseClause(aTokenizer, deletequery)) {
          //theResult = validateQuery(*static_cast<UpdateQuery*>(&dbQuery));
        }
      }
      else {theResult.error=unknownTable;}
    }
    return theResult;
  }

  StatusResult DeleteStatement::dispatch() {
    return proc->deleteRows(deletequery);
  }

  bool DeleteStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::delete_kw);
  }










}