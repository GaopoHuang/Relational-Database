//
//  Row.hpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Storage.hpp"
#include "Entity.hpp"
#include "BasicTypes.hpp"


class Database;

namespace ECE141 {

  class Row : public Storable {
  public:

    //OCF methods...
    // Row(Values aValues, uint32_t aRefId, uint32_t aBlockNum = kNewBlock);
    Row(uint32_t aRefId=0, uint32_t aBlockNum = kNewBlock);
    Row(const Row &aCopy);
    Row& operator=(const Row &aCopy);
    virtual ~Row();

    Row& setField(const std::string &aString, const Value &aValue);
    uint32_t getBlockNum() const {return blockNumber;}
    KeyValues& getData() {return data;}
    uint32_t getEntityId() {return refId;}
    Value getValuebyKey(const std::string &aKey) const;

    Row& selectFields(StringList &aKeyList);

    //   //storable api 
    StatusResult        encode(std::iostream &aWriter) override;
    StatusResult        decode(std::iostream &aReader) override;

    void                initBlock(Block &aBlock) ;


  protected:
    StatusResult        decodeVerifier(std::iostream &aReader, const std::string& aName);
    uint32_t            refId; 
    KeyValues           data;
    uint32_t            blockNumber;

    //do you need any other data members?
    
  };

  //-------------------------------------------

  using Rows = std::vector<std::unique_ptr<Row> >;
  using URowPtr = std::unique_ptr<Row>;

}

#endif /* Row_hpp */
