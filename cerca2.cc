#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <climits> // For INT_MAX

using namespace std;

struct Rectangle {
    int p; // width
    int q; // height
};

struct Coordinates {
    int i;
    int j;
};

struct Position {
    Coordinates ul; // Upper-left
    Coordinates br; // Bottom-right
};

typedef vector<vector<bool>> FR;

int N, W, L_MAX;
vector<Rectangle> Rectangles;
int RESULT = INT_MAX; // Initialize to maximum possible value

// Debugging: Print the matrix
void escriu_matriu(const FR& matriu) {
    for (const auto& row : matriu) {
        for (bool cell : row) cout << cell << " ";
        cout << endl;
    }
    cout << endl;
}

// Insert a new rectangle piece
void insert_new_piece(vector<Position>& p_sol, const Rectangle& R, FR& fabric, const Coordinates& coord, int c, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    p_sol[c] = Position{Coordinates{x, y}, Coordinates{x + w - 1, y + h - 1}};
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) fabric[i][j] = true;
    }
}

// Check if a rectangle fits at the given coordinates
bool piece_fits(const FR& fabric, const Rectangle& R, const Coordinates& coord, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    if (x + w > W || y + h > L_MAX) return false;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) if (fabric[i][j]) return false;
    }
    return true;
}

// Remove a rectangle piece from the fabric
void delete_piece(FR& fabric, const Rectangle& R, const Coordinates& coord, bool reverse) {
    int y = coord.i, x = coord.j;
    int w = reverse ? R.q : R.p;
    int h = reverse ? R.p : R.q;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) fabric[i][j] = false;
    }
}

// Calculate the next coordinates
Coordinates set_new_coord(const Coordinates& coord, int d) {
    if (coord.j + d < W) return {coord.i, coord.j + d};
    return {coord.i + 1, 0};
}

// Recursive exhaustive search
void exh_search_recursive(FR& fabric, int c, Coordinates current_coord, int current_L, vector<Position>& p_sol, vector<bool>& used) {
    
    if (c == N) {
        // All rectangles placed; check the result
        RESULT = min(RESULT, current_L);

    }

    else if (current_L < RESULT && current_L <= L_MAX && current_coord.i <= L_MAX) {
        for (int i = 0; i < Rectangles.size() && !used[i]; ++i) {
            if (!fabric[current_coord.i][current_coord.j]){

                const Rectangle& R = Rectangles[i];

                if (piece_fits(fabric, R, current_coord, true)) {
                    insert_new_piece(p_sol, R, fabric, current_coord, c, true);
                    used[i] = true;
                    Coordinates next_coord = set_new_coord(current_coord, R.q);
                    if (next_coord.i < L_MAX) {
                        exh_search_recursive(fabric, c + 1, next_coord, max(current_L, current_coord.i + R.q), p_sol, used);
                    }
                    delete_piece(fabric, R, current_coord, true);
                    used[i] = false;
                }

                if (piece_fits(fabric, R, current_coord, false)) {
                    insert_new_piece(p_sol, R, fabric, current_coord, c, false);
                    used[i] = true;
                    Coordinates next_coord = set_new_coord(current_coord, R.p);
                    if (next_coord.i < L_MAX) {
                        exh_search_recursive(fabric, c + 1, next_coord, max(current_L, current_coord.i + R.p), p_sol, used);
                    }
                    delete_piece(fabric, R, current_coord, false);
                    used[i] = false;
                }
            }
        }
    }

    // Try next coordinate
    Coordinates next_coord = set_new_coord(current_coord, 1);
    if (next_coord.i < L_MAX) {
        exh_search_recursive(fabric, c, next_coord, current_L, p_sol, used);
    }
}

// Start the exhaustive search
void exh_search() {
    FR fabric(L_MAX, vector<bool>(W, false));
    vector<Position> p_sol(N);
    vector<bool> used(N, false);
    exh_search_recursive(fabric, 0, {0, 0}, 0, p_sol, used);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error: Could not open input file " << argv[1] << endl;
        return 1;
    }

    ofstream outfile(argv[2]);
    if (!outfile) {
        cerr << "Error: Could not open output file " << argv[2] << endl;
        return 1;
    }

    infile >> W >> N;
    L_MAX = 0;

    int n_units, p, q;
    while (infile >> n_units >> p >> q) {
        for (int _ = 0; _ < n_units; ++_) Rectangles.push_back({p, q});
        L_MAX += q * n_units;
    }

    infile.close();

    streambuf* cout_buf = cout.rdbuf();
    cout.rdbuf(outfile.rdbuf());

    exh_search();

    cout << "Minimum fabric length required: " << RESULT << endl;

    cout.rdbuf(cout_buf);
    outfile.close();
    cout << "Minimum fabric length required: " << RESULT << endl;


    return 0;
}




