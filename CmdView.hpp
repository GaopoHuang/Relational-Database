#ifndef CmdView_h
#define CmdView_h

#include "View.hpp"
#include <cmath>
#include <iomanip>

namespace ECE141 {

  class CmdView : public View {
  public:
    CmdView(uint32_t aCount = 0, double aTime = 0.00) : count(aCount), time(aTime) {}
   
    bool show(std::ostream &anOutput) override {

        anOutput << "Query OK, " << count << " ";
        std::string nRow = (count==1 ? "row" : "rows");
        anOutput << std::fixed;
        anOutput << nRow << " affected (" << std::setprecision(3) << time << " sec(s))\n";
        return true;
    }
    
    uint32_t count;
    double time;
    
  };

}

#endif /* CmdView_h */
