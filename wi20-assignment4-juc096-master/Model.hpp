//
//  Model.hpp
//  Assignment4
//
//  Created by rick gessner on 2/15/20.
//

#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include <string>
#include "JSONParser.hpp"
#include <vector>
#include <utility>
#include <memory>
#include <regex>
#include <algorithm>

namespace ECE141 {


  class ModelQuery; //forward declare
  class ModelNode;
  class ModelArray;
  class ModelObj;
  class ModelConstVal;
  using node_ptr = std::shared_ptr<ModelNode>;
  using node_obj_ptr = std::shared_ptr<ModelObj>;
  using node_val_ptr = std::shared_ptr<ModelConstVal>;
  using node_arr_ptr = std::shared_ptr<ModelArray>;
  using objPair = std::pair<std::string, node_ptr>;

  //STUDENT: Your Model is built from a bunch of these...
  
  class ModelNode {
    public:
      ModelNode(const Element & aType) : myType(aType) {}
      ModelNode(const ModelNode & aNode) {myType = aNode.getType();}
      ModelNode & operator=(const ModelNode & aNode) {
        myType = aNode.getType();
        return *this;
      }
      Element getType() const {return myType;}
      void print() {
        if(myType == Element::array)
          std::cout << "array\n";
        if(myType == Element::object)
          std::cout << "obj\n";
        if(myType == Element::constant)
          std::cout << "const\n";
      }

      //interface method to insert
      //how it inserts is entirely dependent on the type of modelnode
      //key is given as an option regardless, just in case we are inserting into an object
      //be careful about key being empty if inserting into object
      ModelNode & insert(node_ptr aNode, const std::string & aKey);
      ModelNode & insert(objPair &aPair);
      std::vector<objPair> getObjVal();
      std::vector<node_ptr> getArrayVal();
      std::string getConstVal();
    protected:
      Element myType;
  };

  class ModelArray : public ModelNode {
    public:
      ModelArray() : ModelNode(Element::array) {}
      ModelArray(const ModelArray & aNode) : ModelNode(Element::array){val = aNode.getVal();}
      ModelArray & operator=(const ModelArray & aNode){
        val.clear();
        val = aNode.getVal();
        return *this;
      }
      ~ModelArray() {val.clear();}
      std::vector<node_ptr> getVal() const {return val;}
      ModelArray & push_back(node_ptr aNode) {
        std::cout << "ModelArray Scope\n";
        aNode->print();
        val.push_back(aNode);
        return *this;
      }
      private:
        std::vector<node_ptr> val;
  };

  class ModelObj : public ModelNode {
    public:
        
        ModelObj() : ModelNode(Element::object) {}
        ModelObj(const ModelObj & aNode) : ModelNode(Element::object){val = aNode.getVal();}
        ModelObj & operator=(const ModelObj & aNode){
          val.clear();
          val = aNode.getVal();
          return *this;
        }
        ~ModelObj() {val.clear();}
        std::vector<objPair> getVal() const {return val;}
        ModelObj & push_back(const std::string & aKey, node_ptr aNode) {
          std::cout << "ModelObj Scope, aKey:" << aKey <<"\n";
          aNode->print();
          val.push_back(objPair(aKey, aNode));
          return *this;
        }
        ModelObj & push_back(objPair & aPair) {
          val.push_back(objPair(aPair));
          return *this;
        }
        private:
            std::vector<objPair> val;
  };
  class ModelConstVal : public ModelNode {
        public:
            ModelConstVal(const std::string &aVal="") : ModelNode(Element::constant), theConstVal(aVal) {}
            std::string getVal() const {return theConstVal;} 
            //todo: include a string to value parser
        private:
            std::string theConstVal;
  };

  class Model : public JSONListener {
   public:

     Model();
     Model(const Model &aCommand);
     Model& operator=(const Model &aCommand);

     ModelQuery query();

   //protected:
      

      virtual bool addKeyValuePair(const std::string &aKey, const std::string &aValue);
      virtual bool addItem(const std::string &aValue);
      virtual bool openContainer(const std::string &aKey, Element aType);
      virtual bool closeContainer(const std::string &aKey, Element aType);
      
      // STUDENT: Your model will contain a collection of ModelNode*'s
      //          Choose your container(s) wisely
      Model & insert(node_ptr aVal,const std::string &aKey);
      node_obj_ptr getRoot() {return root;}
      
      node_obj_ptr createObj();
      node_arr_ptr createArr();
      node_val_ptr createVal(const std::string &aValue);
      
      
      
    private:
      node_obj_ptr root; //holds the root of the json tree
      //node_ptr root;
      std::stack<node_ptr> buildT; //stack to build this tree
   };

    //this is what gets returned when you query a model for a list of elements
  class ModelCollection {
  public:
    ModelCollection();
    ModelCollection(std::vector<node_ptr> & alist, Element & type) {
      queryT = alist;
      aType = type;
    }
    ModelCollection(std::vector<objPair> & alist, Element & type) {
      queryT_pair = alist;
      aType = type;
    }
    ModelCollection(std::string & aval, Element & type) {
      query_constv = aval;
      aType = type;
    }
    ModelCollection(const ModelCollection & aModCol) {
      queryT = aModCol.queryT;
      queryT_pair = aModCol.queryT_pair;
      query_constv = aModCol.query_constv;
      aType = aModCol.aType;
    }
    friend std::ostream& operator << (std::ostream &out, const ModelCollection & aModCol) {
      if (aModCol.aType == Element::object) {
        for (auto it = aModCol.queryT_pair.begin(); it != aModCol.queryT_pair.end(); ++it) {
          out << it->first;
          out << " ";
        }
        return out;
      }
      if (aModCol.aType == Element::array) {
        for (auto it = aModCol.queryT.begin(); it != aModCol.queryT.end(); ++it) {
          out << (*it)->getConstVal();
          out << " ";
        }
        return out;
      }
      if (aModCol.aType == Element::constant) {
        out << aModCol.query_constv;
        return out;
      }
      return out;
    }
    
    std::vector<node_ptr> queryT;
    std::vector<objPair> queryT_pair;
    std::string query_constv;
    Element aType;
    
  };

  class ModelQuery {
  public:
    
    ModelQuery& all(const std::string &aTarget);
    ModelQuery& first(const std::string &aTarget);
    ModelQuery& last(const std::string &aTarget);
    ModelQuery& nth(const std::string &aTarget, size_t anIndex);

    ModelQuery& before(const std::string &aTarget);
    ModelQuery& after(const std::string &aTarget);
    ModelQuery& within(const std::string &aTarget);
    
    size_t            count(const std::string &aField);
    double            sum(const std::string &aField);
    double            avg(const std::string &aField);
    ModelCollection*  get();
    std::string       get(const std::string &aKey); //return value or empty string

    node_ptr keyIter(const std::string &aTarget);
    ModelQuery & createContext(const std::string &aTarget, const std::string &which);
    std::vector<objPair> createTargetList(const std::string &aTarget);

    ModelQuery(Model &aModel);
          
    friend class Model;
    
    Model &model;
    std::vector<objPair> context;
    node_ptr cur_q;
  };
  
 

}

#endif /* Model_hpp */
