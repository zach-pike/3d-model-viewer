#pragma once

#include <string>
#include <algorithm>
#include <vector>

#include "../types.hpp"

class ModelFileParser {
public:
  enum class Features : std::uint8_t {
    VERTEX,
    UV,
    COLOR
  };
  static Features strToFeature(std::string feature);
private:
  std::vector<Features> fileFeatures;

  int writingFeature = -1;
public:
  ModelFileParser() {}
  ~ModelFileParser() {}

  std::vector<Vertex> vertexes;
  std::vector<Color> colors;

  bool modelHasFeature(Features f);
  void parse(std::istream& file);
};