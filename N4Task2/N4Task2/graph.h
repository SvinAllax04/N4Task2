// graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <map>
#include <set> // ��� �������� ���������� ������ � BFS

class Graph {
public:
    Graph();

    // ��������� ���� �� �����. ������: ���� ������ �� �������
    // vertex_id neighbor1 neighbor2 ...
    // ���������� std::runtime_error ��� ������� ����� ��� �������.
    void loadFromFile(const std::string& filename);

    // ������������ ����, ������� �� start_vertex.
    // ���������� map: �����_���� -> ������_������_�_����.
    // ���������� std::invalid_argument, ���� start_vertex ����������� � �����.
    std::map<int, std::vector<int>> calculateLayers(int start_vertex);

    // ��������� ��������� (������� �� �����) � ����.
    // ���������� std::runtime_error ��� ������� �����.
    void saveLayersToFile(const std::string& filename, const std::map<int, std::vector<int>>& layers_output);

    bool hasVertex(int vertex_id) const; // �������� ������� �������
    void clear(); // ������� ������ �����

private:
    std::map<int, std::vector<int>> m_adjList; // ������ ���������

    // ��������������� ����� ��� �������� ������ ����� �����
    bool parseGraphFileLine(const std::string& line, int& vertex, std::vector<int>& neighbors);
};

#endif // GRAPH_H
