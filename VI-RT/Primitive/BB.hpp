//
//  AABB.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef BB_hpp
#define BB_hpp

#include "../utils/vector.hpp"
#include "../Rays/ray.hpp"
#include <algorithm>



typedef struct BB {
public:
    Point min, max;
    void update (Point p) {
        if (p.X < min.X) min.X = p.X;
        else if (p.X > max.X) max.X = p.X;
        if (p.Y < min.Y) min.Y = p.Y;
        else if (p.Y > max.Y) max.Y = p.Y;
        if (p.Z < min.Z) min.Z = p.Z;
        else if (p.Z > max.Z) max.Z = p.Z;
    }
    /*
    * I suggest you implement:
    *  bool intersect (Ray r) { }
    *
    * based on PBRT's 3rd ed. book , sec 3.1.2, pags 125..128 + 214,217,221
    *
    * or https://doi.org/10.1007/978-1-4842-7185-8_2
    *
    */
    bool intersect(Ray r){
        
            // Check for axis X
            float tmin = (min.X - r.o.X) / r.dir.X;
            float tmax = (max.X - r.o.X) / r.dir.X;

            // Ensure tmin is always the minimum
            if (tmin > tmax) std::swap(tmin, tmax);

            // Check against ray t range (0 to infinity)
            if (tmin < 0.0f || tmax > 1.0f) return false;

            // Check for axis Y
            tmin = (min.Y - r.o.Y) / r.dir.Y;
            tmax = (max.Y - r.o.Y) / r.dir.Y;

            // Ensure tmin is always the minimum
            if (tmin > tmax) std::swap(tmin, tmax);

            // Check against previous t range
            if (tmin < 0.0f || tmax < tmin || tmax > 1.0f) return false;

            // Check for axis Z
            tmin = (min.Z - r.o.Z) / r.dir.Z;
            tmax = (max.Z - r.o.Z) / r.dir.Z;

            // Ensure tmin is always the minimum
            if (tmin > tmax) std::swap(tmin, tmax);

            // Check against previous t range
            if (tmin < 0.0f || tmax < tmin || tmax > 1.0f) return false;

            return true;
        
    }

} BB;

#endif /* AABB_hpp */
