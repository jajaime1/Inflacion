#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <omp.h>

struct Transaccion {
    std::string sku;
    std::string nombre;
    int monto;
    int descuento;
    std::string fecha;
    std::string estado;
};

int main() {
    std::ifstream archivo("supermercado.csv"); // Nombre del archivo CSV

    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo." << std::endl;
        return 1;
    }

    std::string linea;
    std::getline(archivo, linea); // Leer la primera línea (encabezados) y descartarla

    // Variables para almacenar la inflación
    int inflacion = 0;
    int monto;
    std::string estado;

    // Procesar cada línea del archivo en paralelo
    #pragma omp parallel private(linea, monto, estado)
    {
        #pragma omp single
        {
            std::cout << "Número de hilos: " << omp_get_num_threads() << std::endl;
        }

        while (std::getline(archivo, linea)) {
            std::stringstream ss(linea);
            std::string campo;

            std::getline(ss, campo, ';'); // SKU
            std::string sku = campo;

            std::getline(ss, campo, ';'); // NOMBRE
            std::string nombre = campo;

            std::getline(ss, campo, ';'); // MONTO
            try {
                monto = std::stoi(campo);
            } catch (const std::invalid_argument& e) {
                std::cout << "Error al convertir el monto en la línea: " << linea << std::endl;
                continue;
            }

            std::getline(ss, campo, ';'); // DESCUENTO
            // Omitido para la versión paralela

            std::getline(ss, campo, ';'); // FECHA
            std::string fecha = campo;

            std::getline(ss, campo, ';'); // ESTADO
            estado = campo;

            #pragma omp task
            {
                if (estado == "AUTHORIZED" || estado == "FINALIZED") {
                    #pragma omp atomic
                    inflacion += monto;
                }
            }
        }
    }

    archivo.close();

    std::cout << "La inflación total es: " << inflacion << std::endl;

    return 0;
}
