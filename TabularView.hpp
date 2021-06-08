
//
//  RecordsView.hpp
//
//  Created by rick gessner on 4/26/20.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#ifndef TabularView_h
#define TabularView_h

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <cmath>
#include <vector>
#include <string>
#include "View.hpp"
#include "Row.hpp"

//IGNORE THIS CLASS if you already have a class that does this...

namespace ECE141 {

  // USE: general tabular view (with columns)
  class TabularView : public View {
  public:
    
    //STUDENT: Add OCF methods...
    TabularView(const Rows& aRows, double aTime=0.00) : rows(aRows), time(aTime), n_col(0) {}
    ~TabularView() {}  


    bool initCols() {
      if(rows.size()>0) {
        KeyValues theData = rows[0]->getData();
        n_col = theData.size();
        for(auto& pair: theData) {
          fields.push_back(pair.first);
        }
        return true;
      }
      return false;
    }

    template<typename T>
    void showFields(std::ostream &anOutput, std::vector<T> aFields) {
      for(auto& theField: aFields) {
        anOutput << "| " << std::left << std::setw(20) << theField;
      }
      anOutput <<"|\n";
    }

    void showFields(std::ostream &anOutput, std::vector<Value> aFields) {
      for(auto& theField: aFields) {
        std::visit([&](auto &theField) {
          anOutput << "| " << std::left << std::setw(20) << theField;
        }, theField); 
      }
      anOutput <<"|\n";
    }

    bool show(std::ostream &anOutput) override{
      if(initCols()) {
        //Initialize the bar as delimiliter
        std::string theBar{"+"};
        for(int i=0; i<n_col; i++) {
          theBar+="---------------------+";
        }
        theBar+="\n";
        //Output the titles
        anOutput << theBar;
        showFields(anOutput, fields);
        anOutput << theBar;
        //Output each row
        for(auto& theRow: rows) {
          Values theValues;
          for(auto& pair: theRow->getData()){
            theValues.push_back(pair.second);
          }
          showFields(anOutput, theValues);
        }
        anOutput << theBar;
      }

      anOutput << std::fixed;
      anOutput << rows.size() << " rows in set (" << std::setprecision(3) << time << " sec(s))\n";
      return true;
    }
    


  protected:
    //what data members do you require?
    StringList fields;
    const Rows& rows;
    double time;
    int n_col;
  };

}

#endif /* TabularView_h */
