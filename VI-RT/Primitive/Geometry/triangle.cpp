//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "triangle.hpp"
#include <iostream>
#include<limits>


const float MAX_FLOAT = std::numeric_limits<float>::max();

bool Triangle::intersect (Ray r, Intersection *isect) {
    const float EPSILON = 1e-6f;
    Vector h, s, q;
    float a, ff, u, v;
    h = r.dir.cross(edge2);
    
    const float par = edge1.dot(h);
    if (par > -EPSILON && par < EPSILON) {
        return false;
    }   // This ray is parallel to this triangle.

    a = edge1.dot(h);
    ff = 1.0f / a;
    s = v1.point2vec(r.o);
    u = ff * s.dot(h);

    if (u < 0.0 || u > 1.0) {
        return false;
    }


    q = s.cross(edge1);
    v = ff * r.dir.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }



    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = ff * edge2.dot(q);
    if (t > EPSILON) {
        // ray intersection
        Point pHit = r.o + (r.dir * t);        
        Vector wo = r.dir * static_cast<float>(-1);
        Vector for_normal = normal;
        for_normal.Faceforward(wo);        
        isect->p = pHit;
        isect->gn = for_normal;
        isect->sn = for_normal;
        isect->wo = r.dir * static_cast<float>(-1);
        isect->depth = t;
        isect->wo.normalize();
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}
