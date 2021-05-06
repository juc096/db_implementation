//
//  JSONParser.hpp
//  Assignment4
//
//  Created by rick gessner on 2/16/20.
//

#ifndef JSONParser_hpp
#define JSONParser_hpp


#include <stdio.h>
#include <iostream>
#include <stack>

namespace ECE141 {

  enum class Element {object, array, closing, constant, quoted, unknown};

  class JSONListener {
  public:
    virtual bool addKeyValuePair(const std::string &aKey, const std::string &aValue)=0;
    virtual bool addItem(const std::string &aValue)=0;
    virtual bool openContainer(const std::string &aKey, Element aType)=0;
    virtual bool closeContainer(const std::string &aKey, Element aType)=0;
  };

  //--------------------------------------------
  struct JSONState {
    JSONState(std::string aKey, Element aType=Element::object) : key(aKey), type(aType) {}
    
    JSONState(const JSONState &aCopy)
      : key(aCopy.key), type(aCopy.type){}
    
    std::string key;
    Element     type;
  };

  //--------------------------------------------
  class JSONParser {
  public:
    JSONParser(std::istream &anInputStream);
    
    bool willParse(JSONListener *aListener=nullptr);
    bool parse(JSONListener *aListener=nullptr);
    bool didParse(bool aStatus);

  protected:

    bool parseElements(char aChar, JSONListener *aListener);

    bool handleOpenContainer(Element aType, JSONListener *aListener); //{
    bool handleCloseContainer(Element aType, JSONListener *);
        
    std::stack<JSONState> states;
    std::string  tempKey;
    std::istream &input;
  };

}

#endif /* JSONParser_hpp */
