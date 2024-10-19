#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Graph.h"

void opcion1(Graph& g) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    if (g.isConnected()) {
        std::cout << "El grafo es conexo." << std::endl;
    }
    else {
        std::cout << "El grafo no es conexo." << std::endl;
        std::cout << "Buscando componentes conexas..." << std::endl;
        g.findComponents();
    }
}
void opcion2(Graph& g) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Calculando el peso del arbol de expansion minima (MST)..." << std::endl;
    g.findComponentsAndCalculateMST();
}
void opcion3(Graph& g) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::string airportCode1, airportCode2;

    std::cout << "Ingrese el codigo del aeropuerto: ";
    std::cin >> airportCode1;

    if (g.airports.find(airportCode1) != g.airports.end()) {
        Airport a = g.airports[airportCode1];
        std::cout << "Informacion del aeropuerto:\n"
            << "Codigo: " << a.code << "\nNombre: " << a.name
            << "\nCiudad: " << a.city << "\nPais: " << a.country
            << "\nLatitud: " << a.latitude << "\nLongitud: " << a.longitude << std::endl;
    }
    else {
        std::cout << "El aeropuerto con codigo " << airportCode1 << " no existe en el grafo." << std::endl;
    }
}
void opcion4(Graph& g) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::string airportCode1, airportCode2;
    std::cout << "Ingrese el codigo del aeropuerto de inicio: ";
    std::cin >> airportCode1;

    if (g.airports.find(airportCode1) != g.airports.end()) {
        std::vector<std::pair<std::string, double>> longestPaths = g.dijkstraLargo(airportCode1);
        std::cout << "Los 10 aeropuertos mas lejanos desde " << airportCode1 << " son:" << std::endl;

        for (int i = 0; i < 10 && i < longestPaths.size(); ++i) {
            const auto& [code, distance] = longestPaths[i];
            Airport a = g.airports[code];
            std::cout << i + 1 << ". " << a.code << " (" << a.name << ", " << a.city << ", " << a.country
                << ") - Distancia: " << distance << " km" << std::endl;
        }
    }
    else {
        std::cout << "El aeropuerto con codigo " << airportCode1 << " no existe en el grafo." << std::endl;
    }
}

void opcion5(Graph& g) {
    std::string airportCode1, airportCode2;
    std::cout << "Ingrese el código del aeropuerto de origen: ";
    std::cin >> airportCode1;
    std::cout << "Ingrese el código del aeropuerto de destino: ";
    std::cin >> airportCode2;

    if (g.airports.find(airportCode1) != g.airports.end() && g.airports.find(airportCode2) != g.airports.end()) {
        // Ejecutar Dijkstra para obtener los caminos mínimos desde airportCode1
        std::unordered_map<std::string, std::vector<const std::string*>> paths = g.dijkstraPaths(airportCode1);

        // Verificar si existe un camino hacia airportCode2
        if (!paths[airportCode2].empty()) {
            std::cout << "El camino mínimo entre " << airportCode1 << " y " << airportCode2 << " es: " << std::endl;

            // Mostrar el camino
            int i(1);
            for (const auto* codePtr : paths[airportCode2]) {
                std::cout << "Aeropuerto " << i << std::endl;
                i++;
                g.airports[*codePtr].print(); // Mostrar información del aeropuerto
                std::cout << std::endl;
            }
            std::cout << "FIN" << std::endl;

            // Generar archivo DOT para Graphviz
            std::ofstream dotFile("camino.dot");
            dotFile << "digraph G {" << std::endl;
            dotFile << "rankdir=LR;" << std::endl; // Para que el grafo sea horizontal (opcional)
            dotFile << "node [shape=circle, style=filled, color=lightblue];" << std::endl;

            // Agregar todos los nodos (aeropuertos en el camino)
            const auto& path = paths[airportCode2];
            for (size_t j = 0; j < path.size() - 1; ++j) {
                // Obtener detalles del aeropuerto
                const auto& airportInfo1 = g.airports[*path[j]];     // Información del primer aeropuerto
                const auto& airportInfo2 = g.airports[*path[j + 1]]; // Información del siguiente aeropuerto

                // Incluir más detalles del aeropuerto en lugar del código solo
                std::string label1 = airportInfo1.code + "\\n -" + airportInfo1.name + "\\n -" + airportInfo1.city + ", " + airportInfo1.country + "\\n -" + "Latitud: " + std::to_string(airportInfo1.latitude) + "\\n" + "-Longitud: " + std::to_string(airportInfo1.longitude);
                std::string label2 = airportInfo2.code + "\\n -" + airportInfo2.name + "\\n -" + airportInfo2.city + ", " + airportInfo2.country + "\\n -" + "Latitud: " + std::to_string(airportInfo2.latitude) + "\\n" + "-Longitud: " + std::to_string(airportInfo2.longitude);

                // Generar la línea del archivo DOT para este tramo del camino
                dotFile << "\"" << label1 << "\" -> \"" << label2 << "\" [label=\"" << g.getDistance(*path[j], *path[j + 1]) << " km\"];" << std::endl;
            }

            dotFile << "}" << std::endl;
            dotFile.close();

            // Usar Graphviz para generar una imagen PNG desde el archivo DOT
            system("dot -Tpng camino.dot -o camino.png");
            std::cout << "El grafo del camino mínimo ha sido generado en 'camino.png'." << std::endl;
        }
        else {
            std::cout << "No hay camino entre " << airportCode1 << " y " << airportCode2 << "." << std::endl;
        }
    }
    else {
        std::cout << "Uno o ambos aeropuertos no existen en el grafo." << std::endl;
    }
}

int main() {
    Graph g;
    std::string filename = "flights_final.csv";

    // Cargar datos desde el CSV
    std::cout << "Cargando datos desde el archivo " << filename << "..." << std::endl;
    g.loadFromCSV(filename);
    std::cout << "Datos cargados exitosamente. Se han añadido " << g.airports.size() << " aeropuertos." << std::endl;

    int option;

    do {
        std::cout << "\nMenu de acciones sobre el grafo de rutas aereas:" << std::endl;
        std::cout << "1. Verificar si el grafo es conexo." << std::endl;
        std::cout << "2. Calcular el peso del arbol de expansion minima (MST)." << std::endl;
        std::cout << "3. Mostrar informacion de un aeropuerto por su codigo." << std::endl;
        std::cout << "4. Mostrar los 10 aeropuertos mas lejanos desde un aeropuerto dado." << std::endl;
        std::cout << "5. Mostrar el camino minimo entre dos aeropuertos dados." << std::endl;
        std::cout << "7. Salir." << std::endl;
        std::cout << "Seleccione una opcion: ";
        try
        {
            std::cin >> option;

        }
        catch (const std::exception&)
        {
            std::cout << "Error al elegir una opcion" << std::endl;
        }

        switch (option) {
        case 1: {
            opcion1(g);
            break;
        }
        case 2: {
            opcion2(g);
            break;
        }
        case 3: {
            opcion3(g);
            break;
        }
        case 4: {
            opcion4(g);
            break;
        }
        case 5: {
            opcion5(g);
            break;
        }
        case 6:
            std::cout << "Saliendo del programa." << std::endl;
            break;
        default:
            std::cout << "Opcion invalida, por favor intente de nuevo." << std::endl;
        }

        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

    } while (option != 6);

    return 0;
}