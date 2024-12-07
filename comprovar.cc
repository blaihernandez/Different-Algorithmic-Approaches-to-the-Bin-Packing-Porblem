// Funciona en linux

// Compilar
// g++ -Wall -O3 comprovar.cc -o comp.x

// Ejecutar normal
// ./comp.x

// Filtrar errores
// ./comp.x 2> errores.txt

// Debe existir el archivo "INPUT_DIR/solutions.txt"
// y "INPUT_DIR" debe contener todos los input files.
// la salida se guardara en la carpeta OUTPUT_DIR

// ¡¡¡¡Cambiar variables globales!!!

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <system_error>
#include <iomanip>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace fs = std::filesystem;
using namespace std;

// Modificar
int MAX_TIME = 5; // segundos
string PROGRAM_NAME = "greedy.out";
string INPUT_DIR = "instances/small";
string OUTPUT_DIR = "out";

vector<pair<string, bool>> errores(0);
string last_timeout;

int run_command(const string& command) {
    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
        exit(1); // Si execl falla
    }
    int status;
    waitpid(pid, &status, 0); // Espera al proceso hijo
    return status;
}

int read_solution(string in_file){
    const char *file = in_file.c_str();
    ifstream in(file);
    double time;
    in >> time;
    int L;
    in >> L;
    return L;
}

int get_best_sol(string in_file){
    string file; // Use std::string for file name
    string opt;  // Use std::string for opt
    ifstream in(INPUT_DIR+"/solutions.txt");

    if (!in){
        cerr << "Error: Could not open the solutions file." << endl;
        return 1;
    }

    int bs;
    while (in >> file){
        in >> opt;
        in >> bs;

        if (file == in_file) break;
    }

    in.close();
    return bs;
}

void check_solution(filesystem::path input_file, fs::path output_file){
    string inp_file = input_file.string().c_str();
    string in_file = inp_file.substr(16);
    string out_file = output_file.string().c_str();
    int b = get_best_sol(in_file);
    int l = read_solution(out_file);
    if (b == l)
        cout << "found best solution: " << b << endl;
    else {
        cerr << "For file " << in_file << " found " << l << " and not " << b << " | Dif: " << l - b << endl;
        if (last_timeout == inp_file)
            errores.push_back({in_file, true});
        else
            errores.push_back({in_file, false});
    }
}

int main(){
    auto p_start = chrono::steady_clock::now();

    // Crear el directorio de salida si no existe
    if (!fs::exists(OUTPUT_DIR)){
        fs::create_directory(OUTPUT_DIR);
    }

    // Obtener la lista de archivos en el directorio de entrada
    vector<fs::path> files;
    for (const auto &entry : fs::directory_iterator(INPUT_DIR)){
        if (entry.path().extension() == ".inp"){
            files.push_back(entry.path());
        }
    }

    // Barra de progreso
    size_t total_files = files.size();
    size_t processed_files = 0;
    int timeout = 0;

    for (const auto &input_file : files){
        cout << "Procesando archivo " << input_file << "..." << endl;

        // Archivo de salida
        fs::path output_file = fs::path(OUTPUT_DIR) / input_file.filename().replace_extension(".out");

        try{
            // Ejecutar exh.x con timeout (simulado con std::this_thread::sleep_for para este ejemplo)
            auto start = chrono::steady_clock::now();
            int exh_result = run_command(("timeout " + to_string(MAX_TIME) + "s ./" + PROGRAM_NAME + " " + input_file.string() + " " + output_file.string()).c_str());

            // Simular timeout o error
            if (exh_result != 0){
                throw runtime_error("Timeout o error en la ejecución de exh.x  ");
            }

            // Verificar tiempo de ejecución (opcional)
            auto end = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(end - start);
            cout << elapsed.count() << " segundos" << endl;
        }
        catch (const runtime_error &e){
            cerr << e.what() << input_file.string() << endl;
            ++timeout;
            last_timeout = input_file.string().c_str();
        }

        // Ejecutar checker.x
        int checker_result = system(("./checker.out " + input_file.string() + " " + output_file.string()).c_str());
        if (checker_result != 0){
            cerr << "Error al ejecutar checker.x para el archivo " << input_file << endl;
        }
        else check_solution(input_file, output_file);

        // Actualizar barra de progreso
        ++processed_files;
        cout << endl
             << "Progreso: " << fixed << setprecision(2)
             << (static_cast<double>(processed_files) / total_files) * 100 << "% ("
             << processed_files << "/" << total_files << ")" << endl
             << endl;
    }

    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(end - p_start);

    cout << "Todos los archivos han sido procesados en " << elapsed.count() << " segundos." << endl
         << endl;
    cout << "-----------------SUMMARY-------------------" << endl
         << endl;
    cout << " 1) " << timeout << " de " << total_files << " archivos no han acabado a tiempo" << endl
         << endl;
    cout << " 2) Archivos que no se ha encontrado la solución optima" << endl;
    for (const auto &er : errores)
        cout << "    - " << er.first << " | Con timeout " << er.second << endl;
    cout << endl;
    return 0;
}
