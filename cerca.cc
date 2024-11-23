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

bool exhaustive_search_solution_recursive(int r, int quant, int x, int y,
                                          vector<vector<int>> teler,
                                          int L, int bL, int c){
    if (c == N) return;
    if (L < bL) {
        for (Rectangles r : Pieces) {
            for(int quant = 0; quant < r.n; ++quant) {
                bool colocat = false;
                for (int x = 0; x < W && !colocat; ++x) {
                    for (int y = 0; y < L && !colocat; ++y) {
                        if (cabe(teler, x, y, rect.ancho, rect.alto, W, L)) {
                            colocar(teler, x, y, rect.ancho, rect.alto, rect.ancho * 1000 + rect.alto);
                            colocat = true;
                        } else if (cabe(teler, x, y, rect.alto, rect.ancho, W, L)) {
                            colocar(teler, x, y, rect.alto, rect.ancho, rect.ancho * 1000 + rect.alto);
                            colocat = true;
                        }
                    }
                }
                if (!colocat) {
                    cabeTodo = false;
                    break;
                }
            }
            if (!cabeTodo) break;
        }
        if (cabeTodo) {
            bL = min(bL, L);
        } else {
            ++L;
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