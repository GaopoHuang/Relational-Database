//
//  TokenSequence.hpp
//  RGAssignment3
//
//  Created by rick gessner on 4/9/21.
//

#ifndef TokenSequence_h
#define TokenSequence_h

#include "Tokenizer.hpp"

namespace ECE141 {

  template<typename T, size_t aSize>
  bool isIn(T (&anArray)[aSize], const T &aValue) {
    return std::find(std::begin(anArray), std::end(anArray), aValue);
  }

  class TokenSequence {
  public:
    
    struct Term {
      Term(TokenSequence &aSeq, bool aTF=true, int anOffset=0)
        : seq(aSeq), matches(aTF), offset(anOffset) {}
     
      Term then(Keywords aKW) {
        if(matches) {
          Token &theToken=seq.tokenizer.peek(++offset);
          matches=theToken.keyword==aKW;
        }
        return *this;
      }
      
      Term thenId(std::string &aName) {
        if(matches) {
          Token &theNext=seq.tokenizer.peek(++offset);
          if((matches=TokenType::identifier==theNext.type)) {
            aName=theNext.data;
          }
        }
        return *this;
      }

      Term thenPunct(char aChar) {
        if(matches) {
          Token &theNext=seq.tokenizer.peek(++offset);
          if((matches=TokenType::punctuation==theNext.type)) {
            matches = (theNext.data[0] == aChar);
          }
        }
        return *this;
      }

      Term thenOp(char aChar) {
        if(matches) {
          Token &theNext=seq.tokenizer.peek(++offset);
          if((matches=TokenType::operators==theNext.type)) {
            matches = (theNext.data[0] == aChar);
          }
        }
        return *this;
      }
            
      operator TokenSequence&() {return seq;}
      operator bool() {return matches;}
      
      TokenSequence  &seq;
      bool            matches;
      int             offset;
    };
    
    TokenSequence(Tokenizer &aTokenizer)
      : tokenizer(aTokenizer) {}
    
    Term has(Keywords aKW) {
      Term theTerm(*this,true,-1);
      return theTerm.then(aKW);
    }
    
    
    template<typename T, size_t aSize>
    StatusResult getKeywords(T (&aKeywords)[aSize]) {
      StatusResult theResult{};
      
      for(auto &theKeyword : aKeywords) {
        if(theResult) {
          theResult.error=keywordExpected;
          Token &theToken=tokenizer.current();
          if(theToken.type==TokenType::keyword) {
            if(theToken.keyword==theKeyword) {
              tokenizer.next();
              theResult.error=noError;
            }
          }
        }
      }
      return theResult;
    }
    
    static bool isNumber(Token &aToken) {
      return TokenType::number==aToken.type;
    }

    StatusResult getToken(Token &aToken) {
      StatusResult theResult{};
      theResult.error=syntaxError;
      Token &theToken=tokenizer.current();
      if(theToken.type==aToken.type) {
        tokenizer.next();
        aToken=theToken;
        theResult.error=noError;
      }
      return theResult;
    }
    
    StatusResult getIdentifier(std::string &anIdentifier) {
      StatusResult theResult{};
      theResult.error=punctuationExpected;
      Token &theToken=tokenizer.current();
      if(TokenType::identifier==theToken.type) {
        anIdentifier=theToken.data;
        tokenizer.next();
        theResult.error=noError;
      }
      return theResult;
    }
    
    StatusResult getOperator(Operators anOp) {
      StatusResult theResult{};

      theResult.error=operatorExpected;
      Token &theToken=tokenizer.current();
      if(theToken.type==TokenType::operators) {
        if(theToken.op==anOp) {
          tokenizer.next();
          theResult.error=noError;
        }
      }
      return theResult;
    }
    
    StatusResult getPunctuation(char aPunct) {
      StatusResult theResult{};
      theResult.error=punctuationExpected;
      Token &theToken=tokenizer.current();
      if(theToken.type==TokenType::punctuation) {
        if(theToken.data[0]==aPunct) {
          tokenizer.next();
          theResult.error=noError;
        }
      }
      return theResult;
    }
    
    Tokenizer &tokenizer;
  };

}

#endif /* TokenSequence_h */


