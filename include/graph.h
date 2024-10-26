//
// Created by vic on 17/10/2024.
//

#ifndef PROJECTJJ_GRAPH_H
#define PROJECTJJ_GRAPH_H

#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <ranges>
#include <random>
#include <span>
#include <iomanip>
#include <database.h>
#include "database.h"




template <typename T>
struct VamanaIndex {
    std::vector<std::unordered_set<int>> graph;
    Matrix<T>* db;
    int vecnum;
    //                   \/ should not be just T, maybe Matrix<T>?
    VamanaIndex(size_t deg, Matrix<T>* db):db(db) {
        this->vecnum = db->vecnum;
        init_graph(deg);

    }
    //den prepei na einai gia kathe ena simeio db?
    void init_graph(size_t r){
        graph.resize(vecnum);
        int idx = 0;
        //new feature, ranges (iterator class)
        const auto range = std::ranges::iota_view(0, vecnum);
        std::vector<int> idcs(range.begin(), range.end());
        std::vector<int> samp;
        
        for (auto &set : graph) {
            //to bazw stin arxi gia na min to psaksw otan prepei na to sbisw
            samp.clear();
            set.insert(idx);
            std::sample(idcs.begin(),idcs.end(), back_inserter(samp), r+1, std::mt19937{std::random_device{}()});
            set.insert(samp.begin(), samp.end());
            set.erase(idx);
            //thelw na parw tyxaious r geitones. an enas apo tous r einai o eautos mou, exw r-1. ara me r+1 tha exw toulaxiston r geitones. an telika
            //den eimai mesa sto sample, exw r+1 geitones, opote apla bgazw enan manually kai eimai komple
            if (set.size() > r) {
                set.erase(set.begin());
            }
            idx++;
        }
    }

    void print_graph(){
        for (auto &set : graph) {
            for (auto &i : set) {
                std::cout <<std::setw(3) << i << " ";
            }
            std::cout << std::endl;
        }
    }

    void greedy_search(int start_idx,const std::span<T>& query,int k,int list_size,std::vector<int>& L,std::vector<int>& V) {
        L.push_back(start_idx);
        V.clear(); //empty set
        std::vector<int>difference;
        //L-V
        //check if index is in L but not in V
        for (int i=0;i<L.size();i++) {
            if (std::find(V.begin(), V.end(), (L[i])) == V.end()) {
                difference.push_back((L[i]));
            }
        }
        double dist,min_dist=std::numeric_limits<double>::max();  
        while( !(difference.empty()) ){
            int p_star_idx=-1;
            for(size_t i=0;i<difference.size();i++){
                                                //db->vecs.subspan[i*db->dim],db->dim
                dist=Matrix<T>::sq_euclid(query,db->vecs.subspan(i*(db->dim),db->dim));
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=i;
                }
            }
            //update L <- L U Nout(p_star)
            for(auto neighbor:graph[p_star_idx])
                L.push_back(neighbor);
            
            if(p_star_idx!=-1)
                V.push_back(p_star_idx);
            if(L.size()>list_size)//keep list_size closest to Xq
                keep_k_closest(L,list_size,query);
            
            //check again L-V
            difference.clear();
            for (const auto& item : L) {
                if (std::find(V.begin(), V.end(), item) == V.end()) {
                    difference.push_back(item);
                }
            }  
        }
        //return k closest points from L
        keep_k_closest(L,k,query);
        return ;
    }

    void keep_k_closest(std::vector<int>& source,int k,const std::span<T>& query){
        if( k<=0 || (size_t) k > source.size()) return;
        std::vector<int> temp;
        for (int i=0;i<k;i++){
            double dist,min_dist=std::numeric_limits<double>::max(); 
            int closest_idx=-1;
            for(size_t j=0;j<source.size();j++){
                dist=Matrix<T>::sq_euclid(query,db->vecs.subspan(source[j]*(db->dim),db->dim));
                if(dist<min_dist){
                    min_dist=dist;
                    closest_idx=j;
                }
            }
            //kratame to mikrotero index, to bgazoume apo to original
            temp.push_back(source[closest_idx]);
            source.erase(source.begin() + closest_idx);
        }
        //kratame auta pou einai sto temp
        source = std::move(temp); 
        return ;
    }

    

};




#endif //PROJECTJJ_GRAPH_H
