//
//  SQLProcessor.cpp
//  RGAssignment3
//
//  Created by rick gessner on 4/1/21.
//

#include "SQLProcessor.hpp"
#include "SQLStatement.hpp"
#include "DBProcessor.hpp"
#include "Database.hpp"
#include "Helpers.hpp"
#include "TabularView.hpp"
#include "Timer.hpp"
#include "CmdView.hpp"
#include "EntityView.hpp"
#include "IndexView.hpp"
#include "TableDescriptionView.hpp"
#include "SQLSelectStatement.hpp"
#include <algorithm>
#include <variant>


namespace ECE141 {
  
  //Constructor
  SQLProcessor::SQLProcessor(std::ostream &anOutput, CmdProcessor *aNext) :
    CmdProcessor(anOutput, aNext), activeDB(nullptr) {
      //Set the next responder in chain to be DB processor 
      if(aNext == nullptr) {
        next = new DBProcessor(anOutput);
      }
    }

  SQLProcessor::~SQLProcessor() {
    if(next) {delete next;}
  }
  
  
  //---------------------------------------------------------
  //Specific Statement Methods 

  Entity* SQLProcessor::getEntity(const std::string &aName) {
    if(useDatabase()) {
      return activeDB->getEntity(aName);
    }
    return nullptr;
  }

  //Get the database pointer from the db processor
  StatusResult SQLProcessor::useDatabase() {
      DBProcessor* dbProc = static_cast<DBProcessor*>(next);
      activeDB = dbProc->getDatabase();
      if(activeDB) {
        return StatusResult{};
      }
      return StatusResult{unknownDatabase};
    }

  //Effectively create a table in the database
  StatusResult SQLProcessor::createTable(const std::string& aName, AttributeList anAttrList) {
    StatusResult theResult = useDatabase();
    if(theResult && activeDB->findEntity(aName)!=kNewBlock) {
      theResult = StatusResult{tableExists};
    }
    if(theResult) {
      theResult = activeDB->addEntity(aName,anAttrList);
    }
    if(theResult) {
      timer.stop();
      CmdView theView(0,timer.elapsed());
      theView.show(output);
    }
    return theResult;
  }

  //show tables if table exists
  StatusResult SQLProcessor::showTables() {
    if(!useDatabase()) {return StatusResult{unknownDatabase};}
    EntityView theView(activeDB, timer);

    theView.show(output);
    return StatusResult{};
  }


  //Effctively remove the table from the database
  StatusResult SQLProcessor::dropTable(const std::string& aName) {
    if(!useDatabase()) {return StatusResult{unknownDatabase};}
    if(activeDB->findEntity(aName)==kNewBlock) {return StatusResult{unknownTable};}
    StatusResult theResult{};
    uint32_t theBlockNum=0;
    theResult = activeDB->removeEntity(aName, theBlockNum);
    timer.stop();

    // Moved the command view to database because passing back the number of rows seems 
    // redundant
    CmdView theView(0, timer.elapsed());
    theView.show(output);
    return theResult;
  }

  //Describe the entity with all the attributes it contains
  StatusResult SQLProcessor::describe(const std::string& anEntityName) {
    if(!useDatabase()) {return StatusResult{unknownDatabase};}
    //First verify if entity exists
    uint32_t theBlockNum = activeDB->findEntity(anEntityName);
    if(theBlockNum==kNewBlock) {return StatusResult{unknownTable};}
    //Construct the entity from the database
    Entity theEntity{"", theBlockNum};
    std::stringstream theEntityStream;
    StatusResult theResult{};
    if(theResult = (activeDB->getStorage().load(theEntityStream,theBlockNum)) ) {
      theEntity.decode(theEntityStream);
      TableDescriptionView theDescriptionView(theEntity);
      theDescriptionView.show(output);
      return theResult;      
    }

    return theResult;
  }

  //Validate the rows before inserting
  StatusResult SQLProcessor:: validateRow(const std::string& aName, 
      StringList& aKeys, Values& aValues, Entity* anEntity) {
    //Verify if the key list size matches the value  list size
    if(aKeys.size() != aValues.size()) {
      return StatusResult{keyValueMismatch};
    }

    //Verify the given keys exist in the entity
    StringList theKeyList = anEntity -> getAttributeNameList();
    for(auto& key: aKeys) {
      if(std::find(theKeyList.begin(),theKeyList.end(),key)==std::end(theKeyList)){
        return StatusResult{keyValueMismatch};
      }
    }

    //maybe more
    return StatusResult{};
  }

  //Insert rows into a database with validation given a collection of keys and values
  //from the statement
  StatusResult SQLProcessor::insertRows(const std::string& aName, 
      StringList aKeys, std::vector<Values> aValueCollect) {
    if(!useDatabase()) {return StatusResult{unknownDatabase};}

    //Verify if table exists
    Entity* theEntity = activeDB->getEntity(aName);
    if(theEntity==nullptr) {return StatusResult{unknownTable};}

    std::vector<uint32_t> theRowBlockNums;
    uint32_t currPrimaryKey = theEntity->auto_increment;

    StatusResult theResult;
    for(auto& value: aValueCollect) {
      theResult=validateRow(aName,aKeys,value, theEntity); //First verify each row
      if(!theResult) {
        return theResult;
      }
      //construct the row
      uint32_t theBlockNum = activeDB->getStorage().getFreeBlock();
      Row theRow(Entity::hashString(aName), theBlockNum);
      for(size_t i=0; i<aKeys.size(); i++) {
        theRow.setField(aKeys[i],value[i]);
      }
      //Handle auto_increment value
      if(theResult = theEntity->incPrimaryKey()) {
        theRow.setField(theEntity->findPrimaryKey().value(), Value{(int)(theEntity->auto_increment)});
      }
      //store the row block
      Block theBlock;
      theBlock.header.id = (int) theEntity->auto_increment;
      theRow.initBlock(theBlock);
      theResult = activeDB->getStorage().writeBlock(theBlockNum,theBlock);
      //Store the row block number for indexing
      theRowBlockNums.push_back(theBlockNum);
    }

    //add the rows to indexing
    theEntity->eachIndex([&](const std::string& anIndexName, uint32_t aBlockNum) -> bool {
      Index theIndex = activeDB->getIndexbyBlockNum(aBlockNum);
      if(anIndexName==theEntity->findPrimaryKey().value()) {
          for(auto theRowBlockNum: theRowBlockNums) {
          IndexKey theKey(++currPrimaryKey);
          activeDB->addRowIntoIndex(theIndex, theKey, theRowBlockNum);
        }
      }
      else{
        int theKeyPos = std::find(aKeys.begin(), aKeys.end(), anIndexName) - aKeys.begin();
        size_t i_row = 0;
        for(auto& value: aValueCollect) {  
          Value theValue = value[theKeyPos];
          int theValueIndex = theValue.index();
          switch(theValueIndex) {
            case 0: {
            uint32_t theKey = std::get<0>(theValue);
            IndexKey theIndexKey(theKey);
            activeDB->addRowIntoIndex(theIndex,theIndexKey,theRowBlockNums[i_row]);
            i_row++;
            return true;
           }
            case 1: {
            uint32_t theKey = std::get<1>(theValue);
            IndexKey theIndexKey(theKey);
            activeDB->addRowIntoIndex(theIndex,theIndexKey,theRowBlockNums[i_row]);
            i_row++;
            return true;
           }
            case 2: {
            uint32_t theKey = std::get<2>(theValue);
            IndexKey theIndexKey(theKey);
            activeDB->addRowIntoIndex(theIndex,theIndexKey,theRowBlockNums[i_row]);
            i_row++;
            return true;
           }
            case 3: {
            std::string theKey = std::get<std::string>(theValue);
            IndexKey theIndexKey(theKey);
            activeDB->addRowIntoIndex(theIndex,theIndexKey,theRowBlockNums[i_row]);
            i_row++;
            return true;
           }
          }

 
          // std::visit([&](auto const &theIndexKey) {
          //   IndexKey theKey((uint32_t)theIndexKey);
          //   activeDB->addRowIntoIndex(theIndex, theKey, theRowBlockNums[i_row]);
          // }, theIndexKey);
          // i_row++;
        }
      }
      activeDB->modifyIndex(theIndex);
      return true;
    });


    //Store the entity back with modified primary key auto_increment value
    if(theResult) {activeDB->modifyEntity(*theEntity, theEntity->getBlockNum());}
    delete theEntity;
    timer.stop();
    CmdView theView(aValueCollect.size(), timer.elapsed());
    theView.show(output);
    return theResult;
  }


  StatusResult SQLProcessor::showQuery(Query& aQuery) {
    Rows theRows;
    StatusResult theResult = useDatabase();
    if(theResult) {
      theResult = activeDB->selectRows(aQuery, theRows);
      aQuery.run(theRows);
      timer.stop();
      TabularView theRowsView(theRows, timer.elapsed());
      theRowsView.show(output);
    }
    return theResult;
  }

  StatusResult SQLProcessor::showQuerywithJoin(Query& aQuery, JoinList& aJoins) {
    Rows theRows;
    StatusResult theResult = useDatabase();
    if(theResult) {
      theResult = activeDB->joinRows(aQuery, theRows, aJoins);
      aQuery.sort(theRows);
      timer.stop();
      TabularView theRowsView(theRows, timer.elapsed());
      theRowsView.show(output);
    }
    return theResult;
  }

  StatusResult SQLProcessor::updateRows(UpdateQuery& anUpdateQuery) {
    Rows theRows;
    StatusResult theResult = useDatabase();
    if(theResult) {
      theResult = activeDB->updateRows(anUpdateQuery);
      timer.stop();
      CmdView theView(anUpdateQuery.getAffected(), timer.elapsed());
      theView.show(output);
    }
    return theResult;
  }

  StatusResult SQLProcessor::deleteRows(DeleteQuery& aDeleteQuery) {
    Rows theRows;
    StatusResult theResult = useDatabase();
    if(theResult) {
      theResult = activeDB->deleteRows(aDeleteQuery);
      timer.stop();
      CmdView theView(aDeleteQuery.getAffected(), timer.elapsed());
      theView.show(output);
    }
    return theResult;
  }
  
  StatusResult SQLProcessor::showIndexes() {
    if(!useDatabase()) {return StatusResult{unknownDatabase};}
    EntityView theView(activeDB, timer, true);
    theView.show(output);
    return StatusResult{};  
  }

  StatusResult SQLProcessor::showIndex(const std::string& anEntityName, 
                                       const std::string& anIndexName) {
    if(!useDatabase()) {return StatusResult{unknownDatabase};}
    Entity* theEntity = activeDB->getEntity(anEntityName);
    Index theIndex = activeDB->getIndexbyBlockNum(theEntity->getIndexBlockNum(anIndexName));
    IndexView theView(theIndex, timer);
    theView.show(output);
    return StatusResult{}; 
  }


  //------------------------------------------------
  //Factories 
  Statement* CreateSQLStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    return new CreateSQLStatement(aProc);
  }

  Statement* ShowSQLStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    if(theSeq.has(Keywords::show_kw).then(Keywords::index_kw)) {
      return new ShowIndexStatement(aProc);
    }
    else if(theSeq.has(Keywords::show_kw).then(Keywords::indices_kw)) {
      return new ShowIndexesStatement(aProc);
    }
    return new ShowSQLStatement(aProc);
  }

  Statement* DescribeSQLStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    return new DescribeSQLStatement(aProc);
  }

  Statement* DropSQLStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    return new DropSQLStatement(aProc);
  }

  Statement* InsertStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    return new InsertStatement(aProc);
  }

  Statement* SelectStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    return new SQLSelectStatement(aProc);
  }

  Statement* UpdateStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    return new UpdateStatement(aProc);
  }

  Statement* DeleteStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
    return new DeleteStatement(aProc);
  }

  // Statement* ShowIndStmtFactory(SQLProcessor* aProc, Tokenizer& aTokenizer) {
  //   TokenSequence theSeq(aTokenizer);
  //   if(theSeq.has(Keywords::show_kw).then(Keywords::index_kw)) {
  //     return new ShowIndexStatement(aProc);
  //   }
  //   else if(theSeq.has(Keywords::show_kw).then(Keywords::indices_kw)) {
  //     return new ShowIndexesStatement(aProc);
  //   }
  //   return nullptr;
  // }

  
  //------------------------------------------------
  //General SQL Processor workflow
  CmdProcessor* SQLProcessor::recognizes(Tokenizer &aTokenizer) {
    return (CreateSQLStatement::recognizes(aTokenizer) ||
            DropSQLStatement::recognizes(aTokenizer) ||
            DescribeSQLStatement::recognizes(aTokenizer) ||
            ShowSQLStatement::recognizes(aTokenizer)) ||
            InsertStatement::recognizes(aTokenizer) ||
            SQLSelectStatement::recognizes(aTokenizer) ||
            UpdateStatement::recognizes(aTokenizer) ||
            DeleteStatement::recognizes(aTokenizer) ||
            ShowIndexStatement::recognizes(aTokenizer) ||
            ShowIndexesStatement::recognizes(aTokenizer)
            ? this : next->recognizes(aTokenizer);
  }

  StatusResult SQLProcessor::run(Statement* aStatement, const Timer &aTimer) {
    timer = aTimer;
    return aStatement->dispatch();
  }

  using Factory = std::function<Statement*(SQLProcessor*, Tokenizer&)>;  
  // USE: retrieve a statement based on given text input...
  Statement* SQLProcessor::makeStatement(Tokenizer &aTokenizer) {
    static std::map<Keywords, Factory> theFactories {
      {Keywords::create_kw, CreateSQLStmtFactory},
      {Keywords::show_kw, ShowSQLStmtFactory},
      {Keywords::describe_kw, DescribeSQLStmtFactory},
      {Keywords::drop_kw, DropSQLStmtFactory},
      {Keywords::insert_kw, InsertStmtFactory},
      {Keywords::select_kw, SelectStmtFactory},
      {Keywords::update_kw,   UpdateStmtFactory},
      {Keywords::delete_kw,   DeleteStmtFactory},
    };
    Keywords theKw = aTokenizer.tokenAt(0).keyword;
    if(theFactories.count(theKw)) {
      Statement* theStmt = theFactories[theKw](this, aTokenizer);
      if(theStmt && theStmt->parse(aTokenizer)) {return theStmt;}
    }
    return nullptr;

  }





}
