//
//  Model.cpp
//  Assignment4
//
//  Created by rick gessner on 2/15/20.
//

#include "Model.hpp"

namespace ECE141 {


  ModelQuery::ModelQuery(Model &aModel) : model(aModel) {
    cur_q = model.getRoot();
  }
  
  ModelQuery& ModelQuery::all(const std::string &aTarget) {
    if (aTarget == "*") {
      return *this;
    }
    auto some_vect = createTargetList(aTarget);
    context = some_vect;
    return *this;
  }

  ModelQuery& ModelQuery::first(const std::string &aTarget) {
    auto some_vect = createTargetList(aTarget);
    if (!some_vect.empty()) {
      node_ptr first_one = some_vect.front().second;
      cur_q = first_one;
    }
    context.clear();
    return *this;
  }

  ModelQuery& ModelQuery::last(const std::string &aTarget) {
    auto some_vect = createTargetList(aTarget);
    if (!some_vect.empty()) {
      
      node_ptr last = some_vect.back().second;
      cur_q = last;
    }
    context.clear();
    return *this;
  }

  ModelQuery& ModelQuery::nth(const std::string &aTarget, size_t anIndex) {
    if (cur_q->getType() == Element::array) {
      auto some_vect = cur_q->getArrayVal();
      cur_q = some_vect.at(anIndex-1);
      return *this;
    }
    auto some_vect = createTargetList(aTarget);
    if (anIndex <= some_vect.size()) {
      node_ptr at_idx = some_vect.at(anIndex-1).second;
      cur_q = at_idx;
    }
    context.clear();
    return *this;
  }

  ModelQuery& ModelQuery::before(const std::string &aTarget) {
    return createContext(aTarget, std::string("before"));  
  }

  ModelQuery& ModelQuery::after(const std::string &aTarget) {
    return createContext(aTarget, std::string("after"));
  }
  
  ModelQuery& ModelQuery::within(const std::string &aTarget) {
    node_ptr ele_found = keyIter(aTarget);
    if (ele_found != nullptr) {
      cur_q = ele_found;
    }
    context.clear();
    return *this;

  }

  /*std::string ModelQuery::valueOf() {
    return std::string("hi");
  }*/

  ModelCollection* ModelQuery::get() {

    if (!context.empty()) {
      std::vector<objPair> temp;
      for (auto it = context.begin(); it != context.end(); ++it)  
        temp.push_back(*it);
      auto atype = Element::object;
      return new ModelCollection(temp, atype);
    }
    else if (cur_q != nullptr) {
      auto aType = cur_q->getType();

      if (aType == Element::array){
        std::vector<node_ptr> temp;
        auto cur_q_array = cur_q->getArrayVal();
        for (auto it = cur_q_array.begin(); it != cur_q_array.end(); ++it)  
          temp.push_back(*it);
        auto atype = Element::array;
        return new ModelCollection(temp, atype);
      }

      if(aType == Element::object){
        std::vector<objPair> temp;
        auto cur_q_obj = cur_q->getObjVal();
        for (auto it = cur_q_obj.begin(); it != cur_q_obj.end(); ++it)  
          temp.push_back(*it);
        auto atype = Element::object;
        return new ModelCollection(temp, atype);
      }
      if (aType == Element::constant) {
        auto atype = Element::constant;
        auto val =cur_q->getConstVal();
        return new ModelCollection(val, atype);

      }

    }
    else {
      return nullptr; // return null if value is not viable
    }
    return nullptr;
  }
  
  std::string ModelQuery::get(const std::string &aKey) {
    node_ptr val = keyIter(aKey);
    
    if((val != nullptr) && (val->getType() == Element::constant))
      return val->getConstVal();

    return std::string(""); //retrieve the key from a value if query matches, otherwise return ""
  }  

  size_t ModelQuery::count(const std::string &aField) {
    node_ptr val = cur_q;
    if (aField != "")
      val = keyIter(aField);
    if (!context.empty()) {
      return context.size();
    }
    else if (val != nullptr) {
      auto aType = val->getType();
      if (aType == Element::array){
        return val->getArrayVal().size();
      }
      if (aType == Element::object) {
        return val->getObjVal().size();
      }
    }
    else {
      return 0;
    }
    return 0;
  }

  double ModelQuery::avg(const std::string &aField) {
    node_ptr val = cur_q;
    if (aField != "") {
      val = keyIter(aField);
    }
    if (val!= nullptr) {
      auto aType = val->getType();
      if (aType == Element::array){
        double sum = 0;
        auto some_arr = val->getArrayVal();
        for (auto it = some_arr.begin(); it != some_arr.end(); ++it) {
          sum += std::stoi((*it)->getConstVal());
        }
        if (!some_arr.empty())
          return (sum/some_arr.size());
      }

    }
    return 0; //if no valid
    
  }

  double ModelQuery::sum(const std::string &aField) {
    node_ptr val = cur_q;
    if (aField != "") {
      val = keyIter(aField);
    }
    if (val!= nullptr) {
      auto aType = val->getType();
      if (aType == Element::array){
        double sum = 0;
        auto some_arr = val->getArrayVal();
        for (auto it = some_arr.begin(); it != some_arr.end(); ++it) {
          sum += std::stoi((*it)->getConstVal());
        }
        return sum;
      }
    }
    return 0.0;
  }
  
  std::vector<objPair> ModelQuery::createTargetList(const std::string &aTarget) {
    std::vector<objPair> temp;
    auto query = context;
    if (query.empty())
      //query = getObjVal(cur_q);
      query = cur_q->getObjVal();
    for (auto it = query.begin(); it != query.end(); ++it) {
      if (it->first == aTarget) 
        temp.push_back(*it);
    }
    return temp;
  }
  ModelQuery & ModelQuery::createContext(const std::string &aTarget, const std::string &which){
    auto query = context;
    if (query.empty())
      //query = getObjVal(cur_q);
      query = cur_q->getObjVal();
    auto it = std::find_if(query.begin(), 
                            query.end(),
                            [&aTarget](const objPair& p)
                            { return p.first == aTarget; });
    if (it != query.end()) {
      context.clear();
      if (which == "after") {
        for (auto iter = it; iter != query.end(); ++iter) {
          if (iter != it)
            context.push_back(*iter);
        }
        return *this;
      }
      if (which == "before") {
        for (auto iter = it; iter != query.begin(); --iter) {
          if (iter != it)
            context.push_back(*iter);
        }
        return *this;
      }
    }
    return *this;
  }
  node_ptr ModelQuery::keyIter(const std::string &aTarget) { 
    auto query = context;
    if (query.empty()) {
      //query = getObjVal(cur_q);
      if (cur_q->getType() == Element::object) {
        query = cur_q->getObjVal();
        auto it = std::find_if(query.begin(), 
                            query.end(),
                            [&aTarget](const objPair& p)
                            { return p.first == aTarget; });
        if (it != query.end())
          return it->second;
      }
      if (cur_q->getType() == Element::array) {
        auto query_array = cur_q->getArrayVal();
        for (auto array_it = query_array.begin(); array_it != query_array.end(); ++array_it) {
          query = (*array_it)->getObjVal();
          auto it = std::find_if(query.begin(), 
                            query.end(),
                            [&aTarget](const objPair& p)
                            { return p.first == aTarget; });
          if (it != query.end())
            return it->second;
        }
        
      } 
    }
  return nullptr;//if checks earlier fail to return; it means it's not valid 
  }
  

  //------------- ModelNode class...----------------------------------------

  ModelNode & ModelNode::insert(node_ptr aNode, const std::string & aKey = "") {
    switch(myType) {
      case Element::array : return static_cast<ModelArray*>(this)->push_back(aNode);
      case Element::object: return static_cast<ModelObj*>(this)->push_back(aKey,aNode);
      default: return *this;
    }
    return *this;
  }
  ModelNode & ModelNode::insert(objPair &aPair){
    if (myType == Element::object)
      return static_cast<ModelObj*>(this)->push_back(aPair);
    std::cout << "not obj\n";
    return *this;
  }

  std::vector<objPair> ModelNode::getObjVal() {
    std::vector<objPair> temp;
    if (myType == Element::object)
      return static_cast<ModelObj*>(this)->getVal();
    else {
      return temp;
      std::cout << "wrong type; in getobjval";
    }
  }
  std::vector<node_ptr> ModelNode::getArrayVal() {
    std::vector<node_ptr> temp;
    if (myType == Element::array)
      return static_cast<ModelArray*>(this)->getVal();
    else {
      std::cout << "wrong type; in getarrval";
      return temp;
      
    }
  }
  std::string ModelNode::getConstVal() {
    if (myType == Element::constant)
      return static_cast<ModelConstVal*>(this)->getVal();
    else {
      std::cout << "wrong type; in getconstval";
      return "";
      
    }
  }
  //---------------------- implement the model class... --------------------
  
  Model::Model() {
    //root = node_obj_ptr(createObj());
    //buildT.push(root);
  }

  Model::Model(const Model &aCommand) {
    root = node_obj_ptr(aCommand.root);
    buildT = aCommand.buildT;
  }
  
  Model& Model::operator=(const Model &aCommand) {
    root = node_obj_ptr(aCommand.root);
    return *this;
  }
    
  ModelQuery Model::query() {
    ModelQuery theQuery(*this);
    return theQuery;
  }
  node_obj_ptr Model::createObj() {
    return node_obj_ptr(new ModelObj());
    //temp->push_back(aKey, aValue);
  }
  node_arr_ptr Model::createArr() {
    return node_arr_ptr(new ModelArray());
  }
  node_val_ptr Model::createVal(const std::string &aValue) {
    return node_val_ptr(new ModelConstVal(aValue));
  }
  

  Model & Model::insert(node_ptr aVal,const std::string &aKey="") {
    buildT.top()->insert(aVal, aKey);
    return *this;
  }
  
  
  bool Model::addKeyValuePair(const std::string &aKey, const std::string &aValue) {
    //std::cout <<  "\t|" << aKey <<":" << aValue << "| \n";
    insert(createVal(aValue), aKey);
    return true;
  }
    
  bool Model::addItem(const std::string &aValue) {
    //std::cout <<  "\t" << aValue << " \n";
    insert(createVal(aValue));
    return true;
  }

  bool Model::openContainer(const std::string &aContainerName, ECE141::Element aType){
    //open a new unnamed container in our model...
    std::cout << aContainerName << " { \n";
    if (aType == Element::array) {
        node_arr_ptr tempArr = createArr();
        insert(tempArr, aContainerName);
        buildT.push(tempArr);
        return true;
    }
    if (aType == Element::object) {
        node_obj_ptr tempObj = createObj();
        if (root == nullptr) {
          root = tempObj;
          buildT.push(root);
          return true;
        }
        insert(tempObj, aContainerName);
        buildT.push(tempObj);
        return true;
    }
    
    return false;

  }

  bool Model::closeContainer(const std::string &aContainerName, ECE141::Element aType){
    //std::cout  << " } \n";
    buildT.pop();
    return true;
  }
}
