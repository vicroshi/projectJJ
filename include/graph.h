//
// Created by vic on 17/10/2024.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <random>
#include <span>
#include <iomanip>
#include "database.h"
#include <iterator> // for std::back_inserter
#include <unordered_map>
#include <fstream>

template <typename T>
struct VamanaIndex {
//    std::vector<std::unordered_set<int>> graph;
    std::unordered_map<int,std::unordered_set<int>> graph;
    Matrix<T>* db;
    size_t vecnum;
    size_t deg;
    std::unordered_map<T,std::vector<int>> Pf;
    VamanaIndex() : db(nullptr), vecnum(0), deg(0) {}

    //constructor used for main
    VamanaIndex(size_t deg, Matrix<T>* db):db(db){
        this->vecnum = db->vecnum;
        init_graph(deg);
    }

    //gia stitched
    VamanaIndex(Matrix<T>*db, size_t deg, size_t vecnum):db(db),vecnum(vecnum),deg(deg){
    }
    //constructor for testing where we need to have a fixed graph each time. we add neighbors manually
    VamanaIndex(Matrix<T>*db):db(db){
//        graph.resize(db->vecnum);
        this->vecnum = db->vecnum;
        graph.reserve(db->vecnum);
    }

    void init_graph(const size_t& r, const std::vector<int>& P = {}){
//        graph.reserve(vecnum);
        //generator for shuffle
        std::random_device rd;
        std::mt19937 rndm(rd());
        std::vector<int> vec;
        if(P.empty()){
            vec.resize(vecnum);
            std::iota(vec.begin(), vec.end(), 0);
        }
        else{
            // std::cout << "P is not empty\n";
            vec=P;
        }
        size_t R = r > vec.size() ? vec.size()-1 : r;
        std::vector shuff_vec = vec;
        for (auto p : vec) {
            std::unordered_set<int> neighbors;
            while (neighbors.size() < R) {
                //get a random permutation of all nodes and add 1 each time making sure we dont add itself
                std::shuffle(shuff_vec.begin(), shuff_vec.end(), rndm);
                for (auto i : shuff_vec) {
                    if (i != p) {
                        neighbors.insert(i);
                    }
                    if (neighbors.size() == R) {
                        break;
                    }
                }
            }
            graph[p] = std::move(neighbors);
        }
    }

    void print_graph(){
        // int i=0;
        for (auto &i : graph) {
            std::cout<<"for node "<<std::setw(3)<< i.first<<": ";
            for (auto &j : i.second) {
                std::cout <<std::setw(3) << j << " ";
            }
            std::cout << std::endl;
        }
    }

    void greedy_search(int start_idx,const std::span<T>& query,int k,size_t list_size,std::unordered_set<int>& L,std::unordered_set<int>& V) {
        L.insert(start_idx);
        V.clear(); //empty set
        //L-V
        // std::vector<int> vecL(L.begin(),L.end()); //copy to vector for mutable operations
        // std::vector<int> vecV(V.begin(), V.end());
        // std::sort(vecL.begin(), vecL.end());
        // std::sort(vecV.begin(), vecV.end());
        // std::vector<int> difference;
        // std::set_difference(vecL.begin(), vecL.end(), vecV.begin(), vecV.end(), std::back_inserter(difference));
        
        //at first iteration only elements of L will be used, since V is empty, L-V = L
        std::vector<int> difference(L.begin(),L.end()); 
        auto vec2 = query; //query point

        while( !(difference.empty()) ){
            int p_star_idx=-1;
            double min_dist=std::numeric_limits<double>::max();  
            for(const auto& item:difference){
                auto vec1 = db->row(item); //p*
                
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
    
    int save_graph(const std::string& path){
        std::ofstream out(path, std::ios_base::binary | std::ios::trunc);

        if(!out.is_open()){
            std::cerr<<"Error opening file for writing"<<std::endl;
            return -1;
        }
        for(const auto& kv:graph){
            int dim = kv.second.size() + 1;
            out.write(reinterpret_cast<const char*>(&dim), sizeof(int));
            out.write(reinterpret_cast<const char*>(&kv.first), sizeof(int));
            for(const auto& neighbor:kv.second){
                out.write(reinterpret_cast<const char*>(&neighbor), sizeof(int));
            }
        }
        out.close();
        return 0;
    }

    int load_graph(const std::string& path){
        std::ifstream in(path, std::ios_base::binary);
        if(!in.is_open()){
            std::cerr<<"Error opening file for reading"<<std::endl;
            return -1;
        }
        int dim;
        while(in.read(reinterpret_cast<char*>(&dim), sizeof(int))){
            std::unordered_set<int> neighbors;
            int idx;
            in.read(reinterpret_cast<char*>(&idx), sizeof(int));
            for(int i=0;i<dim-1;i++){
                int neighbor;
                in.read(reinterpret_cast<char*>(&neighbor), sizeof(int));
                neighbors.insert(neighbor);
            }
            graph[idx]=std::move(neighbors);
        }
        in.close();
        return 0;
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

    void robust_prune(int p,std::vector<int>& V,float a,size_t R){
//        V.erase(p); //bgazoume to p
        graph[p].clear();

        auto vec1 = db->row(p);

        int p_star_idx;
        // std::sort(V.begin(),V.end(),[&](int v1,int v2){
        //     return Matrix<T>::sq_euclid(db->row(p),db->row(v1), db->dim) < Matrix<T>::sq_euclid(db->row(p),db->row(v2),db->dim);
        // });
        std::sort(V.begin(),V.end(),[&](int v1,int v2){
                return Matrix<T>::sq_euclid(db->row(p),db->row(v1), db->dim) < Matrix<T>::sq_euclid(db->row(p),db->row(v2),db->dim);
        }); 
        while(!V.empty()){
            p_star_idx=V[0];
            graph[p].insert(p_star_idx);
            if(graph[p].size()==R)
                break;

            //vec1: p , vec2: p*, vec3: p' in V
            auto vec1 = db->row(p);
            auto vec2 = db->row(p_star_idx);
            for (auto it=V.begin();it!=V.end();){
                int i=*it;
                auto vec3 = db->row(i);
                auto d1 =Matrix<T>::sq_euclid(vec2,vec3, vec1.size()); //d(p*,p')
                                                                    
                auto d2 = Matrix<T>::sq_euclid(vec1,vec3,vec1.size());   //d(p,p')

                if(a*d1<=d2)
                    it=V.erase(it);//erase and get new iterator position
                else
                    it++; //only increment if not erased
                
            }
        }
    }

    void vamana_indexing(const int& medoid,float a,int list_size,size_t R, const std::vector<int> &P = {}){
        //create the permuation
        std::vector<int>sigma;
        if (P.empty()) {
            for(size_t i=0;i<db->vecnum;i++) sigma.push_back(i);
        }
        else{
            sigma=P;
        }
        std::random_device rd;
        std::mt19937 g(rd());
        // Shuffle the vector to get a random permutation
        std::shuffle(sigma.begin(), sigma.end(), g);
        for(auto i:sigma){
            std::unordered_set<int>L,V;
            //i must construct query
            greedy_search(medoid,db->row(i),1,list_size,L,V);
            V.erase(i);
            std::vector<int> tmp;
            tmp.reserve(V.size() + graph[i].size());
            std::set_union(V.begin(), V.end(), graph[i].begin(), graph[i].end(), std::back_inserter(tmp));
            // if(i  == 4867){
            //     std::cout<<"neighbors of 4867 before prune:\n";
            //     for(auto j:tmp){
            //         std::cout<<j<<" ";
            //     }
            //     putchar('\n');
            // }
            robust_prune(i,tmp,a,R);
            // if(i  == 4867){
            //     std::cout<<"neighbors of 4867 after prune:\n";
            //     for(auto j:graph[i]){
            //         std::cout<<j<<" ";
            //     }
            //     putchar('\n');
            // }
            //for all points j in Nout(i)
            for(const auto & j:graph[i]){
                //temp vector to call search
//                std::unordered_set<int> temp_neighbors = gbraph[j];
                std::vector<int> temp_neighbors(graph[j].begin(), graph[j].end());
//                temp_neighbors.insert(i);
                temp_neighbors.push_back(i);
                if(temp_neighbors.size() > R)
                    robust_prune(j,temp_neighbors,a,R);
                else
                    graph[j].insert(i);
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
        // std::vector<int> vecL(L.begin(),L.end());
        // std::vector<int> vecV(V.begin(), V.end());
        // std::sort(vecL.begin(), vecL.end());
        // std::sort(vecV.begin(), vecV.end());
        // std::vector<int> difference;
        // std::set_difference(vecL.begin(), vecL.end(), vecV.begin(), vecV.end(), std::back_inserter(difference));
        
        //at first iteration only elements of L will be used, since V is empty, L-V = L
        std::vector<int> difference(L.begin(),L.end()); 

        while( !(difference.empty()) ){
            //p*
            int p_star_idx=-1;
            double min_dist=std::numeric_limits<double>::max();
            auto vec2 = query; //query point
            for(const auto& item:difference){//p in L-V
                auto vec1 = db->row(item);
                
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


        }
        //return k closest points from L
        keep_k_closest(L,k,query);

    }

    void filtered_robust_prune(const int& p, std::unordered_set<int>& V, const float& a, const size_t& R){
        //add neighbors of p but remove p
        V.insert(graph[p].begin(),graph[p].end());
        V.erase(p);
        //neighbors of p are deleted
        graph[p].clear();
        int p_star_idx;
        
        auto vec1 = db->row(p);

        while(V.size()>0){

            //min distance (p,p'), p' IN V
            double min_dist=std::numeric_limits<double>::max(); 
            for(const auto & item:V){
                //distance of p with each element of V (p')      
                
                auto vec2 = db->row(item); //p'
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

            //for each p' in V
            //vec1: p, vec2:p*, vec3: p' in V
            auto vec2 = db->row(p_star_idx);

            for (auto it=V.begin();it!=V.end();){
                int p_tonos=*it;

                //first if with filters, condition is Fp' == Fp AND Fp* != Fp'
                if( ((*db->vec_filter)[p_tonos] == (*db->vec_filter)[p]) && ((*db->vec_filter)[p_star_idx] != (*db->vec_filter)[p_tonos])){
                    ++it;  // Increment the iterator before skipping to the next iteration
                    continue;
                }
                    

                //second if, for the distances
                
                auto vec3 = db->row(p_tonos);
                //d(p*,p')
                auto d1 =Matrix<T>::sq_euclid(vec2,vec3, vec2.size());                
                                                                    
                vec1 = db->row(p);
                //d(p,p')
                auto d2 = Matrix<T>::sq_euclid(vec1,vec3,vec1.size());   
                if(a*d1<=d2)
                    it=V.erase(it);//erase and get new iterator position
                else
                    it++; //only increment if not erased
            }
        }
    }


    void filtered_vamana_indexing(std::unordered_map<T,int>& M, const float& a,const size_t& list_size, const size_t& R){
        std::random_device rd;
        std::mt19937 g(rd());
        
        //random permutation
        std::vector<int>sigma;
        for(size_t i=0;i<db->vecnum;i++) sigma.push_back(i);
        // std::cout<<"vecnum:"<<db->vecnum<<"\n";
        std::shuffle(sigma.begin(), sigma.end(), g);

        for(size_t i=0;i<sigma.size();i++){
            std::unordered_set<int> L,V;
            filtered_greedy_search(M,db->row(sigma[i]),0,list_size, (*db->vec_filter)[sigma[i]],L,V);    
            
            filtered_robust_prune(sigma[i],V,a,R); 
            
            for(const auto& j:graph[sigma[i]]){

                graph[j].insert(sigma[i]);

                if(graph[j].size() > R){
                    filtered_robust_prune(j,graph[j],a,R);
                }

            }
                    
        }
    
    }

    void stitched_vamana_indexing(float a, size_t R_small, size_t R_stitched, size_t L_small){
        std::unordered_map<T,std::unique_ptr<VamanaIndex>> Gf;
        putchar('\n');
        Gf.reserve(db->filters_set->size());
        for (auto f: *db->filters_set) {
            Gf[f] = std::make_unique<VamanaIndex>(db);
            Gf[f]->init_graph(R_small, Pf[f]);
            if (f==1.0f){
                // std::cout << "Graph for filter 1.0f\n";
                // Gf[f]->print_graph();   
            }
            int med = db->medoid_naive(Pf[f]);
            // std::cout << "Pf[" << f << "].size(): " << Pf[f].size() << std::endl;
            Gf[f]->vamana_indexing(med, a, L_small, R_small, Pf[f]);
            if (f==1.0f){
                // std::cout << "Graph for filter 1.0f\n";
                // Gf[f]->print_graph();   
            }
            for (auto& kv: Gf[f]->graph) { //pare kathe vertice to Gf, pou ousiastika einai to Pf, kai valta ston megalo grafo G (this->graph)
                graph[kv.first] = std::move(kv.second);
            }
        }
        // print_graph();
    }

};



#endif //PROJECTJJ_GRAPH_H
