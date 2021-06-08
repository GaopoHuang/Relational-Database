//
//  Row.cpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Row.hpp"
#include "Database.hpp"
#include "sstream"

namespace ECE141 {
  
  //OCF--------------------------------------------------------------------------
  // Row::Row(Values aValues, uint32_t aRefId, uint32_t aBlockNum = kNewBlock) : 
  //   refId(aRefId), blockNumber(aBlockNum){
  //     entity->each([&](Attribute theAttr) -> bool {
  //       data[theAttr.getName()] = aValues.front();
  //       aValues.pop_back();
  //       return true;
  //     });
  // }

  Row::Row(uint32_t aRefId, uint32_t aBlockNum) : 
    refId(aRefId), blockNumber(aBlockNum){}

  Row::Row(const Row &aCopy) {
    *this = aCopy;
  }
  Row& Row::operator=(const Row &aCopy) {
    data = aCopy.data;
    blockNumber = aCopy.blockNumber;
    refId = aCopy.refId;
    return *this;
  }
  Row::~Row(){}

  //Use------------------------------------------------------------------------

  //Add one field of key value pairs 
  Row& Row::setField(const std::string &aString, const Value &aValue) {
    data[aString] = aValue;
    return *this;
  }

  //Store the row into a given block
  void Row::initBlock(Block& aBlock) {
    aBlock.header.refId = refId;
    std::stringstream theBlockStream;
    encode(theBlockStream);
    theBlockStream.read(aBlock.payload, kPayloadSize);
  }

  //Select the given field to keep, discard the other fields
  Row& Row::selectFields(StringList &aKeyList) {
    for (auto it = data.cbegin(); it != data.cend(); ){
      if (std::find(aKeyList.begin(), aKeyList.end(), it->first) == aKeyList.end())
      {
        data.erase(it++);   
      }
      else{++it;}
    }

    return *this;
  }

  Value Row::getValuebyKey(const std::string &aKey) const{
    return data.at(aKey);
  }
  //------------------------------------------------------------------
  //Storable API

  StatusResult Row::encode(std::iostream &aWriter) {
    aWriter << "Row" << ' ' ;
    aWriter << "RefId" << ' ' << refId << ' ';
    aWriter << "blockNum" << ' ' << blockNumber << ' ';
    aWriter << "valueSize" <<  ' ' << data.size() << ' ';
    for(auto& field : data) {
      aWriter << field.first << ' ' << field.second << ' ';
    }
    return StatusResult{};
  }

  StatusResult Row::decodeVerifier(std::iostream &aReader, const std::string& aName) {
    std::string theInfo;
    aReader >> theInfo;
    if (theInfo != aName) {
      return StatusResult{readError};
    }
    else {return StatusResult{};}
  }


  StatusResult Row::decode(std::iostream &aReader) {
    StatusResult theResult;
    std::string theKey;
    int theSize{0};
    Value theValue;
    theResult = decodeVerifier(aReader, "Row");
    theResult = decodeVerifier(aReader, "RefId");
    aReader >> refId;
    theResult = decodeVerifier(aReader, "blockNum");
    aReader >> blockNumber;
    theResult = decodeVerifier(aReader, "valueSize");
    aReader >> theSize;
    for(int i=0; i<theSize; i++) {
      aReader >> theKey >> theValue;
      setField(theKey, theValue);
    }
    return theResult;
  }
  


}
