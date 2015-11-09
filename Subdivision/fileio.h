//
//  fileio.h
//  Subdivision
//
//  Created by Matthew Dillard on 11/8/15.
//

#ifndef fileio_h
#define fileio_h

#include <vector>
#include <iostream>
#include <fstream>
#include <float.h>

std::vector<vertex> vertices;
std::vector<face> faces;

float xlow, ylow, zlow, xhigh, yhigh, zhigh;

void center() {
    vertex center = {(xlow + xhigh) / 2, (ylow + yhigh) / 2, (zlow + zhigh) / 2};
    for (auto &v : vertices)
        v -= center;
}

void load(const std::string filename) {
    xlow = ylow = zlow = FLT_MAX;
    xhigh = yhigh = zhigh = -FLT_MAX;
    vertices.clear();
    faces.clear();
    
    std::ifstream file(filename);
    char token;
    
    while (!file.eof()) {
        file >> token;
        if (token == 'v') {
            double x, y, z;
            file >> x >> y >> z;
            
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
            
            vertices.push_back({x,y,z});
        }
        else if (token == 'f') {
            unsigned int v1, v2, v3, v4;
            file >> v1 >> v2 >> v3 >> v4;
            faces.push_back({&vertices[v1-1], &vertices[v2-1], &vertices[v3-1], &vertices[v4-1]});
        }
    }
    
    center();
}


#endif /* fileio_h */
