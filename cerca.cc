#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Rectangle {
    int width;
    int height;
};

struct Coordinates{
    int upper_left;
    int lower_right;
};

/*soc molt guapo*/
void insert_piece(Rectangle r, bool reverse /*fabric roll*/){
    /*aquí podem fer la feina d'inserir la peça, guardar les coordenades
    i, si cal, afegir una o més fil.les al rotlle de tel.la*/
}


void exhaustive_search_solution_recursive(vector<Rectangle>& pieces, int W, int N, int used_pieces, int current_L /*fabric_roll*/){
    if (used_pieces == N) return; /* mirem si la longitud actual és millor que la optima que haguem trobat*/;

    for (Rectangle r : pieces){
        insert_piece(r, false /*fabric_roll*/);
        exhaustive_search_solution_recursive(pieces, W, N, used_pieces + 1, current_L /*fabric_roll*/);
        insert_piece(r, true);
        /*hem d'eliminar la última peça que hem posat del dret per posar-la del revès*/
        exhaustive_search_solution_recursive(pieces, W, N, used_pieces + 1, current_L /*fabric_roll*/);
    }
}


void exhaustive_search_solution(vector<Rectangle>& pieces, int W, int N){
    /* com ho fem per retornar??? necessitem la longitud i les coordenades*/
    int used_pieces = 0;
    int fabric_roll; /*ho guardem en una matriu???? o potser millor crear una struct????*/
    int current_L = 0;
    exhaustive_search_solution_recursive(pieces, W, N, used_pieces, current_L /*fabric_roll*/);
}


int main() {
    int W, N, n_units, p, q;
    cin >> W >> N;
    vector<Rectangle> pieces(N);
    for (int i = 0; i < N; i += n_units) {
        cin >> n_units >> p >> q;
        pieces.push_back({p, q});
    }
    exhaustive_search_solution(pieces, W, N);
}