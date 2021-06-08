#ifndef SQLStatement_hpp
#define SQLStatement_hpp

#include <iostream>
#include <string>
#include "Statement.hpp"
#include "TokenSequence.hpp"
#include "Entity.hpp"
#include "BasicTypes.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  class Tokenizer;
  class SQLProcessor;

  class SQLStatement : public Statement {
  public:
    SQLStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::unknown_kw);
    SQLStatement(const SQLStatement &aCopy);
    
    virtual               ~SQLStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch() {return StatusResult{};}

    StatusResult hasProperLen(Tokenizer& aTokenizer, size_t aMin, size_t aMax=-1);
    
  protected:
    std::string name;
    SQLProcessor* proc;
  };



  class CreateSQLStatement : public SQLStatement {
  public:
    CreateSQLStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::create_kw);
    CreateSQLStatement(const CreateSQLStatement &aCopy);
    
    virtual  ~CreateSQLStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);

    Attribute* parseAttribute(Tokenizer &aTokenizer);

  protected:
    AttributeList attrList;
  };



  class DropSQLStatement : public SQLStatement {
  public:
    DropSQLStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::drop_kw);
    DropSQLStatement(const DropSQLStatement &aCopy);
    
    virtual  ~DropSQLStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);

  };



  class DescribeSQLStatement : public SQLStatement {
  public:
    DescribeSQLStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::describe_kw);
    DescribeSQLStatement(const DescribeSQLStatement &aCopy);
    
    virtual  ~DescribeSQLStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);
  };


  class ShowSQLStatement : public SQLStatement {
  public:
    ShowSQLStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::show_kw);
    ShowSQLStatement(const ShowSQLStatement &aCopy);
    
    virtual  ~ShowSQLStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);
  };

  class InsertStatement : public SQLStatement {
  public:
    InsertStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::insert_kw);
    InsertStatement(const InsertStatement &aCopy);
    
    virtual  ~InsertStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);

    template<typename T>
    StatusResult parseRowField(Tokenizer &aTokenizer, std::vector<T> &aList);

  protected:
    StringList keys;
    std::vector<Values> valueCollections;
  };



  //--------------------------------------------------------------------------------
  class ShowIndexStatement: public SQLStatement {
  public:
    ShowIndexStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::show_kw);
    ShowIndexStatement(const ShowIndexStatement &aCopy);
    
    virtual               ~ShowIndexStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);

  protected:
    std::string indexName;
  };


  class ShowIndexesStatement: public SQLStatement {
  public:
    ShowIndexesStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::show_kw);
    ShowIndexesStatement(const ShowIndexesStatement &aCopy);
    
    virtual               ~ShowIndexesStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);
  protected:

  };
  
}

#endif /* SQLStatement_hpp */
