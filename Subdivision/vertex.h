//
//  vertex.h
//  Subdivision
//
//  Created by Matthew Dillard on 11/8/15.
//

#ifndef vertex_h
#define vertex_h

#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <math.h>

struct vertex {
    double x, y, z;
    
    vertex operator+(const vertex& v) const { return {x + v.x, y + v.y, z + v.z}; }
    vertex operator-(const vertex& v) const { return {x - v.x, y - v.y, z - v.z}; }
    vertex* operator+=(const vertex& v) { x += v.x; y += v.y; z += v.z; return this; }
    vertex* operator-=(const vertex& v) { x -= v.x; y -= v.y; z -= v.z; return this; }
    vertex* operator=(const vertex& v) { x = v.x; y = v.y; z = v.z; return this; }
    
    double abs() const { return sqrt(x*x + y*y + z*z); }
    double dot(const vertex& v) const { return x*v.x + y*v.y + z*v.z; }
    vertex cross(const vertex& v) const { return {y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x}; }
    vertex normalize() const { double len = abs(); return {x/len, y/len, z/len}; }
    
    void setcolor() { }
    void draw() {
        setcolor();
        glPushMatrix(); {
            glTranslated(x, y, z);
            glutSolidSphere(0.01, 10, 5);
        } glPopMatrix();
    }
};

#endif /* vertex_h */
