#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;


struct Order {
    int n; // Remaining pieces yet to be put
    int p;
    int q;
};

struct Coordinates{
    int x;
    int y;
};

// Upper-left and bottom-right coordinates of the rectangle
struct Position{
    Coordinates ul; 
    Coordinates br; 
};

typedef vector<vector<int>> Matrix;

int N, W, L_MAX;
vector<Order> Orders;



// Returns the elapsed time of the execution in seconds.
double elapsed_time(clock_t startTime) {
    clock_t endTime = clock();
    double timeInSeconds = double(endTime - startTime) / CLOCKS_PER_SEC;
    return round(timeInSeconds * 10.0) / 10.0;
}

// Writes the solution sol on the ouput file following the required format. Returns nothing.
void write_solution(char* outputFile, int L, const vector<Position>& sol, clock_t startTime) {
    double time = elapsed_time(startTime);
    ofstream output;
    output.open(outputFile);
    output << time << endl;
    output << L << endl;
    for (Position p : sol){
        output << p.ul.x << " " << p.ul.y << "   " << p.br.x << " " << p.br.y << endl;
    }
    output.close();
}

// Returns a bool based on whether coordinates (j..j + dj, i..i + di) are empty or not
bool piece_fits(const Matrix& Fabric, int di, int dj, int i, int j){
    if(dj + j > W) return false;

    for (int row = i; row < i + di; ++row){
        for (int col = j; col < j + dj; ++col){
            if(Fabric[row][col] != 0) return false;
        } 
    }
    return true;
}

// Inserts a new piece ti the Fabric in the coordinates 
// (j..j + dj, i..i + di). Returns nothing.
void insert_piece(Matrix& Fabric, int di, int dj, int i, int j, int c, vector<Position>& sol){
    for (int fil = i; fil < i + di; ++fil){
        for (int col = j; col < j + dj; ++col) Fabric[fil][col] = c;
    }
    sol[c - 1] = Position{Coordinates{j, i}, Coordinates{j + dj - 1, i + di - 1}};
}

// Returns the next free coordinates of the Fabric moving to the right.
Coordinates set_new_coordinates(const Matrix& Fabric, int i, int j){
    for (int fil = i; fil < L_MAX; ++fil) {
        for(int col = (fil == i) ? j : 0; col < W; ++col) if (!Fabric[fil][col]) return {col, fil};
    }
    return {};
}

// Returns a (possibly) optimal solution to the problem using a greedy algorithm
int greedy(const vector<Order>& Orders, vector<Position>& sol){
    int c = 0; 
    int L = 0;
    int i = 0; int j = 0;
    vector<int> used(Orders.size(), 0);
    Matrix Fabric(L_MAX, vector<int>(W, 0));
    while (c < N) {
        bool in = false;
        for(int s = 0; s < Orders.size() && !in; ++s){

            Order R = Orders[s];
            
            for (int q = 0; q < R.n && !in; ++q){
                if (used[s] < R.n && piece_fits(Fabric, R.p, R.q, i, j)){
                    insert_piece(Fabric, R.p, R.q, i, j, c + 1, sol);
                    in = true; ++c; ++used[s];
                    L = max(L, i + R.p);
                    Coordinates newCoord = set_new_coordinates(Fabric, i, j);
                    i = newCoord.y; j = newCoord.x;
                }
                if (used[s] < R.n && piece_fits(Fabric, R.q, R.p, i, j) && !in){
                    insert_piece(Fabric, R.q, R.p, i, j, c + 1, sol);
                    in = true; ++c; ++used[s];
                    L = max(L, i + R.q);
                    Coordinates newCoord = set_new_coordinates(Fabric, i, j);
                    i = newCoord.y; j = newCoord.x;
                }
            }
        }
        if(!in){
            Coordinates newCoord = set_new_coordinates(Fabric, (j + 1 < W ? i : i + 1), (j + 1 < W ? j + 1 : 0));
            i = newCoord.y; j = newCoord.x;
        }
    }
    return L;
}

// Returns a bool based on whether the area of a is bigger than b's, if it's equal, it compares the largest side.
bool compare_by_area(const Order& a, const Order& b) {
    if (a.p * a.q == b.p * b.q) return a.q > b.q;
    return a.p * a.q > b.p * b.q; 
}

// Sorts the list Orders by area of the rectangle. Returns nothing.
void sort_by_area(vector<Order>& Orders) {
    sort(Orders.begin(), Orders.end(), compare_by_area);
}


int main(int agrc, char** argv) {
    ifstream input;
    input.open(argv[1]);
    input >> W >> N;
    L_MAX = 0;
    int n_units, p, q;
    while (input >> n_units) {
        input >> p >> q;
        Orders.push_back(Order{n_units, p, q});
        L_MAX += q * n_units;
    }
    input.close();
    clock_t startTime = clock();
    vector<Position> sol(N);
    sort_by_area(Orders);
    write_solution(argv[2], greedy(Orders, sol), sol, startTime);
}