#ifndef _READMODEL_H_
#define _READMODEL_H_

#include <iostream>
#include <climits>
#include <vector>
#include <cstring>
#include <cstdio>
#include <ctime>


// ===================== aidel ====================================
template<typename indexType> struct node
{
  indexType index;
  double w;
  double b;
  double negError;
  double fosError;
};
typedef node<long> lNode;
typedef node<int> iNode;


template<typename indexType, typename nodeType> 
std::vector<nodeType> read_model(const char *path)
{
  std::vector<nodeType> vec;

  ifstream fin(path);
  if (!fin){

    cout << "Erro message: "  << strerror(errno) << endl;
    exit(0);
  }

  char buffer[100];

  while( !fin.eof() ){
    fin.getline(buffer, 100);
    if (strlen(buffer) == 0) break;
    
    if (buffer[0] == '\n') break;
    
    char *p = strtok(buffer, ",");
    indexType index = stol(p);

    p = strtok(NULL, ",");
    double w = stod(p);

    p = strtok(NULL, ",");
    double b = stod(p);
    
    p = strtok(NULL, ",");
    double negError = stod(p);
    
    p = strtok(NULL, ",");
    double fosError = stod(p);

    nodeType node;
    node.index = index;
    node.w = w;
    node.b = b;
    node.negError = negError;
    node.fosError = fosError;
    vec.push_back(node);
  }

  fin.close();
  return vec;
}

template<typename nodeType> nodeType* process_models(std::vector<nodeType> models)
{
  nodeType *p = (nodeType *)malloc(sizeof(nodeType) * models.size());
  for (long unsigned int i = 0; i < models.size(); i++){
    p[i] = models[i];
  }
 
  return p;
}


// =====================learned index===================
struct learned_node
{
  double w;
  double b;
  double min;
  double max;
  double error;
};


template<typename nodeType> 
std::vector<nodeType> read_learned_model(const char *path)
{
  std::vector<nodeType> vec;

  ifstream fin(path);
  if (!fin){

    cout << "Erro message: "  << strerror(errno) << endl;
    exit(0);
  }

  char buffer[200];

  while( !fin.eof() ){
    fin.getline(buffer, 200);
    if (strlen(buffer) == 0) break;
    
    if (buffer[0] == '\n') break;
    
    char *p = strtok(buffer, ",");
    double w = stod(p);

    p = strtok(NULL, ",");
    double b = stod(p);

    p = strtok(NULL, ",");
    double min = stod(p);
    
    p = strtok(NULL, ",");
    double max = stod(p);
    
    p = strtok(NULL, ",");
    double error = stod(p);

    nodeType node;
    node.w = w;
    node.b = b;
    node.min = min;
    node.max = max;
    node.error = error;
    vec.push_back(node);
  }

  fin.close();
  return vec;
}


#endif