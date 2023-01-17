#include "parser.hpp"

#include <istream>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

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

ModelFileParser::Features ModelFileParser::strToFeature(std::string feature) {
  if (feature == "VERTEX") {
    return Features::VERTEX;
  } else if (feature == "COLOR") {
    return Features::COLOR;
  } else if (feature == "UV") {
    return Features::UV;
  }

  return Features::VERTEX;
}

bool ModelFileParser::modelHasFeature(Features f) {
  return std::find(
    fileFeatures.begin(),
    fileFeatures.end(),
    f
  ) != fileFeatures.end();
}

void ModelFileParser::parse(std::istream& file) {
  std::string line;

  while(std::getline(file, line)) {
    if (line.find(".FEATUREEND") == 0){
      writingFeature = -1;
    } else if (writingFeature != -1) {
      Features f = (Features)writingFeature;
        
      if (f == Features::VERTEX) {
        // Split parts
        Vertex v;
        std::vector<std::string> parts = split(line, ",");

        trim(parts[0]);
        trim(parts[1]);
        trim(parts[2]);

        v.x = std::stof(parts[0]);
        v.y = std::stof(parts[1]);
        v.z = std::stof(parts[2]);

        vertexes.push_back(v);
      } else if (f == Features::COLOR) {
        // Split parts
        Color v;
        std::vector<std::string> parts = split(line, ",");

        trim(parts[0]);
        trim(parts[1]);
        trim(parts[2]);

        v.r = std::stof(parts[0]);
        v.g = std::stof(parts[1]);
        v.b = std::stof(parts[2]);

        colors.push_back(v);
      }
    } else if (line.find(".FILEFEATURES:") == 0) {
      // Get everything after the file features
      std::string featuresString = line.substr(14);
      std::vector<std::string> parts = split(featuresString, ",");

      for (auto utPart : parts) {
        trim(utPart);
        fileFeatures.push_back(strToFeature(utPart));
      }
    } else if (line.find(".FEATUREBEGIN:") == 0) {
      // Determine what feature we are reading for
      std::string feature = line.substr(14);
      trim(feature);
      writingFeature = (int)strToFeature(feature);
    } 
  }
}