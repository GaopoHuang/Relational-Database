//
//  Attribute.hpp
//
//  Created by rick gessner on 4/02/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include <iostream>
#include "Attribute.hpp"

namespace ECE141 {

  //OCF...
  Attribute::Attribute(std::string aName, DataTypes aType, 
     int aMaxLen, bool aNullable) : 
    name(aName), type(aType) {
      constraints = AttrConstraint(aMaxLen, aNullable);
    }
  Attribute::Attribute(const Attribute &aCopy) {
    *this = aCopy;
  }

  Attribute& Attribute::operator=(const Attribute &aCopy) {
    name = aCopy.name;
    constraints = aCopy.constraints;
    type = aCopy.type;
    return *this;
  }

  Attribute::~Attribute(){}

  //Use----------------------------------------------
  Attribute& Attribute::setMaxLen(int aMaxLen) {
    if(type==DataTypes::varchar_type) {
      constraints.maxLen = aMaxLen;
    }
    return *this;
  }

  Attribute& Attribute::setNullable(bool aNullable) {
    constraints.nullable = aNullable;
    return *this;
  }
  Attribute& Attribute::setPrimary(bool aPrimary) {
    constraints.primary = aPrimary;
    return *this;
  }
  Attribute& Attribute::setAutoInc(bool anAutoInc) {
    constraints.autoInc = anAutoInc;
    return *this;
  }

  Attribute& Attribute::setDefault(Value aDefaultVal) {
    constraints.hasDefault = true;
    constraints.defaultVal = aDefaultVal;
    return *this;
  }

  std::string Attribute::getName() const {
    return name;
  }

  bool Attribute::isPrimary() const {
    return constraints.primary;
  }
  
  void Attribute::descriptionView(std::ostream &out) {

  }
  //--------------------------------------------------------
  //Storable Interface
  StatusResult Attribute::encode(std::iostream &aWriter) {
    aWriter << "Attribute" << ' ' << name << ' ' << (char)type << ' ';
    if(type==DataTypes::varchar_type) {
      aWriter << constraints.maxLen << ' ';
    }
    aWriter << "n" << ' ' << constraints.nullable << ' ' 
            << "p" << ' ' << constraints.primary << ' ' 
            << "a" << ' ' <<  constraints.autoInc << ' ' 
            << "d" << ' ' <<  constraints.hasDefault << ' ';
    if(constraints.hasDefault) {
      aWriter << constraints.defaultVal << ' ';
    }

    return StatusResult{noError};
  }

  StatusResult Attribute::decode(std::iostream &aReader) {
    char theType{'N'};
    std::string theClass;
    aReader >> theClass;
    if(theClass != "Attribute") {
      return StatusResult{readError};
    }
    aReader >> name >> theType;
    type = static_cast<DataTypes>(theType);
    if(type==DataTypes::varchar_type) {
      aReader >> constraints.maxLen;
    }
    aReader >> theType >> constraints.nullable 
            >> theType >> constraints.primary 
            >> theType >> constraints.autoInc
            >> theType >> constraints.hasDefault;
    if(constraints.hasDefault) {
      aReader >> constraints.defaultVal;
    }
    return StatusResult{noError};
  }


}