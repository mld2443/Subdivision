//
//  manifold.h
//  Subdivision
//
//  Created by Matthew Dillard on 11/9/15.
//

#ifndef manifold_h
#define manifold_h

#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <map>

#include "halfedge.h"

class manifold {
private:
    std::list<vertex> vertices, vertices0;
    std::list<face> faces, faces0;
    std::list<edge> edges, edges0;
    std::list<halfedge> halfedges, halfedges0;
    
    std::map<std::pair<vertex*, vertex*>, edge*> edge_hash;
    
    edge* get_edge(vertex *v1, vertex* v2) {
        auto p = std::pair<vertex*, vertex*>((v1 > v2)? v1 : v2, (v1 > v2)? v2 : v1);
        auto ii = edge_hash.find(p);
        
        if (ii != edge_hash.end())
            return ii->second;
        
        edges0.push_back({v1, v2, nullptr});
        
        return edge_hash[p] = &edges0.back();
    }
    
public:
    manifold() {}
    
    vertex* add_vert(const double x, const double y, const double z) {
        vertices0.push_back({x,y,z});
        return &vertices0.back();
    }
    
    void add_face(const std::vector<vertex*>& ref, const std::vector<unsigned int> verts) {
        faces0.push_back({nullptr});
        
        halfedge *first = nullptr, *prev = nullptr;
        
        for (int i = 0; i < verts.size(); ++i) {
            edge *e = get_edge(ref[verts[i]], ref[verts[(i < verts.size() - 1)? i+1 : 0]]);
            
            halfedges0.push_back({nullptr,prev,e->he,&faces0.back(),ref[verts[i]],e});
            if (i > 0)
                prev->next = &halfedges0.back();
            
            if (e->he)
                e->he->flip = &halfedges0.back();
            
            e->he = prev = &halfedges0.back();
            if (i == 0)
                first = prev;
        }
        
        prev->next = first;
        first->prev = faces0.back().he = &halfedges0.back();
    }
    
    void cleanup() {
        edge_hash.clear();
        vertices = vertices0;
        faces = faces0;
        edges = edges0;
        halfedges = halfedges0;
    }
    
    void subdivide() {
        std::list<vertex> newV;
        std::list<face> newF;
        std::list<edge> newE;
        std::list<halfedge> newH;
        std::map<edge*,vertex*> hash;
        
        for (auto &v : vertices) {
            unsigned int valence;
            std::list<vertex> centP, edgeP;
            
            halfedge *trav = v.he;
            vertex Q = {0,0,0,nullptr}, R = {0,0,0,nullptr};
            do {
                ++valence;
                centP.push_back(trav->f->centroid());
                Q += centP.back();
                
                trav = trav->flip->next;
            } while(trav != v.he);
            Q /= valence;
            
            auto it2 = centP.begin(), it1 = it2++;
            do {
                edgeP.push_back((*it1 + ((it2 != centP.end())? *it2 : centP.front()) + *trav->o + *trav->flip->o)/4);
                
                ++it1;
                ++it2;
                trav = trav->flip->next;
            } while(trav != v.he);
            
            do {
                R += trav->e->midpointx2();
                
                trav = trav->flip->next;
            } while(trav != v.he);
            
            vertex newCenter = (Q + R + v * (valence - 3))/valence;
        }
    }
    
    void draw(const bool drawcontrol) {
        for (auto &v : vertices0)
            v.draw(0.0005);
        
        for (auto &f : faces0)
            f.draw();
    }
};

#endif /* manifold_h */
