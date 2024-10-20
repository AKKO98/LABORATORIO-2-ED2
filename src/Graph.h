#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "Airport.h"
#include <cmath>
#include <unordered_set>
#include <queue>
#include <limits>
#include <map>
#include <iomanip>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


class Graph {
public:
    std::unordered_map<std::string, Airport> airports;
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> adjList;

    void addAirport(Airport airport);

    void addEdge(std::string srcCode, std::string destCode, double distance);

    void loadFromCSV(const std::string& filename);

    void DFS(const std::string& airportCode, std::unordered_set<std::string>& visited);

    bool isConnected();

    void findComponents();

    std::vector<std::pair<std::string, double>> dijkstra(const std::string& start);

    std::unordered_map<std::string, std::vector<const std::string*>> dijkstraPaths(const std::string& start);

    double getDistance(const std::string& airportCode1, const std::string& airportCode2) const;


    std::vector<std::pair<std::string, double>> dijkstraLargo(const std::string& start);

    void findComponentsAndCalculateMST();

    double kruskalMST(const std::unordered_set<std::string>& component);



private:

    struct Edge {
        std::string src, dest;
        double weight;
    };

    std::vector<Edge> edges;

    inline double deg2rad(double deg) { return deg * (M_PI / 180); }

    //Calcular la distancia entre dos puntos de latitud y longitud
    inline double haversine(double lat1, double lon1, double lat2, double lon2) {
        double R = 6371;
        double dLat = deg2rad(lat2 - lat1);
        double dLon = deg2rad(lon2 - lon1);
        double a = sin(dLat / 2) * sin(dLat / 2) +
            cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
            sin(dLon / 2) * sin(dLon / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        double distance = R * c;
        return distance;
    }

    // Estructura para conjuntos disjuntos (Union-Find)
    struct DisjointSet {
        std::unordered_map<std::string, std::string> parent;
        std::unordered_map<std::string, int> rank; // Esto solo se usa para cuando se quiera unir dos subconjustos, se una el más pequeño al más grande

        void makeSet(const std::string& airportCode) {
            parent[airportCode] = airportCode;
            rank[airportCode] = 0;
        }

        //Encuentra la raiz del subconjunto al que pertenece un elemento
        //Esto permite verificar si dos elementos pertenecen al mismo subconjunto
        std::string find(const std::string& airportCode) {
            if (parent[airportCode] != airportCode)
                parent[airportCode] = find(parent[airportCode]);
            return parent[airportCode];
        }

        //Este solo une los dos subconjustos en uno solo
        void unionSets(const std::string& airportCode1, const std::string& airportCode2) {
            std::string root1 = find(airportCode1);
            std::string root2 = find(airportCode2);

            if (root1 != root2) {
                if (rank[root1] > rank[root2]) {
                    parent[root2] = root1;
                }
                else if (rank[root1] < rank[root2]) {
                    parent[root1] = root2;
                }
                else {
                    parent[root2] = root1;
                    rank[root1]++;
                }
            }
        }
    };

};
