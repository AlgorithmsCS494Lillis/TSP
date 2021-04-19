#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include "minheap.h"

using namespace std;

// Assuming that the optimal tour is always smaller than this value;
#define INF 999999999
// Set DEBUG to 1 to see more useful info ;
#define DEBUG 0

// The function that prints the bits of an integer n;
// limit is the number of bits printed(least significant);
void bin(unsigned int n, int limit)
{
    for (unsigned int i = 1 << (limit - 1); i > 0; i = i / 2)
    {
        (n & i) ? cout << 1 : cout << 0;
    }
}

// at - the index of the last bit;
// bitstring - current bitstring;
// bitstrings - the vector of all bitstrings;
void combinations_helper(int at, int r, int n, unsigned int bitstring, vector<unsigned int> &bitstrings)
{
    if(r == 0)
    {
        bitstrings.push_back(bitstring);
        return;
    }
    if(n - at < r)
    {
        return;
    }

    bitstring = bitstring | (1 << at); // Set bit to 1
    combinations_helper(at + 1, r - 1, n, bitstring, bitstrings);
    bitstring = bitstring & (~(1 << at)); // Set bit to 0
    combinations_helper(at + 1, r, n, bitstring, bitstrings);
}

// Returns all combinations of r bits set in n bits;
// For example: (r = 2, n = 3) => (011, 101, 110)
vector<unsigned int> combinations(int r, int n)
{
    vector<unsigned int> bitstrings;
    combinations_helper(0, r, n, 0, bitstrings);
    return bitstrings;
}


// Extracts the cities from unsigned integer;
// For example, if n = 01101 then cities are: {1, 2, 4, 5}; Note: implicitly adds city 1.
vector<int> subset_to_cities(unsigned int n)
{
    vector<int> cities;
    cities.push_back(1);
    int idx = 2;

    while(n)
    {
        if(n & 1)
        {
            cities.push_back(idx);
        }
        n >>= 1;
        idx++;
    }
    return cities;
}



struct Vertex
{
    unsigned int cities;
    int last_city;
};


//bool is_present_in_hash_table(uint32_t cities, uint32_t last_city, unordered_map<uint32_t, unordered_map<uint32_t, int>> &v2Qi)
//{
//    return v2Qi.find(cities) != v2Qi.end() && v2Qi[cities].find(last_city) != v2Qi[cities].end();
//}


int main()
{
    // Doing .txt file parsing
    std::fstream inputFile;
    inputFile.open("tsp1.txt", std::ios::in);
    int numCities = 0;
    vector<vector<int>> inputGraph;
    int countA = 0;
    int countB = 0;
    string temp;

    if (inputFile.is_open()) {
        string numCitiesC;
        getline(inputFile, numCitiesC);

        numCities = stoi(numCitiesC);

        while (getline(inputFile, temp)) {

            std::stringstream stream(temp);
            inputGraph.push_back(std::vector<int>());
            while (1) {
                int n;
                stream >> n;
                if (!stream)
                    break;

                inputGraph[countA].push_back(n);

            }
            countB = 0;
            countA += 1;
        }

    }
    inputFile.close();

    // Pinpoint the start time;
    auto start = std::chrono::steady_clock::now();

    // Initializing that table C;
    // It is 2^(N - 1)xN;
    // It is N - 1 because the tour always starts at 1;
    // So we store the one in each tour implicitly, meaning
    // that (S[{1, 4, 6, 7}, 4], S[{1, 2}, 2]) => (S[{4, 6, 7}, 4], S[{2}, 2]) etc;
    // The subsets are encoded using unsigned int, for example:
    // {4, 6, 7} => 110100 (4 is at pos 3 from right because of that implicit 1);
    // We start counting cities from 1 from right to left;
    // So, C[0110, 3] = the length of the optimal tour that starts at city 1, goes through cities 3 and 4 and ends at city 4 (C[0110, city - 1]);
    int N = numCities;
    auto rows_num = pow(2, N - 1);
    vector<vector<int>> C(rows_num);
    for (auto i = 0; i < rows_num; i++)
    {
        C[i].resize(i + 1);
    }

    vector<vector<Vertex *>> pred(rows_num);
    for (auto i = 0; i < rows_num; i++)
    {
        pred[i].resize(N);
    }


    // The lenght of tour that starts at city 1 and ends at city 1 is 0;
    C[0][0] = 0;
    // There is no predecessor of city 1;
    pred[0][0] = nullptr;

    for(int s = 1; s < N; s++)
    {
        auto subsets = combinations(s, N - 1);
        for(auto& S : subsets)
        {
            C[S][0] = INF;
            // Get all cities;
            // Basically decoding an unsigned int to the set of cities;
            auto cities = subset_to_cities(S);

            // j - city that ends a tour that consists of cities
            for(auto& j : cities)
            {
                // city 1 can't end the tour
                if(j == 1)
                    continue;

                int min = INF;

                // i - city that is a predessessor of city j
                for(auto& i : cities)
                {
                    // j can't be a predessessor of itself
                    if(i == j)
                        continue;

                    // We are indexing by i - 1 and j - 1 because city2index = city - 1;
                    // The expression S & (~(1 << (j - 2))) turns off the bit at pos j - 2 in S;
                    // It is j - 2 because city2index = city - 1 and because of that implicit 1;
                    int path_length = C[S & (~(1 << (j - 2)))][i - 1] + inputGraph[i - 1][j - 1];
                    if(path_length < min)
                    {
                        min = path_length;
                        pred[S][j - 1] = new Vertex{S & (~(1 << (j - 2))), i - 1};
                    }

                }

                C[S][j - 1] = min;

                if(DEBUG)
                {
                    cout << "the current path: "; bin(S, N - 1); cout << endl;
                    cout << "All cities are: ";
                    for(auto& j : cities) cout << j << " ";
                    cout << endl;
                    cout << "j = " << j << " min = " << min << endl;
                }
            }
        }
    }

    Vertex *last_tour_city = new Vertex;

    int min_length = INF;
    for(int i = 1; i < N; i++)
    {
        int d = C[rows_num - 1][i] + inputGraph[i][0];
        if(d < min_length)
        {
            min_length = d;
            last_tour_city->cities = rows_num - 1;
            last_tour_city->last_city = i;
        }
    }

    // Pinpoint the end time;
    auto end = std::chrono::steady_clock::now();

    // Printing the time in nanoseconds, optimal tour length and ending :)
    cout << "Time elapsed = " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << endl;
    cout << "The length of the optimal tour = " << min_length << endl;

    cout << "Optimal tour is: " << "1 -> " << last_tour_city->last_city + 1 << " -> ";

    Vertex *curr_v = pred[last_tour_city->cities][last_tour_city->last_city];
    while(curr_v != nullptr)
    {
        cout << curr_v->last_city + 1;

        curr_v = pred[curr_v->cities][curr_v->last_city];
        if(curr_v)
        {
            cout << " -> ";
        }
    }
    
    cout << endl;
    cout << "Done" << endl;

    return 0;
}
