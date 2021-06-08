#include "SQLStatement.hpp"
#include "Tokenizer.hpp"
#include "SQLProcessor.hpp"



namespace ECE141 {

  SQLStatement::SQLStatement(SQLProcessor* aProc, Keywords aStatementType) : 
      Statement(aStatementType), proc(aProc){}

  SQLStatement::SQLStatement(const SQLStatement &aCopy) : 
  Statement(aCopy), proc(aCopy.proc), name(aCopy.name){}

  SQLStatement::~SQLStatement() {}

  StatusResult SQLStatement::parse(Tokenizer &aTokenizer) {
    return StatusResult{};
  }

  StatusResult SQLStatement::hasProperLen(Tokenizer &aTokenizer, size_t aMin, size_t aMax) {
    if(aTokenizer.remaining() < aMin) {
      return StatusResult{syntaxError};
    }
    else if(aMax != -1 && aTokenizer.remaining() > aMax) {
      return StatusResult{syntaxError};
    }
    return StatusResult{};
  }

  //---------------------------------------------------------
  CreateSQLStatement::CreateSQLStatement(SQLProcessor* aProc, Keywords aStatementType) :
      SQLStatement(aProc, aStatementType) {}

  CreateSQLStatement::CreateSQLStatement(const CreateSQLStatement &aCopy) :
    SQLStatement(aCopy) {}
  
  CreateSQLStatement::~CreateSQLStatement() {}

  Attribute* CreateSQLStatement::parseAttribute(Tokenizer &aTokenizer) {
    static std::map<Keywords,DataTypes> theDt{
      {Keywords::integer_kw, DataTypes::int_type},
      {Keywords::boolean_kw, DataTypes::bool_type},
      {Keywords::varchar_kw, DataTypes::varchar_type},
      {Keywords::float_kw, DataTypes::float_type},
      {Keywords::datetime_kw, DataTypes::datetime_type}
    };
    TokenSequence theAttrSeq(aTokenizer);
    std::string theName;
    if(theAttrSeq.getIdentifier(theName)) {
      Keywords theKw = aTokenizer.current().keyword;
      if(theDt.count(theKw)) {
        Attribute* theAttr = new Attribute{theName, theDt[theKw]}; 
        //Record the max length if the type is varchar
        if(theKw == Keywords::varchar_kw) {
          int maxLen = std::stoi(aTokenizer.peek(2).data);
          theAttr->setMaxLen(maxLen);
          aTokenizer.next(3);
        }
        aTokenizer.next();
        Token& theToken = aTokenizer.current();
        //Add verification of end-of-command
        while(theToken.type!=TokenType::punctuation) {
          if(theAttrSeq.has(Keywords::not_kw).then(Keywords::null_kw)) {
            theAttr->setNullable(false);
            aTokenizer.next(2);
          }
          else if(theAttrSeq.has(Keywords::auto_increment_kw)) {
            theAttr->setAutoInc();
            aTokenizer.next();
          }
          else if(theAttrSeq.has(Keywords::primary_kw).then(Keywords::key_kw)) {
            theAttr->setPrimary();
            aTokenizer.next(2);
          }

          else if(theAttrSeq.has(Keywords::default_kw)) {
            Value theValue = aTokenizer.peek().data;
            theAttr->setDefault(theValue);
            aTokenizer.next(2);
          }
            theToken = aTokenizer.current();
        }

        return theAttr;
      }
    }

    return nullptr;
  }

  StatusResult CreateSQLStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,9)) {return StatusResult{syntaxError};}
    TokenSequence theSeq(aTokenizer);
    if(theSeq.has(Keywords::create_kw).then(Keywords::table_kw).thenId(name).thenPunct('(')) {
      aTokenizer.next(4);
      bool hasComma = true;
      while(hasComma) {
        if(Attribute* theAttr = parseAttribute(aTokenizer)) {
          attrList.push_back(*theAttr);
          delete theAttr;
        }
          hasComma = aTokenizer.skipIf(',');
      }
      if(aTokenizer.skipIf(')')) {return StatusResult{};}
    }
    return StatusResult{syntaxError};
  }


  StatusResult CreateSQLStatement::dispatch() {
    return proc->createTable(name, attrList);
  }

  bool CreateSQLStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::create_kw).then(Keywords::table_kw);
  }
  
  //--------------------------------------------------------------
  DropSQLStatement::DropSQLStatement(SQLProcessor* aProc, Keywords aStatementType) :
      SQLStatement(aProc, aStatementType) {}

  DropSQLStatement::DropSQLStatement(const DropSQLStatement &aCopy) :
    SQLStatement(aCopy) {}
  
  DropSQLStatement::~DropSQLStatement() {}

  StatusResult DropSQLStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,3)) {return StatusResult{syntaxError};}
    TokenSequence theSeq(aTokenizer);
    if(theSeq.has(Keywords::drop_kw).then(Keywords::table_kw)) {
      aTokenizer.next(2);
      return theSeq.getIdentifier(name);
    }
    return StatusResult{syntaxError};
  }

  StatusResult DropSQLStatement::dispatch() {
    
    return proc->dropTable(name);
  }

  bool DropSQLStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::drop_kw).then(Keywords::table_kw);
  }

  //--------------------------------------------------------------
  DescribeSQLStatement::DescribeSQLStatement(SQLProcessor* aProc, 
    Keywords aStatementType) : SQLStatement(aProc, aStatementType) {}

  DescribeSQLStatement::DescribeSQLStatement(const DescribeSQLStatement &aCopy) :
    SQLStatement(aCopy) {}
  
  DescribeSQLStatement::~DescribeSQLStatement() {}

  StatusResult DescribeSQLStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,2)) {return StatusResult{syntaxError};}
    TokenSequence theSeq(aTokenizer);
    if(theSeq.has(Keywords::describe_kw)) {
      aTokenizer.next();
      return theSeq.getIdentifier(name);
    }
    return StatusResult{syntaxError};
  }

  StatusResult DescribeSQLStatement::dispatch() {
    return proc->describe(name);
  }

  bool DescribeSQLStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::describe_kw);
  }

  //--------------------------------------------------------------
  ShowSQLStatement::ShowSQLStatement(SQLProcessor* aProc,Keywords aStatementType) :
      SQLStatement(aProc, aStatementType) {}

  ShowSQLStatement::ShowSQLStatement(const ShowSQLStatement &aCopy) :
    SQLStatement(aCopy) {}
  
  ShowSQLStatement::~ShowSQLStatement() {}

  StatusResult ShowSQLStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,2)) {return StatusResult{syntaxError};}
    TokenSequence theSeq(aTokenizer);
    if(theSeq.has(Keywords::show_kw).then(Keywords::tables_kw)) {
      aTokenizer.next(2);
      return StatusResult{noError};
    }
    return StatusResult{syntaxError};
  }

  StatusResult ShowSQLStatement::dispatch() {
    return proc->showTables();
  }

  bool ShowSQLStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::show_kw).then(Keywords::tables_kw);
  }

//--------------------------------------------------------------
  InsertStatement::InsertStatement(SQLProcessor* aProc,Keywords aStatementType) :
      SQLStatement(aProc, aStatementType) {}

  InsertStatement::InsertStatement(const InsertStatement &aCopy) :
    SQLStatement(aCopy) {}
  
  InsertStatement::~InsertStatement() {}

  //Needs update to apply to general type (row values)
  template<typename T>
  StatusResult InsertStatement::parseRowField(Tokenizer &aTokenizer, std::vector<T>& aList) {
    TokenSequence theFieldSeq{aTokenizer};
    while(aTokenizer.current().data != ")") {
      std::string theId;
      if(theFieldSeq.getIdentifier(theId)) {
        aList.push_back(theId);
      }
      else if(aTokenizer.current().type == TokenType::number) {
        Token& theToken = aTokenizer.current();
        aList.push_back(theToken.data);
        aTokenizer.next();
      }
      else if(aTokenizer.current().data == ",") {
        aTokenizer.next();
      }
      else {return StatusResult{syntaxError};}
    }
    aTokenizer.next();
    return StatusResult{};
  }

  //Maybe add another function to parse collection of row values
  StatusResult InsertStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    if(!hasProperLen(aTokenizer,10)) {return StatusResult{syntaxError};}
    StatusResult theResult;
    TokenSequence theSeq(aTokenizer);
    if(theSeq.has(Keywords::insert_kw).then(Keywords::into_kw).
        thenId(name).thenPunct('(')) {
      aTokenizer.next(4);
      parseRowField(aTokenizer, keys);
      if(theSeq.has(Keywords::values_kw)) {
        aTokenizer.next();
        bool hasComma = true;
        while(hasComma & theResult) {
          if(aTokenizer.skipIf('(')) {
            Values theValues;
            if(theResult = parseRowField(aTokenizer,theValues)) {
              valueCollections.push_back(theValues);
              hasComma = aTokenizer.skipIf(',');
            }
          }
          else {theResult = StatusResult{punctuationExpected}; }
        }
        return theResult;
      }
    }
    return StatusResult{syntaxError};
  }

  StatusResult InsertStatement::dispatch() {
    return proc->insertRows(name,keys,valueCollections);
  }

  bool InsertStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::insert_kw).then(Keywords::into_kw);
  }



  
  //---------------------------------------------------------
  ShowIndexStatement::ShowIndexStatement(SQLProcessor* aProc, Keywords aStatementType) :
    SQLStatement(aProc,aStatementType){}

  ShowIndexStatement::ShowIndexStatement(const ShowIndexStatement &aCopy) :
    SQLStatement(aCopy), indexName(aCopy.indexName){}
  
  ShowIndexStatement::~ShowIndexStatement() {}

  bool ShowIndexStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::show_kw).then(Keywords::index_kw);
  }

  StatusResult ShowIndexStatement::parse(Tokenizer &aTokenizer) {
    StatusResult theResult{syntaxError};
    if(!hasProperLen(aTokenizer,5)) {return theResult;}
    TokenSequence theSeq(aTokenizer);
    //Parse fields 
    if(theSeq.has(Keywords::show_kw).then(Keywords::index_kw).thenId(indexName).then(Keywords::from_kw)) {
      aTokenizer.next(4);
      if(theSeq.getIdentifier(name)) {
        theResult.error = noError;
      }
      else {theResult.error=unknownTable;}
    }
    return theResult;
  }

  StatusResult ShowIndexStatement::dispatch() {
    return proc->showIndex(name, indexName);
  }

  //---------------------------------------------------------
  ShowIndexesStatement::ShowIndexesStatement(SQLProcessor* aProc, Keywords aStatementType) :
    SQLStatement(aProc,aStatementType){}

  ShowIndexesStatement::ShowIndexesStatement(const ShowIndexesStatement &aCopy) :
    SQLStatement(aCopy){}
  
  ShowIndexesStatement::~ShowIndexesStatement() {}

  bool ShowIndexesStatement::recognizes(Tokenizer &aTokenizer) {
    TokenSequence theSeq(aTokenizer);
    return theSeq.has(Keywords::show_kw).then(Keywords::indices_kw);
  }

  StatusResult ShowIndexesStatement::parse(Tokenizer &aTokenizer) {
    StatusResult theResult{syntaxError};
    if(!hasProperLen(aTokenizer,2)) {return theResult;}
    TokenSequence theSeq(aTokenizer);
    if(theSeq.has(Keywords::show_kw).then(Keywords::indices_kw)) {
      aTokenizer.next(2);
      theResult.error = noError;
    }
    return theResult;
  }

  StatusResult ShowIndexesStatement::dispatch() {
    return proc->showIndexes();
  }





}
