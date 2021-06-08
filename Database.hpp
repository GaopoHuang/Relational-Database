//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <fstream> 
#include "Storage.hpp"
#include "BasicTypes.hpp"
#include "Entity.hpp"
#include "Query.hpp"
#include "Index.hpp"
#include "Join.hpp"

namespace ECE141 {

  class Database : public Storable {
  public:
    
            Database(const std::string aPath, CreateDB);
            Database(const std::string aPath, OpenDB);
    virtual ~Database();

    //storable...
    StatusResult    encode(std::iostream &anOutput) override;
    StatusResult    decode(std::iostream &anInput) override;

    
    std::string getName() const;
    Storage& getStorage();

    StatusResult modifyEntity(Entity& anEntity, uint32_t aBlockNum);
    StatusResult addEntity(const std::string & aName, 
                           AttributeList anAttrList);
    StatusResult removeEntity(const std::string & aName, uint32_t& aNumBlock);
    StringList getEntityList() const;
    uint32_t findEntity(const std::string & aName) const;
    Entity* getEntity(const std::string &aName);
    StringOpt getEntityNameById(size_t aHashId);

    StatusResult addRowIntoIndex(Index& anIndex, IndexKey &aKey, uint32_t aValue);
    StatusResult modifyIndex(Index& anIndex);
    StatusResult addIndex(const std::string& anEntityName, const std::string& anIndexName,
                          uint32_t aBlockNum, IndexType aType=IndexType::intKey);
    std::map<std::string, uint32_t> getIndexesfromEntity(const std::string& aName);
    std::map<std::string, uint32_t> getIndexesfromEntity(Entity* anEntity);
    Index getIndexbyBlockNum(uint32_t aBlockNum);
    // Index Database::getIndexbyIndexName(const std::string& aName);
    Index getPrimaryIndex(const std::string& anEntityName);

    StatusResult selectRows(const Query& aQuery, Rows& aRows);
    StatusResult updateRows(UpdateQuery& anUpdateQuery);
    StatusResult deleteRows(DeleteQuery& aDeleteQuery);
    StatusResult joinRows(const Query& aQuery, Rows& aRows, JoinList& aJoins);

    StatusResult saveMeta();

  protected:

    std::string     name;
    Storage         storage;
    bool            changed;
    std::fstream    stream; //stream storage uses for IO
    NamedIndex      meta; //Key value pair that stores the entity index for quick access
  };

}
#endif /* Database_hpp */
