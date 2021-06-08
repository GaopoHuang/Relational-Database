//
//  Database.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include "Storage.hpp"
#include "Database.hpp"
#include "Config.hpp"
#include "Filters.hpp"
#include "Compare.hpp"


namespace ECE141 {
  
  Database::Database(const std::string aName, CreateDB)
    : name(aName), storage(stream), changed(true)  {
    std::string thePath = Config::getDBPath(name);
    stream.clear(); // Clear Flag, then create file...
    stream.open(thePath.c_str(), std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc);
    stream.close();
    stream.open(thePath.c_str(), std::fstream::binary | std::fstream::binary | std::fstream::in | std::fstream::out);
      
    //Write meta info to block 0...
    Block theBlock(BlockType::meta_block);
    storage.writeBlock(0,theBlock);
  }

  Database::Database(const std::string aName, OpenDB)
    : name(aName), changed(false), storage(stream) {
      
      std::string thePath = Config::getDBPath(name);
      stream.open (thePath.c_str(), std::fstream::binary
                   | std::fstream::in | std::fstream::out);
      
      //Load meta info from block 0
      std::stringstream theStringStream;
      storage.load(theStringStream, 0);

      decode(theStringStream);
  }

  Database::~Database() {

    if(changed) {
      //save db meta information to block 0?
      saveMeta();
    }
    stream.close();

  }

//Use-----------------------------------------------------------------------------
  std::string Database::getName() const {
    return name;
  }

  Storage& Database::getStorage() {
    return storage;
  }

  //Returns the block number of the entity if it exists, and -1 otherwise
  uint32_t Database::findEntity(const std::string & aName) const {
    return meta.count(aName) ? meta.at(aName) : kNewBlock;
  }


  //Effectively change the block at the given block number to the given entity
  StatusResult Database::modifyEntity(Entity& anEntity, uint32_t aBlockNum) {
    //Use storage stream io to store the entity
    std::stringstream theEntityStream;
    anEntity.encode(theEntityStream);
    StorageInfo theInfo(anEntity.refID, theEntityStream.tellp(),
                        aBlockNum, BlockType::entity_block);
    storage.save(theEntityStream, theInfo);
    changed = true;
    return StatusResult{};
  }

  //Effectively add the entity into storage, while also modifying the meta information
  StatusResult Database::addEntity(const std::string & aName, 
                                   AttributeList anAttrList) {
    StatusResult theResult;
    // Get 2 free blocks, one for the entity and another for the index
    std::vector<uint32_t> theFreeBlocks = storage.getFreeBlocks(2);
    uint32_t theEntityBlockNum=theFreeBlocks[0];
    uint32_t theIndexBlockNum=theFreeBlocks[1];
    Entity theEntity(aName, anAttrList, theEntityBlockNum);
    //Check if the entity has primary key
    StringOpt thePK =  theEntity.findPrimaryKey();
    if(thePK==std::nullopt) {return StatusResult{primaryKeyRequired};}
    //Add an index block when creating a table 
    theResult = addIndex(aName, thePK.value(), theIndexBlockNum); //Use default int primary key
    theEntity.setIndex(thePK.value(), theIndexBlockNum);
    theResult = modifyEntity(theEntity, theEntityBlockNum);
    meta[aName] = theEntityBlockNum; //modify meta information
    return theResult;
  }

  //Effectively remove the entity and its associated rows, while also modifying the 
  //meta information accordingly
  StatusResult Database::removeEntity(const std::string & aName, uint32_t& aNumBlock) {
    StatusResult theResult{};
    //Verify table exists
    uint32_t theBlokNum = findEntity(aName);
    if(theBlokNum==kNewBlock) {
      return StatusResult{unknownTable};
    }
    //Remove all the associated rows in the entity via scanning across the priamry index
    Index theIndex = getPrimaryIndex(aName);
    theIndex.each([&](const Block& aBlock, uint32_t blockNum) {
      if(aBlock.isDataBlock())  {
        storage.markBlockAsFree(blockNum);
      }
      return true;
    });
    //Remove all indexes associated with the entity along with the entity
    std::map<std::string, uint32_t> theIndexes = getIndexesfromEntity(aName);
    for(auto theIndexBlock: theIndexes) {
      storage.markBlockAsFree(theIndexBlock.second);
    }
    theResult = storage.markBlockAsFree(theBlokNum);
    aNumBlock = theIndex.getSize();
    meta.erase(aName);
    changed = true;
    return theResult;
  }


  //Get the name of all the entities
  StringList Database::getEntityList() const {
    StringList theTableList;
    for(auto thePair : meta) {
      theTableList.push_back(thePair.first);
    }
    return theTableList;
  }

  //Return an entity pointer that must be deleted when called
  //Maybe change to a smart pointer in the future
  Entity* Database::getEntity(const std::string &aName) {
    uint32_t theBlockNum = findEntity(aName);
    if(theBlockNum!=kNewBlock) {
      //Use storage stream io and entity decoder API to construct the entity
      Entity* theEntity = new Entity(aName,theBlockNum);
      std::stringstream theEntityStream;
      storage.load(theEntityStream,theBlockNum);
      theEntity->decode(theEntityStream);

      // //debugging 
      // std::cout << "In Database::getEntity, the entity stream is: " << theEntityStream.str() <<" \n";

      return theEntity;
    }
    return nullptr;
  }

  //find the Entity Name given a hash ID
  StringOpt Database::getEntityNameById(size_t aHashId) {
    for(auto& entityInfo: meta) {
      size_t theId = Entity::hashString(entityInfo.first);
      if(theId == aHashId) {
        return entityInfo.first;
      }
    }
    return std::nullopt;
  }

  StatusResult Database::selectRows(const Query& aQuery, Rows& aRows) {
    std::string theEntityName = aQuery.getFrom()->getName();
    Index thePKIndex = getPrimaryIndex(theEntityName);
    thePKIndex.each([&](const Block& aBlock, uint32_t aBlockNum)->bool {
      Row theRow(aBlock.header.refId);
      std::stringstream theRowStream;
      storage.load(theRowStream, aBlockNum);
      theRow.decode(theRowStream);
      aRows.push_back(std::make_unique<Row>(theRow));
      return true;
    });
    return StatusResult{};
  }


  StatusResult Database::updateRows(UpdateQuery& anUpdateQuery) {
    Rows theRows;
    storage.each([&](const Block& aBlock, uint32_t blockNum) -> bool {
      uint32_t theRefID = anUpdateQuery.getFrom()->refID;
      if(aBlock.isDataBlock() && aBlock.header.refId == theRefID) {
        Row theRow(theRefID);
        std::stringstream theRowStream;
        storage.load(theRowStream, blockNum);
        theRow.decode(theRowStream);
        theRows.push_back(std::make_unique<Row>(theRow));
      }     
      return true;
    });
    anUpdateQuery.run(theRows);
    for (auto& theRow : theRows) {
      for (auto& theField: anUpdateQuery.getUpdateFields()) {
        theRow->setField(theField.first, theField.second);
      }
      Block theBlock;
      theRow->initBlock(theBlock);
      storage.writeBlock(theRow->getBlockNum(),theBlock);

      // //debugging
      // std::cout << "In Database::updateRows, the block number updated is: "<< theRow->getBlockNum()<< " \n";
    }
    changed = true;
    return StatusResult{};
  }


  StatusResult Database::deleteRows(DeleteQuery& aDeleteQuery) {
    Rows theRows;
    std::string theEntityName = aDeleteQuery.getFrom()->getName();
    Index thePKIndex = getPrimaryIndex(theEntityName);
    thePKIndex.each([&](const Block& aBlock, uint32_t aBlockNum)->bool {
      Row theRow(aBlock.header.refId);
      std::stringstream theRowStream;
      storage.load(theRowStream, aBlockNum);
      theRow.decode(theRowStream);
      theRows.push_back(std::make_unique<Row>(theRow));
      return true;
    });
    aDeleteQuery.run(theRows);
    std::vector<uint32_t> theRowBlockNums;
    for (auto& theRow : theRows) {
      storage.markBlockAsFree(theRow->getBlockNum());
      theRowBlockNums.push_back(theRow->getBlockNum());
    }

    //Remove rows from index
    aDeleteQuery.getFrom()->eachIndex([&](const std::string anIndexName, uint32_t indexBlockNum) {
      Index theIndex = getIndexbyBlockNum(indexBlockNum);
      std::vector<IndexKey> theRemovingKeys;
      
      // //Debugging
      // std::stringstream theDebugStream;
      // theIndex.encode(theDebugStream);
      // std::cout << "in Database::deleteRows at the index " << theIndex.getName() << " whose info is: " << theDebugStream.str() << "\n";

      theIndex.eachKV([&](const IndexKey& aKey, uint32_t aRowBlockNum) {
        if(std::find(theRowBlockNums.begin(), theRowBlockNums.end(), aRowBlockNum)!= theRowBlockNums.end()) {
          theRemovingKeys.push_back(aKey);
        }
        return true;
      });
      for(auto& theRemovingKey: theRemovingKeys) {
        std::visit([&theIndex](const auto& theRemovingKey){
          theIndex.erase(theRemovingKey);
        }, theRemovingKey);
      }
      modifyIndex(theIndex);
      return true;

    });

    changed = true;
    return StatusResult{};
  }

  StatusResult Database::joinRows(const Query& aQuery, Rows& aRows, JoinList& aJoins) {
    std::string theEntityName = aQuery.getFrom()->getName();
    std::string theMainEntity, theSecEntity, theMainField, theSecField;
    switch (aJoins[0].joinType) {
      case Keywords::left_kw:
        theMainEntity = theEntityName;
        theSecEntity = aJoins[0].table;
        theMainField = (aJoins[0].lhs.table == theMainEntity) ? aJoins[0].lhs.attribute : aJoins[0].rhs.attribute;
        theSecField = aJoins[0].lhs.table == theMainEntity ? aJoins[0].rhs.attribute : aJoins[0].lhs.attribute;
        break;
      case Keywords::right_kw:
        theMainEntity = aJoins[0].table;
        theSecEntity = theEntityName;
        theMainField = aJoins[0].lhs.table == theMainEntity ? aJoins[0].lhs.attribute : aJoins[0].rhs.attribute;
        theSecField = aJoins[0].lhs.table == theMainEntity ? aJoins[0].rhs.attribute : aJoins[0].lhs.attribute;
        break;
      default:
        break;
    }

    Index thePKIndex = getPrimaryIndex(theMainEntity);
    Index theSecIndex = getPrimaryIndex(theSecEntity);
    thePKIndex.each([&](const Block& aBlock, uint32_t aBlockNum)->bool {
      Row theRow(aBlock.header.refId);
      std::stringstream theRowStream;
      storage.load(theRowStream, aBlockNum);
      theRow.decode(theRowStream);
      int count = 0;
      Value theValue1 = theRow.getValuebyKey(theMainField);
      StringList theList = aQuery.getFields();
      KeyValues& theData = theRow.getData();

      theSecIndex.each([&](const Block& aSecBlock, uint32_t aSecNum) -> bool {
        Row theSecRow(aSecBlock.header.refId);
        std::stringstream theSecRowStream;
        storage.load(theSecRowStream, aSecNum);
        theSecRow.decode(theSecRowStream);
        Value theValue2 = theSecRow.getValuebyKey(theSecField);
        if(equals(theValue1, theValue2)) {
          theSecRow.selectFields(theList);
          for (auto& theField: theData) {
            theSecRow.setField(theField.first, theField.second);
          }
          theSecRow.selectFields(theList);
          count++;
          aRows.push_back(std::make_unique<Row>(theSecRow));
        }
        return true;
      });

      if(count==0) {
        for(auto& theField: theList) {
          if(theData.find(theField) == theData.end()) {
            theRow.setField(theField, Value{std::string{"Null"}});
          }
        }
        theRow.selectFields(theList);
        aRows.push_back(std::make_unique<Row>(theRow));
      }
      return true;
    });
    return StatusResult{};
  }







  //Save meta to the storage
  StatusResult Database::saveMeta() {
    std::stringstream theMetaStream;
    encode(theMetaStream);
    StorageInfo theInfo(0,theMetaStream.tellp(), 0, BlockType::meta_block);
    storage.save(theMetaStream,theInfo);

    return StatusResult{};
  }

  StatusResult Database::addRowIntoIndex(Index& anIndex, IndexKey &aKey, uint32_t aValue) {
    if(anIndex.setKeyValue(aKey,aValue)) {return StatusResult{};}
    return StatusResult{indexExists};
  }

  StatusResult Database::modifyIndex(Index& anIndex) {
    std::stringstream theIndexStream;
    anIndex.encode(theIndexStream);
    // //Debugging 
    // std::cout << "In Database::modifyIndex " << "at block number: " << anIndex.getBlockNum()
    //           << ". The index stream is: " << theIndexStream.str() << "\n";
    
    StorageInfo theInfo = anIndex.getStorageInfo(theIndexStream.tellp());
    storage.save(theIndexStream,theInfo);
    changed = true;
    return StatusResult();
  }


  StatusResult Database::addIndex(const std::string& anEntityName, 
        const std::string& anIndexName, uint32_t aBlockNum, IndexType aType) {
    Index theIndex(storage,aBlockNum,aType);
    theIndex.setName(anIndexName);
    theIndex.setId(Entity::hashString(anEntityName));
    modifyIndex(theIndex);
    return StatusResult();
  }


  std::map<std::string, uint32_t> Database::getIndexesfromEntity(const std::string& aName) {
    Entity* theEntity = getEntity(aName);
    std::map<std::string, uint32_t> theIndexes = theEntity->getIndexes();
    delete theEntity;
    return theIndexes;
  }

  std::map<std::string, uint32_t> Database::getIndexesfromEntity(Entity* anEntity) {
    std::map<std::string, uint32_t> theIndexes = anEntity->getIndexes();
    return theIndexes;
  }

  Index Database::getIndexbyBlockNum(uint32_t aBlockNum) {
    std::stringstream theIndexStream;
    storage.load(theIndexStream,aBlockNum);
    Index theIndex(storage);
    theIndex.decode(theIndexStream);
    
    // //Debugging 
    // std::cout << "In Database::getIndexbyBlockNum " << "at block number: " << aBlockNum
    //           << ". The index stream is: " << theIndexStream.str() << "\n" ;
    return theIndex;
  }

  // Index Database::getIndexbyIndexName(const std::string& aName) {

  // }

  Index Database::getPrimaryIndex(const std::string& anEntityName) {
    Entity* theEntity = getEntity(anEntityName);
    std::map<std::string, uint32_t> theIndexes = theEntity->getIndexes();
    uint32_t thePKBlockNum = theIndexes.at(theEntity->findPrimaryKey().value());
    delete theEntity;
    return getIndexbyBlockNum(thePKBlockNum);
  }











  //Storable Interface-----------------------------------------
  //Encode the meta information
  StatusResult Database::encode(std::iostream &anOutput) {
    anOutput << meta.size() << ' ';
    for(auto i: meta) {
      anOutput << i.first << ' ' << i.second << ' ';
    }
    return StatusResult{Errors::noError};
  }

  //Decode the meta information
  StatusResult Database::decode(std::iostream &anInput) {
    uint32_t theCount=0, theValue=0;
    std::string theName;
    anInput >> theCount;
    for(uint32_t i=0; i<theCount; i++) {
      anInput >> theName >> theValue;
      meta[theName] = theValue;
    }
    return StatusResult{Errors::noError};
  }

}
