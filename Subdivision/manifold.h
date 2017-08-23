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
    std::list<vertex> vertices0;
    std::list<face> faces0;
    std::list<edge> edges0;
    std::list<halfedge> halfedges0;
    
    std::list<vertex> *vertices;
    std::list<face> *faces;
    std::list<edge> *edges;
    std::list<halfedge> *halfedges;
    
    std::map<std::pair<vertex*, vertex*>, edge*> edge_hash;
    std::map<edge*,vertex*> vert_hash;
    std::map<face*,vertex*> face_hash;
    
    edge* get_edge(std::list<edge>& edge_list, vertex *v1, vertex* v2) {
        auto key = std::pair<vertex*, vertex*>((v1 > v2)? v1 : v2, (v1 > v2)? v2 : v1);
        auto ii = edge_hash.find(key);
        
        if (ii != edge_hash.end())
            return ii->second;
        
        edge_list.push_back({v1, v2, nullptr});
        
        return edge_hash[key] = &edge_list.back();
    }
    
    vertex* get_edge_vert(std::list<vertex>& newV, const vertex& v, edge* key) {
        auto ii = vert_hash.find(key);
        
        if (ii != vert_hash.end())
            return ii->second;
        
        newV.push_back(v);
        
        return vert_hash[key] = &newV.back();
    }
    
    vertex* get_face_vert(std::list<vertex>& newV, face* key) {
        auto ii = face_hash.find(key);
        
        if (ii != face_hash.end())
            return ii->second;
        
        newV.push_back(key->centroid());
        
        return face_hash[key] = &newV.back();
    }
    
    std::vector<vertex*> flower(std::list<vertex>& newV, const vertex& v) {
        std::vector<vertex*> ref;
        
        unsigned int valence = 0;
        std::list<vertex*> facePointPointers;
        halfedge *trav = v.he;
        vertex Q = {0,0,0,nullptr}, R = {0,0,0,nullptr};
        
        // check for the centroids
        do {
            // check if v is an edge vertex
            if (!trav->flip)
                return ref;
            
            ++valence;
            facePointPointers.push_back(get_face_vert(newV, trav->f));
            Q += *facePointPointers.back();
            
            trav = trav->flip->next;
        } while(trav != v.he);
        Q /= facePointPointers.size();
        
        // get the edge points
        auto it2 = facePointPointers.begin(), it1 = it2++;
        do {
            // add reference to centroid
            ref.push_back(*it1);
            // commit and add ref to edge point
            vertex newEdgePoint = (**it1 + ((it2 != facePointPointers.end())? **it2 : *facePointPointers.front()) + *trav->o + *trav->flip->o)/4;
            ref.push_back(get_edge_vert(newV, newEdgePoint, trav->e));
            
            // update R
            R += trav->e->midpointx2();
            
            ++it1;
            ++it2;
            trav = trav->flip->next;
        } while(trav != v.he);
        R /= facePointPointers.size();
        
        // new point for the vertex
        newV.push_back((Q + R + (v * (facePointPointers.size() - 3)))/facePointPointers.size());
        ref.push_back(&newV.back());
        
        return ref;
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
            edge *e = get_edge(edges0, ref[verts[i]], ref[verts[(i < verts.size() - 1)? i+1 : 0]]);
            
            halfedges0.push_back({nullptr,prev,e->he,&faces0.back(),ref[verts[i]],e});
            if (i > 0)
                prev->next = &halfedges0.back();
            
            if (e->he)
                e->he->flip = &halfedges0.back();
            
            e->he = prev = &halfedges0.back();
            if (i == 0)
                first = prev;
            
            prev->o->he = prev;
        }
        
        prev->next = first;
        first->prev = faces0.back().he = &halfedges0.back();
    }
    
    void cleanup() {
        edge_hash.clear();
        vertices = &vertices0;
        faces = &faces0;
        edges = &edges0;
        halfedges = &halfedges0;
    }
    
    void subdiv_to(const unsigned int divisions) {
        if (vertices != &vertices0)
            vertices->~list();
        vertices = &vertices0;
        
        if (faces != &faces0)
            faces->~list();
        faces = &faces0;
        
        if (edges != &edges0)
            edges->~list();
        edges = &edges0;
        
        if (halfedges != &halfedges0)
            halfedges->~list();
        halfedges = &halfedges0;
        
        for (int i = 0; i < divisions; ++i)
            subdivide();
    }
    
    void subdivide() {
        auto newV = new std::list<vertex>();
        auto newF = new std::list<face>();
        auto newE = new std::list<edge>();
        auto newH = new std::list<halfedge>();
        
        for (auto &v : *vertices) {
            auto ref = flower(*newV, v);
            
            if (ref.size()) {
                vertex *centerPoint = ref.back(), *leftPoint, *edgePoint, *rightPoint;
                face *lastF = nullptr;
                halfedge *firstHE = nullptr, *lastHE = nullptr, *lastHEnxt = nullptr;
                for (int i = 0; i < ref.size() - 1; i+=2) {
                    leftPoint = ref[i];
                    edgePoint = ref[i+1];
                    rightPoint = ref[(i == ref.size() - 3)? 0 : i+2];
                    
                    newE->push_back({centerPoint, edgePoint, nullptr});
                    edge *spokeEdge = &newE->back();
                    edge *LeftEdge = get_edge(*newE, leftPoint, edgePoint);
                    edge *RightEdge = get_edge(*newE, rightPoint, edgePoint);
                    
                    newH->push_back({lastHE,nullptr,LeftEdge->he, lastF,leftPoint,LeftEdge});
                    if (LeftEdge->he) LeftEdge->he->flip = &newH->back();
                    LeftEdge->he = &newH->back();
                    leftPoint->he = &newH->back();
                    if (i == 0) firstHE = &newH->back();
                    else newH->back().next->prev = &newH->back();
                    
                    newH->push_back({&newH->back(),lastHEnxt,nullptr, lastF,edgePoint,spokeEdge});
                    newH->back().next->prev = &newH->back();
                    spokeEdge->he = &newH->back();
                    edgePoint->he = &newH->back();
                    if (i > 0) newH->back().prev->next = &newH->back();
                    
                    newF->push_back({nullptr});
                    lastF = &newF->back();
                    
                    newH->push_back({nullptr,nullptr,&newH->back(), lastF,centerPoint,spokeEdge});
                    newH->back().flip->flip = &newH->back();
                    centerPoint->he = &newH->back();
                    
                    newH->push_back({&newH->back(),nullptr,RightEdge->he, lastF,edgePoint,RightEdge});
                    if (RightEdge->he) RightEdge->he->flip = &newH->back();
                    newH->back().next->prev = &newH->back();
                    RightEdge->he = &newH->back();
                    
                    lastF->he = &newH->back();
                    lastHE = lastF->he;
                    lastHEnxt = lastHE->next;
                }
                
                firstHE->f = lastF;
                firstHE->prev->f = lastF;
                
                firstHE->next = lastHE;
                lastHE->prev = firstHE;
                firstHE->prev->prev = lastHEnxt;
                lastHEnxt->next = firstHE->prev;
            }
        }
        
        if (vertices != &vertices0)
            vertices->~list();
        vertices = newV;
        
        if (faces != &faces0)
            faces->~list();
        faces = newF;
        
        if (edges != &edges0)
            edges->~list();
        edges = newE;
        
        if (halfedges != &halfedges0)
            halfedges->~list();
        halfedges = newH;
        
        vert_hash.clear();
        face_hash.clear();
        edge_hash.clear();
    }
    
    void draw(const bool drawcontrol, const float pointsize) {
        if (drawcontrol) {
            for (auto &v : vertices0)
                v.draw(pointsize);
            
            if (edges->size() > edges0.size())
                for (auto &e : edges0)
                    e.draw();
        }
        for (auto &f : *faces)
            f.draw();
    }
};

#endif /* manifold_h */
