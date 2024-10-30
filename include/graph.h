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
#include "distance.h"
#include <set>

#define MEDOID 3732


template <typename T>
struct VamanaIndex {
    std::vector<std::set<int>> graph;
    Matrix<T>* db;
    int vecnum;
    size_t L_size;

    int k;

    float a;

    size_t R;

    VamanaIndex(size_t R, size_t L_size, int k, float a, Matrix<T>* db)
        :R(R), L_size(L_size), k(k), a(a), db(db) {
        this->vecnum = db->vecnum;
        std::ranges::iota_view<int,int> range = std::ranges::iota_view(0, vecnum);
        std::cout << "Indexing..." << std::endl;
        init_graph(R, range);
        int medoid = MEDOID;
        vamana_indexing(a,L_size,R);

//        std::vector<int> perm(range.begin(), range.end());
//        std::random_device rd;
//        std::mt19937 g(rd());
//        std::ranges::shuffle(perm, g);
////        print_graph();
//        for(auto &i:perm){
////            std::cout<< i<< " ";
//            std::set<int> L,V;
//            greedy_search(medoid,db->get_row(i),1,L_size, L,V);
//            robust_prune(i,V,a,R);
//            for (auto &j:graph[i]){
//                std::set<int> tmp = graph[j];
//                tmp.insert(i);
//                if(tmp.size() > R){
//                    robust_prune(j,tmp,a,R);
//                }
//                else {
//                    graph[j].insert(i);
//                }
//            }
//        }
    }

    VamanaIndex(size_t R, Matrix<T>*db):db(db){}

    ; //default constructor for testing??
    void init_graph(int r, std::ranges::iota_view<int, int> range ) {
        graph.resize(vecnum);
        int idx = 0;
        //new feature, ranges (iterator class)
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
        int i=0;
        for (auto &set : graph) {
            std::cout<<"for node "<<std::setw(3)<< i++<<": ";
            for (auto &i : set) {
                std::cout <<std::setw(3) << i << " ";
            }
            std::cout << std::endl;
        }
    }

    void greedy_search(int start_idx,const std::span<T>& query,int k,size_t list_size,std::set<int>& L, std::set<int>& V) {
        L.insert(start_idx);
        V.clear(); //empty set
        //L-V
        std::vector<int>difference;
        difference.reserve(L.size());
        std::set_difference(L.begin(), L.end(), V.begin(), V.end(),
//                        std::inserter(difference, difference.begin()));
                        std::back_inserter(difference));
        // std::cout<<"size(L-V): "<<difference.size()<<std::endl;
        // std::cout<<"L-V:"<<std::endl;
        //     for(auto &item:difference) std::cout<<item<<" ";
        //     std::cout<<std::endl;
        // std::cout<<"good"<<std::endl;
        while( !(difference.empty()) ){
            int p_star_idx=-1;
            double min_dist=std::numeric_limits<double>::max();
            for(auto i:difference){
                auto dist=Matrix<T>::sq_euclid(query,db->get_row(i));
//                dist=sq_euclid(reinterpret_cast<float *>(query.data()),reinterpret_cast<float *>(db->get_row(i).data()),db->dim);
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=i;
                }
            }
            //update L <- L U Nout(p_star)
//            for(auto neighbor:graph[p_star_idx]){
//                L.insert(neighbor);
//            }
            std::set_union(
                L.begin(), L.end(),
                graph[p_star_idx].begin(), graph[p_star_idx].end(),
                std::inserter(L, L.end())
            );
            if(p_star_idx!=-1)
                V.insert(p_star_idx);
            if(L.size()>list_size){//keep list_size closest to Xq
                keep_k_closest(L,list_size,query);
            }
            // std::cout<<"checking L-V after loop"<<std::endl;
            //L-V
            difference.clear();
            std::set_difference(L.begin(), L.end(), V.begin(), V.end(),
                        std::inserter(difference, difference.begin()));
            // std::cout<<"size(L-V): "<<difference.size()<<std::endl;
            // std::cout<<"L-V:"<<std::endl;
            // for(auto &item:difference) std::cout<<item<<" ";
            // std::cout<<std::endl;
        }
        //return k closest points from L
        keep_k_closest(L,k,query);
    }

    void keep_k_closest(std::set<int>& source,int k,const std::span<T>& query){
        if( k<=0 || (size_t) k > source.size()) return;
        std::vector<int> temp(std::make_move_iterator(source.begin()),std::make_move_iterator(source.end()));
        std::sort(temp.begin(),temp.end(),[&](int a,int b){
            return Matrix<T>::sq_euclid(query,db->get_row(a))<Matrix<T>::sq_euclid(query,db->get_row(b));
        });
        source.clear();
        for(int i=0;i<k;i++){
            source.insert(temp[i]);
        }
////        temp.reserve(k);
//        // std::cout<<"\nfor query: ";
//        // for(auto & item: query) std::cout<<item<<" ";
//        // std::cout<<std::endl;
//            for (int i=0;i<k;i++){
//            // std::cout<<"---"<<std::endl;
//            double dist=0,min_dist=std::numeric_limits<double>::max();
//            int closest_idx=-1;
//            for(auto item:source){
//                // std::cout<<"point["<<j<<"]: ";
//                // for(auto& item:db->vecs.subspan(source[j]*(db->dim),db->dim)){
//                //     std::cout<<item<<" ";
//                // }
//                dist=Matrix<T>::sq_euclid(query,db->get_row(item));
////                dist=sq_euclid(reinterpret_cast<float *>(query.data()),reinterpret_cast<float *>(db->get_row(item).data()),db->dim);
//                // std::cout<<"dist:"<<dist<<std::endl;
//                if(dist<min_dist){
//                    min_dist=dist;
//                    closest_idx=item;
//                }
//            }
//            // std::cout<<"closest point: "<<closest_idx<<std::endl;
//            //kratame to mikrotero index, to bgazoume apo to original
//            temp.insert(closest_idx);
//            source.erase( closest_idx);
//            // std::cout<<"---"<<std::endl;
//        }
//        //kratame auta pou einai sto temp
//        source = std::move(temp);
//        return ;
    }

    void robust_prune(int p,std::set<int>& V,float a,size_t R){
        for(auto item:graph[p]){ //bazoume tous geitones tou p
            V.insert(item);
        }
        V.erase(p); //bgazoume to p
        // std::cout<<"p="<<p<<",(start)in V: ";
        // for(auto item:V) std::cout<<item<<" ";
        // std::cout<<std::endl;
        graph[p].clear();
        // std::cout<<"cleared neighbors of p"<<std::endl;
        int p_star_idx;
        while(!V.empty()){
            //std::cout<<"."<<std::endl;
            double dist=0,min_dist=std::numeric_limits<double>::max();
            for(auto i:V){
                //distance tou p me kathe stoixeio tou V (p')
                dist=Matrix<T>::sq_euclid(db->get_row(p),db->get_row(i));
//                dist=sq_euclid(reinterpret_cast<float *>(db->get_row(p).data()),reinterpret_cast<float *>(db->get_row(i).data()),db->dim);
                //std::cout<<"dist:"<<dist<<std::endl;
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=i;
                    //std::cout<<"(in if)p*: "<<p_star_idx<<std::endl;
                }
            }
            // std::cout<<"p*: "<<p_star_idx<<std::endl;
            //update neighbors of p
            graph[p].insert(p_star_idx);
            // std::cout<<"added p* in neighbor,size:"<<graph[p].size()<<std::endl;
            if(graph[p].size()==R){
                // std::cout<<"about to exit."<<std::endl;
                break;
            }
            // std::cout<<"about to remove p' "<<std::endl;
            for (auto it=V.begin();it!=V.end();){
                int item=*it;
                auto vec1 = db->get_row(p_star_idx);
                auto vec2 = db->get_row(item);
                auto d1 = db->sq_euclid(vec1, vec2);
                vec1 = db->get_row(p);
                auto d2 = db->sq_euclid(vec1, vec2);


                                                    //p                                                //p'
                // double d2=Matrix<T>::sq_euclid(db->vecs.subspan(p*(db->dim),db->dim),db->vecs.subspan(item*(db->dim),db->dim));
                // std::cout<<"d(p,p'): "<<d2<<std::endl;
                if(a*d1<=d2){
                    //std::cout<<"gotta remove p'"<<std::endl;
                    it=V.erase(it);// Erase and get new iterator position
                }
                else{
                    it++; // Only increment if not erased
                }
            }
        }
    }

    void vamana_indexing(float a,int list_size,size_t R){
        // int medoid=db->medoid_naive();
        int medoid=MEDOID;
        // std::cout<<"medoid: "<<medoid<<std::endl;
        //create the permuation
        std::vector<int>sigma;
        for(size_t i=0;i<db->vecnum;i++) sigma.push_back(i);
        std::random_device rd;
        std::mt19937 g(rd());
        // Shuffle the vector to get a random permutation
        std::shuffle(sigma.begin(), sigma.end(), g);
        // std::cout<<"permutation:\n";
        // for(auto item:sigma) std::cout<<item<<" ";
        // std::cout<<"\n";
        for(size_t i=0;i<sigma.size();i++){
//            std::cout<<i<<"\n";
            std::set<int>L,V;
            //i must construct query
            greedy_search(medoid,db->vecs.subspan(sigma[i]*(db->dim),db->dim),1,list_size,L,V);
            robust_prune(sigma[i],V,a,R);
            //for all points j in Nout(sigma[i])
            for(auto j:graph[sigma[i]]){
                //temp set to call search
                std::set<int> temp_neighbors = graph[j];
                temp_neighbors.insert(sigma[i]);
                if(temp_neighbors.size() > R){
                    robust_prune(j,temp_neighbors,a,R);
                }
                else{
                    graph[j].insert(sigma[i]);
                }
            }
        }
    }



};


#endif //PROJECTJJ_GRAPH_H
