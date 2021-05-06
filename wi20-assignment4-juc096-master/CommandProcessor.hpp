//
//  CommandProcessor.hpp
//  Assignment4
//
//  Created by rick gessner on 2/15/20.
//

#ifndef CommandProcessor_hpp
#define CommandProcessor_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include "Model.hpp"
namespace ECE141 {
  
  class CommandProcessor {
  public:
    CommandProcessor();
    CommandProcessor(const CommandProcessor &aCommand);
    CommandProcessor& operator=(const CommandProcessor &aCommand);
    ECE141::ModelQuery& getQuery(std::string &aCommand);
    bool run(std::string &aCommand, ECE141::Model &aModel, std::ostream &anOut);
    static std::vector<std::string> query_str1;
    static std::vector<std::string> query_str2;
    static std::vector<std::string> query_str3;
    std::unordered_map<std::string, std::string> cmd_to_type;
    bool process(std::string &aCommand, std::vector<std::string> & args, ECE141::ModelQuery &aQ, std::ostream &anOutput);
    std::queue<std::string> parse(std::string &aCommand);

    
    
  };

}

#endif /* CommandProcessor_hpp */
