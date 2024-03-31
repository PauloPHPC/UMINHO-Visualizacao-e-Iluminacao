//
//  mesh.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include "geometry.hpp"
#include <math.h>


class Triangle: public Geometry{
public:
    Point v1, v2, v3;
    Vector normal;
    Vector edge1, edge2;
    BB bb;  // face bounding box
           // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
    bool intersect (Ray r, Intersection *isect);
    Triangle(Point _v1, Point _v2, Point _v3, Vector _normal): v1(_v1), v2(_v2), v3(_v3), normal(_normal) {
        edge1 = v1.point2vec(v2); 
        edge2 = v1.point2vec(v3);
        bb.min.set(v1.X, v1.Y, v1.Z); 
        bb.max.set(v1.X, v1.Y, v1.Z);
        bb.update(v2); 
        bb.update(v3);
    }

    // Heron's formula
    // https://www.mathopenref.com/heronsformula.html
    float area () {
        float a = (v2 - v1).norm();
        float b = (v3 - v2).norm();
        float c = (v1 - v3).norm();
        float s = (a+b+c) /2.0f;
        return sqrtf(s * (s - a) * (s - b) * (s - c));}
};
#endif /* Triangle_hpp */