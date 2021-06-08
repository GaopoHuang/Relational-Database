#ifndef TableDescriptionView_h
#define TableDescriptionView_h

#include "View.hpp"
#include "Timer.hpp"
#include <cmath>
#include <iomanip>
#include "Entity.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {

  class TableDescriptionView : public View {
  public:
    TableDescriptionView(Entity anEntity) : entity(anEntity) {}
   
    bool show(std::ostream &anOutput) override {
      static std::map<DataTypes, std::string> theTypes {
        {DataTypes::bool_type, "boolean"},
        {DataTypes::int_type, "integer"},
        {DataTypes::varchar_type, "varchar"},
        {DataTypes::float_type, "float"},
        {DataTypes::datetime_type, "date"},
      };
      Timer theTimer;
      std::string theBar = "+------------------+-------------+------+------+---------+-----------------------------+\n";
      size_t theCount=0;
      anOutput << theBar;
      anOutput << "| Field            | Type        | Null | Key  | Default | Extra                       |\n" << theBar;
      entity.each([&](Attribute theAttr) {
        anOutput << "| " << std::left << std::setw(17) << theAttr.name ;
        if(theAttr.type==DataTypes::varchar_type) {
          std::string theVarchar{"varchar("};
          theVarchar+=std::to_string(theAttr.constraints.maxLen);
          theVarchar+=")";
          anOutput << "| " << std::left << std::setw(12) << theVarchar;
        }
        else {
          anOutput << "| " << std::left << std::setw(12) << theTypes[theAttr.type];
        }
        anOutput << "| " << std::left << std::setw(5) 
                 << (theAttr.constraints.nullable ? "YES" : "NO");
        anOutput << "| " << std::left << std::setw(5) 
                 << (theAttr.constraints.primary ? "YES" : "");
        if(theAttr.constraints.hasDefault) {
          anOutput << "| " << std::left << std::setw(9) 
                 << theAttr.constraints.defaultVal;
        }
        else{
          anOutput << "| " << std::left << std::setw(8) << "NULL";
        }
        anOutput << "| " << std::left << std::setw(28) 
                 << (theAttr.constraints.autoInc ? "auto_increment primary key" : "");
        anOutput << "|\n";
        theCount++;
        return true;
      });
      anOutput << theBar;
      theTimer.stop();
      double elapsed = std::round(theTimer.elapsed() * 100) / 100;
      anOutput << theCount << " rows in set (" << elapsed << " sec(s))\n";
      return true;
    }

  protected: 
    Entity entity;    
  };

}

#endif /* TableDescriptionView_h */
