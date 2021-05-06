//
//  CommandProcessor.cpp
//  Assignment4
//
//  Created by rick gessner on 2/15/20.
//

#include "CommandProcessor.hpp"


namespace ECE141 {

  CommandProcessor::CommandProcessor() {
    cmd_to_type[std::string("before")] = 1;
    cmd_to_type[std::string("after")] = 1;
    cmd_to_type[std::string("within")] = 1;

    cmd_to_type[std::string("all")] = 2;
    cmd_to_type[std::string("first")] = 2;
    cmd_to_type[std::string("last")] = 2;
    cmd_to_type[std::string("nth")] = 2;

    cmd_to_type[std::string("sum")] = 3;
    cmd_to_type[std::string("avg")] = 3;
    cmd_to_type[std::string("count")] = 3;
    cmd_to_type[std::string("get")] = 3;
  }
  CommandProcessor::CommandProcessor(const CommandProcessor &aCommand) {}
  CommandProcessor& CommandProcessor::operator=(const CommandProcessor &aCommand) {
  return *this;
}

  bool CommandProcessor::process(std::string &aCommand, std::vector<std::string> & args, 
                  ECE141::ModelQuery &aQ, std::ostream &anOutput) {
    
    size_t idx = 0;
    std::string aKey = "";
    if (args.size()) {
      if (args.size() == 1)
        aKey = args[0];
      if(args.size() == 2)
        idx = std::stoi(args[1]);
    }   
    if (aCommand == std::string("before")) {
      aQ.before(aKey);
      return true;
    }

    if (aCommand == std::string("after")) {
      aQ.after(aKey);
      return true;
    }
    if (aCommand == std::string("within")) {
      aQ.within(aKey);
      return true;
    }
      
  

  
    if(aCommand == std::string("all")){
      aQ.all(aKey);
      return true;
    }
    if(aCommand == std::string("first")) {
      aQ.first(aKey);
      return true;
    }
      
    if (aCommand == std::string("last")) {
      aQ.last(aKey);
      return true;
    }
    if (aCommand == std::string("nth")){
      if(args.size() ==1)
        idx = std::stoi(args[0]);
      aQ.nth(aKey,idx);
      return true;
    }
  

  
    if(aCommand == std::string("sum")) {
      anOutput << aQ.sum(aKey);
      return true;
    }
    
    if(aCommand == std::string("avg")) {
      anOutput << aQ.avg(aKey);
      return true;
    }
    if(aCommand == std::string("count")) {
      anOutput << aQ.count(aKey);
      return true;
    }
    if(aCommand == std::string("get")) {
      if (aKey == "") {
        auto col = aQ.get();
        anOutput << *col;
        delete col;
      }
      else {
        anOutput << aQ.get(aKey);
      }
      return true;
    }
    return false;
  }
  
  std::queue<std::string> CommandProcessor::parse(std::string &aCommand) {
    

    std::queue<std::string> toks;
    std::string temp, token;
    std::replace_copy_if(aCommand.begin(), aCommand.end(), std::back_inserter(temp),
                                                          [](char x){ return ispunct(x) && x != '*';}, ' ');

    std::istringstream ss(temp);
    while (ss >> token) {
      toks.push(token);
    }
    return toks;
  }

  

  bool CommandProcessor::run(std::string &aCommand, Model &aModel, std::ostream &anOut) {
    //STUDENT -- Run your query and send your output to the output stream ...
    std::cout << aCommand<<std::endl;
    std::queue<std::string> toks = parse(aCommand);
    std::vector<std::string> container;
    //std::vector<std::string> container_with;
    std::string cmd;
    //bool with_cmd = false;
    ECE141::ModelQuery aQ(aModel);
    
    
    while(!toks.empty()) {
      std::string temp = toks.front();
      toks.pop();

      if (cmd_to_type.count(temp) == 0) { // we have value
          container.push_back(temp);
        
        continue;
      }
      if (cmd_to_type.count(temp) > 0) { //we have a cmd
        
        if (cmd != "") {
          process(cmd,container,aQ,anOut);
        }

        container.clear();
        cmd = temp;
      }
    

    }
    anOut << "Results for cmd \""<<aCommand<<"\": \n";
    process(cmd,container,aQ,anOut);
    anOut << "\n\n";

    
    
    
    return true;
  }

}
