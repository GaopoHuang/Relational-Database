//
//  Entity.cpp
//
//  Created by rick gessner on 4/03/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include <sstream>
#include "Entity.hpp"


namespace ECE141 {


  const int gMultiplier = 37;

  //hash given string to numeric quantity...
  uint32_t Entity::hashString(const char *str) {
    uint32_t h{0};
    unsigned char *p;
    for (p = (unsigned char*)str; *p != '\0'; p++)
      h = gMultiplier * h + *p;
    return h;
  }

  uint32_t Entity::hashString(const std::string &aStr) {
    return hashString(aStr.c_str());
  }
  
  //------------------------------------------------------
  //OCF...
  Entity::Entity(std::string aName, uint32_t aBlockNum) : 
    name(aName), blockNum(aBlockNum), auto_increment(0) {
      refID = hashString(aName);
    }
  Entity::Entity(std::string aName, AttributeList aList, uint32_t aBlockNum) : 
    name(aName), blockNum(aBlockNum), attributes(aList), auto_increment(0) {
      refID=hashString(aName);
    }

  Entity::Entity(const Entity &aCopy) {
    *this = aCopy;
  }
  Entity& Entity::operator=(const Entity &aCopy) {
    name = aCopy.name;
    blockNum = aCopy.blockNum;
    attributes = aCopy.attributes;
    refID = aCopy.refID;
    auto_increment = aCopy.auto_increment;
    return *this;
  }
  Entity::~Entity() {}


  //------------------------------------------------------
  //other entity methods...

  bool Entity::each(const AttributeVisitor &aVisitor) {
    //implement this as a visitor pattern for attributes
    for(auto& attr :attributes) {
      if(!aVisitor(attr)) {
        return true;
      }
    }
    return false;
  }

  bool Entity::eachIndex(const IndexesVisitor &aVisitor) {
    //implement this as a visitor pattern for attributes
    for(auto& pair :indexes) {
      if(!aVisitor(pair.first, pair.second)) {
        return true;
      }
    }
    return false;
  }

  StringList Entity::getAttributeNameList() {
    StringList theList;
    for(auto& attr: attributes) {
      theList.push_back(attr.getName());
    }
    return theList;
  }

  //Returns the attribute with primary key, or nullopt if doesn't exist
  StringOpt Entity::findPrimaryKey() {
    StringOpt thePrimaryKey;
    each([&thePrimaryKey](Attribute theAttr) {
      if(theAttr.isPrimary()) {
        thePrimaryKey = theAttr.getName();
        return false;
      }
      return true;
    });
    return thePrimaryKey; 
  }

  //Increment the primary key auto_increment value
  StatusResult Entity::incPrimaryKey() {
    StringOpt theKey = findPrimaryKey();
    if(theKey == std::nullopt) {
      return StatusResult{primaryKeyRequired};
    }
    auto_increment++;
    return StatusResult{};
  }

  bool Entity::attrExist(const std::string &aName) {
    for(auto& attr: attributes) {
      if(aName == attr.getName()) {
        return true;
      }
    }
    return false;
  }

  //Return a pointer to the attribute (should be deleted after), or nullptr if
  //no such attribute exists.
  Attribute* Entity::getAttribute(const std::string &aName) {
    if(attrExist(aName)) {
      for(auto& attr: attributes) {
        if(aName == attr.getName()) {
          return new Attribute(attr);
        }
      }
    }

    return nullptr;
  }
  
  //Storable Interface---------------------------------------------
  StatusResult Entity::encode(std::iostream &aWriter) {
    std::string theInfo;
    aWriter << "Entity" << ' ' << name << ' ';
    aWriter << "blockNum" << ' ' << blockNum << ' ';
    aWriter << "indexes" << ' ' << indexes.size() << ' ';
    eachIndex([&](const std::string& aName, uint32_t aBlockNum) -> bool {
      aWriter << aName << ' ' << aBlockNum << ' ';
      return true;
    });
    aWriter << "autoKey" <<  ' ' << auto_increment << ' ';
    aWriter << "attrSize" << ' ' << attributes.size() << ' ';
    //Encode the attribute list
    for(auto attr : attributes) {
      attr.encode(aWriter);
    }
    return StatusResult{noError};
  }

  StatusResult Entity::decodeVerifier(std::iostream &aReader, const std::string& aName) {
    std::string theInfo;
    aReader >> theInfo;
    if (theInfo != aName) {
      return StatusResult{readError};
    }
    else {return StatusResult{};}
  }


  StatusResult Entity::decode(std::iostream &aReader) {
    StatusResult theResult;
    int theSize{0}, theNum{0};
    std::string theIndexName;
    theResult = decodeVerifier(aReader, "Entity");
    aReader >> name;
    theResult = decodeVerifier(aReader, "blockNum");
    aReader >> blockNum;
    theResult = decodeVerifier(aReader, "indexes");
    aReader >> theSize;
    for(int i=0; i<theSize; i++) {
      aReader >> theIndexName >> theNum;
      indexes[theIndexName] = theNum;
    }
    theResult = decodeVerifier(aReader, "autoKey");
    aReader >> auto_increment;
    theResult = decodeVerifier(aReader, "attrSize");
    aReader >> theSize;
    //load the attribute list
    for(int i=0; i<theSize; i++) {
      Attribute theAttr{"",DataTypes::no_type};
      theAttr.decode(aReader);
      attributes.push_back(theAttr);
    }
    refID = hashString(name); //Set reference ID
    return theResult;
  }

}
