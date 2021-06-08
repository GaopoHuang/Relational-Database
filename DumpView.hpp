#ifndef DumpView_h
#define DumpView_h

#include "View.hpp"
#include "Storage.hpp"
#include "Database.hpp"
#include "Timer.hpp"
#include <iomanip>
#include <cmath>

namespace ECE141 {

  class DumpView : public View {
  public:
    DumpView(Database* aDB, const Timer& aTimer) : 
        storage(aDB->getStorage()), timer(aTimer) {};
   
    bool show(std::ostream &anOutput) override {
      std::string theBar = "+----------------+-------------+-------+-----------+\n";
      size_t theCount=0;
      anOutput << theBar;
      anOutput << "| Type           | refID       | ID    | Next      |\n" << theBar;
      //Use storage each visitor to dump all blocks
      storage.each([&](const Block& aBlock, uint32_t blockNum) {
        anOutput << "| " << std::left << std::setw(15) << aBlock.header.type ;
        anOutput << "| " << std::left << std::setw(12) << aBlock.header.refId ;
        anOutput << "| " << std::left << std::setw(6) << aBlock.header.id ;
        anOutput << "| " << std::left << std::setw(10) << aBlock.header.next << "|\n";
        anOutput << theBar;
        theCount++;
        return true;
      });
      timer.stop();
      // double elapsed = std::round(timer.elapsed() * 100) / 100; //round to 2 decimal digit
      double elapsed = timer.elapsed();
      anOutput << std::fixed;
      anOutput << theCount << " rows in set (" << std::setprecision(3) << elapsed << " sec(s))\n";
      return true;
    }
    
    Storage storage;  
    Timer timer;
  };

}

#endif /* DumpView_h */
