//
//  JSONParser2.cpp
//  Assignment4
//
//  Created by rick gessner on 2/16/20.
//

#include "JSONParser.hpp"
#include <cctype>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace ECE141 {

  using Actions = bool (JSONParser::*)(char aChar, JSONState &aState, JSONListener *);
  using parseCallback = bool(char aChar);

  const char kColon = ':';
  const char kComma = ',';
  const char kQuote = '"';
  const char kBraceOpen  = '{';
  const char kBraceClose = '}';
  const char kBracketOpen = '[';
  const char kBracketClose = ']';

  JSONParser::JSONParser(std::istream &anInput) : input(anInput) {
    anInput >> std::skipws;
  }
  
  //--------------- functions to handle various types of input ----------------

  bool isWhitespace(char aChar) {
    static const char* theWS = " \t\r\n\b\xff";
    return strchr(theWS,aChar);
  }
 
  bool isColon(char aChar) {return kColon==aChar ? true  : false;}
  bool isComma(char aChar) {return kComma==aChar ? true  : false;}
  bool isQuote(char aChar) {return kQuote==aChar ? true  : false;}
  bool isNonAlphanum(char aChar) {return !isalnum(aChar);}
  
  //----------------- simple parsing utilities ----------------------

  std::string readUntil(std::istream &anInput, parseCallback aCallback, bool addTerminal) {
    std::string theResult;
    while (!anInput.eof() && !(*aCallback)(anInput.peek())) {
      theResult+=anInput.get();
    }
    if(addTerminal) {
      if(!anInput.eof()) {
        theResult+=anInput.get();
      }
    }
    return theResult;
  }
  
  bool skipWhile(std::istream &anInput, parseCallback aCallback) {
    while (!anInput.eof() && (*aCallback)(anInput.peek())) {
      anInput.get();
    }
    return true;
  }

  bool skipIfChar(std::istream &anInput, char aChar) {
    return (aChar==anInput.peek()) ? aChar==anInput.get() : false;
  }

//--------------------------------------------------------------------

  bool JSONParser::handleOpenContainer(Element aType, JSONListener *aListener) {
    JSONState theState(tempKey, aType);
    tempKey="";
    states.push(theState);
    return (!aListener) ? true : aListener->openContainer(theState.key, aType);
  }

  bool JSONParser::handleCloseContainer(Element aType, JSONListener *aListener) {
    std::string temp(states.top().key);
    if(states.size()) states.pop();
    return (!aListener) ? true : aListener->closeContainer(temp, aType);
  }

//--------------------------------------------------------------------

  bool JSONParser::didParse(bool aState) {return aState;}

  bool JSONParser::willParse(JSONListener *aListener) {
    input >> std::skipws;
    if(kBraceOpen==input.get()) {
      return handleOpenContainer(Element::object ,aListener); //open default container...
    }
    return false;
  }

  //---- determine next in the json stream ------

  Element determineType(char aChar) {
    const char* kConstantChars="01234567890tf";
    switch(aChar) {
      case kQuote: return Element::quoted;
      case kBraceOpen: return Element::object;
      case kBracketOpen: return Element::array;
      case kBraceClose:
      case kBracketClose: return Element::closing;
      default:
        return strchr(kConstantChars, aChar) ? Element::constant : Element::unknown;
    }
    return Element::unknown;
  }

  //--- handle the goo inside our container ------------------

  bool JSONParser::parseElements(char aChar, JSONListener *aListener) {
    bool theResult=true;
    
    Element theType = determineType(aChar);
    JSONState &theTop = states.top();
    std::string temp;
    switch(theType) {
      case Element::object:
      case Element::array:  theResult= handleOpenContainer(theType, aListener); break;

      case Element::closing:
        theResult= handleCloseContainer(theTop.type, aListener);
        skipWhile(input, isWhitespace);
        skipIfChar(input,kComma);
        break;
        
      case Element::quoted:
        temp = readUntil(input, isQuote, false);
        skipIfChar(input,kQuote);
        skipWhile(input, isWhitespace);
        skipIfChar(input,kComma);
        
        if(Element::object==theTop.type) {
          aChar=input.peek();
          if(kColon==input.peek()) {
            input.get(); //skip it...
            tempKey=temp;
          }
          else theResult= (!aListener) ? true : aListener->addKeyValuePair(tempKey, temp);
        }
        else theResult= (!aListener) ? true : aListener->addItem(temp);
        break;
                
      case Element::constant:
        temp=readUntil(input, isNonAlphanum, false);
        temp.insert(0, 1, aChar);
        skipWhile(input, isWhitespace);
        skipIfChar(input,kComma);
        
        if(Element::object==theTop.type) {
          theResult= (!aListener) ? true : aListener->addKeyValuePair(tempKey, temp);
        }
        else theResult= (!aListener) ? true : aListener->addItem(temp);

        break;
                        
      default: break;
    }
    
    skipWhile(input, isWhitespace);
    return theResult;
  }
  
  bool JSONParser::parse(JSONListener *aListener) {
    if(willParse(aListener)) {
      
      bool isValid=true;
      while(isValid) {
        skipWhile(input, isWhitespace);
        if(!input.eof()) {
          char theChar=input.get();
          isValid = parseElements(theChar, aListener);
        }
        else isValid=false;
      }
      
      return didParse(isValid);
    }
    return true;
  }
}
