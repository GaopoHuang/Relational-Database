#ifndef EntityView_h
#define EntityView_h

#include "View.hpp"
#include "Timer.hpp"
#include <cmath>
#include <iomanip>
#include "Database.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {

  class EntityView : public View {
  public:
    EntityView(Database* aDB, const Timer& aTimer, bool anIndex = false) : 
        db(aDB), showIndexes(anIndex), timer(aTimer) {}
   
    bool show(std::ostream &anOutput) override {
      size_t nCol = showIndexes ? 2 : 1;
      std::string theBar = "+" ;
      for(size_t i=0; i<nCol; i++) {
        theBar+= "--------------------------+";
      }
      theBar+="\n";
      size_t theCount=0;
      anOutput << theBar;
      if(showIndexes) {
        anOutput << "| table                    | field(s)                 |\n" << theBar;
      }
      else {
        anOutput << "| Tables in " <<std::left << std::setw(14) << db->getName() << " |\n" << theBar;
      }
      StringList theTableList = db->getEntityList();
      for(auto& tableName : theTableList) {
        if(showIndexes) {
          std::map<std::string, uint32_t> theIndexes = db->getIndexesfromEntity(tableName);
          for(auto& theIndex: theIndexes) {
            anOutput << "| " << std::left << std::setw(24) <<tableName 
                     << " | " << std::left << std::setw(24) << theIndex.first << " |\n";
            theCount++;
            anOutput << theBar;
          }
        }
        else{
          anOutput << "| " << std::left << std::setw(24) << tableName << " |\n";
          theCount++;
          anOutput << theBar;
        }
      }

      timer.stop();
      double elapsed = timer.elapsed();
      anOutput << std::fixed;
      anOutput << theCount << " rows in set (" << std::setprecision(3) << elapsed << " sec(s))\n";
      return true;
    }



    Database* db;    
    bool showIndexes;
    Timer timer;
  };

}

#endif /* EntityView_h */
