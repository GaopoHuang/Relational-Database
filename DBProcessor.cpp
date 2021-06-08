
#include <iostream>
#include "DBProcessor.hpp"
#include "Tokenizer.hpp"
#include "DBStatement.hpp"
#include "FolderView.hpp"
#include "CmdView.hpp"
#include "DumpView.hpp"
#include "Timer.hpp"
#include "Config.hpp"
#include <memory>
#include <vector>
#include <algorithm>

namespace ECE141 {

  
  DBProcessor::DBProcessor(std::ostream &anOutput, CmdProcessor *aNext, Database* aDB)
    : CmdProcessor(anOutput, aNext), activeDB(aDB) {
  }
  
  DBProcessor::~DBProcessor() {
    if(next) {delete next;}
    releaseDB(); //delete the database  
  }

  // USE: -----------------------------------------------------
  using KwSeq = std::vector<Keywords>;

  bool isKnown(KwSeq aSeq) {
    //Store the initial key sequence that can be recognized by DB processor
    KwSeq createSeq, dropSeq, showSeq, useSeq, dumpSeq;
    createSeq.push_back(Keywords::create_kw);
    createSeq.push_back(Keywords::database_kw);
    dropSeq.push_back(Keywords::drop_kw);
    dropSeq.push_back(Keywords::database_kw);
    showSeq.push_back(Keywords::show_kw);
    showSeq.push_back(Keywords::databases_kw);
    useSeq.push_back(Keywords::use_kw);
    dumpSeq.push_back(Keywords::dump_kw);
    dumpSeq.push_back(Keywords::database_kw);
    static KwSeq theKnown[]={createSeq, dropSeq, showSeq, useSeq, dumpSeq};

    //Iterate the given key sequence against the known sequence by the processor
    for(auto knownSeq : theKnown) {
      int theLen = knownSeq.size();
      bool matching = true;
      for(int i=0; i<theLen; i++) {
        if(aSeq[i] != knownSeq[i]) {
          matching = false;
          break;
        }
      }
      if(matching) {return true;}
    }

    return false;
  }

  //Should update it with associative dispatch idiom
  CmdProcessor* DBProcessor::recognizes(Tokenizer &aTokenizer) {
    KwSeq theSeq;
    aTokenizer.restart();
    for(int i=0; i<2; i++) {
      theSeq.push_back(aTokenizer.tokenAt(i).keyword);
    }
    aTokenizer.next(2);
    if(isKnown(theSeq)) {
      return this;
    }
    
    //Pass control to your next processor (no next processor yet)
    return nullptr;
  }



  //Use-----------------------------------------
  //Specific Commands implementation 

  bool DBProcessor::DBExists(const std::string& aName) {
    std::ifstream theStream(Config::getDBPath(aName));  
    return !theStream ? false : true;
  }

  DBProcessor& DBProcessor::releaseDB() {
    if(activeDB != nullptr) {delete activeDB;}
    activeDB = nullptr;
    return *this;
  }

  Database* DBProcessor::loadDB(const std::string& aName) {
    Database* theDB = new Database(aName, ECE141::OpenDB());
    return theDB;
  }

  //Create database if the database doesn't exist
  //Does not actively set the current database to the created database
  StatusResult DBProcessor::createDB(const std::string& aName) {
    if(DBExists(aName)) {return StatusResult{databaseExists};}
    Database* theDB = new Database(aName,ECE141::CreateDB());
    delete theDB;
    timer.stop();
    CmdView theView(1,timer.elapsed());
    theView.show(output);
    return StatusResult{};
  }

  //Show the stored database list in the folder
  StatusResult DBProcessor::showDB() const{
    FolderView theView(Config::getStoragePath(), timer, Config::getDBExtension());
    theView.show(output);
    return StatusResult{};
  }

  //Effectively remove the database from the folder
  StatusResult DBProcessor::dropDB(const std::string& aName) {
    if(!DBExists(aName)) {
      return StatusResult{unknownDatabase};
    }
    //get the number of tables in the currently active DB and release it if
    //it's the dropping DB
    size_t n_tables{0};
    if(activeDB && activeDB->getName() == aName) {
      n_tables = activeDB->getEntityList().size();
      releaseDB();
    }
    //Else load the DB, get the number of tables then drop it.
    else {
      Database* theTempDB = loadDB(aName);
      n_tables = theTempDB->getEntityList().size();
      delete theTempDB;     
    }

    if(std::remove(Config::getDBPath(aName).c_str())!=0) {
      return StatusResult{databaseRemovalError};
    }
    
    timer.stop();
    CmdView theView(n_tables, timer.elapsed());
    theView.show(output);
    return StatusResult{};
  }

  //Load the database into memory if exists
  StatusResult DBProcessor::useDB(const std::string& aName) {
    if(!DBExists(aName)) {
      return StatusResult{unknownDatabase};
    }
    activeDB = releaseDB().loadDB(aName);
    output << "Database changed\n";
    return StatusResult{};

  }

  //Debug a database. Does not set current used Database to the dumped database
  StatusResult DBProcessor::dumpDB(const std::string& aName) {
    Database* theDB = loadDB(aName);
    DumpView theView(theDB, timer);
    theView.show(output);
    delete theDB;
    return StatusResult{};
  }

  //------------------------------------------------
  //General DB Processor workflow

  StatusResult DBProcessor::run(Statement* aStatement, const Timer &aTimer) {
    timer = aTimer;
    return aStatement->dispatch();
  }
    
  // USE: retrieve a statement based on given text input...
  Statement* DBProcessor::makeStatement(Tokenizer &aTokenizer) {
    Keywords theKw = aTokenizer.tokenAt(0).keyword;
    return DBStatmentFactory::create(this,theKw, aTokenizer);
  }

}
