#ifndef DataProvider_hpp
#define DataProvider_hpp

#include "BasicTypes.hpp"
#include <string>

namespace ECE141 {
  struct TableField {
    TableField (const std::string & anAttrName):
                attribute(anAttrName) {};
    TableField (const std::string & anAttrName, const std::string &aTableName):
                table(aTableName), attribute(anAttrName) {};
    void setTableName(const std::string &aTableName) {table = aTableName;}
    std::string table;
    std::string attribute;
  };


  class DataProvider {
  public: 
    virtual Value getValue(const TableField& aField)=0;
  };

}








#endif /*DataProvider_hpp*/