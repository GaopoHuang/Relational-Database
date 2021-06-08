//
//  SQLProcessor.hpp
//  RGAssignment3
//
//  Created by rick gessner on 4/1/21.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "Entity.hpp"
#include "Database.hpp"

namespace ECE141 {

  class Statement;

  class SQLProcessor : public CmdProcessor {
  public:
    
    //OCF methods...
    SQLProcessor(std::ostream &anOutput, CmdProcessor* aNext=nullptr);
    ~SQLProcessor();
        
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer) override;
    StatusResult  run(Statement *aStmt, const Timer &aTimer) override;
    // virtual StatusResult handleInput(std::istream &anInput);
  
    StatusResult createTable(const std::string& aName, AttributeList anAttrList);
    StatusResult dropTable(const std::string& aName);
    StatusResult describe(const std::string& anEntityName);
    StatusResult showTables();
    StatusResult insertRows(const std::string& aName, StringList aKeys, 
                            std::vector<Values> aValueCollect);
    StatusResult showQuery(Query& aQuery);
    StatusResult showQuerywithJoin(Query& aQuery, JoinList& aJoins);
    StatusResult updateRows(UpdateQuery& anUpdateQuery);
    StatusResult deleteRows(DeleteQuery& aDeleteQuery);

    StatusResult showIndexes();
    StatusResult showIndex(const std::string& anEntityName, const std::string& anIndexName);
    //Helper function
    Entity* getEntity(const std::string& aName);
    StatusResult useDatabase();
    StatusResult validateRow(const std::string& aName, StringList& aKeys, 
                            Values& aValues, Entity* anEntity);
    
  protected:
    Database* activeDB;
  };

}

#endif /* SQLProcessor_hpp */
