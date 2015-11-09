//
//  fileio.h
//  Subdivision
//
//  Created by Matthew Dillard on 11/8/15.
//

#ifndef fileio_h
#define fileio_h

#include <iostream>
#include <fstream>
#include <utility>
#include <float.h>
#include <cctype>
#include <vector>
#include <map>

float xlow, ylow, zlow, xhigh, yhigh, zhigh;

manifold m;

void load(const std::string filename) {
    std::vector<vertex*> v_pointers;
    xlow = ylow = zlow = FLT_MAX;
    xhigh = yhigh = zhigh = -FLT_MAX;
    
    std::ifstream file(filename);
    char token;
    
    while (!file.eof()) {
        file >> token;
        if(token == '#'){
            std::string dummy;
            getline(file, dummy);
        }
        else if (token == 'v') {
            double x, y, z;
            file >> x >> y >> z;
            
            {
                if (x < xlow)
                    xlow = x;
                else if (x > xhigh)
                    xhigh = x;
                if (y < ylow)
                    ylow = y;
                else if (y > yhigh)
                    yhigh = y;
                if (z < zlow)
                    zlow = z;
                else if (z > zhigh)
                    zhigh = z;
            }
            
            //vertices.push_back({x,y,z,nullptr});
            v_pointers.push_back(m.add_vert(x, y, z));
        }
        else if (token == 'f') {
            std::vector<unsigned int> face_verts;
            file >> std::ws;
            while (std::isdigit(file.peek())) {
                unsigned int v;
                file >> v >> std::ws;
                face_verts.push_back(v-1);
            }
            
            m.add_face(v_pointers, face_verts);
        }
    }
    
    m.cleanup();
}


#endif /* fileio_h */
