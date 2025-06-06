#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;


struct Rectangle {
    int id;  // Id used for metaheuristic
    int p;  
    int q;
};

// Representation of an individual for metaheuristic purposes.
// The grater the fitness, the better the individual.
struct Individual {
    vector<Rectangle> rectArray; // List of rectangles. Each individual's array is sorted differently
    int fitness;                 // Defined as L_MAX - L (L = greedy(rectArray))
};

struct Coordinates{
    int x;
    int y;
};

// Upper-left and bottom-right coordinates of the rectangle
struct Position{
    Coordinates ul;
    Coordinates br;
};

typedef vector<vector<int>> Matrix;
typedef vector<Position> Positions;


// Metaheuristic parameters
const int K = 30;            // Population size
const int S = 20;            // Number of parents
const float P = 0.30;          // Mutation probability (usually smaller, but for this problem works better)
const int ITER_MAX = 2000;   // Number of iterations

const int UNDEF = -1;
int N, W, L_MAX;



// // // // // // GREEDY \\ \\ \\ \\ \\ \\


// Returns the elapsed time of the execution in seconds.
double elapsed_time(clock_t startTime) {
    clock_t endTime = clock();
    double timeInSeconds = double(endTime - startTime) / CLOCKS_PER_SEC;
    return round(timeInSeconds * 10.0) / 10.0;
}

// Writes the solution sol on the ouput file following the required format. Returns nothing.
void write_solution(char* outputFile, int L, const Positions& sol, clock_t startTime) {
    double time = elapsed_time(startTime);
    ofstream output;
    output.open(outputFile);
    output << time << endl;
    output << L << endl;
    for (Position pos : sol){
        output << pos.ul.x << " " << pos.ul.y << "   " << pos.br.x << " " << pos.br.y << endl;
    }
    output.close();
}

// Sorts the list of rectangles Rectangles by area. Returns nothing.
void sort_by_area(vector<Rectangle>& Rectangles) {
    sort(Rectangles.begin(), Rectangles.end(), [](const Rectangle& a, const Rectangle& b) {
        if(a.p * a.q == b.p * b.q) return a.q > b.q;
        return a.p * a.q > b.p * b.q;
    });
}

// Returns a bool based on whether coordinates (j..j + dj, i..i + di) are empty or not.
bool piece_fits(int di, int dj, int i, int j, const Matrix& Fabric){
    if(dj + j > W) return false;
    for (int row = i; row < i + di; ++row){
        for (int col = j; col < j + dj; ++col) if(Fabric[row][col] != 0) return false;
    }
    return true;
}

// Inserts a new piece ti the Fabric in the coordinates 
// (j..j + dj, i..i + di). Returns nothing.
void insert_piece(int di, int dj, int i, int j, Matrix& Fabric, int c, Positions& sol){
    for (int row = i; row < i + di; ++row){
        for (int col = j; col < j + dj; ++col) Fabric[row][col] = c;
    }
    sol[c - 1] = Position{Coordinates{j, i}, Coordinates{j + dj - 1, i + di - 1}};
}

// Returns the next free coordinates of the Fabric moving to the right.
Coordinates set_new_coordinates(int i, int j, const Matrix& Fabric){
    for (int row = i; row < L_MAX; ++row) {
        for(int col = (row == i) ? j : 0; col < W; ++col){
            if (!Fabric[row][col]) return {col, row};
        }
    }
    return {};
}

// Finds a (possibly) optimal solution to the problem using a greedy algorithm
int greedy(const vector<Rectangle>& Rectangles, Positions& sol){
    int c = 0; 
    int L = 0;
    int i = 0; int j = 0;
    vector<int> used(Rectangles.size(), 0);
    Matrix Fabric(L_MAX, vector<int>(W, 0));
    while(c < N) {
        int in = false;
        for(int s = 0; s < Rectangles.size() && !in; ++s){

            Rectangle R = Rectangles[s];

            if (!used[s] && piece_fits(R.p, R.q, i, j, Fabric)){
                insert_piece(R.p, R.q, i, j, Fabric, c + 1, sol);
                in = true; ++c; ++used[s];
                L = max(L, i + R.p);
                Coordinates newCoord = set_new_coordinates(i, j, Fabric);
                i = newCoord.y; j = newCoord.x;
            }
            if (!in && !used[s] && piece_fits(R.q, R.p, i, j, Fabric)){
                insert_piece(R.q, R.p, i, j, Fabric, c + 1, sol);
                in = true; ++c; ++used[s];
                L = max(L, i + R.q);
                Coordinates newCoord = set_new_coordinates(i, j, Fabric);
                i = newCoord.y; j = newCoord.x;
            }
        }
        if(!in){
            Coordinates newCoord = set_new_coordinates((j + 1 < W ? i : i + 1), (j + 1 < W ? j + 1 : 0), Fabric);
            i = newCoord.y; j = newCoord.x;
        }
    }
    return L;
}


// // // // // // METAHEURISTIC \\ \\ \\ \\ \\ \\


// Sorts the individuals list P by fitness.
void sort_by_fitness(vector<Individual>& P) {
    sort(P.begin(), P.end(), [](const Individual& a, const Individual& b) {
        return a.fitness > b.fitness;
    });
}

// Performs an order crossover of an offspring filled already with the rectangles in the midle from 
// the other parent and fills the ends with the rectangles from the parent 'parent'.
void order_crossover(int cut1, int cut2, vector<Rectangle>& parent, vector<Rectangle>& offspring){
    int k = 0;
    for (int m = 0; m < N; ++m){
        if (k < cut1){
            bool isInMiddle = false;
            for (int q = cut1; q < cut2; ++q) if (parent[m].id == offspring[q].id) isInMiddle = true;
            if (!isInMiddle){offspring[k] = parent[m]; ++k;}
        }
        else if (k == cut1) k = cut2;
        if (k >= cut2){
            bool isInMiddle = false;
            for (int q = cut1; q < cut2; ++q) if (parent[m].id == offspring[q].id) isInMiddle = true;
            if (!isInMiddle){offspring[k] = parent[m]; ++k;}
        } 
    }
}

// Recombines all the solutions in list 'parents' and returns the result of the recombination
// performing an 'order crossover' and returns the list of those offsprings.
vector<Individual> recombine(const vector<Individual>& parents, Positions& sol){
    vector<Individual> offsprings(S);
    int cut1 = rand() % (N - 1) + 1; // Can't be 0 nor N
    int cut2 = rand() % (N) + 1; // Can't be 0
    if (cut1 > cut2) swap(cut1, cut2); // cut1 must be lower than cut2
    else if (cut1 == cut2) ++cut2; // At least differ in one postion

    for (int i = 0; i < S; i = i + 2) {

        vector<Rectangle> dad = parents[i].rectArray;
        vector<Rectangle> mom = parents[i + 1].rectArray;
        vector<Rectangle> offspring1(N); vector<Rectangle> offspring2(N);
        
        // Determine rectangles from the middle
        for (int j = cut1; j < cut2; ++j) {
            offspring1[j] = dad[j]; 
            offspring2[j] = mom[j];
        }
        order_crossover(cut1, cut2, mom, offspring1);
        order_crossover(cut1, cut2, dad, offspring2);
        offsprings[i] = {offspring1, L_MAX - greedy(offspring1, sol)};
        offsprings[i + 1] = {offspring2, L_MAX - greedy(offspring2, sol)};
    }
    return offsprings;
}

// Reverses a random rectangle R from the list R. Returns nothing.
void random_reverse(vector<Rectangle>& R){
    int a = rand() % N; 
    swap(R[a].p, R[a].q);
}

// Swaps two random recangle's positions on the list R. Returns nothing
void random_swap(vector<Rectangle>& R) {  
    int a = rand() % N; 
    int b = rand() % N;
    swap(R[a], R[b]);
}

// Perfoms a random mutations to random individuals on the list. Returns nothing.
void mutate(vector<Individual>& individuals){
    for (Individual ind : individuals) {
        if (rand()/static_cast<double>(RAND_MAX) < P){
            random_swap(ind.rectArray);
        }
        if (rand()/static_cast<double>(RAND_MAX) < P){
            random_reverse(ind.rectArray);
        }
    }
}

// Selects the K fittest individuals between newP and P. Returns nothing.
void select_individuals(const vector<Individual>& newP, vector<Individual>& P){
    P.insert(P.end(), newP.begin(), newP.end());
    sort_by_fitness(P);
    if (P.size() > K){
        auto it = P.begin();
        advance(it, K);
        P.erase(it, P.end());
    }
}

// Selects and returns the S fittest individuals which will be recombined.  
vector<Individual> select_parents(vector<Individual>& P){
    vector<Individual> selected(S);
    sort_by_fitness(P);
    for (int i = 0; i < S; ++i) selected[i] = P[i];
    return selected;
}

// Generates and returns the initial population of solutions by randomly swapping 
// positions from sorted list and reversing rectangles.
vector<Individual> generate_initial_population(const vector<Rectangle>& Rectangles, Positions& sol){
    vector<Individual> iniP(K);
    iniP[0] = {Rectangles, L_MAX - greedy(Rectangles, sol)};
    for (int i = 1; i < K; ++i){
        vector<Rectangle> newRect = Rectangles;
        random_swap(newRect);
        random_reverse(newRect);
        iniP[i] = {newRect, L_MAX - greedy(newRect, sol)}; 
    }
    return iniP;
}

// Gives every rectangle an integer id. Returns nothing. 
void set_rectangles_id(vector<Rectangle>& R){
    for (int i = 1; i <= R.size(); ++i) R[i - 1].id = i;
}

// Reproduces a methaeuristic based on genetic selection. Starting from an intital population
// of K individuals, each time we select the best of them and breed or mutate them.
int genetic_metaheuristic(vector<Rectangle>& Rectangles, Positions& sol) {
    sort_by_area(Rectangles);
    set_rectangles_id(Rectangles);
    vector<Individual> Population = generate_initial_population(Rectangles, sol);
    sort_by_fitness(Population);
    for (int i = 0; i < ITER_MAX; ++i){
        vector<Individual> parents = select_parents(Population);
        vector<Individual> offspring = recombine(parents, sol);
        mutate(offspring);
        select_individuals(offspring, Population);
    }
    return greedy(Population[0].rectArray, sol);
}


int main(int agrc, char** argv) {
    ifstream input;
    input.open(argv[1]);
    input >> W >> N;
    vector<Rectangle> Rectangles;
    int nUnits, p, q;
    L_MAX = 0;
    while (input >> nUnits) {
        input >> p >> q;
        for(int n = 0; n < nUnits; ++n){
            Rectangles.push_back(Rectangle{UNDEF, p, q}); // We'll define the id later
        }
        L_MAX += q * nUnits;
    }
    input.close();
    clock_t start_time = clock();
    vector<Position> sol(N);
    write_solution(argv[2], genetic_metaheuristic(Rectangles, sol), sol, start_time);
}
