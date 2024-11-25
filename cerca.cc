#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Rectangles {
    int n; /* nombre de rectangles de tipus (width x height)*/
    int p;
    int q;
};

struct Coordinates{
    int x;
    int y;
};

struct Position{
    Coordinates upper_left;
    Coordinates lower_right;
};

int N, W;
vector<Rectangles> Pieces;

void exhaustive_search_solution_recursive(int r, int quant, int x, int y,
                                          vector<int>& ple,
                                          int L, int bL, int c){
    if (c == N){
        if (L < bL) bL = L;
    }
    if (L < bL) {
        if (cap(Pieces[r], x, y, ple)){
            //el poso
            if(quant == Pieces[r].n) exhaustive_search_solution_recursive(r + 1, 0, nx, ny, ple, L, bL, c + 1);
            else exhaustive_search_solution_recursive(r, quant + 1, nx, ny, ple, L, bL, c + 1);
        }
    }
}

void exhaustive_search_solution(){
    /* com ho fem per retornar??? necessitem la longitud i les coordenades*/
    ...
}


int main() {
    int n_units, p, q;
    cin >> W >> N;
    while (cin >> n_units) {
        cin >> p >> q;
        Pieces.push_back({n_units, p, q});
    }
    exhaustive_search_solution();
}