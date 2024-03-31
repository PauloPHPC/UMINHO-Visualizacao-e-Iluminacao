//
//  AmbientShader.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef PathTracerShaderShader_hpp
#define PathTracerShaderShader_hpp


#define _USE_MATH_DEFINES
#include "../shader.hpp"
#include "../../Primitive/BRDF/Phong.hpp"

class PathTracerShader: public Shader {
    RGB background;
    RGB directLighting (Intersection isect, Phong *f);
    RGB specularReflection (Intersection isect, Phong *f, int depth);
    RGB diffuseReflection(Intersection isect, Phong* f, int depth);
    float continue_p;
    int MAX_DEPTH;
public:
    PathTracerShader (Scene *scene, RGB bg): background(bg),Shader(scene) {continue_p = 0.5f; MAX_DEPTH=2;}
    RGB shade (bool intersected, Intersection isect, int depth);
};


#endif /* PathTracerShader_hpp */