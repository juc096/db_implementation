//
//  main.cpp
//  Assignment4
//
//  Created by rick gessner on 2/12/20.
//

#include <iostream>
#include <fstream>
//#include "JSONParser.hpp"
//#include "Model.hpp"
#include "Testing.hpp"



class ModelBuilder : public ECE141::JSONListener {
public:
  
  ModelBuilder() {}
  
  bool addKeyValuePair(const std::string &aKey, const std::string &aValue) {
    return true;
  }
    
  bool startObject(const std::string &aContainerName){
    //open a new unnamed container in our model...
    return true;
  }
  
  bool endObject(const std::string &aContainerName){
    return true;
  }
  
  bool startArray(){
    return true;
  }
  
  bool endArray(){
    return true;
  }

};


int main(int argc, const char * argv[]) {

  std::ifstream theInput("./test.json");
  std::ifstream theInput2("./test.txt");
  //ECE141::JSONParser theParser(theInput);
  //ECE141::Model theBuilder;
  //ECE141::Testing aTest;
  ECE141::Testing::runTests(theInput, theInput2, std::cout);
  //theParser.parse(&theBuilder);
  
  return 0;
}
