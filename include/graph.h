//
// Created by vic on 17/10/2024.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <set>
#include <unordered_set>
#include <random>
#include <span>
#include <iomanip>
#include "database.h"
#include <iterator> // for std::back_inserter
#include <unordered_map>
#include <fstream>
#include <omp.h>

template <typename T>
struct VamanaIndex
{
    //    std::vector<std::unordered_set<int>> graph;
    std::unordered_map<int, std::unordered_set<int>> graph;
    const Matrix<T> &db;
    const size_t vecnum;
    const size_t deg;
    std::unordered_map<T, std::vector<int>> Pf;
    VamanaIndex() : db(Matrix<T>::default_instance()), vecnum(0), deg(0) {
    }

    // constructor used for main
    VamanaIndex(size_t deg, const Matrix<T> &db) : db(db), vecnum(db.vecnum), deg(deg)
    {
        init_graph(deg);
    }

    // gia stitched
    VamanaIndex(const Matrix<T> &db, const size_t deg, const size_t vecnum) : db(db), vecnum(vecnum), deg(deg)
    {
    }

    // constructor for testing where we need to have a fixed graph each time. we add neighbors manually
    VamanaIndex(const Matrix<T> &db) : db(db), vecnum(db.vecnum), deg(0)
    {
        //        graph.resize(db.vecnum);
        //        graph.reserve(db.vecnum);
    }

    void init_graph(const std::vector<int> &P = {})
    {
        std::vector<int> vec;
        size_t sz;
        if (P.empty())
        {
            sz = vecnum;
//            vec.resize(vecnum);   
//            std::iota(vec.begin(), vec.end(), 0);
        }
        else
        {
            // std::cout << "P is not empty\n";
            sz = P.size();
            vec = P;
        }
        graph.reserve(sz);
        for (auto i : vec)
        {
            graph[i] = {};
        }
    }

    void init_graph(const size_t &r, const std::vector<int> &P = {})
    {
        //        graph.reserve(vecnum);
        // generator for shuffle

        std::random_device rd;
        // std::mt19937 rndm(rd());
        std::vector<int> vec;
        if (P.empty())
        {
            vec.resize(vecnum);
            std::iota(vec.begin(), vec.end(), 0);
        }
        else
        {
            // std::cout << "P is not empty\n";
            vec = P;
        }
        size_t R = r > vec.size() ? vec.size() - 1 : r;
        std::vector shuff_vec = vec;

#pragma omp parallel
        {
            std::cout << "OpenMP is running init_graph with " << omp_get_num_threads() << " thread(s)." << std::endl;
            // Thread-local random number generator
            thread_local std::mt19937 rndm(rd() + omp_get_thread_num());
#pragma omp for
            for (size_t idx = 0; idx < vec.size(); idx++)
            {

                int p = vec[idx];

                // if(idx % 10000 ==0 ) std::cout<<"adding neighbors for p:"<<idx<<std::endl;
                std::unordered_set<int> neighbors;
                // while (neighbors.size() < R) {
                // get a random permutation of all nodes and add 1 each time making sure we dont add itself
                std::shuffle(shuff_vec.begin(), shuff_vec.end(), rndm);
                for (auto i : shuff_vec)
                {
                    if (i != p)
                    {
                        neighbors.insert(i);
                    }
                    if (neighbors.size() == R)
                    {
                        break;
                    }
                }
                // }
                graph[p] = std::move(neighbors);
            }
        }
    }

    void print_graph()
    {
        // int i=0;
        for (auto &i : graph)
        {
            std::cout << "for node " << std::setw(3) << i.first << ": ";
            for (auto &j : i.second)
            {
                std::cout << std::setw(3) << j << " ";
            }
            std::cout << std::endl;
        }
    }

    struct cmp_dist
    {
        const std::span<T> &p;
        const Matrix<T> &db;
        // mutable std::unordered_map<int, float> dist_cache;
        cmp_dist(const std::span<T> &p, const Matrix<T> &db) : p(p), db(db) {}
        bool operator()(int v1, int v2) const
        {
            //distance cache didnt do any improvements
            // if(dist_cache.find(v1) == dist_cache.end()) {
            //     dist_cache[v1] = Matrix<T>::sq_euclid(p, db.row(v1), db.dim);
            // }
            // if(dist_cache.find(v2) == dist_cache.end()) {
            //     dist_cache[v2] = Matrix<T>::sq_euclid(p, db.row(v2), db.dim);
            // }
            // return dist_cache[v1] < dist_cache[v2];
            return Matrix<T>::sq_euclid(p, db.row(v1), db.dim) < Matrix<T>::sq_euclid(p, db.row(v2), db.dim);
        }
    };
    void greedy_search_s(int start_idx, const std::span<T> &query, int k, size_t list_size, std::vector<int> &L, std::vector<int> &V)
    {
        L.reserve(k);
        V.reserve(2 * list_size);
        // auto cmp_L = [&](int v1, int v2)
        // {
        //     return Matrix<T>::sq_euclid(query, db.row(v1), db.dim) < Matrix<T>::sq_euclid(query, db.row(v2), db.dim);
        // };
        cmp_dist cmp(query, db);
        std::set<int, cmp_dist> Ls(cmp);
        std::set<int, cmp_dist> Vs(cmp);
        Ls.insert(start_idx);
        std::vector<int> diff;
        
        diff.push_back(start_idx);
        while (!diff.empty())
        {
            int p_star_idx = 0;
            p_star_idx = diff[0];
            // probably unnecessary check
            Vs.insert(p_star_idx);
            Ls.insert(graph[p_star_idx].begin(), graph[p_star_idx].end());
            // keep list_size closest to Xq
            if (Ls.size() > list_size)
            {
                auto it = Ls.begin();
                std::advance(it, list_size);
                Ls.erase(it, Ls.end());
            }
            diff.clear();
            std::ranges::set_difference(Ls, Vs, std::back_inserter(diff), cmp);
            // std::ranges::set_difference(Ls, Vs, std::back_inserter(diff), cmp_L);
        }
        // return k closest points from L
        auto it = Ls.end();
        if (Ls.size() > (size_t) k) {
            it = Ls.begin();
            std::advance(it, k);
        }
        std::copy(Ls.begin(), it, std::back_inserter(L));
        std::copy(Vs.begin(), Vs.end(), std::back_inserter(V));
    }

    void greedy_search(int start_idx, const std::span<T> &query, int k, size_t list_size, std::vector<int> &L, std::vector<int> &V)
    {
        L.reserve(2 * list_size);
        V.reserve(2 * list_size);
        L.push_back(start_idx);
        std::vector<int> diff;
        diff.reserve(list_size);
        diff.push_back(start_idx);
        auto cmp_dist = [&](int v1, int v2)
        {
            return Matrix<T>::sq_euclid(query, db.row(v1), db.dim) < Matrix<T>::sq_euclid(query, db.row(v2), db.dim);
        };
        while (!diff.empty())
        {
            int p_star_idx = 0;
            p_star_idx = diff[0];
            // probably unnecessary check
            V.push_back(p_star_idx);
            L.insert(L.end(), graph[p_star_idx].begin(), graph[p_star_idx].end());
            std::sort(V.begin(), V.end(), cmp_dist);
            std::sort(L.begin(), L.end(), cmp_dist);
            auto l = std::unique(L.begin(), L.end());
            L.erase(l, L.end());
            // keep list_size closest to Xq
            if (L.size() > list_size)
            {
                keep_k_closest(L, list_size);
            }
            diff.clear();
            std::ranges::set_difference(L, V, std::back_inserter(diff), cmp_dist);
        }
        // return k closest points from L
        if (L.size() > (size_t)k)
        {
            keep_k_closest(L, k);
        }
    }

    int save_graph(const std::string &path)
    {
        std::ofstream out(path, std::ios_base::binary | std::ios::trunc);

        if (!out.is_open())
        {
            std::cerr << "Error opening file for writing" << std::endl;
            return -1;
        }
        for (const auto &kv : graph)
        {
            int dim = kv.second.size() + 1;
            out.write(reinterpret_cast<const char *>(&dim), sizeof(int));
            out.write(reinterpret_cast<const char *>(&kv.first), sizeof(int));
            for (const auto &neighbor : kv.second)
            {
                out.write(reinterpret_cast<const char *>(&neighbor), sizeof(int));
            }
        }
        out.close();
        return 0;
    }

    int load_graph(const std::string &path)
    {
        std::ifstream in(path, std::ios_base::binary);
        if (!in.is_open())
        {
            std::cerr << "Error opening file for reading" << std::endl;
            return -1;
        }
        int dim;
        while (in.read(reinterpret_cast<char *>(&dim), sizeof(int)))
        {
            std::unordered_set<int> neighbors;
            int idx;
            in.read(reinterpret_cast<char *>(&idx), sizeof(int));
            for (int i = 0; i < dim - 1; i++)
            {
                int neighbor;
                in.read(reinterpret_cast<char *>(&neighbor), sizeof(int));
                neighbors.insert(neighbor);
            }
            graph[idx] = std::move(neighbors);
        }
        in.close();
        return 0;
    }

    void keep_k_closest(std::vector<int> &source, const int &k)
    {
        source.erase(source.begin() + k, source.end());
    }

    void robust_prune(int p, std::vector<int> &V, float a, size_t R)
    {
        graph[p].clear();
        auto vec1 = db.row(p);
        int p_star_idx;
        while (!V.empty())
        {
            p_star_idx = V[0];
            graph[p].insert(p_star_idx);
            if (graph[p].size() == R)
                break;
            auto vec2 = db.row(p_star_idx);
            V.erase(
                std::remove_if(V.begin(), V.end(),
                               [&](int i)
                               {
                                   auto v = db.row(i);
                                   return a * Matrix<T>::sq_euclid(vec2, v, db.dim) <= Matrix<T>::sq_euclid(vec1, v, db.dim);
                               }));
        }
    }

    void vamana_indexing(const int &medoid, float a, int list_size, size_t R, const std::vector<int> &P = {})
    {
        // create the permuation
        std::vector<int> sigma;
        if (P.empty())
        {
            for (size_t i = 0; i < db.vecnum; i++)
                sigma.push_back(i);
        }
        else
        {
            sigma = P;
        }
        std::random_device rd;
        std::mt19937 g(rd());
        // Shuffle the vector to get a random permutation
        std::shuffle(sigma.begin(), sigma.end(), g);
        for (auto i : sigma)
        {
            std::vector<int> L, V;
            // i must construct query
            auto q = db.row(i);
            greedy_search_s(medoid, q, 1, list_size, L, V);
            if (V[0] == i)
            {
                V.erase(V.begin());
            }
            for (auto nghbr : graph[i])
            {
                V.push_back(nghbr);
            }
            std::ranges::sort(V, [&](int v1, int v2)
                              { return Matrix<T>::sq_euclid(q, db.row(v1), db.dim) < Matrix<T>::sq_euclid(q, db.row(v2), db.dim); });

            robust_prune(i, V, a, R);
            // for all points j in Nout(i)
            for (const auto &j : graph[i])
            {
                graph[j].insert(i);
                if (graph[j].size() > R)
                {
                    std::vector<int> temp_neighbors(graph[j].begin(), graph[j].end());
                    std::sort(temp_neighbors.begin(), temp_neighbors.end(), [&](int v1, int v2)
                              { return Matrix<T>::sq_euclid(db.row(j), db.row(v1), db.dim) < Matrix<T>::sq_euclid(db.row(j), db.row(v2), db.dim); });
                    robust_prune(j, temp_neighbors, a, R);
                }
            }
        }
    }

    void filtered_greedy_search_s(std::unordered_map<T, int> &S, const std::span<T> &query, const size_t &k, const size_t &list_size, const T &Fq, std::vector<int> &L,
                                  std::vector<int> &V, int L_unfiltered = 1)
    {
        L.reserve(k);
        V.reserve(2 * list_size);
        cmp_dist cmp(query, db);
        std::set<int, cmp_dist> Ls(cmp);
        std::set<int, cmp_dist> Vs(cmp);
        // auto cmp_L = [&](int v1, int v2)
        // {
        //     // std::cout << "v1:" << v1 << " v2:" << v2 << std::endl;
        //     return Matrix<T>::sq_euclid(query, db.row(v1), db.dim) < Matrix<T>::sq_euclid(query, db.row(v2), db.dim);
        // };
        if (Fq == -1.0f)
        {
            // its a query node with type:0, so we assume it has all filters, must pass all starting points for each filter
            //  L.reserve(db.filters_set.size() + list_size);
            for (size_t i = 0; i < db.filters_set.size(); i++)
            {
                auto f = db.filters_set[i];
                std::vector<int> tempL, tempV;
                auto it = S.find(f); //[] operator adds a default value if the key is not already inside it, we dont want that
                if (it != S.end())
                {
                    // int startPoint = (it->second); // get the starting point for this filter

                    // run greedy to get the k=1 closest
                    //  printf("filter:%f\n",f);
                    filtered_greedy_search_s(S, query, L_unfiltered, 2*L_unfiltered, f, tempL, tempV);
                    // printf("%d\n",tempL[0]);
                        Ls.insert(tempL.begin(), tempL.end());
                }
            }
            // auto it = Ls.end();
            // if (Ls.size() > k)
            // {
            //     it = Ls.begin();
            //     std::advance(it, k);
            // }
            // std::copy(Ls.begin(), it, std::back_inserter(L));
            // return;
        }
        else
        {
            // its a node with only one filter, I only have one starting node.
            auto it = S.find(Fq);
            if (it != S.end())
            {
                Ls.insert(it->second);
            }
        }
        std::vector<int> diff(Ls.begin(), Ls.end());
        diff.reserve(list_size + Ls.size());
        while (!diff.empty())
        {
            int p_star_idx = diff[0];
            std::vector<int> temp; // keeping N'out(p*)
            std::vector<int> N_out;
#pragma omp critical
            {
                N_out = std::vector<int>(graph[p_star_idx].begin(), graph[p_star_idx].end());
            }
            // N_out.erase(std::remove_if(N_out.begin(), N_out.end(),
            // [&](int i) {
            //     return ((db.vec_filter)[i] == Fq || Fq == -1.0f) && (!std::ranges::binary_search(Vs, i, cmp_L));
            // }));
            // {
            temp.reserve(N_out.size());
            for (auto &p : N_out)
            {
                if (((db.vec_filter)[p] == Fq || Fq == -1.0f) && (!std::ranges::binary_search(Vs, p, cmp)))
                {
                    temp.push_back(p); // insert it
                }
            }
            // }
            Vs.insert(p_star_idx);
            Ls.insert(temp.begin(), temp.end()); // add it in L
            // keep list_size closest to Xq
            if (Ls.size() > list_size)
            {
                auto it = Ls.begin();
                std::advance(it, list_size);
                Ls.erase(it, Ls.end());
            }
            diff.clear();
            std::ranges::set_difference(Ls, Vs, std::back_inserter(diff), cmp);
        }
        // return k closest points from L
        auto it = Ls.end();
        if (Ls.size() > k)
        {
            it = Ls.begin();
            std::advance(it, k);
        }
        std::copy(Ls.begin(), it, std::back_inserter(L));
        std::copy(Vs.begin(), Vs.end(), std::back_inserter(V));
    }

    void filtered_greedy_search(std::unordered_map<T, int> &S, const std::span<T> &query, const size_t &k, const size_t &list_size, const T &Fq, std::vector<int> &L,
                                std::vector<int> &V)
    {
        L.reserve(2 * list_size);
        V.reserve(2 * list_size);
        L.clear();
        V.clear();
        if (Fq == -1.0f) {
            //its a query node with type:0, so we assume it has all filters, must pass all starting points for each filter
            L.reserve(db.filters_set.size() + list_size);            
            for (auto &f: (db.filters_set)) {
                // int startPoint;
                std::vector<int> tempL,tempV;
                auto it = S.find(f); //[] operator adds a default value if the key is not already inside it, we dont want that
                if (it != S.end()) {
                    // startPoint = (it->second); //get the starting point for this filter
                    
                    //run greedy to get the k=1 closest
                    filtered_greedy_search(S,query,1,1,f,tempL,tempV);
                    L.push_back(tempL[0]);
                }
            }
        }
        else
        {
            // its a node with only one filter, I only have one starting node.
            auto it = S.find(Fq);
            if (it != S.end())
            {
                L.push_back(it->second);
            }
        }
        auto cmp_dist = [&](int v1, int v2)
        {
            return Matrix<T>::sq_euclid(query, db.row(v1), db.dim) < Matrix<T>::sq_euclid(query, db.row(v2), db.dim);
        };
        if (L.size() > 1)
        {
            std::sort(L.begin(), L.end(), cmp_dist);
        }
        std::vector<int> diff(L.begin(), L.end());
        diff.reserve(list_size + L.size());
        while (!diff.empty())
        {
            // p*
            int p_star_idx = diff[0];
            std::vector<int> temp; // keeping N'out(p*)
            temp.reserve(graph[p_star_idx].size());
            for (auto &p : graph[p_star_idx])
            {
                if (((db.vec_filter)[p] == Fq || Fq == -1.0f) && (!std::ranges::binary_search(V, p, cmp_dist)))
                {
                    temp.push_back(p); // insert it
                }
            }
            V.push_back(p_star_idx);
            // update L with N'out(p*)
            L.insert(L.end(), temp.begin(), temp.end()); // add it in L
            std::sort(V.begin(), V.end(), cmp_dist);
            std::sort(L.begin(), L.end(), cmp_dist);
            auto l = std::unique(L.begin(), L.end());
            L.erase(l, L.end());
            // keep list_size closest to Xq
            if (L.size() > list_size)
            {
                keep_k_closest(L, list_size);
            }
            diff.clear();
            std::ranges::set_difference(L, V, std::back_inserter(diff), cmp_dist);
        }
        // return k closest points from L
        if (L.size() > (size_t)k)
        {
            keep_k_closest(L, k);
        }
    }

    void filtered_robust_prune(const int &p, std::vector<int> &V, const float &a, const size_t &R, std::vector<int> &N_out)
    {
        // add neighbors of p but remove p
        // neighbors of p are deleted
        int p_star_idx;

        auto vec1 = db.row(p);
        // std::vector<int> N_out;
        // #pragma omp critical
        // {
        //     std::cout << V.size() << std::endl;
        // }
        N_out.clear();
        while (!V.empty())
        {
            p_star_idx = V[0];
            // update neighbors of p
            //  graph[p].insert(p_star_idx);
            N_out.push_back(p_star_idx);
            if (N_out.size() == R)
                break;

            // for each p' in V
            // vec1: p, vec2:p*, vec3: p' in V
            auto vec2 = db.row(p_star_idx);
            V.erase(std::remove_if(V.begin(), V.end(),
                                   [&](int i)
                                   {
                                       auto v = db.row(i);
                                       return !(((db.vec_filter)[i] == (db.vec_filter)[p]) && ((db.vec_filter)[p_star_idx] != (db.vec_filter)[i])) && (a * Matrix<T>::sq_euclid(vec2, v, db.dim) <= Matrix<T>::sq_euclid(vec1, v, db.dim));
                                   }));
        }
// update neighbors of p
#pragma omp critical
        {
            graph[p].clear();
            graph[p].insert(N_out.begin(), N_out.end());
            // std::cout << graph[p].size() << std::endl;
        }
    }

    void filtered_vamana_indexing(std::unordered_map<T, int> &M, const float &a, const size_t &list_size, const size_t &R)
    {
        std::random_device rd;
        std::mt19937 g(rd());

        // random permutation
        std::vector<int> sigma;
        for (size_t i = 0; i < db.vecnum; i++)
            sigma.push_back(i);
        // std::cout<<"vecnum:"<<db.vecnum<<"\n";
        std::shuffle(sigma.begin(), sigma.end(), g);
// #pragma omp parallel
// {
// #pragma omp single
// {
#pragma omp parallel for schedule(runtime)
        for (size_t s = 0; s < db.vecnum; s++)
        {
            // #pragma omp task
            // {
            int i = sigma[s];
            //            std::cout << "for node  " << i << std::endl;
            std::vector<int> L, V;
            auto q = db.row(i);
            // #pragma omp critical
            // {
            filtered_greedy_search_s(M, q, 0, list_size, (db.vec_filter)[i], L, V);
            // }
            if (V[0] == i)
            {
                V.erase(V.begin());
            }
            std::vector<int> N_out;
#pragma omp critical
            {
                // std::cout << V.size() << std::endl;
                N_out = std::vector<int>(graph[i].begin(), graph[i].end());
                // std::cout <<"before " graph[i].size() << std::endl;
            }
            for (auto nghbr : N_out)
            {
                V.push_back(nghbr);
            }
            std::ranges::sort(V, [&](int v1, int v2)
                              { return Matrix<T>::sq_euclid(q, db.row(v1), db.dim) < Matrix<T>::sq_euclid(q, db.row(v2), db.dim); });
            // #pragma omp critical
            // {
            filtered_robust_prune(i, V, a, R, N_out);
            // std::cout << graph[i].size() << std::endl;
            // }
            // #pragma omp critical
            // {
            for (const auto &j : N_out)
            {
                size_t len;
#pragma omp critical
                {
                    graph[j].insert(i);
                    len = graph[j].size();
                }
                if (len > R)
                {
                    std::vector<int> temp_neighbors;
#pragma omp critical
                    {
                        temp_neighbors = std::vector<int>(graph[j].begin(), graph[j].end());
                    }
                    std::sort(temp_neighbors.begin(), temp_neighbors.end(), [&](int v1, int v2)
                              { return Matrix<T>::sq_euclid(db.row(j), db.row(v1), db.dim) < Matrix<T>::sq_euclid(db.row(j), db.row(v2), db.dim); });
                    std::vector<int> temp;
                    filtered_robust_prune(j, temp_neighbors, a, R, temp);
                }
            }
            // }
            // }
            // }
            // }
        }
    }

    // void stitched_vamana_indexing(float a, size_t R_small, size_t R_stitched, size_t L_small) {
    void stitched_vamana_indexing(float a, size_t R_small, size_t L_small) {

        std::unordered_map<T, std::unique_ptr<VamanaIndex> > Gf;
        putchar('\n');
        Gf.reserve(db.filters_set.size());
        for (auto f : db.filters_set)
        {
            Gf.emplace(f, nullptr);
        }
        //        std::cout << "filters_set size:" << db.filters_set.size() << std::endl;
// #pragma omp parallel for schedule(static,1) proc_bind(close)
#pragma omp parallel for schedule(static,1) proc_bind(close)
        for (size_t i = 0; i < db.filters_set.size(); i++)
        {
            auto f = db.filters_set[i];
            //            std::cout << "filter:" << f << " ";
            //            auto start = std::chrono::high_resolution_clock::now();
            Gf[f] = std::make_unique<VamanaIndex>(db);
            Gf[f]->init_graph(Pf[f]);
            int med = db.medoid_rand(Pf[f]);
            Gf[f]->vamana_indexing(med, a, L_small, R_small, Pf[f]);
            for (auto &kv : Gf[f]->graph)
            {
                // pare kathe vertice to Gf, pou ousiastika einai to Pf, kai valta ston megalo grafo G (this->graph)
                graph[kv.first] = std::move(kv.second);
            }
            //            auto end = std::chrono::high_resolution_clock::now();
            //            std::cout << "took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /1e6 << " s\n";
        }
    }
};

#endif // PROJECTJJ_GRAPH_H
