#ifndef IndexView_h
#define IndexView_h

#include "View.hpp"
#include "Timer.hpp"
#include <cmath>
#include <iomanip>
#include "Index.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {

  class IndexView : public View {
  public:
    IndexView(Index& anIndex, const Timer& aTimer) : 
        index(anIndex), timer(aTimer) {}
   
    bool show(std::ostream &anOutput) override {
      std::string theBar = "+--------------------------+--------------------------+\n";
      size_t theCount=0;
      anOutput << theBar;
      anOutput << "| key                      | block#                   |\n" << theBar;
      index.eachKV([&](const IndexKey& aKey, uint32_t aBlockNum) {
        std::visit([&](const auto& aKey) {
          anOutput << "| " << std::left << std::setw(24) << aKey 
                  << " | " << std::left << std::setw(24) << aBlockNum << " |\n";
          anOutput << theBar;
          theCount++;
        }, aKey);  
        return true;
      });

      timer.stop();
      double elapsed = timer.elapsed();
      anOutput << std::fixed;
      anOutput << theCount << " rows in set (" << std::setprecision(3) << elapsed << " sec(s))\n";
      return true;
    }



    Index&    index;
    Timer     timer;
  };

}

#endif /* IndexView_h */

