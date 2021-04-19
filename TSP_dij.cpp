#include <vector>
#include <fstream>
#include <sstream>
#include "minheap.h"

using namespace std;

// returns the vector of cities that are not in cities;
// limit is the number of vertices in the input graph;
vector<int> complement(uint64_t cities, int limit)
{
    vector<int> cities_complement;

    for(int i = 0; i < limit - 1; i++)
    {
        if((cities & 1) == 0)
        {
            // +2 because city 1 is always included and
            // because the numeration of cities starts from 1 (not 0);
            cities_complement.push_back(i + 2);
        }

        cities >>= 1;
    }

    return cities_complement;
}


uint64_t add_city(uint64_t cities, int city)
{
    cities |= (1 << (city - 2));
    return cities;
}

bool contains_all_cities(uint64_t cities, int N)
{
    return cities == pow(2, N - 1) - 1;
}

bool is_present_in_hash_table(uint64_t cities, int last_city, unordered_map<uint64_t, unordered_map<int, int>> &v2Qi)
{
    return v2Qi.find(cities) != v2Qi.end() && v2Qi[cities].find(last_city) != v2Qi[cities].end();
}

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


    int N = numCities;
    Vertex start_v = {0, 1, 0};

    Minheap Q;
    unordered_map<uint64_t, unordered_map<int, int>> v2Qi;
    Q.set_map(&v2Qi);

    Q.insert(start_v);

    while(true)
    {
        Vertex *u = new Vertex;
        *u = Q.extract_min();

        if(u->cities == 0 && u->last_city == 1 && u->d > 0)
        {
            cout << "Optimal tour found!! The distance is: " << u->d << endl;
            break;
        }

        if(contains_all_cities(u->cities, N))
        {
            cout << "All cities vertex present!\n";

            Vertex final_v = { 0, 1, inputGraph[0][u->last_city - 1] + u->d };

            if(is_present_in_hash_table(final_v.cities, final_v.last_city, v2Qi) == false)
            {
                Q.insert(final_v);
            }
            else
            {
                int i = v2Qi.at(final_v.cities).at(final_v.last_city);
                if(Q.m_array[i].d > final_v.d)
                {
                    Q.decrease_key(i, final_v.d);
                }
            }

            continue;
        }

        auto cities_complement = complement(u->cities, N);
        for(auto city : cities_complement)  // for (city c) not in u->cities:
        {
            uint64_t new_cities = add_city(u->cities, city);

            Vertex neigh_u = { new_cities, city, inputGraph[city - 1][u->last_city - 1] + u->d };

            if(is_present_in_hash_table(neigh_u.cities, neigh_u.last_city, v2Qi) == false)
            {
                Q.insert(neigh_u);
            }
            else // Do the relax operation;
            {
                int i = v2Qi.at(neigh_u.cities).at(neigh_u.last_city);

                if(Q.m_array[i].d > neigh_u.d)
                {
                    Q.decrease_key(i, neigh_u.d);
                }
            }
        }
    }

    // Pinpoint the end time;
    auto end = std::chrono::steady_clock::now();

    // Printing the time in nanoseconds, optimal tour length and ending :)
    cout << "Time elapsed = " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << endl;
    cout << "Done" << endl;

    return 0;
}
