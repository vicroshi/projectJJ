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





template <typename T>
struct VamanaIndex {
    std::vector<std::unordered_set<int>> graph;
    Matrix<T>* db;
    int vecnum;
    
    
    VamanaIndex(size_t deg, Matrix<T>* db):db(db){
        this->vecnum = db->vecnum;
        init_graph(deg);
    }
    VamanaIndex(Matrix<T>*db):db(db){//default constructor for testing
        graph.resize(db->vecnum);
    }; 
    
    void init_graph(size_t r){
        graph.resize(vecnum);
        //generator for shuffle
        std::random_device rd; 
        std::mt19937 rndm(rd()); 
        for(int idx = 0; idx < vecnum; ++idx){
            std::unordered_set<int> neighbors;
            while(neighbors.size()<r){
                int sample=std::uniform_int_distribution<int>(0,vecnum-1)(rndm);
                if(sample!=idx){
                    neighbors.insert(sample);
                    // graph[idx].insert(sample);
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
        // std::cout<<"size(L-V): "<<difference.size()<<std::endl; 
        // std::cout<<"L-V:"<<std::endl;
        //     for(auto &item:difference) std::cout<<item<<" ";
        //     std::cout<<std::endl;
        // std::cout<<"good"<<std::endl;
        while( !(difference.empty()) ){
            int p_star_idx=-1;
            double min_dist=std::numeric_limits<double>::max();  
            for(auto item:difference){
                                                //db->vecs.subspan[i*db->dim],db->dim
                // std::cout<<"."<<std::endl;
                // std::cout<<"calculating distance from query to point: ";
                // for(auto &item:db->vecs.subspan(item*(db->dim),db->dim)){
                //     std::cout<<item<<" ";
                // }
                // std::cout<<std::endl;


                auto vec1 = db->row(item); //p*
                auto vec2 = query; //query point
                auto dist =Matrix<T>::sq_euclid(vec1,vec2, vec1.size());
                // dist=Matrix<T>::sq_euclid(query,db->vecs.subspan(item*(db->dim),db->dim));
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=item;
                }
            }
            // std::cout<<"p_star_idx: "<<p_star_idx<<std::endl;

            //update L <- L U Nout(p_star)
            for(auto neighbor:graph[p_star_idx]){
                // std::cout<<"neighbor: "<<neighbor<< " ->";
                // for(auto& item:db->vecs.subspan(neighbor*(db->dim),db->dim)) std::cout<<item<<" ";
                // std::cout<<std::endl;
                L.insert(neighbor);
            }
            // std::cout<<"added all neighbors in L,new nize: "<<L.size()<<std::endl;
            if(p_star_idx!=-1)
                V.insert(p_star_idx);
            // std::cout<<"added p* in V,V:"<<std::endl;
            // for(auto &item:V) std::cout<<item<<" ";
            // std::cout<<std::endl;
            if(L.size()>list_size){//keep list_size closest to Xq
                // std::cout<<"|L| > list_size, have to cut some.."<<std::endl;
                // std::cout<<"L before:"<<std::endl;
                // for(auto item:L) std::cout<<item<<" ";
                keep_k_closest(L,list_size,query);
                // std::cout<<"\nL after:"<<std::endl;
                // for(auto item:L) std::cout<<item<<" ";
            }
            // std::cout<<"checking L-V after loop"<<std::endl;
            //L-V
            difference.clear();
            std::vector<int> vecL(L.begin(),L.end()); //copy to vector for mutable operations
            std::vector<int> vecV(V.begin(),V.end());
            std::sort(vecL.begin(),vecL.end());
            std::sort(vecV.begin(),vecV.end());
            std::set_difference(vecL.begin(), vecL.end(),vecV.begin(),vecV.end(), std::back_inserter(difference));
            // std::cout<<"size(L-V): "<<difference.size()<<std::endl; 
            // std::cout<<"L-V:"<<std::endl;
            // for(auto &item:difference) std::cout<<item<<" ";
            // std::cout<<std::endl;
        }
        //return k closest points from L
        keep_k_closest(L,k,query);
        return ;
    }
    
    void keep_k_closest(std::unordered_set<int>& source,int k,const std::span<T>& query){
        if( k<=0 || (size_t) k > source.size()) return;
        std::unordered_set<int> temp;
        // std::cout<<"\nfor query: ";
        // for(auto & item: query) std::cout<<item<<" ";
        // std::cout<<std::endl;
        for (int i=0;i<k;i++){
            // std::cout<<"---"<<std::endl;
            double min_dist=std::numeric_limits<double>::max(); 
            int closest_idx=-1;
            for(auto item:source){
                // std::cout<<"point["<<j<<"]: ";
                // for(auto& item:db->vecs.subspan(source[j]*(db->dim),db->dim)){
                //     std::cout<<item<<" ";
                // }

                auto vec1 = db->row(item);
                auto vec2 = query;
                auto dist =Matrix<T>::sq_euclid(vec1, vec2, vec1.size());

                // dist=Matrix<T>::sq_euclid(query,db->vecs.subspan(item*(db->dim),db->dim));
                // std::cout<<"dist:"<<dist<<std::endl;
                if(dist<min_dist){
                    min_dist=dist;
                    closest_idx=item;
                }
            }
            // std::cout<<"closest point: "<<closest_idx<<std::endl;
            //kratame to mikrotero index, to bgazoume apo to original
            temp.insert(closest_idx);
            source.erase(closest_idx);
            // std::cout<<"---"<<std::endl;
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
        // std::cout<<"p="<<p<<",(start)in V: ";
        // for(auto item:V) std::cout<<item<<" ";
        // std::cout<<std::endl;
        graph[p].clear();
        // std::cout<<"cleared neighbors of p"<<std::endl;
        int p_star_idx;
        while(!V.empty()){
            // std::cout<<"."<<std::endl;
            double min_dist=std::numeric_limits<double>::max(); 
            for(auto item:V){
                //distance tou p me kathe stoixeio tou V (p')      
                auto vec1 = db->row(p);
                auto vec2 = db->row(item);
                auto dist = Matrix<T>::sq_euclid(vec1, vec2,vec1.size());
                
                
                                               //p' \/
                // dist=Matrix<T>::sq_euclid(db->vecs.subspan(p*(db->dim),db->dim),db->vecs.subspan(item*(db->dim),db->dim));
                // std::cout<<"dist:"<<dist<<std::endl;
                if(dist<min_dist){
                    min_dist=dist;
                    p_star_idx=item;
                    // std::cout<<"(in if)p*: "<<p_star_idx<<std::endl;

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
                // std::cout<<"p': "<<item<<",p*: "<<p_star_idx<<std::endl;
                auto vec1 = db->row(p_star_idx);
                auto vec2 = db->row(item);
                auto d1 =Matrix<T>::sq_euclid(vec1,vec2, vec1.size());                
                                                                    //p*                                             //p'
                // double d1=Matrix<T>::sq_euclid(db->vecs.subspan(p_star_idx*(db->dim),db->dim),db->vecs.subspan(item*(db->dim),db->dim));
                // std::cout<<"d(p*,p'): "<<d1<<std::endl;

                vec1 = db->row(p);
                // auto vec2 = db->row(item);
                auto d2 = Matrix<T>::sq_euclid(vec1,vec2,vec1.size());               


                                                    //p                                                //p'
                // double d2=Matrix<T>::sq_euclid(db->vecs.subspan(p*(db->dim),db->dim),db->vecs.subspan(item*(db->dim),db->dim));
                // std::cout<<"d(p,p'): "<<d2<<std::endl;
                if(a*d1<=d2){
                    //std::cout<<"gotta remove p'"<<std::endl;
                    it=V.erase(it);// Erase and get new iterator position
                    // std::cout<<"a*d1:"<<a*d1<<"<=d2: "<<d2<<" , removing " <<item<<"from V.\n";
                }
                else{
                    it++; // Only increment if not erased
                }
            }
        }
    }

    void vamana_indexing(const int& medoid,float a,int list_size,size_t R){
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
            // std::cout<<i<<"\n";
            std::unordered_set<int>L,V;
            //i must construct query
            greedy_search(medoid,db->row(sigma[i]),1,list_size,L,V);
            robust_prune(sigma[i],V,a,R);
            //for all points j in Nout(sigma[i])
            for(auto j:graph[sigma[i]]){
                //temp vector to call search
                std::unordered_set<int> temp_neighbors = graph[j];
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
