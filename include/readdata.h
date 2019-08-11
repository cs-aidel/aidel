#ifndef _READDATA_H_
#define _READDATA_H_

#include <iostream>
#include <climits>
#include <vector>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <fstream>


using namespace std;


#define BUF_SIZE 2048

std::vector<int> read_data(const char *path) {
  	std::vector<int> vec;
  	FILE *fin = fopen(path, "rb");
  	int buf[BUF_SIZE];
  	while (true) {
  	  size_t num_read = fread(buf, sizeof(int), BUF_SIZE, fin);
  	  for (unsigned long i = 0; i < num_read; i++) {
  	    vec.push_back(buf[i]);
  	  }
  	  if (num_read < BUF_SIZE) break;
  	}
  	fclose(fin);
  	return vec;
}

std::vector<long> read_timestamp(const char *path) {
  std::vector<long> vec;
  FILE *fin = fopen(path, "rb");
  long buf[BUF_SIZE];
  while (true) {
      size_t num_read = fread(buf, sizeof(long), BUF_SIZE, fin);
      for (unsigned long i = 0; i < num_read; i++) {
        vec.push_back(buf[i]);
      }
      if (num_read < BUF_SIZE) break;
  }
  fclose(fin);
  return vec;
}

std::vector<long> read_document(const char *path)
{
  std::vector<long> vec;
  
  ifstream fin(path);
  if (!fin){

    cout << "Erro message: "  << strerror(errno) << endl;
    exit(0);
  }

  long a;
  char buffer[50];
  while( !fin.eof() ){
    fin.getline(buffer, 50);
    if (strlen(buffer) == 0) break;

    sscanf(buffer, "%ld", &a);
    vec.push_back(a);
  }
  cout << "read data number: " << vec.size() << endl;
  
  fin.close();
  return vec;
}


#endif