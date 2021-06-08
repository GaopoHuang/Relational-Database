
#ifndef DBProcessr_cpp
#define DBProcessr_cpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Database.hpp"
#include <vector>

namespace ECE141 {
  const static std::vector<std::vector<Keywords>> DBKeywordSequnce;

  class DBProcessor : public CmdProcessor {
  public:
    
    DBProcessor(std::ostream &anOutput, CmdProcessor* aNext = nullptr, Database* aDB=nullptr);
    virtual ~DBProcessor();

    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer) override;
    StatusResult  run(Statement *aStmt, const Timer &aTimer) override;
    
    StatusResult createDB(const std::string& aName);
    StatusResult dropDB(const std::string& aName);
    StatusResult useDB(const std::string& aName);
    StatusResult showDB() const;
    StatusResult dumpDB(const std::string& aName);

    bool DBExists(const std::string& aName);
    Database* getDatabase() {return activeDB;}

  protected: 
    Database* loadDB(const std::string& aName);
    DBProcessor& releaseDB();
    Database* activeDB; //Contains a Database pointer
  };
  
}

#endif /* DBProcessr_cpp */
