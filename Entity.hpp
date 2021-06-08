//
//  Entity.hpp
//
//  Created by rick gessner on 4/03/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//


#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <functional>
#include <map>

#include "Attribute.hpp"
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "Storage.hpp"
#include "Index.hpp"


namespace ECE141 {
  
  using AttributeList = std::vector<Attribute>;
  using AttributeVisitor = std::function<bool(Attribute)>;
  using IndexesVisitor = std::function<bool(const std::string&, uint32_t)>;
  //------------------------------------------------

  class Entity : public Storable  {
  public:

    static uint32_t       hashString(const char *str);
    static uint32_t       hashString(const std::string &aStr);

      //declare ocf methods...
      Entity(std::string aName, uint32_t aBlockNum = kNewBlock);
      Entity(std::string aName, AttributeList aList, uint32_t aBlockNum=kNewBlock);
      Entity(const Entity &aCopy);
      Entity& operator=(const Entity &aCopy);
      virtual ~Entity();

      //Use
      std::string  getName() const {return name;}
      uint32_t getBlockNum() const {return blockNum;}
      uint32_t getIndexBlockNum(const std::string& aName) const {
        return indexes.count(aName) ? indexes.at(aName) : kNewBlock;
      }
      std::map<std::string, uint32_t> getIndexes() const {return indexes;}
      Entity&  setIndex(const std::string& aName, uint32_t aNum) {
        indexes[aName] = aNum;
        return *this;
      }

      Attribute* getAttribute(const std::string &aName);
      StringList getAttributeNameList();
      StringOpt findPrimaryKey();
      StatusResult incPrimaryKey();
      bool         attrExist(const std::string &aName);
      

      bool each(const AttributeVisitor &aVisitor);
      bool eachIndex(const IndexesVisitor &aVisitor);
      
      //this is the storable interface...
    StatusResult          encode(std::iostream &aWriter) override;
    StatusResult          decode(std::iostream &aReader) override;

    uint32_t        refID;       
    uint32_t        auto_increment; //stores current auto increment value
    
  protected:
    StatusResult        decodeVerifier(std::iostream &aReader, const std::string& aName);

    AttributeList                          attributes;
    std::string                            name;
    uint32_t                               blockNum;
    std::map<std::string, uint32_t>        indexes;
  };
  
}
#endif /* Entity_hpp */
