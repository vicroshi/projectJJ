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
#include <set>




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
    VamanaIndex(Matrix<T>*db):db(db){}; //default constructor for testing??
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

    void greedy_search(int start_idx,const std::span<T>& query,int k,size_t list_size,std::set<int>& L,std::set<int>& V) {
        L.insert(start_idx);
        V.clear(); //empty set

        //L-V
        std::set<int>difference;
        std::set_difference(L.begin(), L.end(), V.begin(), V.end(),
                        std::inserter(difference, difference.begin()));
    

        std::cout<<"size(L-V): "<<difference.size()<<std::endl; 
        std::cout<<"L-V:"<<std::endl;
            for(auto &item:difference) std::cout<<item<<" ";
            std::cout<<std::endl;
        // std::cout<<"good"<<std::endl;
        int stop=0;
        while( !(difference.empty()) ){
            stop++;
            int p_star_idx=-1;
            double dist,min_dist=std::numeric_limits<double>::max();  
            for(auto item:difference){
                                                //db->vecs.subspan[i*db->dim],db->dim
                std::cout<<"."<<std::endl;
                // std::cout<<"diff["<<i<<"]: ";
                for(auto &item:db->vecs.subspan(item*(db->dim),db->dim)){
                    std::cout<<item<<" ";
                }
                std::cout<<std::endl;
                dist=Matrix<T>::sq_euclid(query,db->vecs.subspan(item*(db->dim),db->dim));
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=item;
                }
            }
            std::cout<<"p_star_idx: "<<p_star_idx<<std::endl;

            //update L <- L U Nout(p_star)
            for(auto neighbor:graph[p_star_idx]){
                std::cout<<"neighbor: "<<neighbor+1<< " ->";
                for(auto& item:db->vecs.subspan(neighbor*(db->dim),db->dim)) std::cout<<item<<" ";
                std::cout<<std::endl;
                L.insert(neighbor);
            }
            std::cout<<"added all neighbors in L,new nize: "<<L.size()<<std::endl;
            if(p_star_idx!=-1)
                V.insert(p_star_idx);
            std::cout<<"added p* in V,V:"<<std::endl;
            for(auto &item:V) std::cout<<item<<" ";
            std::cout<<std::endl;
            if(L.size()>list_size){//keep list_size closest to Xq
                std::cout<<"|L| > list_size, have to cut some.."<<std::endl;
                std::cout<<"L before:"<<std::endl;
                for(auto item:L) std::cout<<item<<" ";
                keep_k_closest(L,list_size,query);
                std::cout<<"\nL after:"<<std::endl;
                for(auto item:L) std::cout<<item<<" ";
            }
            std::cout<<"checking L-V after loop"<<std::endl;
            //L-V
            difference.clear();
            std::set_difference(L.begin(), L.end(), V.begin(), V.end(),
                        std::inserter(difference, difference.begin()));
            std::cout<<"size(L-V): "<<difference.size()<<std::endl; 
            std::cout<<"L-V:"<<std::endl;
            for(auto &item:difference) std::cout<<item<<" ";
            std::cout<<std::endl;
        }

        //return k closest points from L
        keep_k_closest(L,k,query);
        return ;
    }
    
    void keep_k_closest(std::set<int>& source,int k,const std::span<T>& query){
        if( k<=0 || (size_t) k > source.size()) return;
        std::set<int> temp;
        // std::cout<<"\nfor query: ";
        // for(auto & item: query) std::cout<<item<<" ";
        // std::cout<<std::endl;
        for (int i=0;i<k;i++){
            // std::cout<<"---"<<std::endl;
            double dist=0,min_dist=std::numeric_limits<double>::max(); 
            int closest_idx=-1;
            for(auto item:source){
                // std::cout<<"point["<<j<<"]: ";
                // for(auto& item:db->vecs.subspan(source[j]*(db->dim),db->dim)){
                //     std::cout<<item<<" ";
                // }
                dist=Matrix<T>::sq_euclid(query,db->vecs.subspan(item*(db->dim),db->dim));
                // std::cout<<"dist:"<<dist<<std::endl;
                if(dist<min_dist){
                    min_dist=dist;
                    closest_idx=item;
                }
            }
            // std::cout<<"closest point: "<<closest_idx<<std::endl;
            //kratame to mikrotero index, to bgazoume apo to original
            temp.insert(closest_idx);
            source.erase( closest_idx);
            // std::cout<<"---"<<std::endl;
        }
        //kratame auta pou einai sto temp
        source = std::move(temp); 
        return ;
    }

    

};




#endif //PROJECTJJ_GRAPH_H
