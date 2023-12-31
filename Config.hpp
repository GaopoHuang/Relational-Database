//
//  Config.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Config_h
#define Config_h
#include <sstream>
#include <filesystem>

struct Config {

  static const char* getDBExtension() {return ".db";}

  static const char* getStoragePath() {
      
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
      
      //STUDENT: If you're on windows, return a path to folder on your machine...
      return "C:\\Users\\Gaopo\\Desktop\\UCSD Assorted\\21SP\\ECE141B\\Assignments\\sp21-assignment9-zongcheng1\\db_files";
    
    #elif __APPLE__ || defined __linux__ || defined __unix__
      
      return "/tmp";  //MAC, UNIX, LINUX here...
    
    #endif
  }
  
  static std::string getDBPath(const std::string &aDBName) {
    std::ostringstream theStream;
    theStream << Config::getStoragePath() << "/" << aDBName << ".db";
    return theStream.str();
  }
  
};

#endif /* Config_h */
