//
//  halfedge.h
//  Subdivision
//
//  Created by Matthew Dillard on 11/9/15.
//

#ifndef halfedge_h
#define halfedge_h

#include <math.h>

struct vertex;
struct edge;
struct face;

struct halfedge {
    halfedge *next, *prev, *flip;
    face *f;
    vertex *o;
    edge *e;
};

struct vertex {
    double x,y,z;
    halfedge *he;
    
    vertex operator+(const vertex& v) const { return {x + v.x, y + v.y, z + v.z, nullptr}; }
    vertex operator-(const vertex& v) const { return {x - v.x, y - v.y, z - v.z, nullptr}; }
    vertex operator*(const double d) const { return {x * d, y * d, z * d, nullptr}; }
    vertex operator/(const double d) const { return {x / d, y / d, z / d, nullptr}; }
    vertex* operator+=(const vertex& v) { x += v.x; y += v.y; z += v.z; return this; }
    vertex* operator-=(const vertex& v) { x -= v.x; y -= v.y; z -= v.z; return this; }
    vertex* operator*=(const double d) { x *= d; y *= d; z *= d; return this; }
    vertex* operator/=(const double d) { x /= d; y /= d; z /= d; return this; }
    vertex* operator=(const vertex& v) { x = v.x; y = v.y; z = v.z; he = v.he; return this; }
    
    double abs() const { return sqrt(x*x + y*y + z*z); }
    double dot(const vertex& v) const { return x*v.x + y*v.y + z*v.z; }
    vertex cross(const vertex& v) const { return {y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x, nullptr}; }
    vertex normalize() const { double len = abs(); return {x/len, y/len, z/len, he}; }

    void draw(const float radius) {
        glPushMatrix(); {
            glTranslated(x, y, z);
            glutSolidSphere(radius, 10, 5);
        } glPopMatrix();
    }
};

struct edge {
    vertex *v1, *v2;
    halfedge *he;
    
    vertex midpointx2() { return *v1 + *v2; }
    
    void draw() {
        GLfloat white[] = {1.0,1.0,1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT, white);
        glBegin(GL_LINES); {
            glVertex3d(v1->x, v1->y, v1->z);
            glVertex3d(v2->x, v2->y, v2->z);
        } glEnd();
    }
};

struct face {
    halfedge *he;
    
    vertex normal() {
        return (*he->next->next->o - *he->o).cross(*he->next->next->next->o - *he->next->o).normalize();
    }
    
    vertex centroid() {
        unsigned int val = 0;
        vertex r = {0,0,0,nullptr};
        
        halfedge *trav = he;
        do {
            ++val;
            r += *trav->o;
            trav = trav->next;
        } while(trav != he);
        
        return r/val;
    }
    
    void draw() {
        glBegin(GL_POLYGON); {
            vertex n = normal();
            glNormal3d(n.x, n.y, n.z);
            
            halfedge *trav = he;
            do {
                glVertex3d(trav->o->x, trav->o->y, trav->o->z);
                trav = trav->next;
            } while(trav != he);
        } glEnd();
    }
};

#endif /* halfedge_h */
