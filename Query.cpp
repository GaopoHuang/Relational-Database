//
//  Query.cpp
//  Assignment5
//
//  Created by rick gessner on 4/26/21.
//

#include "Query.hpp"
#include <algorithm>
namespace ECE141 {

  Query::Query(Entity* anEntityPtr, bool anAll, int anOffset, int aLimit): 
      _from(anEntityPtr), all(anAll), offset(anOffset), limit(aLimit), ordered(false) {
  }

  Query::Query(const Query& aCopy) {
    //doesn't handle pointer from and filters
    _from = aCopy._from;
    all = aCopy.all;
    offset = aCopy.offset;
    limit = aCopy.limit;
    ordered = aCopy.ordered;
    filters = aCopy.filters;
  }

  Query::~Query() {
    if(_from) {
      delete _from;
      _from = nullptr;
    }
  }

  bool Query::selectAll() const {
    return all;
  }

  Query& Query::setSelectAll(bool aState) {
    all = aState;
    return *this;
  }

  Query& Query::setSelect(const StringList &aFields) {
    fields = aFields;
    all = false;
    return *this;
  }
  
  Query& Query::setFrom(Entity *anEntity) {
    _from = anEntity;
    return *this;
  }

  Query& Query::setOffset(int anOffset) {
    offset = anOffset;
    return *this;
  }

  Query& Query::setLimit(int aLimit) {
    limit = aLimit;
    return *this;
  }

  Query& Query::setOrderBy(const std::string &aField, bool ascending) {
    orderField.orderBy = aField;
    orderField.ascending = ascending;
    ordered = true;
    return *this;
  }

  Query& Query::setFilters(Filters& aFilters) {
    filters = aFilters;
    return *this;
  }


  Query& Query::run(Rows &aRows) {
    //Filter out the rows that doesn't satisfy the requirement

    //Failed approach
    // Filters theFilters = getFilters();
    // aRows.erase(std::remove_if(aRows.begin(), aRows.end(), 
    //             [&theFilters](URowPtr& theRow) {return !theFilters.matches(theRow->getData()); }), 
    //              aRows.end());

    Rows::iterator it = aRows.begin();
    while(it != aRows.end()) {

        if(!filters.matches(it->get()->getData())) {
            it = aRows.erase(it);
        }
        else ++it;
    }

    if(!all) {
      for (auto& row: aRows) {
        row->selectFields(fields);
      }
    }
    // if(ordered) {
    //   OrderCompare rowCompare(orderField.orderBy);
    //   std::sort(aRows.begin(),aRows.end(), rowCompare);
    // }
    if(limit!=0 && limit < aRows.size()) {
      aRows.erase(aRows.begin()+limit, aRows.end());
    }

    return sort(aRows);
  }

  Query& Query::sort(Rows &aRows) {
    if(ordered) {
      OrderCompare rowCompare(orderField.orderBy);
      std::sort(aRows.begin(),aRows.end(), rowCompare);
    }
    return *this;
  }
  //-----------------------------------------------------
 
  UpdateQuery::UpdateQuery(Entity* anEntity,  bool anAll, int anOffset, int aLimit) :
      Query(anEntity, anAll, anOffset, aLimit), affected(0) {}
  UpdateQuery::UpdateQuery(const UpdateQuery& aCopy) :
      Query(aCopy), update(aCopy.update) {}
  UpdateQuery::~UpdateQuery() {
      if(_from) {
        delete _from;
        _from = nullptr;
    }
  }

  UpdateQuery& UpdateQuery::setUpdate(const KeyValue aKv) {
    update.insert(aKv);
    return *this;
  }

  UpdateQuery& UpdateQuery::setUpdate(const std::string aKey, Value& aValue) {
    update[aKey] = aValue;
    return *this;
  }

  UpdateQuery& UpdateQuery::setAffected(int anAffected) {
    affected = anAffected;
    return *this;
  }

  UpdateQuery& UpdateQuery::run(Rows &aRows) {
    if(0!=filters.getCount()) {
      Rows::iterator it = aRows.begin();
      while(it != aRows.end()) {

        if(!filters.matches(it->get()->getData())) {
            it = aRows.erase(it);
        }
        else ++it;
      }
    }
    return setAffected(aRows.size()); 
  }


  //-----------------------------------------------------
 
  DeleteQuery::DeleteQuery(Entity* anEntity,  bool anAll, int anOffset, int aLimit) :
      Query(anEntity, anAll, anOffset, aLimit), affected(0) {}
  DeleteQuery::DeleteQuery(const DeleteQuery& aCopy) :
      Query(aCopy) {}
  DeleteQuery::~DeleteQuery() {
    if(_from) {
      delete _from;
      _from = nullptr;
    }
  }

  DeleteQuery& DeleteQuery::setAffected(int anAffected) {
    affected = anAffected;
    return *this;
  }

  DeleteQuery& DeleteQuery::run(Rows &aRows) {
    if(0!=filters.getCount()) {
      Rows::iterator it = aRows.begin();
      while(it != aRows.end()) {

        if(!filters.matches(it->get()->getData())) {
            it = aRows.erase(it);
        }
        else ++it;
      }
    }
    return setAffected(aRows.size()); 
  }





}
