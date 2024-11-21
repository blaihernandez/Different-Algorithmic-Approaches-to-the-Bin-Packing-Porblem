#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Rectangles {
    int n; /* nombre de rectangles de tipus (width x height)*/
    int width;
    int height;
};

struct Coordinates{
    int upper_left;
    int lower_right;
};

int N, W;
vector<Rectangles> Pieces;

void insert_piece(Rectangles r, bool reverse){
    /*aquí podem fer la feina d'inserir la peça, guardar les coordenades
    i, si cal, afegir una o més fil.les al rotlle de tel.la*/
}

/* mira si el rectangle r cap al final de la línia */
bool it_fits(Rectangles r, int i, int j) {
    /*if reverse r.w = r.h */
}

bool it_fits_reverse(){
}

bool insert_piece_reverse(){
}

void set_new_coordinates(int& i, int& j, Rectangles r){
}

void exhaustive_search_solution_recursive(int i, int j, vector<Coordinates>& p_sol, 
                                          int current_L, vector<Coordinates>& best_sol, 
                                          int best_L, int counter){
    if (counter == N) return; /* mirem si la longitud actual és millor que la optima que haguem trobat*/;
    if (current_L < best_L){
        for (Rectangles r : Pieces){
            if (it_fits(r, i, j)){
                insert_piece(r);
                set_new_coordinates(i, j, r);
                exhaustive_search_solution_recursive(i, j, p_sol, current_L, best_sol, best_L, counter + 1);
            }
            if (it_fits_reverse()){
                insert_piece_reverse();
                exhaustive_search_solution_recursive();
            }
        }       
    }
}


void exhaustive_search_solution(){
    /* com ho fem per retornar??? necessitem la longitud i les coordenades*/
    vector<Coordinates> p_sol(N), best_sol(N); /*ho guardem en una matriu???? o potser millor crear una struct????*/
    int current_L = 0;
    int best_L = INT_MAX;
    int counter = 0;
    int i = 0; int j = 0;
    vector<int> left_rectangles(Pieces.size());
    exhaustive_search_solution_recursive(i, j, p_sol, current_L, best_sol, best_L, counter);
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