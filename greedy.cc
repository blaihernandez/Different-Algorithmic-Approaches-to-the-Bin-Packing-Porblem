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