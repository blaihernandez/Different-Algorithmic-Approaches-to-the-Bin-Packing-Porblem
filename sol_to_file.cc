#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <system_error>
#include <iomanip>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

string OUTPUT_DIR = "out";


pair<int, double> read_solution(string in_file){
    const char *file = in_file.c_str();
    ifstream in(file);
    double time;
    in >> time;
    int L;
    in >> L;
    return {L, time};
}

int main(int argc, char** argv){
    // Obtener la lista de archivos en el directorio de entrada
    vector<string> files;
    for (const auto &entry : fs::directory_iterator(OUTPUT_DIR)){
        if (entry.path().extension() == ".out"){
            string file = entry.path().string();
            files.push_back(file);
        }
    }

    for (string file : files){
        pair<int, double> res = read_solution(file);
        cout << file << "  |  " << res.first << "  |  " << res.second << endl;
    }


}