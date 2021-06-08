#ifndef SQLSelectStatement_hpp
#define SQLSelectStatement_hpp

#include <iostream>
#include <string>
#include "Statement.hpp"
#include "BasicTypes.hpp"
#include "Query.hpp"
#include "Join.hpp"


namespace ECE141 {
  
  class Tokenizer;
  class SQLProcessor;

  class SQLSelectStatement : public Statement {
  public:
    SQLSelectStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::select_kw);
    SQLSelectStatement(const SQLSelectStatement &aCopy);
    
    virtual               ~SQLSelectStatement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    virtual StatusResult  dispatch();

    static bool recognizes(Tokenizer &aTokenizer);
    
    StatusResult hasProperLen(Tokenizer& aTokenizer, size_t aMin, size_t aMax=-1);
    
  protected:
    StatusResult parseClause(Tokenizer &aTokenizer, Query& aQuery);
    StatusResult parseJoin(Tokenizer& aTokenizer);
    StatusResult parseTableName(Tokenizer& aTokenizer, std::string& aTable);
    StatusResult parseTableField(Tokenizer& aTokenizer, TableField& aField);
    virtual StatusResult validateQuery(Query& aQuery);
    Query dbQuery;
    std::string name;
    SQLProcessor* proc;
    JoinList joins;
  };

//------------------------------------------------------------------------------
  class UpdateStatement : public SQLSelectStatement {
  public:
    UpdateStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::update_kw);
    UpdateStatement(const UpdateStatement &aCopy);    
    virtual               ~UpdateStatement();
    
    StatusResult  parse(Tokenizer &aTokenizer) override;
    
    StatusResult  dispatch() override;

    static bool recognizes(Tokenizer &aTokenizer);
  
  protected:
    StatusResult validateQuery(UpdateQuery& anUpdateQuery);
    UpdateQuery updatequery;
  };

//--------------------------------------------------------------------------------
  class DeleteStatement : public SQLSelectStatement {
  public:
    DeleteStatement(SQLProcessor* aProc, Keywords aStatementType=Keywords::update_kw);
    DeleteStatement(const DeleteStatement &aCopy);    
    virtual               ~DeleteStatement();
    
    StatusResult  parse(Tokenizer &aTokenizer) override;
    
    StatusResult  dispatch() override;

    static bool recognizes(Tokenizer &aTokenizer);

  protected:
    DeleteQuery deletequery;
  };















  
}

#endif /* SQLSelectStatement_hpp */
