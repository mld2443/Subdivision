//
//  face.h
//  Subdivision
//
//  Created by Matthew Dillard on 11/8/15.
//

#ifndef face_h
#define face_h

#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>

#include "vertex.h"

struct face {
    vertex *v1, *v2, *v3, *v4;
    
    vertex normal() { return (*v3 - *v1).cross(*v4 - *v2).normalize(); }
    
    void draw() {
        glBegin(GL_QUADS); {
            vertex n = normal();
            glNormal3d(n.x, n.y, n.z);
            v1->setcolor();
            glVertex3d(v1->x, v1->y, v1->z);
            v2->setcolor();
            glVertex3d(v2->x, v2->y, v2->z);
            v3->setcolor();
            glVertex3d(v3->x, v3->y, v3->z);
            v4->setcolor();
            glVertex3d(v4->x, v4->y, v4->z);
        } glEnd();
    }
};

#endif /* face_h */
