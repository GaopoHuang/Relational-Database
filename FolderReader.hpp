#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <filesystem>
#include <fstream>
#include <functional>

namespace ECE141 {

  using FileVisitor = std::function<bool(const std::string&)>;

  class FolderReader {
  public:
    FolderReader(const char *aPath) : path(aPath) {}
    virtual ~FolderReader() {}

    virtual bool exists(const std::string& aFilename) {
      std::ifstream theStream(aFilename);
      return !theStream ? false : true;
    }

    virtual void each(const std::string& anExt, const FileVisitor &aVisitor) const {
      std::filesystem::path thePath(path);
      for(auto & theItem : std::filesystem::directory_iterator(path)) {
        if(!theItem.is_directory()) {
          std::filesystem::path temp(theItem.path());
          std::string theExt(temp.extension().string());
          if(0==anExt.size() || 0==anExt.compare(theExt)) {
            if(!aVisitor(temp.stem().string())) break;
          }
        }
      }
    }

    std::string path;
  };



}

#endif