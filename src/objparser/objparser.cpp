#include "objparser.hpp"

void ObjParser::parse(std::istream& file) {
    std::string line;

    while(std::getline(file, line)) {
        if (line.size() == 0) continue;
        
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

std::vector<Vertex> ObjParser::assembleToVertexes() {
    std::vector<Vertex> v;

    for (Indicie ind : indicies) {
        v.push_back(vertexes.at(ind.p1 - 1));
        v.push_back(vertexes.at(ind.p2 - 1));
        v.push_back(vertexes.at(ind.p3 - 1));
    }

    return v;
}