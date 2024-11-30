#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Order {
    int n; /* nombre de Order de tipus (width x height)*/
    int width;
    int height;
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
int N, W;
vector<Order> Orders;

void escriu_matriu(vector<vector<bool>> matriu) {
    for(int i = 0; i < matriu.size(); ++i) {
        for(int j = 0; j < matriu[0].size(); ++j) {
            cout << matriu[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}


// insereix la peça de 'Order' a la coordenada actual coord (del dret o del revès).
// Marca fabric_roll indicant que allà hem posat la peça
void insert_new_piece(vector<Position>& p_sol, Order order, FR& fabric_roll, Coordinates coord, int c, bool reverse){
    int x = coord.x; int y = coord.y;
    int w = order.width; int h = order.height;
    if (reverse) {int k = w; w = h; h = k;}
    p_sol[c] = Position{Coordinates{x, y}, Coordinates{x + h, y + w}};
    for (int i = x; i < x + h; ++i){
        for(int j = y; j < y + w; ++j) fabric_roll[i][j] = true;
    }
}

// mira si la nova peça cap a la coordenada actual (li podem dir que provi la peça del revès)
bool piece_fits(FR& fabric_roll, Order order, Coordinates coord, bool reverse) {
    int x = coord.x; int y = coord.y;
    int w = order.width; int h = order.height;
    if (reverse) {int k = w; w = h; h = k;}
    for (int i = x; i < x + h; ++i){
        for (int j = y; j < y + w; ++j) if (fabric_roll[i][j] or j >= W) return false;
    }
    return true;
}

// retorna les noves coordenades on provar de posar una peça
Coordinates set_new_coordinates(int despl, FR& fabric_roll, Coordinates coord){
    if (coord.y + despl < W - 1) return Coordinates{coord.x, coord.y + despl};
    else return Coordinates{coord.x + 1, 0};
}

// desmarca la matriu 'fabric_roll' i n'elimina la peça 'Order' (del dret o del revès)
// de les coordenades coord.
void delete_piece(FR& fabric_roll, Order order, Coordinates coord, bool reverse){
    int x = coord.x; int y = coord.y;
    int w = order.width; int h = order.height;
    if (reverse) {int k = w; w = h; h = k;}
    for (int i = x; i < x + h; ++i){
        for (int j = y; j < y + w; ++j) fabric_roll[i][j] = false;
    }
}

// ordena les comandes segons l'area del rectangle
/*vector<Order> sort_orders(vector<Order>& Orders){
    sort(Orders.begin(), Orders.end(), [](const Order& a, const Order& b) {
        int areaA = a.width * a.height;
        int areaB = b.width * b.height;
        return areaA > areaB; // Descending order
    });
}*/


void exhaustive_search_solution_recursive(FR& fabric_roll, int count, Coordinates current_coord, 
                                        int current_L, int best_L, vector<Position>& p_sol){
    if (count == N) {
        cout << "he acabat" << endl;
        if (current_L < best_L) {
            best_L = current_L;
            cout << best_L << endl;
            for (int i = 0; i < N; ++i){
                cout << p_sol[i].ul.x << p_sol[i].ul.y << "      " << p_sol[i].br.x << p_sol[i].br.y << endl;
            }
        // find_best_positions
        }
    }
    else if (current_L < best_L){
        cout << count << endl;
        for (Order order : Orders){
            cout << order.width << order.height << endl;
            for (int q = 0; q < order.n; ++q) {
                // mirem si la posició on volem col.locar la peça està ocupada
                if (fabric_roll[current_coord.x][current_coord.y] == 0) {
                    cout << "no ocupada" << current_coord.x << current_coord.y << endl;
                    // si no està ocupada:
                    if (piece_fits(fabric_roll, order, current_coord, false)) {
                        cout << "fits" << endl;
                        // la posem del dret
                        escriu_matriu(fabric_roll);
                        insert_new_piece(p_sol, order, fabric_roll, current_coord, count, false);
                        escriu_matriu(fabric_roll);
                        current_L = max(current_L, current_coord.x + order.height + 1);
                        cout << "cridem" << endl;
                        exhaustive_search_solution_recursive(fabric_roll, count + 1,
                                                             set_new_coordinates(order.width, fabric_roll, current_coord),
                                                             current_L, best_L, p_sol);
                        delete_piece(fabric_roll, order, current_coord, false); // eliminem la peça per trobar una nova combinació
                    }
                    if (order.width != order.height && piece_fits(fabric_roll, order, current_coord, true)){
                        cout << "fits reves" << endl;
                        // la posem del revès
                        insert_new_piece(p_sol, order, fabric_roll, current_coord, count, true);
                        current_L = max(current_L, current_coord.x + order.width + 1);
                        cout << "cridem reves" << endl;
                        exhaustive_search_solution_recursive(fabric_roll, count + 1,
                                                             set_new_coordinates(order.height, fabric_roll, current_coord),
                                                             current_L, best_L, p_sol);
                        delete_piece(fabric_roll, order, current_coord, true); // eliminem la peça per trobar una nova combinació
                    }
                }
                else {
                    cout << "ocupada" << current_coord.x << current_coord.y << endl;
                    // si està ocupada busquem una nova coordenada on intentar ficar la peça
                    if (current_coord.y < W - 1) current_coord = Coordinates{current_coord.x, current_coord.y + 1};
                    else current_coord = Coordinates{current_coord.x + 1, 0};
                    cout << "cridem abaix" << endl;
                    exhaustive_search_solution_recursive(fabric_roll, count, current_coord, current_L, best_L, p_sol);
                }
            }
        }
    }
    cout << "acabo recursio" << endl;
}

void exhaustive_search_solution(int L_max){
    /* com ho fem per retornar??? necessitem la longitud i les coordenades*/
    FR fabric_roll(L_max, vector<bool>(W));
    vector<Position> p_sol(N);
    exhaustive_search_solution_recursive(fabric_roll, 0, Coordinates{0, 0}, 1, 1000000000, p_sol);
}


int main() {
    int n_units, p, q, L_max;
    cin >> W >> N;
    L_max = 0;
    while (cin >> n_units) {
        cin >> p >> q;
        Orders.push_back(Order{n_units, p, q});
        L_max += q;
    }
    //sort_orders(Orders);
    exhaustive_search_solution(L_max);
}