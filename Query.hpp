//
//  Query.hpp
//  Assignment5
//
//  Created by rick gessner on 4/26/21.
//

#ifndef Query_hpp
#define Query_hpp

#include <stdio.h>
#include <string>
#include "Attribute.hpp"
#include "Row.hpp"
#include "Entity.hpp"
#include "Tokenizer.hpp"
#include "Filters.hpp"

namespace ECE141 {
  struct Order {
    Order(std::string aField, bool aFlag=false) : orderBy(aField), ascending(aFlag) {}
    Order() : ascending(false) {}
    ~Order() {}
    Order& operator=(const Order &aCopy) {
      orderBy = aCopy.orderBy;
      ascending = aCopy.ascending;
      return *this;
    }
    std::string orderBy;
    bool ascending;
  };

  struct OrderCompare {
    std::string key;
    OrderCompare(const std::string& aKey) : key(aKey) {}
    ~OrderCompare() {}

    bool operator() (const URowPtr& i, const URowPtr& j) {
      return i->getValuebyKey(key) < j->getValuebyKey(key);
    }
  };


  class Query  {
  public:
    
    Query(Entity* anEntityPtr=nullptr, bool anAll=true, int anOffset=0, int aLimit=0);
    Query(const Query& aCopy);
    virtual ~Query();

    bool selectAll() const;
    
    Query& setSelectAll(bool aState);
    Query& setSelect(const StringList &aFields);
          
    Query& setFrom(Entity *anEntity);

    Query& setOffset(int anOffset);
    Query& setLimit(int aLimit);

    Query& setOrderBy(const std::string &aField, bool ascending=false);

    Query& setFilters(Filters& aFilters);

    StringList getFields() const {return fields;}
    Entity*    getFrom() const {return _from;}
    size_t     getLimit() const {return limit;}
    size_t     getOffset() const {return offset;}
    Filters&    getFilters()  {return filters;}
    StringOpt   getOrder() const {return ordered ? StringOpt{orderField.orderBy} :std::nullopt;}
    //how will you handle where clause?
    //how will you handle group by?

    virtual Query& run(Rows &aRows);
    Query& sort(Rows& aRows);
    
  protected:
    Entity*     _from;
    bool        all;
    bool        ordered;
    int         offset;
    int         limit;
    StringList  fields;
    Order       orderField;
    Filters     filters;
  };

  class UpdateQuery : public Query {
  public:
    UpdateQuery(Entity* anEntity=nullptr, bool anAll=true, int anOffset=0, int aLimit=0);
    UpdateQuery(const UpdateQuery& aCopy);
    ~UpdateQuery() override;

    UpdateQuery& run(Rows &aRows) override;
    UpdateQuery& setUpdate(const KeyValue aKv);
    UpdateQuery& setUpdate(const std::string aKey, Value& aValue);
    UpdateQuery& setAffected(int anAffected);

    int       getAffected()     const {return affected;}
    KeyValues getUpdateFields() const {return update;}
  protected:
    KeyValues update;
    int       affected;
  };

  class DeleteQuery : public Query {
  public:
    DeleteQuery(Entity* anEntity=nullptr, bool anAll=true, int anOffset=0, int aLimit=0);
    DeleteQuery(const DeleteQuery& aCopy);
    ~DeleteQuery() override;

    DeleteQuery& run(Rows &aRows) override;
    DeleteQuery& setUpdate(const KeyValue aKv);
    DeleteQuery& setUpdate(const std::string aKey, Value aValue);
    DeleteQuery& setAffected(int anAffected);

    int       getAffected()     const {return affected;}
  protected:
    int       affected;
  };

}
#endif /* Query_hpp */
