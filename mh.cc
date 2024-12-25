#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <unordered_set>
#include <ctime>

using namespace std;

struct Rectangle {
    int id;
    int p;
    int q;

    // Equality operator for unordered_set comparison
    bool operator==(const Rectangle& other) const {
        return id == other.id;
    }
};

struct Solution {
    vector<Rectangle> solution;
    int fitness;
};

struct Coordinates {
    int x;
    int y;
};

struct Position {
    Coordinates ul;
    Coordinates br;
};

typedef vector<vector<bool>> FR;

// Parameters for the metaheuristic
float mutation_prob = 0.01; // Mutation probability
int ITER_MAX, K, S;         // Max iterations                 // Population size and number of parents

const int UNDEF = -1;
int N, W, L_MAX;


double elapsed_time(clock_t st) {
    clock_t end_time = clock();
    double time_in_seconds = double(end_time - st) / CLOCKS_PER_SEC;
    return round(time_in_seconds * 10.0) / 10.0;
}


void write_solution(char* out_file, int L, const vector<Position>& sol, clock_t st) {
    double temps = elapsed_time(st);
    ofstream output;
    output.open(out_file);
    output << temps << endl;
    output << L << endl;
    for (Position pos : sol){
        output << pos.ul.x << " " << pos.ul.y << "   " << pos.br.x << " " << pos.br.y << endl;
    }
    output.close();
}


bool piece_fits(int di, int dj, int i, int j, const vector<vector<int>>& fabric){
    if (dj + j > W) return false;
    for (int row = i; row < i + di; ++row){
        for (int col = j; col < j + dj; ++col) if(!fabric[row][col]) return false;
    }
    return true;
}


void insert_piece(int di, int dj, int i, int j, vector<vector<int>>& fabric, int c, vector<Position>& sol){
    for (int row = i; row < i + di; ++row){
        for (int col = j; col < j + dj; ++col) fabric[row][col] = c;
    }
    sol[c - 1] = Position{Coordinates{j, i}, Coordinates{j + dj - 1, i + di - 1}};
}


Coordinates set_new_coordinates(int i, int j, const vector<vector<int>>& fabric){
    for (int row = i; row < L_MAX; ++row) {
        for(int col = (row == i) ? j : 0; col < W; ++col) if (!fabric[row][col]) return {col, row};
    }
    return {};
}


int greedy(const vector<Rectangle>& rectangles, vector<Position>& sol){
    int c = 0;
    int L = 0;
    int i = 0; int j = 0;
    Coordinates new_coord;
    vector<int> usats(rectangles.size(), 0);
    vector<vector<int>> fabric(L_MAX, vector<int>(W, 0));
    while(c < N) {
        int posat = false;
        for(int s = 0; s < rectangles.size() and not posat; ++s){
            Rectangle r = rectangles[s];
            if (!usats[s] && piece_fits(r.p, r.q, i, j, fabric)){
                insert_piece(r.p, r.q, i, j, fabric, c + 1, sol);
                posat = true;
                ++c;
                ++usats[s];
                L = max(L, i + r.p);
                new_coord = set_new_coordinates(i, j, fabric);
                i = new_coord.y;
                j = new_coord.x;
            }
            if (!posat && !usats[s] && piece_fits(r.q, r.p, i, j, fabric)){
                insert_piece(r.q, r.p, i, j, fabric, c + 1, sol);
                posat = true;
                ++c;
                ++usats[s];
                L = max(L, i + r.q);
                new_coord = set_new_coordinates(i, j, fabric);
                i = new_coord.y;
                j = new_coord.x;
            }
        }
        if(not posat){
            new_coord = set_new_coordinates((j + 1 < W ? i : i + 1), (j + 1 < W ? j + 1 : 0), fabric);
            i = new_coord.y;
            j = new_coord.x;
        }
    }
    return L;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

// METAHEURISTIC //


void sort_by_fitness(vector<Solution>& P) {
    sort(P.begin(), P.end(), [](const Solution& a, const Solution& b) {
        return a.fitness > b.fitness;
    });
}

vector<Solution> generate_initial_population(const vector<Rectangle>& rectangles, vector<Position>& sol) {
    vector<Solution> Pini(K);
    Pini[0] = {rectangles, L_MAX - greedy(rectangles, sol)};
    for (int i = 1; i < K; ++i) {
        vector<Rectangle> newrect = rectangles;
        for (int n = 0; n < 2; ++n) { // Perform 2 random swaps
            int a = rand() % N;
            int b = rand() % N;
            swap(newrect[a], newrect[b]);
        }
        Pini[i] = {newrect, L_MAX - greedy(newrect, sol)};
    }
    return Pini;
}

vector<Solution> select_parents(const vector<Solution>& P) {
    vector<Solution> selectedParents(S);
    for (int i = 0; i < S; ++i) {
        selectedParents[i] = P[i]; // Direct assignment
    }
    return selectedParents;
}

Solution orderCrossover(Solution p1, Solution p2, vector<Position>& sol) {
    int n = p1.solution.size();
    vector<Rectangle> offspring(n, Rectangle{UNDEF, 0, 0});

    int start = rand() % n;
    int end = rand() % n;
    if (start > end) swap(start, end);

    // Copy segment from parent1
    for (int i = start; i <= end; ++i) {
        offspring[i] = p1.solution[i];
    }

    // Fill remaining from parent2 in order
    unordered_set<int> copiedIds;
    for (int i = start; i <= end; ++i) {
        copiedIds.insert(p1.solution[i].id);
    }
    int currentIdx = (end + 1) % n;
    for (int i = 0; i < n; ++i) {
        int idx = (end + 1 + i) % n;
        if (copiedIds.find(p2.solution[idx].id) == copiedIds.end()) {
            offspring[currentIdx] = p2.solution[idx];
            currentIdx = (currentIdx + 1) % n;
        }
    }

    return Solution{offspring, L_MAX - greedy(offspring, sol)};
}

vector<Solution> recombine(const vector<Solution>& parents, vector<Position>& sol) {
    vector<Solution> offspring;
    for (int i = 0; i < parents.size() - 1; ++i) {
        offspring.push_back(orderCrossover(parents[i], parents[i + 1], sol));
    }
    return offspring;
}

void mutate(vector<Solution>& offspring) {
    for (auto& child : offspring) {
        if (rand() / static_cast<double>(RAND_MAX) < mutation_prob) {
            int a = rand() % N;
            int b = rand() % N;
            swap(child.solution[a], child.solution[b]);
        }
    }
}


void select_individuals(vector<Solution>& population, const vector<Solution>& offspring) {
    population.insert(population.end(), offspring.begin(), offspring.end());
    sort_by_fitness(population);
    if (population.size() > K) {
        population.resize(K); // Keep top k individuals
    }
}


void add_rectangle_id(vector<Rectangle>& rectangles) {
    for (int i = 0; i < rectangles.size(); ++i) {
        rectangles[i].id = i + 1;
    }
}


void sort_by_area(vector<Rectangle>& rectangles) {
    sort(rectangles.begin(), rectangles.end(), [](const Rectangle& a, const Rectangle& b) {
        if (a.p * a.q != b.p * b.q) return a.p * a.q > b.p * b.q;
        return a.q > b.q;
    });
}


void set_mh_parameters(){
    K = 68;
    S = 2;
}


int metaheuristic(vector<Rectangle>& rectangles, vector<Position>& sol) {
    set_mh_parameters();
    sort_by_area(rectangles);
    add_rectangle_id(rectangles);
    vector<Solution> population = generate_initial_population(rectangles, sol);
    sort_by_fitness(population);
    for (int i = 0; i < ITER_MAX; ++i) {
        vector<Solution> parents = select_parents(population);
        vector<Solution> offsprings = recombine(parents, sol);
        mutate(offsprings);
        select_individuals(population, offsprings);
    }
    return greedy(population[0].solution, sol);
}



int main(int agrc, char** argv) {
    int n_units, p, q;
    ifstream input;
    input.open(argv[1]);
    input >> W >> N;
    L_MAX = 0;
    vector<Rectangle> rectangles(N);
    while (input >> n_units) {
        input >> p >> q;
        for(int n = 0; n < n_units; ++n){
            rectangles.push_back(Rectangle{UNDEF, p, q});
        }
        L_MAX += q * n_units;
    }
    input.close();
    clock_t st = clock();
    vector<Position> sol(N);
    write_solution(argv[2], metaheuristic(rectangles, sol), sol, st);
}