#pragma once

#include "../types.hpp"

#include <vector>
#include <iostream>
#include <string>

static std::vector<std::string> split(std::string s, std::string delimiter) {
  std::vector<std::string> tokens;
  
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      tokens.push_back(token);
      s.erase(0, pos + delimiter.length());
  }

  tokens.push_back(s);

  return tokens;
}

struct Indicie {
    int p1;
    int p2;
    int p3;
};

class ObjParser {
public:
    std::vector<Vertex> vertexes;
    std::vector<Indicie> indicies;

    void parse(std::istream& file);

    std::vector<Vertex> assembleToVertexes();
};