#include "DBStatement.hpp"
#include "Tokenizer.hpp"
#include "DBProcessor.hpp"


namespace ECE141 {

  DBStatement::DBStatement(DBProcessor* aProc, Keywords aStatementType) : 
      Statement(aStatementType), proc(aProc){}

  DBStatement::DBStatement(const DBStatement &aCopy) : 
  Statement(aCopy), proc(aCopy.proc), name(aCopy.name){}

  DBStatement::~DBStatement() {}

  StatusResult DBStatement::parse(Tokenizer &aTokenizer) {
    return StatusResult{};
  }

  //Check if the command satisfies length requirement
  StatusResult DBStatement::hasProperLen(Tokenizer &aTokenizer, size_t aMin, size_t aMax) {
    if(aTokenizer.remaining() < aMin) {
      return StatusResult{syntaxError};
    }
    else if(aMax != -1 && aTokenizer.remaining() > aMax) {
      return StatusResult{syntaxError};
    }
    return StatusResult{};
  }

  //---------------------------------------------------------
  //Create statement
  CreateDBStatement::CreateDBStatement(DBProcessor* aProc, Keywords aStatementType) :
      DBStatement(aProc, aStatementType) {}

  CreateDBStatement::CreateDBStatement(const CreateDBStatement &aCopy) :
    DBStatement(aCopy) {}
  
  CreateDBStatement::~CreateDBStatement() {}

  StatusResult CreateDBStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,3)) {return StatusResult{syntaxError};}
    aTokenizer.next(2);
    //Set database name
    if(aTokenizer.more()) {
      name = aTokenizer.current().data;
      aTokenizer.next();
      return StatusResult{noError};
    }
    return StatusResult{identifierExpected};
  }

  StatusResult CreateDBStatement::dispatch() {
    return proc->createDB(name);
  }


  //--------------------------------------------------------------
  //Drop statement
  DropDBStatement::DropDBStatement(DBProcessor* aProc, Keywords aStatementType) :
      DBStatement(aProc, aStatementType) {}

  DropDBStatement::DropDBStatement(const DropDBStatement &aCopy) :
    DBStatement(aCopy) {}
  
  DropDBStatement::~DropDBStatement() {}

  StatusResult DropDBStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,3)) {return StatusResult{syntaxError};}
    aTokenizer.next(2);
    //Set database name
    if(aTokenizer.more()) {
      name = aTokenizer.current().data;
      aTokenizer.next();
      return StatusResult{noError};
    }
    return StatusResult{identifierExpected};
  }

  StatusResult DropDBStatement::dispatch() {
    
    return proc->dropDB(name);
  }


  //--------------------------------------------------------------
  //Use statement
  UseDBStatement::UseDBStatement(DBProcessor* aProc, Keywords aStatementType) :
      DBStatement(aProc, aStatementType) {}

  UseDBStatement::UseDBStatement(const UseDBStatement &aCopy) :
    DBStatement(aCopy) {}
  
  UseDBStatement::~UseDBStatement() {}

  StatusResult UseDBStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,2)) {return StatusResult{syntaxError};}
    aTokenizer.next();
    //Set database name
    if(aTokenizer.more()) {
      name = aTokenizer.current().data;
      aTokenizer.next();
      return StatusResult{noError};
    }
    return StatusResult{identifierExpected};
  }

  StatusResult UseDBStatement::dispatch() {
    return proc->useDB(name);
  }


  //--------------------------------------------------------------
  //dump statement
  DumpDBStatement::DumpDBStatement(DBProcessor* aProc,Keywords aStatementType) :
      DBStatement(aProc, aStatementType) {}

  DumpDBStatement::DumpDBStatement(const DumpDBStatement &aCopy) :
    DBStatement(aCopy) {}
  
  DumpDBStatement::~DumpDBStatement() {}

  StatusResult DumpDBStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,3)) {return StatusResult{syntaxError};}
    aTokenizer.next(2);
    //Set database name
    if(aTokenizer.more()) {
      name = aTokenizer.current().data;
      aTokenizer.next();
      return StatusResult{noError};
    }
    return StatusResult{identifierExpected};
  }

  StatusResult DumpDBStatement::dispatch() {
    return proc->dumpDB(name);
  }



  //--------------------------------------------------------------
  //Show statement
  ShowDBStatement::ShowDBStatement(DBProcessor* aProc, Keywords aStatementType) :
      DBStatement(aProc, aStatementType) {}

  ShowDBStatement::ShowDBStatement(const ShowDBStatement &aCopy) :
    DBStatement(aCopy) {}
  
  ShowDBStatement::~ShowDBStatement() {}

  StatusResult ShowDBStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    //Set database name
    if(!hasProperLen(aTokenizer,2)) {return StatusResult{syntaxError};}
    aTokenizer.next(2);
    return StatusResult{noError};
  }

  StatusResult ShowDBStatement::dispatch() {
    return proc->showDB();
  }

}