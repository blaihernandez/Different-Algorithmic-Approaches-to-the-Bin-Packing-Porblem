#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

struct Order {
    int n; /* nombre de Order de tipus (width x height)*/
    int p;
    int q;
};

struct Coordinates{
    int x;
    int y;
};

struct Position{
    Coordinates ul;
    Coordinates br;
};

typedef vector<vector<bool>> FR;
int N, W, L_max;
vector<Order> orders;

void escriu(vector<vector<int>> matriu) {
    for(int i = 0; i < matriu.size(); ++i) {
        for(int j = 0; j < matriu[0].size(); ++j) {
            if (matriu[i][j] < 10) cout << matriu[i][j] << "  ";
            else cout << matriu[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void ordenar_rectangles(vector<Order>& orders) {
    sort(orders.begin(), orders.end(), [](const Order& a, const Order& b) {
        if (a.q != b.q) return a.q > b.q;
        else return a.p > b.p;
    });
}

bool cap(int di, int dj, int i, int j, vector<vector<int>>& teler){
    if(dj + j > W) return false;
    for (int fil = i; fil < i + di; ++fil){
        for (int col = j; col < j + dj; ++col) if(teler[fil][col] != 0) return false;
    }
    return true;
}

void poso(int di, int dj, int i, int j, vector<vector<int>>& teler, int c, vector<Position>& sol){
    for (int fil = i; fil < i + di; ++fil){
        for (int col = j; col < j + dj; ++col) teler[fil][col] = c;
    }
    sol[c - 1] = Position{Coordinates{j, i}, Coordinates{j + dj - 1, i + di - 1}};
}

Coordinates set_new_coordinates(int i, int j, vector<vector<int>>& teler){
    for (int fil = i; fil < L_max; ++fil) {
        for(int col = (fil == i) ? j : 0; col < W; ++col) if (!teler[fil][col]) return {col, fil};
    }
    return {};
}

double elapsed_time(clock_t start_time) {
    clock_t end_time = clock();
    double time_in_seconds = double(end_time - start_time) / CLOCKS_PER_SEC;
    return round(time_in_seconds * 10.0) / 10.0;
}

void escriu_solucio(char* sortida, int L, const vector<Position>& sol, clock_t start_time) {
    double temps = elapsed_time(start_time);
    ofstream output;
    output.open(sortida);
    output << temps << endl;
    output << L << endl;
    for (Position pos : sol){
        output << pos.ul.x << " " << pos.ul.y << "   " << pos.br.x << " " << pos.br.y << endl;
    }
    output.close();
}

void greedy(char* sortida){
    clock_t start_time = clock();
    ordenar_rectangles(orders);
    int c = 0;
    int L = 0;
    int i = 0; int j = 0;
    Coordinates new_coord;
    vector<Position> sol(N);
    vector<int> usats(orders.size(), 0);
    vector<vector<int>> teler(L_max, vector<int>(W, 0));
    while(c < N) {
        int posat = false;
        for(int s = 0; s < orders.size() and not posat; ++s){
            Order r = orders[s];
            for(int q = 0; q < r.n and not posat; ++q){
                if (usats[s] < r.n and cap(r.p, r.q, i, j, teler)){
                    poso(r.p, r.q, i, j, teler, c + 1, sol);
                    posat = true;
                    ++c;
                    ++usats[s];
                    L = max(L, i + r.p);
                    new_coord = set_new_coordinates(i, j, teler);
                    i = new_coord.y;
                    j = new_coord.x;
                }
                if (usats[s] < r.n and cap(r.q, r.p, i, j, teler) and not posat){
                    poso(r.q, r.p, i, j, teler, c + 1, sol);
                    posat = true;
                    ++c;
                    ++usats[s];
                    L = max(L, i + r.q);
                    new_coord = set_new_coordinates(i, j, teler);
                    i = new_coord.y;
                    j = new_coord.x;
                }
            }
        }
        if(not posat){
            new_coord = set_new_coordinates((j + 1 < W ? i : i + 1), (j + 1 < W ? j + 1 : 0), teler);
            i = new_coord.y;
            j = new_coord.x;
        }
    }
    escriu_solucio(sortida, L, sol, start_time);
}

int main(int agrc, char** argv) {
    int n_units, p, q;
    ifstream input;
    input.open(argv[1]);
    input >> W >> N;
    L_max = 0;
    while (input >> n_units) {
        input >> p >> q;
        orders.push_back(Order{n_units, p, q});
        L_max += q * n_units;
    }
    input.close();
    greedy(argv[2]);
}