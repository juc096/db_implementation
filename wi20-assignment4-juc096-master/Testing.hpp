//
//  Testing.hpp
//  Assignment4
//
//  Created by rick gessner on 2/16/20.
//

#ifndef Testing_h
#define Testing_h

#include <string>
#include <fstream>
#include "CommandProcessor.hpp"
#include "JSONParser.hpp"
#include "Model.hpp"

namespace ECE141 {

  class Testing {
  public:
    
    static bool runTests (std::istream &aJSONFile, std::istream &aCmdFile, std::ostream &anOutput) {
      
      Testing theTests(anOutput);
      return theTests.run(aJSONFile, aCmdFile);
    }
          
  protected:
      
    //let's wire up the testing mechansim here.
    //testing involves reading commands from file, and running through command processor...
    bool run(std::istream &aJSONFile, std::istream &aCmdFile) {
      std::cout<< "hi\n";

      ECE141::JSONParser theParser(aJSONFile);
      ECE141::Model theModel;
      theParser.parse(&theModel);

//      auto theQuery = theModel.query();
//      size_t theCount=theQuery.first("sammy").after("list").count();
//      double theSum=theQuery.first("sammy").after("list").sum("foo");
//      auto   *theSet =  theQuery.first("sammy").after("list").get();
       std::cout<< "hi2\n";
      std::string theLine;
      CommandProcessor theProcessor;
      
      bool isValid=true;
      while (std::getline(aCmdFile, theLine) && isValid) {
        if(theLine.size()) {
          isValid=theProcessor.run(theLine, theModel, output);
        }
      }
      

      return isValid;
    }
    
    Testing(std::ostream &anOutput) : output(anOutput){}
    
    std::ostream &output;

  };

}

#endif /* Testing_h */
