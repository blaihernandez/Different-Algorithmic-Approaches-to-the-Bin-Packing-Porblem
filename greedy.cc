#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

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
            cout << matriu[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void ordenar_rectangles(vector<Order>& orders) {
    sort(orders.begin(), orders.end(), [](const Order& a, const Order& b) {
        return a.p * a.q > b.p * b.q;
    });
}

bool cap(int dx, int dy, int x, int y, vector<vector<int>>& teler){
    if(dx + x > W) return false;
    for (int i = y; i < y + dy; ++i){
        for (int j = x; j < x + dx; ++j) if(teler[i][j] != 0) return false;
    }
    return true;
}

void poso(int dx, int dy, int x, int y, vector<vector<int>>& teler, int id){
    for (int i = y; i < y + dy; ++i){
        for (int j = x; j < x + dx; ++j) teler[i][j] = id;
    }
}

void greedy(){
    ordenar_rectangles(orders);
    int id = 0; int L = 0;
    vector<vector<int>> teler(L_max, vector<int>(W, 0));
    for (Order r : orders){
        for(int q = 0; q < r.n; ++q){
            ++id;
            cout << "rectangle " << r.p << r.q << endl; //treure
            bool posat = false;
            for(int y = 0; y < L_max and not posat; ++y){
                for(int x = 0; x < W and not posat; ++x){
                    if (cap(r.q, r.p, x, y, teler)){
                        cout << "cap H" << endl; //treure
                        poso(r.q, r.p, x, y, teler, id);
                        escriu(teler); //treure
                        posat = true;
                        L = max(L, y + r.p);
                    }
                    else if (cap(r.p, r.q, x, y, teler)){
                        cout << "cap V" << endl; //treure
                        poso(r.p, r.q, x, y, teler, id);
                        escriu(teler); //treure
                        posat = true;
                        L = max(L, y + r.q);
                    }
                }
            }
        }
    }
    cout << L << endl;
}

int main() {
    int n_units, p, q;
    cin >> W >> N;
    L_max = 0;
    while (cin >> n_units) {
        cin >> p >> q;
        orders.push_back(Order{n_units, p, q});
        L_max += q * n_units;
    }
    greedy();
}