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

/*
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
*/

void greedy(){
    ordenar_rectangles(orders);
    int c = 0;
    int L = 0;
    int x = 0; int y = 0;
    vector<int> usats(orders.size(), 0);
    vector<vector<int>> teler(L_max, vector<int>(W, 0));
    while(c < N) {
        int id = 0;
        int posat = false;
        cout << "coords" << x << y << endl; //treure
        for(int i = 0; i < orders.size() and not posat; ++i){
            Order r = orders[i];
            for(int q = 0; q < r.n and not posat; ++q){
                ++id;
                cout << "rectangle" << r.p << r.q << endl; //treure
                if (usats[i] < r.n and cap(r.q, r.p, x, y, teler)){
                    cout << "cap H" << endl; //treure
                    poso(r.q, r.p, x, y, teler, id);
                    escriu(teler); //treure
                    posat = true;
                    ++c;
                    ++usats[i];
                    L = max(L, y + r.p);
                    if(x + r.q < W) x += r.q;
                    else if(y + r.p < L) y += r.p;
                    else {x = 0; y += r.p;}
                }
            }
        }
        if(not posat){
            cout << "provem verticals" << endl;
            id = 0;
            for(int i = 0; i < orders.size() and not posat; ++i){
                Order r = orders[i];
                for(int q = 0; q < r.n and not posat; ++q){
                    ++id;
                    cout << "rectangle" << r.p << r.q << endl; //treure
                    if (usats[i] < r.n and cap(r.p, r.q, x, y, teler)){
                        cout << "cap V" << endl; //treure
                        poso(r.p, r.q, x, y, teler, id);
                        escriu(teler); //treure
                        posat = true;
                        ++c;
                        ++usats[i];
                        L = max(L, y + r.q);
                        if(x + r.p < W) x += r.p;
                        else if(y + r.q < L) y += r.q;
                        else {x = 0; y += r.q;}
                    }
                }
            }
        }
        if(not posat){x = 0; y = L;}
    }
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