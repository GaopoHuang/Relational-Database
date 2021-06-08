//
//  Attribute.hpp
//
//  Created by rick gessner on 4/02/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include "Storage.hpp"
#include "keywords.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {
  //Stores all the attribute constraints with default initial state
  struct AttrConstraint {
    AttrConstraint(int aMaxLen=0, bool aNullable=true) :
      nullable(aNullable), maxLen(aMaxLen), primary(false), autoInc(false), hasDefault(false){}
    bool nullable;
    bool primary;
    bool autoInc;
    bool hasDefault;
    Value defaultVal;
    int maxLen;
  };

  class Attribute : public Storable {
  protected:
    std::string   name;
    DataTypes     type;
    AttrConstraint      constraints;
    
    //what other data members do attributes required?
    
  public:
    friend class TableDescriptionView;
    
    //STUDENT: declare ocf methods...
    Attribute(std::string aName, DataTypes type, int aMaxLen=0, bool aNullable=true);
    Attribute(const Attribute &aCopy);
    Attribute& operator=(const Attribute &aCopy);
    virtual ~Attribute();
    //What methods do you need to interact with Attributes?
    
    DataTypes getType() const {return type;}
    Attribute& setMaxLen(int aMaxLen);
    Attribute& setNullable(bool aNullable=true);
    Attribute& setPrimary(bool aPrimary=true);
    Attribute& setAutoInc(bool anAutoInc=true);
    Attribute& setDefault(Value aDefaultVal);
    std::string getName() const;
    bool isPrimary() const;

    void descriptionView(std::ostream &out);
    //Added so that the attribute is a storable...
    StatusResult        encode(std::iostream &aWriter);
    StatusResult        decode(std::iostream &aReader);
    
  };
  
}


#endif /* Attribute_hpp */
