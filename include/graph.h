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
#include <set>
#include <unordered_set>
#include <ranges>
#include <random>
#include <span>
#include <iomanip>
#include <database.h>
#include "database.h"
#include <iterator> // for std::back_inserter
#include <unordered_map>


template <typename T>
struct VamanaIndex {
    std::vector<std::unordered_set<int>> graph;
    Matrix<T>* db;
    int vecnum;
    
    //constructor used for main
    VamanaIndex(size_t deg, Matrix<T>* db):db(db){
        this->vecnum = db->vecnum;
        init_graph(deg);
    }
    //constructor for testing where we need to have a fixed graph each time. we add neighbors manually
    VamanaIndex(Matrix<T>*db):db(db){
        graph.resize(db->vecnum);
    } 
    
    void init_graph(const size_t& r){
        graph.resize(vecnum);
        //generator for shuffle
        std::random_device rd; 
        std::mt19937 rndm(rd()); 
        for(int idx = 0; idx < vecnum; ++idx){
            std::unordered_set<int> neighbors;
            while(neighbors.size()<r){
                //get a random permutation of all nodes and add 1 each time making sure we dont add itself
                int sample=std::uniform_int_distribution<int>(0,vecnum-1)(rndm);
                if(sample!=idx){
                    neighbors.insert(sample);
                }
            }
            graph[idx]=std::move(neighbors);        
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

    void greedy_search(int start_idx,const std::span<T>& query,int k,size_t list_size,std::unordered_set<int>& L,std::unordered_set<int>& V) {
        L.insert(start_idx);
        V.clear(); //empty set
        //L-V
        std::vector<int> vecL(L.begin(),L.end()); //copy to vector for mutable operations
        std::vector<int> vecV(V.begin(), V.end());
        std::sort(vecL.begin(), vecL.end());
        std::sort(vecV.begin(), vecV.end());
        std::vector<int> difference;
        std::set_difference(vecL.begin(), vecL.end(), vecV.begin(), vecV.end(), std::back_inserter(difference));
        
        while( !(difference.empty()) ){
            int p_star_idx=-1;
            double min_dist=std::numeric_limits<double>::max();  
            for(const auto& item:difference){
                auto vec1 = db->row(item); //p*
                auto vec2 = query; //query point
                auto dist =Matrix<T>::sq_euclid(vec1,vec2, vec1.size());
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=item;
                }
            }

            //update L <- L U Nout(p_star)
            for(const auto& neighbor:graph[p_star_idx]){
                L.insert(neighbor);
            }
            //probably unnecessary check
            if(p_star_idx!=-1)
                V.insert(p_star_idx);

            //keep list_size closest to Xq
            if(L.size()>list_size)
                keep_k_closest(L,list_size,query);

            //L-V
            difference.clear();
            std::vector<int> vecL(L.begin(),L.end()); //copy to vector for mutable operations
            std::vector<int> vecV(V.begin(),V.end());
            std::sort(vecL.begin(),vecL.end());
            std::sort(vecV.begin(),vecV.end());
            std::set_difference(vecL.begin(), vecL.end(),vecV.begin(),vecV.end(), std::back_inserter(difference));
        }
        //return k closest points from L
        keep_k_closest(L,k,query);
        return ;
    }
    
    void keep_k_closest(std::unordered_set<int>& source,const int& k,const std::span<T>& query){
        if( k<=0 || (size_t) k > source.size()) return;
        std::unordered_set<int> temp;
        for (int i=0;i<k;i++){
            double min_dist=std::numeric_limits<double>::max(); 
            int closest_idx=-1;
            for(const auto& item:source){
                auto vec1 = db->row(item);
                auto vec2 = query;
                auto dist =Matrix<T>::sq_euclid(vec1, vec2, vec1.size());
                if(dist<min_dist){
                    min_dist=dist;
                    closest_idx=item;
                }
            }
            //kratame to mikrotero index, to bgazoume apo to original
            temp.insert(closest_idx);
            source.erase(closest_idx);
        }
        //kratame auta pou einai sto temp
        source=std::move(temp); 
        return ;
    }

    void robust_prune(int p,std::unordered_set<int>& V,float a,size_t R){
        for(const auto & item:graph[p]){ //bazoume tous geitones tou p
            V.insert(item);
        }
        V.erase(p); //bgazoume to p
        graph[p].clear();
        int p_star_idx;
        while(!V.empty()){
            double min_dist=std::numeric_limits<double>::max(); 
            for(const auto & item:V){
                //distance tou p me kathe stoixeio tou V (p')      
                auto vec1 = db->row(p);
                auto vec2 = db->row(item); //
                auto dist = Matrix<T>::sq_euclid(vec1, vec2,vec1.size());
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=item;
                }
            }
            //update neighbors of p
            graph[p].insert(p_star_idx);

            if(graph[p].size()==R)
                break;
            
            for (auto it=V.begin();it!=V.end();){
                int item=*it;
                auto vec1 = db->row(p_star_idx);
                auto vec2 = db->row(item);
                auto d1 =Matrix<T>::sq_euclid(vec1,vec2, vec1.size());                
                                                                    
                vec1 = db->row(p);
                auto d2 = Matrix<T>::sq_euclid(vec1,vec2,vec1.size());   

                if(a*d1<=d2)
                    it=V.erase(it);//erase and get new iterator position
                else
                    it++; //only increment if not erased
                
            }
        }
    }

    void vamana_indexing(const int& medoid,float a,int list_size,size_t R){
        //create the permuation
        std::vector<int>sigma;
        for(size_t i=0;i<db->vecnum;i++) sigma.push_back(i);
        std::random_device rd;
        std::mt19937 g(rd());
        // Shuffle the vector to get a random permutation
        std::shuffle(sigma.begin(), sigma.end(), g);
        for(size_t i=0;i<sigma.size();i++){
            std::unordered_set<int>L,V;
            //i must construct query
            greedy_search(medoid,db->row(sigma[i]),1,list_size,L,V);
            robust_prune(sigma[i],V,a,R);
            //for all points j in Nout(sigma[i])
            for(const auto & j:graph[sigma[i]]){
                //temp vector to call search
                std::unordered_set<int> temp_neighbors = graph[j];
                temp_neighbors.insert(sigma[i]);
                if(temp_neighbors.size() > R)
                    robust_prune(j,temp_neighbors,a,R);
                else
                    graph[j].insert(sigma[i]);
            }
        }
    }
    void filtered_greedy_search(std::unordered_map<T,int>& S, const std::span<T>& query, const size_t& k, const size_t& list_size,const T& Fq, std::unordered_set<int>& L, std::unordered_set<int>& V) {
        L.clear();
        V.clear();
        if(Fq==-1.0f){ //its a query node with type:0, so we assume it has all filters, must pass all starting points for each filter
            for(auto &f:(*db->filters_set)){
                auto it = S.find(f); //[] operator adds a default value if the key is not already inside it, we dont want that
                if (it != S.end()) {
                    L.insert(it->second); //add all starting points
                }
            }
        }
        else{ //its a node with only one filter, I only have one starting node.
            auto it = S.find(Fq);
            if (it != S.end()) {
                L.insert(it->second);
            }
        }
        //L-V
        std::vector<int> vecL(L.begin(),L.end());
        std::vector<int> vecV(V.begin(), V.end());
        std::sort(vecL.begin(), vecL.end());
        std::sort(vecV.begin(), vecV.end());
        std::vector<int> difference;
        std::set_difference(vecL.begin(), vecL.end(), vecV.begin(), vecV.end(), std::back_inserter(difference));
        while( !(difference.empty()) ){
            //p*
            int p_star_idx=-1;
            double min_dist=std::numeric_limits<double>::max();
            for(const auto& item:difference){//p in L-V
                auto vec1 = db->row(item);
                auto vec2 = query; //query point
                auto dist =Matrix<T>::sq_euclid(vec1,vec2, vec1.size());
                // std::cout<<dist<<std::endl;
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=item;
                    // std::cout<<"new min dist:"<<min_dist<<std::endl;
                }
            }
            //probably unnecessary check, p* gets inserted in V
            if(p_star_idx!=-1)
                V.insert(p_star_idx);

            std::unordered_set<int> temp; //keeping N'out(p*)
            for(auto& p: graph[p_star_idx]){
                // if(q)                    std::cout<<"--\n";

                // if(q){
                //     std::cout<<"Fp':"<<(*db->vec_filter)[p]<<", Fq:"<<Fq<<"\n";


                // }
                if(((*db->vec_filter)[p]==Fq || Fq==-1.0f ) && (V.count(p)==0)){
                    temp.insert(p); //insert it
                }
            }
            //update L with N'out(p*)
            L.insert(temp.begin(),temp.end()); //add it in L

            //keep list_size closest to Xq
            if(L.size()>list_size)
                keep_k_closest(L,list_size,query);

            //L-V
            difference.clear();
            std::vector<int> vecL(L.begin(),L.end()); //copy to vector for mutable operations
            std::vector<int> vecV(V.begin(),V.end());
            std::sort(vecL.begin(),vecL.end());
            std::sort(vecV.begin(),vecV.end());
            std::set_difference(vecL.begin(), vecL.end(),vecV.begin(),vecV.end(), std::back_inserter(difference));
                            // if(q)                    std::cout<<"--\n";

            // if(q){
            //     std::cout<<"VecV:\n";
            //     for(auto &i: vecV){
            //         std::cout<<i<<" ";

            //     }
            //     std::cout<<std::endl;
            //     std::cout<<"VecL:\n";
            //     for(auto &i: vecL){
            //         std::cout<<i<<" ";

            //     }
            //     std::cout<<std::endl;

            //     std::cout<<"---\n";
            // }

        }
        //return k closest points from L
        keep_k_closest(L,k,query);
        // std::cout<<"L.size before return:"<<L.size();
        // std::cout<<"\n----------\n";
    }
};



#endif //PROJECTJJ_GRAPH_H
