#include <iostream>
#include <vector>
#include <fstream>
#include <climits> 
#include <cmath>
#include <unordered_set>
#include <algorithm>
#include <string>

using namespace std;

struct Order {
    int n; // Remaining pieces yet to be put
    int p; 
    int q; 
};

struct Coordinates {
    int i;
    int j;
};

// Upper-left and bottom-right coordinates of the rectangle
struct Position {
    Coordinates ul; 
    Coordinates br; 
};

typedef vector<vector<bool>> Matrix;
typedef vector<Position> Positions;


int N, W, L_MAX, L_MIN, bestL;  // best solution found so far
vector<Order> Orders;
const char* outputFile;


// Given the start time in 'clocks', returns the elapsed time of the execution in seconds
double elapsed_time(clock_t startTime){
    clock_t endTime = clock();
    double timeInSeconds = double(endTime - startTime) / CLOCKS_PER_SEC;
    return round(timeInSeconds * 10.0) / 10.0; // Round to 1 decimal place
}

// Writes the solution in the output file, re-writes it if there was already one.
void write_solution(vector<Position>& pSol, int bestL, clock_t startTime) {
    ofstream outfile(outputFile); // Open in append mode
    outfile << elapsed_time(startTime) << endl;
    outfile << bestL << endl;
    for (const Position& p : pSol) {
        outfile << p.ul.i << " " << p.ul.j << "     " << p.br.i << " " << p.br.j << endl;
    }
    outfile.close();
}

// Returns a bool based on whether the piece from the order R fits in the coordinates 
// coord of the Fabric (reversed or not)
bool piece_fits(const Matrix& Fabric, const Order& R, const Coordinates& coord, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    if (x + w > W || y + h > L_MAX) return false;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) if (Fabric[i][j]) return false;
    }
    return true;
}

// Inserts a new piece (reversed or nnot) into the Fabric with its upper-left coordinate in coord
// and adds it to the partial solution pSol. Returns nothing.
void insert_new_piece(Matrix& Fabric, const Order& R, const Coordinates& coord, 
                        Positions& pSol, int c, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    pSol[c] = Position{Coordinates{x, y}, Coordinates{x + w - 1, y + h - 1,}};
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) Fabric[i][j] = true; 
    }
}

// Removes the piece R sitting in the coordinates coord from the Fabric (reversed or not).
// Returns nothing.
void delete_piece(Matrix& Fabric, const Order& R, const Coordinates& coord, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) Fabric[i][j] = false;
    }
}

// Returns the next coordinates to visit moving d squares to the right.
// If out of bounds, then jumps to the next row.
Coordinates set_next_coord(const Coordinates& coord, int d) {
    if (coord.j + d < W) return Coordinates{coord.i, coord.j + d};
    return Coordinates{coord.i + 1, 0};
}

// Recursive exhaustive search funtion. Parameters are a matrix representing the
// Fabric roll, an integer c representing the number or rectangles used, the current
// coordinates currCoord, the current length currL, partial solution pSol and start time st.
// Every time it finds a better solution, re-writes it in the output file. Returns nothing.
void exh_search_rec(Matrix& Fabric, int c, Coordinates currCoord, 
                    int currL, Positions& pSol, clock_t st) {

    if (c == N) {
        bestL = min(bestL, currL);
        write_solution(pSol, bestL, st);
    }
    
    else if (currL < bestL && currL <= L_MAX && currCoord.i < bestL && bestL > L_MIN) {
        for (int i = 0; i < Orders.size(); ++i) {
            if (Orders[i].n && !Fabric[currCoord.i][currCoord.j]) {

                Order& R = Orders[i];

                if (currCoord.i + R.p < bestL && piece_fits(Fabric, R, currCoord, true)) {
                    Coordinates newCoord = set_next_coord(currCoord, R.q);
                    if (newCoord.i < bestL && newCoord.i < L_MAX) {
                        insert_new_piece(Fabric, R, currCoord, pSol, c, true);
                        --R.n;
                        exh_search_rec(Fabric, c + 1, newCoord, max(currL, currCoord.i + R.p), pSol, st);
                        delete_piece(Fabric, R, currCoord, true);
                        ++R.n;
                    }
                }
                if (R.p != R.q && currCoord.i + R.q < bestL && piece_fits(Fabric, R, currCoord, false)) {
                    Coordinates newCoord = set_next_coord(currCoord, R.p);
                    if (newCoord.i < bestL && newCoord.i < L_MAX) {
                        insert_new_piece(Fabric, R, currCoord, pSol, c, false);
                        --R.n;
                        exh_search_rec(Fabric, c + 1, newCoord, max(currL, currCoord.i + R.q), pSol, st);
                        delete_piece(Fabric, R, currCoord, false);
                        ++R.n;
                    }
                }
            }
        }
        int d = 1; // right displacement
        Coordinates nextCoord = set_next_coord(currCoord, d);
        if (nextCoord.i < bestL && nextCoord.i < L_MAX) {
            exh_search_rec(Fabric, c, nextCoord, max(currL, nextCoord.i), pSol, st);
        }
    }
}

// Sets the initial parameters the exhaustive search. Returns nothing
void exh_search() {
    Matrix Fabric(L_MAX, vector<bool>(W, false));
    Coordinates initialCoords = Coordinates{0, 0};
    Positions pSol(N);
    double startTime = clock();
    bestL = INT_MAX;
    exh_search_rec(Fabric, 0, initialCoords, 0, pSol, startTime);
}

// Returns a bool based on whether the area of a is bigger than b's.
bool compare_by_area(const Order& a, const Order& b) {
    return a.p * a.q > b.p * b.q; 
}

// Sorts the list Orders by area of the rectangle. Returns nothing.
void sort_by_area() {
    sort(Orders.begin(), Orders.end(), compare_by_area);
}


int main(int argc, char* argv[]) {

    ifstream infile(argv[1]);
    outputFile = argv[2];
    infile >> W >> N;
    L_MAX = 0; // maximum possible lenght of the Fabric (sum(q * n))
    int area = 0;
    int nUnits, p, q;
    while (infile >> nUnits >> p >> q) {
        Orders.push_back(Order{nUnits, p, q});
        L_MAX += q * nUnits;
        area += p * q * nUnits;
    }
    L_MIN = (area % W == 0) ? (area / W) : (area / W + 1); // Optimitzation: best theoretical solution
    sort_by_area();
    exh_search();

    return 0;
}
