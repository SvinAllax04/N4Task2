// main.cpp
#include "logger.h"
#include "graph.h"
#include <iostream>
#include <string> // ��� std::stoi, std::string

void printHelp() {
    std::cout << "Usage: graph_processor <input_graph_file> <start_vertex_id> <output_layers_file>" << std::endl;
    std::cout << "Example: ./graph_processor graph_data.txt 0 result_layers.txt" << std::endl;
}

int main(int argc, char* argv[]) {
    // ������������� �������
    // ����� �������� setLogFile ���� ��� � ������ � ��������� ���������
    Logger::getInstance().setLogFile("application_activity.log");
    Logger::getInstance().info("Application started.");

    if (argc != 4) {
        printHelp();
        Logger::getInstance().error("Invalid number of command-line arguments. Expected 3, got " + std::to_string(argc - 1));
        Logger::getInstance().info("Application terminated due to incorrect arguments.");
        return 1;
    }

    std::string input_filename = argv[1];
    int start_vertex;
    std::string output_filename = argv[3];

    try {
        // ��������� � ����������� start_vertex
        try {
            // ���������� std::stoll ��� ����� �������� ���������, ����� ��������� �� int, ���� �����
            long long sv_ll = std::stoll(argv[2]);
            if (sv_ll < std::numeric_limits<int>::min() || sv_ll > std::numeric_limits<int>::max()) {
                throw std::out_of_range("Start vertex ID out of int range");
            }
            start_vertex = static_cast<int>(sv_ll);

        }
        catch (const std::invalid_argument& ia) {
            std::string errMsg = "Invalid start vertex ID: '" + std::string(argv[2]) + "'. Must be an integer.";
            Logger::getInstance().error(errMsg);
            std::cerr << "Error: " << errMsg << std::endl;
            throw;
        }
        catch (const std::out_of_range& oor) {
            std::string errMsg = "Start vertex ID out of range: '" + std::string(argv[2]) + "'. " + oor.what();
            Logger::getInstance().error(errMsg);
            std::cerr << "Error: " << errMsg << std::endl;
            throw;
        }

        Graph g;
        Logger::getInstance().info("Loading graph from: " + input_filename);
        g.loadFromFile(input_filename);

        Logger::getInstance().info("Calculating layers for graph, starting from vertex: " + std::to_string(start_vertex));
        std::map<int, std::vector<int>> layered_vertices = g.calculateLayers(start_vertex);

        Logger::getInstance().info("Saving layered vertices to: " + output_filename);
        g.saveLayersToFile(output_filename, layered_vertices);

        std::cout << "Processing complete. Output saved to " << output_filename << std::endl;
        std::cout << "Log saved to application_activity.log" << std::endl;
        Logger::getInstance().info("Processing completed successfully.");

    }
    catch (const std::exception& e) {
        std::string errorMsg = "An critical error occurred: " + std::string(e.what());
        Logger::getInstance().error(errorMsg);
        std::cerr << "Critical Error: " << errorMsg << std::endl;
        Logger::getInstance().info("Application terminated due to a critical error.");
        return 1; // ���������� ��������� ��� ������
    }

    Logger::getInstance().info("Application finished successfully.");
    return 0;
}
