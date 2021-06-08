#ifndef DBStatement_hpp
#define DBStatement_hpp

#include <iostream>
#include <string>
#include "Statement.hpp"

namespace ECE141 {
  
  class Tokenizer;
  class DBProcessor;

  //Database statement mixin-class with length checking method 
  //Contains a database name and db processor field
  class DBStatement : public Statement {
  public:
    DBStatement(DBProcessor* aProc, Keywords aStatementType=Keywords::unknown_kw);
    DBStatement(const DBStatement &aCopy);
    
    virtual               ~DBStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    Keywords              getType() const {return stmtType;}
    
    virtual StatusResult  dispatch() {return StatusResult{};}

    StatusResult hasProperLen(Tokenizer& aTokenizer, size_t aMin, size_t aMax=-1);

  protected:
    std::string name;
    DBProcessor* proc;
  };



  class CreateDBStatement : public DBStatement {
  public:
    CreateDBStatement(DBProcessor* aProc, Keywords aStatementType=Keywords::create_kw);
    CreateDBStatement(const CreateDBStatement &aCopy);
    
    virtual  ~CreateDBStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();
  };



  class DropDBStatement : public DBStatement {
  public:
    DropDBStatement(DBProcessor* aProc, Keywords aStatementType=Keywords::drop_kw);
    DropDBStatement(const DropDBStatement &aCopy);
    
    virtual  ~DropDBStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();
  };



  class UseDBStatement : public DBStatement {
  public:
    UseDBStatement(DBProcessor* aProc, Keywords aStatementType=Keywords::use_kw);
    UseDBStatement(const UseDBStatement &aCopy);
    
    virtual  ~UseDBStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();
  };


  class DumpDBStatement : public DBStatement {
  public:
    DumpDBStatement(DBProcessor* aProc, Keywords aStatementType=Keywords::dump_kw);
    DumpDBStatement(const DumpDBStatement &aCopy);
    
    virtual  ~DumpDBStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();
  };



  class ShowDBStatement : public DBStatement {
  public:
    ShowDBStatement(DBProcessor* aProc, Keywords aStatementType=Keywords::show_kw);
    ShowDBStatement(const ShowDBStatement &aCopy);
    
    virtual  ~ShowDBStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();
  };

  


  //--------------------------------------------------------
  //DB Statement factory given a keyword
  class DBStatmentFactory {
  public:
    static DBStatement* create(DBProcessor* aProc, Keywords aStatementType, Tokenizer& aTokenizer) {
      DBStatement* theStmt;
      switch (aStatementType)
      {
      case Keywords::create_kw:
        theStmt =  new CreateDBStatement(aProc);
        break;
      case Keywords::drop_kw:
        theStmt =   new DropDBStatement(aProc);
        break;
      case Keywords::use_kw:
        theStmt =  new UseDBStatement(aProc);
        break;
      case Keywords::show_kw:
        theStmt =  new ShowDBStatement(aProc);  
        break;  
      default:
        theStmt =  new DumpDBStatement(aProc);    
      }
      theStmt->parse(aTokenizer);
      return theStmt;
    }
  };
  
}

#endif /* DBStatement_hpp */
