#include <iostream>
#include <vector>
#include <fstream>
#include <climits> // For INT_MAX
#include <cmath>

using namespace std;

struct Rectangle {
    int p; // width
    int q; // height
};

struct Coordinates {
    int i;
    int j;
};

struct Position {
    Coordinates ul; // Upper-left
    Coordinates br; // Bottom-right
};

typedef vector<vector<bool>> Matrix;

int N, W, L_MAX;
vector<Rectangle> Rectangles;
int bestL = INT_MAX; 
const char* output_file;

// void escriu_matriu(const Matrix& matriu) {
//     for (const auto& row : matriu) {
//         for (bool cell : row) 
//             if (!cell )cout << "0";
//             else cout << "1";
//         cout << endl;
//     }
//     cout << endl;
// }


double elapsed_time(clock_t start_time){
    clock_t end_time = clock();
    double time_in_seconds = double(end_time - start_time) / CLOCKS_PER_SEC;
    return round(time_in_seconds * 10.0) / 10.0; // Round to 1 decimal place
}

void write_solution(vector<Position>& pSol, int bestL, clock_t start_time) {
    ofstream outfile(output_file); // Open in append mode
    if (!outfile) {
        cerr << "Error: Could not open output file " << output_file << endl;
        return;
    }
    outfile << elapsed_time(start_time) << endl;
    outfile << bestL << endl;
    for (const Position& p : pSol) {
        outfile << p.ul.i << " " << p.ul.j << "     " << p.br.i << " " << p.br.j << endl;
    }
    outfile.close();
}

// Check if a rectangle fits at the given coordinates
bool piece_fits(const Matrix& fabric, const Rectangle& R, const Coordinates& coord, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    if (x + w > W || y + h > L_MAX) return false;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) if (fabric[i][j]) return false;
    }
    return true;
}

// Insert a new rectangle piece
void insert_new_piece(Matrix& fabric, const Rectangle& R, const Coordinates& coord, vector<Position>& pSol, int c, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    pSol[c] = Position{Coordinates{x, y}, Coordinates{x + w - 1, y + h - 1,}};
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) fabric[i][j] = true;
    }
}

// Remove a rectangle piece from the fabric
void delete_piece(Matrix& fabric, const Rectangle& R, const Coordinates& coord, vector<Position>& pSol, int c, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) fabric[i][j] = false;
    }
}

// Calculate the next coordinates
Coordinates set_next_coord(const Coordinates& coord, int d) {
    if (coord.j + d < W) return {coord.i, coord.j + d};
    return {coord.i + 1, 0};
}

// Recursive exhaustive search
void exh_search_recursive(Matrix& fabric, int c, Coordinates currentCoord, int currentL, 
                        vector<Position>& pSol, vector<bool>& used, clock_t st) {

    if (c == N) {
        bestL = min(bestL, currentL);
        write_solution(pSol, bestL, st);
    }
    
    else if (currentL < bestL && currentL <= L_MAX && currentCoord.i <= bestL) {
        for (int i = 0; i < Rectangles.size() && !used[i]; ++i) {
            if (!fabric[currentCoord.i][currentCoord.j]){

                const Rectangle& R = Rectangles[i];

                if (piece_fits(fabric, R, currentCoord, true) && currentCoord.i + R.p <= bestL) {
                    insert_new_piece(fabric, R, currentCoord, pSol, c, true);
                    used[i] = true;
                    Coordinates newCoord = set_next_coord(currentCoord, R.q);
                    if (newCoord.i <= bestL && newCoord.i < L_MAX) {
                        exh_search_recursive(fabric, c + 1, newCoord, max(currentL, currentCoord.i + R.p), pSol, used, st);
                    }
                    delete_piece(fabric, R, currentCoord, pSol, c, true);
                    used[i] = false;
                }

                if (R.p != R.q && piece_fits(fabric, R, currentCoord, false) && currentCoord.i + R.q <= bestL) {
                    insert_new_piece(fabric, R, currentCoord, pSol, c, false);
                    used[i] = true;
                    Coordinates newCoord = set_next_coord(currentCoord, R.p);
                    if (newCoord.i <= bestL && newCoord.i < L_MAX) {
                        exh_search_recursive(fabric, c + 1, newCoord, max(currentL, currentCoord.i + R.q), pSol, used, st);
                    }
                    delete_piece(fabric, R, currentCoord, pSol, c, false);
                    used[i] = false;
                }
            }
        }
        Coordinates nextCoord = set_next_coord(currentCoord, 1);
        if (nextCoord.i <= currentL && nextCoord.i < L_MAX){
            exh_search_recursive(fabric, c, nextCoord, max(currentL, nextCoord.i), pSol, used, st);
        }
    }
}

// Start the exhaustive search
void exh_search() {
    Matrix fabric(L_MAX, vector<bool>(W, false));
    vector<Position> pSol(N);
    vector<bool> used(N, false);
    double start_time = clock();
    Coordinates initial_coords = Coordinates{0, 0};
    exh_search_recursive(fabric, 0, initial_coords, 0, pSol, used, start_time);
}

// Comparator function to sort rectangles by area
bool compareByArea(const Rectangle& a, const Rectangle& b) {
    return a.p * a.q > b.p * b.q; 
}

// Function to sort a list of rectangles by area
void sort_by_area() {
    sort(Rectangles.begin(), Rectangles.end(), compareByArea);
}



int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }
    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error: Could not open input file " << argv[1] << endl;
        return 1;
    }
    output_file = argv[2];
    infile >> W >> N;
    L_MAX = 0;
    int n_units, p, q;
    while (infile >> n_units >> p >> q) {
        for (int _ = 0; _ < n_units; ++_) Rectangles.push_back({p, q});
        L_MAX += q * n_units;
    }
    sort_by_area();
    exh_search();
    cout << bestL;

    return 0;
}



