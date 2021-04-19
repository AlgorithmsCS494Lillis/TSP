#ifndef minheap_hpp
#define minheap_hpp

#include <vector>
#include <cstdint>
#include <iostream>
#include <unordered_map>

#define _INFINITY_ 999999999

struct Vertex
{
    uint64_t cities;
    int last_city;

    int d;
};

class Minheap
{
public:

    // maps cities to hash_map, which in turn maps last_city to vertex index in min heap;
    // if vertex is present in minheap => return index;
    // if vertex is not present in minheap => return -1;
    void set_map(std::unordered_map<uint64_t, std::unordered_map<int, int>> *map)
    {
        v2Qi = map;
    }

    void percolate_down(int i)
    {
        int l = LEFT(i);
        int r = RIGHT(i);

        int smallest = i;

        if(l < m_array.size() && m_array[l].d < m_array[i].d)
            smallest = l;

        if(r < m_array.size() && m_array[r].d < m_array[smallest].d)
            smallest = r;


        if(smallest != i)
        {
            // Also swap entries in the hash table;
            v2Qi->at(m_array[i].cities).at(m_array[i].last_city) = smallest;
            v2Qi->at(m_array[smallest].cities).at(m_array[smallest].last_city) = i;

            auto temp = m_array[smallest];
            m_array[smallest] = m_array[i];
            m_array[i] = temp;

            percolate_down(smallest);
        }
    }

    void percolate_up(int i)
    {
        if(i && m_array[i].d < m_array[PARENT(i)].d)
        {
            // Also swap entries in the hash table;
            v2Qi->at(m_array[i].cities).at(m_array[i].last_city) = PARENT(i);
            v2Qi->at(m_array[PARENT(i)].cities).at(m_array[PARENT(i)].last_city) = i;

            auto temp = m_array[PARENT(i)];
            m_array[PARENT(i)] = m_array[i];
            m_array[i] = temp;

            percolate_up(PARENT(i));
        }
    }

    void decrease_key(int i, int new_key, int idd = 12)
    {
        if(new_key > m_array[i].d)
        {
            throw std::runtime_error("In decrease_key(): new_key must be less or equal than current key\n");
        }

        m_array[i].d = new_key;
        percolate_up(i);
    }

    void insert(Vertex &vertex)
    {
        m_array.push_back({ vertex.cities, vertex.last_city, _INFINITY_ });

        // Also insert an entry in the hash table;
        (*v2Qi)[vertex.cities][vertex.last_city] = static_cast<int>(m_array.size() - 1);

        decrease_key(static_cast<int>(m_array.size() - 1), vertex.d, 1);
    }

    Vertex extract_min()
    {
        if(m_array.size() == 0)
        {
            throw std::runtime_error("In extract_min(): heap mustn't be empty\n");
        }

        Vertex v_min = m_array[0];

        // Since v_max will not be present in minheap => remove it from hash table;
        v2Qi->at(v_min.cities).erase(v_min.last_city);

        if(m_array.size() - 1 > 0)
        {
            m_array[0] = m_array[m_array.size() - 1];
            v2Qi->at(m_array[m_array.size() - 1].cities).at(m_array[m_array.size() - 1].last_city) = 0;
        }

        m_array.pop_back();

        percolate_down(0);

        return v_min;
    }


    bool check_heap_vs_map_sync()
    {
        for(int i = 0; i < m_array.size(); i++)
        {
            auto v = m_array[i];
            if(v2Qi->at(v.cities).at(v.last_city) == i)
            {

            }
            else
            {
                std::cout << "The index stored in map: " << (*v2Qi)[v.cities][v.last_city] << " != " << i << std::endl;
                std::cout << v.cities << " " << v.last_city << std::endl;
                return false;
            }
        }

        return true;
    }

    void print()
    {
        for(int i = 0; i < m_array.size(); i++)
        {
            std::cout << i + 1 <<" Cities: " << m_array[i].cities << "       Last City: " << m_array[i].last_city << "          Dist: " << m_array[i].d << std::endl;
        }

        std::cout << "Size = " << m_array.size();

        std::cout << std::endl;
    }

private:
    int PARENT(int i)
    {
        return (i - 1) / 2;
    }

    int LEFT(int i)
    {
        return 2 * i + 1;
    }

    int RIGHT(int i)
    {
        return 2 * i + 2;
    }

public:
    std::vector<Vertex> m_array;

private:
    std::unordered_map<uint64_t, std::unordered_map<int, int>> *v2Qi;
};


#endif /* minheap_hpp */
