//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "mesh.hpp"
#include <iostream>
#include<limits>


const float MAX_FLOAT = std::numeric_limits<float>::max();


// see pbrt book (3rd ed.), sec 3.6.2, pag 157
//
// Suggestion: use:
// // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// Moller Trumbore intersection algorithm

bool Mesh::TriangleIntersect(Ray r, Face face, Intersection* isect) {
    const float EPSILON = 1e-6f;
    Point v0 = this->vertices.at(face.vert_ndx[0]);
    Point v1 = this->vertices.at(face.vert_ndx[1]);
    Point v2 = this->vertices.at(face.vert_ndx[2]);

    Vector edge1, edge2, h, s, q;
    float a, f, u, v;

    edge1 = Vector(v1.X-v0.X, v1.Y - v0.Y, v1.Z - v0.Z);// edge1 = v1 - v0;
    edge2 = Vector(v2.X - v0.X, v2.Y - v0.Y, v2.Z - v0.Z);// edge2 = v2 - v0;
    h = r.dir.cross(edge2);
    a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON) {
        return false;
    }
    f = 1.0f / a;
    s = Vector((r.o).X - v0.X, (r.o).Y - v0.Y, (r.o).Z - v0.Z); // s = r.o - v0;
    u = f * s.dot(h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = s.cross(edge1);
    v = f * r.dir.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }
    float t = f * edge2.dot(q);

    if (t > EPSILON) {
        Point inter = r.o + (r.dir * t);

        // Update the Intersection structure with the relevant information
        Vector normal = face.geoNormal;
        isect->wo = r.dir * static_cast<float>(-1);
        Vector wo = r.dir * static_cast<float>(-1);
        normal.Faceforward(wo);
        isect->depth = t;
        isect->p = inter;
        isect->gn = normal;
        isect->sn = normal;
        isect->wo.normalize();
        return true;
    }
    else {
        return false;
    }
   
}


bool Mesh::intersect (Ray r, Intersection *isect) {
    bool intersect=true;
    Intersection min_isect, curr_isect;
    float min_depth=  MAX_FLOAT;

    // intersect the ray with the mesh BB
    // if (!bb.intersect(r)) return false;

    // If it intersects then loop through the faces
    intersect = false;
    for (auto face_it=faces.begin() ; face_it != faces.end() ; face_it++) {
        
        if (!TriangleIntersect(r, *face_it, &curr_isect)) continue;

        intersect = true;
        if (curr_isect.depth < min_depth) {  // this is closer
            min_depth = curr_isect.depth;
            min_isect = curr_isect;
        }
    }
    *isect = min_isect;
    return intersect;
}


