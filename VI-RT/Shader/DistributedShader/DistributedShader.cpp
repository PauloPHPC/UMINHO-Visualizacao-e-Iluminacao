#include "DistributedShader.hpp"
#include <stdlib.h>
#include <math.h>

RGB DistributedShader::specularReflection(Intersection isect, Phong* f, int depth) {

    // direction R = 2 (N.V) N - V
    float cos = isect.gn.dot(isect.wo);
    Vector Rdir = 2.f * cos * isect.gn - isect.wo;
    Ray specular(isect.p, Rdir);
    specular.adjustOrigin(isect.gn);
    Intersection s_isect;

    // trace ray
    bool intersected = scene->trace(specular, &s_isect);

    // shade this intersection
    RGB color = shade(intersected, s_isect, 0);

    return color;

}

RGB DistributedShader::directLighting(Intersection isect, Phong* f) {
    RGB color(0., 0., 0.);

    int light_index = rand() % (scene->numLights);


    Light *l = scene->lights[light_index];



        if ((*l).type == AMBIENT_LIGHT) {  // is it an ambient light ?
            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                color += Ka * (*l).L();
            }
            //continue;
        }
        if ((*l).type == POINT_LIGHT) {  // is it a point light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                Point lpoint;

                // get the position and radiance of the light source
                L = (*l).Sample_L(NULL, &lpoint);

                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.point2vec(lpoint);
                const float Ldistance = Ldir.norm();

                // now normalize Ldir
                Ldir.normalize();

                // compute the cosine between Ldir and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);

                if (cosL > 0.) { // the light is NOT behind the primitive
                    // generate the shadow ray
                    Ray shadow(isect.p, Ldir);

                    shadow.x = isect.pix_x;  // track pixel x-coordinate
                    shadow.y = isect.pix_y;  // track pixel y-coordinate

                    shadow.FaceID = isect.FaceID;  // track face ID for debugging or other purposes

                    // adjust origin by an EPSILON along the normal to avoid self-occlusion at the origin
                    shadow.adjustOrigin(isect.gn);

                    if (scene->visibility(shadow, Ldistance - EPSILON)) {  // if light source not occluded
                        RGB this_l_color = Kd * L * cosL;  // calculate light contribution
                        color += this_l_color;  // add contribution to color
                    }
                } // end cosL > 0.
            }
        }

        if ((*l).type == AREA_LIGHT) { // is it an area light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                Point lpoint;
                float l_pdf;
                // l é iterador
                AreaLight* al = (AreaLight*)(l);
                float rnd[2];
                rnd[0] = ((float)rand()) / ((float)RAND_MAX);
                rnd[1] = ((float)rand()) / ((float)RAND_MAX);
                L = al->Sample_L(rnd, &lpoint, l_pdf);
                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.point2vec(lpoint);
                const float Ldistance = Ldir.norm();
                // now normalize Ldir
                Ldir.normalize();

                // cosine between Ldir and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                // cosine between Ldir and the area light source normal
                float cosL_LA = Ldir.dot(al->gem->normal);
                //shade
                if (cosL > 0. && cosL_LA <= 0.) {
                    //generate ray
                    Ray shadow(isect.p, Ldir);
                    shadow.adjustOrigin(isect.gn);
                    if (scene->visibility(shadow, Ldistance - EPSILON)) {
                        color += (Kd * L * cosL) / l_pdf;
                    }
                }
            }
        }
    
    return color* scene->numLights;
}


RGB DistributedShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0., 0., 0.);
    if (!intersected)
        return (background);
    // intersection with a light source
    if (isect.isLight)
        return isect.Le;
    Phong* f = (Phong*)isect.f;
    // if there is a specular component sample it
    if (!f->Ks.isZero() && depth < 4)
        color += specularReflection(isect, f, depth + 1);
    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) color += directLighting(isect, f);
    return color;
}

/*
RGB DistributedShader:: specularReflection(Intersection isect, Phong *f, int depth){

    // direction R = 2 (N.V) N - V
    float cos = isect.gn.dot(isect.wo);
    Vector Rdir = 2.f * cos * isect.gn - isect.wo;
    Ray specular(isect.p, Rdir);
    specular.adjustOrigin(isect.gn);
    Intersection s_isect;

    // trace ray
    bool intersected = scene->trace(specular, &s_isect);

    // shade this intersection
    RGB color = shade(intersected, s_isect, 0);

    return color;

}

RGB DistributedShader::directLighting (Intersection isect, Phong *f) {
    RGB color(0.,0.,0.);

    for (auto l = scene->lights.begin() ; l != scene->lights.end() ; l++) {
        
        
        
        if ((*l)->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                color += Ka * (*l)->L();
            }
            //continue;
        }
        if ((*l)->type == POINT_LIGHT) {  // is it a point light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                Point lpoint;

                // get the position and radiance of the light source
                L = (*l)->Sample_L(NULL, &lpoint);

                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.point2vec(lpoint);
                const float Ldistance = Ldir.norm();

                // now normalize Ldir
                Ldir.normalize();

                // compute the cosine between Ldir and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);

                if (cosL > 0.) { // the light is NOT behind the primitive
                    // generate the shadow ray
                    Ray shadow(isect.p, Ldir);

                    shadow.x = isect.pix_x;  // track pixel x-coordinate
                    shadow.y = isect.pix_y;  // track pixel y-coordinate

                    shadow.FaceID = isect.FaceID;  // track face ID for debugging or other purposes

                    // adjust origin by an EPSILON along the normal to avoid self-occlusion at the origin
                    shadow.adjustOrigin(isect.gn);

                    if (scene->visibility(shadow, Ldistance - EPSILON)) {  // if light source not occluded
                        RGB this_l_color = Kd * L * cosL;  // calculate light contribution
                        color += this_l_color;  // add contribution to color
                    }
                } // end cosL > 0.
            }
        }

      if ((*l)->type == AREA_LIGHT) { // is it an area light ?
            if(!f -> Kd.isZero()){
                RGB L, Kd = f->Kd;
                Point lpoint;
                float l_pdf;
                // l é iterador
                AreaLight *al = (AreaLight *) (*l);
                float rnd[2];
                rnd[0] = ((float)rand()) / ((float)RAND_MAX);
                rnd[1] = ((float)rand()) / ((float)RAND_MAX);
                L = al->Sample_L(rnd, &lpoint, l_pdf);
                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.point2vec(lpoint);
                const float Ldistance = Ldir.norm();
                // now normalize Ldir
                Ldir.normalize();

                // cosine between Ldir and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                // cosine between Ldir and the area light source normal
                float cosL_LA = Ldir.dot(al->gem->normal);
                //shade
                if (cosL>0. && cosL_LA<=0.) {
                    //generate ray
                    Ray shadow(isect.p,Ldir);
                    shadow.adjustOrigin(isect.gn);
                    if(scene->visibility(shadow,Ldistance-EPSILON)){
                        color+= (Kd * L * cosL) / l_pdf;
                    }
                }
            }
        }
    }
    return color;
}


RGB DistributedShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0., 0., 0.);
    if (!intersected)
        return (background);
    // intersection with a light source
    if (isect.isLight)
        return isect.Le;
    Phong* f = (Phong*)isect.f;
    // if there is a specular component sample it
    if (!f->Ks.isZero() && depth < 4)
        color += specularReflection(isect, f, depth + 1);
    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) color += directLighting(isect, f);
    return color;
};*/