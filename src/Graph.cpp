#include "Graph.h"

void Graph::addAirport(Airport airport)
{
    if (airports.find(airport.code) == airports.end()) {
        airports[airport.code] = airport;
    }
}

void Graph::addEdge(std::string srcCode, std::string destCode, double distance)
{
    adjList[srcCode].emplace_back(destCode, distance);
    adjList[destCode].emplace_back(srcCode, distance); // Grafo no dirigido

    edges.push_back({ srcCode, destCode, distance });  // Agregar a la lista de aristas
}


// por lineas como esta TRD,Trondheim Airport Værnes,Trondheim,Norway,63.4578018,10.9239998,TRF,"Sandefjord Airport, Torp",Sandefjord,Norway,59.18669891,10.25860024
// Me toco cambiar porque algunos nombres que esta nentre comillas tienen una "," mal parqueada 
void Graph::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue; // Saltar la primera línea (cabeceras)
        }

        // Procesar cada línea respetando las comillas
        std::vector<std::string> tokens;
        std::string token;
        bool inQuotes = false;

        for (char ch : line) {
            if (ch == '\"') {
                inQuotes = !inQuotes; // Cambiar estado si se encuentra una comilla doble
            }
            else if (ch == ',' && !inQuotes) {
                // Si encontramos una coma y no estamos dentro de comillas, separamos el token
                tokens.push_back(token);
                token.clear();
            }
            else {
                // Si es un carácter normal o una coma dentro de comillas, agregamos al token
                token += ch;
            }
        }
        tokens.push_back(token); // Agregar el último token

        // Verificar que tengamos el número correcto de columnas (12)
        if (tokens.size() != 12) {
            std::cerr << "Error: Línea malformada o incompleta: " << line << std::endl;
            continue; // Saltar esta línea si no tiene 12 columnas
        }

        // Asignar los valores a las variables correspondientes
        std::string sourceCode = tokens[0];
        std::string sourceName = tokens[1];
        std::string sourceCity = tokens[2];
        std::string sourceCountry = tokens[3];
        std::string sourceLat = tokens[4];
        std::string sourceLon = tokens[5];
        std::string destCode = tokens[6];
        std::string destName = tokens[7];
        std::string destCity = tokens[8];
        std::string destCountry = tokens[9];
        std::string destLat = tokens[10];
        std::string destLon = tokens[11];

        // Verificar si alguno de los campos necesarios está vacío
        if (sourceCode.empty() || sourceLat.empty() || sourceLon.empty() ||
            destCode.empty() || destLat.empty() || destLon.empty()) {
            std::cerr << "Error: Faltan datos en la línea: " << line << std::endl;
            continue; // Saltar esta línea
        }

        try {
            // Convertir las coordenadas a double
            double sourceLatitude = std::stod(sourceLat);
            double sourceLongitude = std::stod(sourceLon);
            double destLatitude = std::stod(destLat);
            double destLongitude = std::stod(destLon);

            // Crear aeropuertos
            Airport sourceAirport = { sourceCode, sourceName, sourceCity, sourceCountry, sourceLatitude, sourceLongitude };
            Airport destAirport = { destCode, destName, destCity, destCountry, destLatitude, destLongitude };

            // Añadir los aeropuertos al grafo
            addAirport(sourceAirport);
            addAirport(destAirport);

            // Calcular la distancia y agregar la arista
            double distance = haversine(sourceLatitude, sourceLongitude, destLatitude, destLongitude);
            addEdge(sourceCode, destCode, distance);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Error: Datos inválidos en la línea: " << line << std::endl;
        }
    }
}

void Graph::DFS(const std::string& airportCode, std::unordered_set<std::string>& visited)
{
    visited.insert(airportCode);

    for (const auto& neighbor : adjList[airportCode]) {
        if (visited.find(neighbor.first) == visited.end()) {
            DFS(neighbor.first, visited);
        }
    }
}

bool Graph::isConnected()
{
    std::unordered_set<std::string> visited;

    // Iniciar DFS desde cualquier vértice (primer aeropuerto en la lista)
    if (!airports.empty()) {
        DFS(airports.begin()->first, visited);
    }

    // Si el número de vértices visitados es igual al número total de vértices, el grafo es conexo
    return visited.size() == airports.size();
}

void Graph::findComponents()
{
    std::unordered_set<std::string> visited;
    int components = 0;

    for (const auto& airport : airports) {
        if (visited.find(airport.first) == visited.end()) {
            components++;
            std::unordered_set<std::string> componentVisited;
            DFS(airport.first, componentVisited);

            // Agregar los aeropuertos visitados en esta componente
            visited.insert(componentVisited.begin(), componentVisited.end());
            std::cout << "Componente " << components << " tiene " << componentVisited.size() << " vértices." << std::endl;
        }
    }

    std::cout << "Número de componentes conexas: " << components << std::endl;
}

std::vector<std::pair<std::string, double>> Graph::dijkstra(const std::string& start) {
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> previous;  // Para rastrear el camino si es necesario
    for (const auto& airport : airports) {
        distances[airport.first] = std::numeric_limits<double>::infinity();
    }
    distances[start] = 0;

    // Min-heap para procesar los nodos de menor distancia primero
    std::priority_queue<std::pair<double, std::string>, std::vector<std::pair<double, std::string>>, std::greater<>> pq;
    pq.push({ 0, start });

    while (!pq.empty()) {
        auto [currentDistance, currentAirport] = pq.top();
        pq.pop();

        // Iterar sobre los vecinos del aeropuerto actual
        for (const auto& neighbor : adjList[currentAirport]) {
            double newDist = currentDistance + neighbor.second;
            if (newDist < distances[neighbor.first]) {
                distances[neighbor.first] = newDist;
                pq.push({ newDist, neighbor.first });
                previous[neighbor.first] = currentAirport;  // Mantener el seguimiento del camino
            }
        }
    }

    // Convertir las distancias a un vector de pares (aeropuerto, distancia)
    std::vector<std::pair<std::string, double>> result(distances.begin(), distances.end());

    // Ordenar por distancia descendente para obtener los caminos más largos
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });

    return result;
}

std::unordered_map<std::string, std::vector<const std::string*>> Graph::dijkstraPaths(const std::string& start) {
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> previous;  // Para rastrear el camino
    std::unordered_map<std::string, std::vector<const std::string*>> paths;  // Para almacenar los caminos

    // Inicialización: todas las distancias son infinitas, excepto para el aeropuerto inicial
    for (const auto& airport : airports) {
        distances[airport.first] = std::numeric_limits<double>::infinity();
        paths[airport.first] = {};  // Inicialmente, no hay camino para ningún aeropuerto
    }
    distances[start] = 0;

    // Cola de prioridad para procesar los aeropuertos con menor distancia
    std::priority_queue<std::pair<double, std::string>, std::vector<std::pair<double, std::string>>, std::greater<>> pq;
    pq.push({ 0, start });

    while (!pq.empty()) {
        auto [currentDistance, currentAirport] = pq.top();
        pq.pop();

        // Iterar sobre los vecinos del aeropuerto actual
        for (const auto& neighbor : adjList[currentAirport]) {
            double newDist = currentDistance + neighbor.second;

            // Si encontramos un camino más corto hacia el vecino
            if (newDist < distances[neighbor.first]) {
                distances[neighbor.first] = newDist;
                pq.push({ newDist, neighbor.first });
                previous[neighbor.first] = currentAirport;  // Mantener el aeropuerto anterior

                // Reconstruir el camino para este vecino
                std::vector<const std::string*> path;
                std::string current = neighbor.first;

                // Recorremos el mapa 'previous' hacia atrás, desde el destino hasta el origen
                while (current != start) {
                    path.push_back(&airports[current].code);  // Agregamos el aeropuerto actual
                    current = previous[current];  // Nos movemos al aeropuerto anterior
                }
                path.push_back(&airports[start].code);  // Agregar el origen al final del camino

                // Invertir el camino para que vaya desde el origen hasta el destino
                std::reverse(path.begin(), path.end());

                // Almacenar el camino reconstruido
                paths[neighbor.first] = path;
            }
        }
    }

    return paths;  // Devolver el mapa con los caminos
}

double Graph::getDistance(const std::string& airportCode1, const std::string& airportCode2) const {
    // Verificar si ambos códigos de aeropuerto existen
    if (adjList.find(airportCode1) == adjList.end() || adjList.find(airportCode2) == adjList.end()) {
        return std::numeric_limits<double>::infinity(); // Retornar infinito si no existen
    }

    // Recorrer la lista de adyacencia de airportCode1
    for (const auto& neighbor : adjList.at(airportCode1)) {
        if (neighbor.first == airportCode2) {
            return neighbor.second; // Retornar la distancia si se encuentra
        }
    }

    return std::numeric_limits<double>::infinity(); // Retornar infinito si no se encuentra la distancia
}

std::vector<std::pair<std::string, double>> Graph::dijkstraLargo(const std::string& start) {
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> previous;  // Para rastrear el camino si es necesario
    for (const auto& airport : airports) {
        distances[airport.first] = std::numeric_limits<double>::infinity();
    }
    distances[start] = 0;

    // Min-heap para procesar los nodos de menor distancia primero
    std::priority_queue<std::pair<double, std::string>, std::vector<std::pair<double, std::string>>, std::greater<>> pq;
    pq.push({ 0, start });

    while (!pq.empty()) {
        auto [currentDistance, currentAirport] = pq.top();
        pq.pop();

        // Iterar sobre los vecinos del aeropuerto actual
        for (const auto& neighbor : adjList[currentAirport]) {
            double newDist = currentDistance + neighbor.second;
            if (newDist < distances[neighbor.first]) {
                distances[neighbor.first] = newDist;
                pq.push({ newDist, neighbor.first });
                previous[neighbor.first] = currentAirport;  // Mantener el seguimiento del camino
            }
        }
    }

    // Convertir las distancias a un vector de pares (aeropuerto, distancia)
    std::vector<std::pair<std::string, double>> result;

    // Filtrar las distancias que no sean infinitas
    for (const auto& entry : distances) {
        if (entry.second != std::numeric_limits<double>::infinity()) {
            result.push_back(entry);
        }
    }

    // Ordenar por distancia descendente para obtener los caminos más largos
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });


    return result;
}


void Graph::findComponentsAndCalculateMST()
{
    std::unordered_set<std::string> visited;
    int componentCount = 0;

    for (const auto& airport : airports) {
        if (visited.find(airport.first) == visited.end()) {
            componentCount++;
            std::unordered_set<std::string> componentVisited;
            DFS(airport.first, componentVisited);

            // Agregar los aeropuertos visitados en esta componente al conjunto total
            visited.insert(componentVisited.begin(), componentVisited.end());

            // Calcular el MST de esta componente
            double mstWeight = kruskalMST(componentVisited);
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "El peso del MST de la componente " << componentCount << " es: " << mstWeight << " km." << std::endl;
        }
    }

    std::cout << "Número total de componentes conexas: " << componentCount << std::endl;
}

double Graph::kruskalMST(const std::unordered_set<std::string>& component)
{
    DisjointSet ds;
    for (const auto& airport : component) {
        ds.makeSet(airport);
    }

    // Filtrar las aristas que solo conectan nodos dentro de la componente
    std::vector<Edge> componentEdges;
    for (const auto& edge : edges) {
        if (component.find(edge.src) != component.end() && component.find(edge.dest) != component.end()) {
            componentEdges.push_back(edge);
        }
    }

    // Ordenar las aristas por peso (distancia)
    std::sort(componentEdges.begin(), componentEdges.end(), [](const Edge& a, const Edge& b) {
        return a.weight < b.weight;
        });

    double mstWeight = 0;
    int edgesInMST = 0;

    // Agregar aristas al MST
    for (const auto& edge : componentEdges) {
        if (ds.find(edge.src) != ds.find(edge.dest)) {
            ds.unionSets(edge.src, edge.dest);
            mstWeight += edge.weight;
            edgesInMST++;
            if (edgesInMST == component.size() - 1) break; // MST completado
        }
    }

    return mstWeight;
}

