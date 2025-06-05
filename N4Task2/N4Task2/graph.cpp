// graph.cpp
#include "graph.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <set>
#include <iomanip>
#include <Windows.h>

Graph::Graph() {
    Logger::getInstance().debug("Graph object created.");
}

void Graph::clear() {
    m_adjList.clear();
    Logger::getInstance().info("Graph data cleared.");
}

bool Graph::parseGraphFileLine(const std::string& line, int& vertex_id, std::vector<int>& neighbors) {
    neighbors.clear();
    std::istringstream iss(line);
    if (!(iss >> vertex_id)) {
        return false;
    }

    int neighbor;
    std::set<int> unique_neighbors;
    while (iss >> neighbor) {
        if (neighbor != vertex_id) {
            unique_neighbors.insert(neighbor);
        }
    }
    neighbors.assign(unique_neighbors.begin(), unique_neighbors.end());
    return true;
}

void Graph::loadFromFile(const std::string& filename) {
    clear();
    Logger::getInstance().info("Attempting to load graph from file: " + filename);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::string errorMsg = "Failed to open graph file: " + filename;
        Logger::getInstance().error(errorMsg);
        throw std::runtime_error(errorMsg);
    }

    std::string line;
    int line_number = 0;
    std::map<int, std::set<int>> temp_adj_list;

    while (std::getline(file, line)) {
        line_number++;
        if (line.empty() || line[0] == '#') {
            continue;
        }

        int vertex_id;
        std::vector<int> neighbors;
        if (!parseGraphFileLine(line, vertex_id, neighbors)) {
            file.close();
            std::string errorMsg = "Invalid format in graph file: " + filename + " at line " + std::to_string(line_number);
            Logger::getInstance().error(errorMsg);
            throw std::runtime_error(errorMsg);
        }

        for (const int& neighbor : neighbors) {
            temp_adj_list[vertex_id].insert(neighbor);
            temp_adj_list[neighbor].insert(vertex_id);
        }
    }
    file.close();

    for (const auto& pair : temp_adj_list) {
        m_adjList[pair.first] = std::vector<int>(pair.second.begin(), pair.second.end());
        std::sort(m_adjList[pair.first].begin(), m_adjList[pair.first].end());
    }

    if (m_adjList.empty()) {
        Logger::getInstance().warning("Graph is empty after loading from " + filename);
    } else {
        std::string info = "Graph loaded successfully. Vertices: " + std::to_string(m_adjList.size());
        Logger::getInstance().info(info);
    }
}

bool Graph::hasVertex(int vertex_id) const {
    return m_adjList.count(vertex_id) > 0;
}

std::map<int, std::vector<int>> Graph::calculateLayers(int start_vertex) {
    Logger::getInstance().info("Calculating layers starting from vertex: " + std::to_string(start_vertex));
    
    if (!hasVertex(start_vertex)) {
        std::string errorMsg = "Start vertex " + std::to_string(start_vertex) + " not found in the graph.";
        Logger::getInstance().error(errorMsg);
        throw std::invalid_argument(errorMsg);
    }

    std::map<int, std::vector<int>> layers_output;
    std::map<int, int> vertex_layer;
    std::queue<int> current_layer_queue;
    std::queue<int> next_layer_queue;

    current_layer_queue.push(start_vertex);
    vertex_layer[start_vertex] = 0;
    layers_output[0].push_back(start_vertex);

    int current_layer_number = 0;
    
    while (!current_layer_queue.empty() || !next_layer_queue.empty()) {
        if (current_layer_queue.empty()) {
            current_layer_number++;
            std::swap(current_layer_queue, next_layer_queue);
            
            if (!layers_output[current_layer_number].empty()) {
                std::sort(layers_output[current_layer_number].begin(), 
                         layers_output[current_layer_number].end());
            }
            continue;
        }

        int current_vertex = current_layer_queue.front();
        current_layer_queue.pop();

        const auto& neighbors = m_adjList[current_vertex];
        for (const int& neighbor : neighbors) {
            if (vertex_layer.find(neighbor) == vertex_layer.end()) {
                vertex_layer[neighbor] = current_layer_number + 1;
                next_layer_queue.push(neighbor);
                layers_output[current_layer_number + 1].push_back(neighbor);
                
                Logger::getInstance().debug(
                    "Vertex " + std::to_string(neighbor) + 
                    " added to layer " + std::to_string(current_layer_number + 1)
                );
            }
        }
    }

    if (!layers_output.empty()) {
        auto it = layers_output.rbegin();
        if (!it->second.empty()) {
            std::sort(it->second.begin(), it->second.end());
        }
    }

    for (const auto& pair : layers_output) {
        std::ostringstream ss;
        ss << "Layer " << pair.first << " contains vertices: ";
        const auto& vertices = pair.second;
        for (size_t i = 0; i < vertices.size(); ++i) {
            ss << vertices[i];
            if (i < vertices.size() - 1) ss << ", ";
        }
        Logger::getInstance().info(ss.str());
    }

    return layers_output;
}

std::wstring utf8_to_wide(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

std::string wide_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
    return str;
}

void Graph::saveLayersToFile(const std::string& filename, const std::map<int, std::vector<int>>& layers_output) {
    Logger::getInstance().info("Attempting to save layered graph to file: " + filename);
    
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::string errorMsg = "Failed to open output file for writing: " + filename;
        Logger::getInstance().error(errorMsg);
        throw std::runtime_error(errorMsg);
    }

    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    file.write(reinterpret_cast<char*>(bom), sizeof(bom));

    std::string header = "Результаты разбиения графа на слои:\n";
    std::string separator = "=================================\n\n";
    
    file.write(header.c_str(), header.length());
    file.write(separator.c_str(), separator.length());

    for (const auto& pair : layers_output) {
        std::string layer_header = "Слой " + std::to_string(pair.first) + 
                                 " (количество вершин: " + std::to_string(pair.second.size()) + 
                                 "):\nВершины:";
        file.write(layer_header.c_str(), layer_header.length());

        const auto& vertices = pair.second;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (i % 10 == 0) {
                file.write("\n  ", 3);
            }
            std::string vertex = std::to_string(vertices[i]);
            file.write(vertex.c_str(), vertex.length());
            if (i < vertices.size() - 1) {
                file.write(", ", 2);
            }
        }
        file.write("\n\n", 2);
    }

    file.write(separator.c_str(), separator.length());
    std::string footer = "Общее количество слоёв: " + std::to_string(layers_output.size()) + "\n";
    file.write(footer.c_str(), footer.length());
    
    file.close();
    Logger::getInstance().info("Layered graph saved successfully to " + filename);
}
