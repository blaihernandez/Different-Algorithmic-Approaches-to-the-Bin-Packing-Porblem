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
    int x;
    int y;
};

struct Position{
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
bool it_fits(Rectangles r, int i, int j, int prev) {
    /*if reverse r.w = r.h */
    return i + r.width <= W && j + r.height <= prev;
}

bool it_fits_reverse(Rectangles r, int i, int j, int prev){
    return j + r.width <= W && i + r.height <= prev;
}

bool insert_piece_reverse(){
}

void set_new_coordinates(int& i, int& j, Rectangles r, int prev){
}

void exhaustive_search_solution_recursive(int i, int j, vector<Position>& p_sol, 
                                          int current_L, vector<Position>& best_sol, 
                                          int best_L, int counter, int prev /*aixo es l'alçada del rectangle just
                                          a l'esquerra del q estic posant (es -1 si estic a l'esquerra del tot)*/){
    if (counter == N){
        if (current_L < best_L){
            best_L = current_L;
            best_sol = p_sol;
        }
    } /* mirem si la longitud actual és millor que la optima que haguem trobat*/;
    if (current_L < best_L){
        for (Rectangles r : Pieces){
            if (it_fits(r, i, j, prev)){
                p_sol[counter] = {{i, j}, {i + r.width, j + r.height}};
                if (prev == -1) current_L += r.height;
                prev = j + r.height;
                set_new_coordinates(i, j, r, prev);
                exhaustive_search_solution_recursive(i, j, p_sol, current_L, best_sol, best_L, counter + 1, prev);
            }
            if (it_fits_reverse(r, i, j, prev)){
                insert_piece_reverse();
                exhaustive_search_solution_recursive();
            }
        }       
    }
}


void exhaustive_search_solution(){
    /* com ho fem per retornar??? necessitem la longitud i les coordenades*/
    vector<Position> p_sol(N), best_sol(N); /*ho guardem en una matriu???? o potser millor crear una struct????*/
    int current_L = 0;
    int best_L = INT_MAX;
    int counter = 0;
    int i = 0; int j = 0;
    int suma = 0;
    for (Rectangles r : Pieces) suma += r.height;
    vector<int> left_rectangles(Pieces.size());
    exhaustive_search_solution_recursive(i, j, p_sol, current_L, best_sol, best_L, counter, -1);
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