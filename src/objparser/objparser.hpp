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

    void parse(std::istream& file) {
        std::string line;

        while(std::getline(file, line)) {
            if (line[0] == 'v') {
                Vertex v;

                std::string values = line.substr(2);
                std::vector<std::string> splitValues = split(values, " ");

                v.x = std::stof(splitValues[0]);
                v.y = std::stof(splitValues[1]);
                v.z = std::stof(splitValues[2]);

                vertexes.push_back(v);
            } else if (line[0] == 'f') {
                Indicie v;

                std::string values = line.substr(2);
                std::vector<std::string> splitValues = split(values, " ");

                v.p1 = std::stoi(splitValues[0]);
                v.p2 = std::stoi(splitValues[1]);
                v.p3 = std::stoi(splitValues[2]);

                indicies.push_back(v);
            }
        }
    }

    std::vector<Vertex> assembleToVertexes() {
        std::vector<Vertex> v;

        for (Indicie ind : indicies) {
            Vertex newVertex;

            
        }
    }
};