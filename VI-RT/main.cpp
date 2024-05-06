#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include "Scene/scene.hpp"
#include "Camera/perspective.hpp"
#include "Renderer/renderer.hpp"
#include "Renderer/StandardRenderer.hpp"
#include "Image/ImagePPM.hpp"
#include "Shader/AmbientShader/AmbientShader.hpp"
#include "Shader/WhittedShader/WhittedShader.hpp"
#include "Shader/DistributedShader/DistributedShader.hpp"
#include "Shader/PathTracerShader/PathTracerShader.hpp"
#include "Light/AmbientLight.hpp"
#include "Light/PointLight.hpp"
#include "Light/AreaLight.hpp"
#include <chrono>
#include <stdlib.h>
#include <math.h>


int main(int argc, const char * argv[]) {
    Scene scene;
    Perspective *cam; // Camera
    ImagePPM *img;    // Image 
    Shader *shd;
    bool success;
    clock_t start, end;
    double cpu_time_used;

    //change for specific path in local machine
    std::string my_path = "C:/Users/marco/Desktop/RepoGit/VI-RT/Scene/tinyobjloader/models/cornell_box_VI.obj";   
    //std::string my_path = "C:/Users/paulo/Desktop/Visualizacao e Iluminacao/VI-RT/Scene/tinyobjloader/models/cornell_box_VI.obj";

    //createAreaLights(scene);
    success = scene.Load(my_path); 

    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }        
    std::cout << "Scene Load Success!:-)\n";
    

    // add an ambient light to the scene
   /* AmbientLight ambient(RGB(0.1, 0.1, 0.1));
    scene.lights.push_back(&ambient);
    scene.numLights++;*/
    
    
   /* PointLight* pl1 = new PointLight(RGB(0.65, 0.65, 0.65),
        Point(288, 508, 282));
    scene.lights.push_back(pl1);
    scene.numLights++; */
    
   // Luminária 1 - Frontal Esquerda
    Point v1 = { 178, 508, 93 };
    Point v2 = { 228, 508, 93};
    Point v3 = { 228, 508, 143 };
    Vector n = { 0, -1, 0 };
    RGB power = { 0.5, 0.5, 0.5 };
    auto* al1 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;
    
    v1 = { 178, 508, 93 };
    v2 = { 228, 508, 143 };
    v3 = { 178, 508, 143 };
    auto* al2 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al2);
    scene.numLights++;

    // Luminária 2 - Frontal Direita
    v1 = { 327, 508, 93 };
    v2 = { 377, 508, 93 };
    v3 = { 377, 508, 143 };
    al1 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;

    v1 = { 327, 508, 93 };
    v2 = { 377, 508, 143 };
    v3 = { 327, 508, 143 };
    al2 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al2);
    scene.numLights++;

    // Luminária 3 - Centro
    v1 = { 253, 508, 279 };
    v2 = { 303, 508, 279 };
    v3 = { 303, 508, 329 };
    al1 = new AreaLight(power*2, v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;

    v1 = { 253, 508, 279 };
    v2 = { 303, 508, 329 };
    v3 = { 253, 508, 329 };
    al2 = new AreaLight(power*2, v1, v2, v3, n);
    scene.lights.push_back(al2);
    scene.numLights++;
    
    // Luminária 4 - Traseira Esquerda
    v1 = { 178, 508, 466 };
    v2 = { 228, 508, 466 };
    v3 = { 228, 508, 516 };
    al1 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;

    v1 = { 178, 508, 466 };
    v2 = { 228, 508, 516 };
    v3 = { 178, 508, 516 };
    al2 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al2);
    scene.numLights++;

    // Luminária 5 - Traseira Direita
    v1 = { 327, 508, 466 };
    v2 = { 377, 508, 466 };
    v3 = { 377, 508, 516 };
    al1 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;

    v1 = { 327, 508, 466 };
    v2 = { 377, 508, 516 };
    v3 = { 327, 508, 516 };
    al2 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al2);
    scene.numLights++;

    /*
    int n_lights = 360;
    int side_length = sqrt(n_lights); // Assumindo uma distribuição quadrada de luzes
    float distance = 20.0; // Distância entre as luzes
    float triangle_size = 10.0; // Tamanho dos triângulos em cada luz quadrada
    Vector n = { 0, -1, 0 }; // Direção normal das luzes
    RGB power = { 0.5, 0.5, 0.5 }; // Intensidade das luzes

    for (int i = 0; i < side_length; i++) {
        for (int j = 0; j < side_length; j++) {
            Point v1 = { distance * i, 508, distance * j };
            Point v2 = { distance * i + triangle_size, 508, distance * j };
            Point v3 = { distance * i + triangle_size, 508, distance * j + triangle_size };

            // Primeira luz triangular
            auto* al1 = new AreaLight(power, v1, v2, v3, n);
            scene.lights.push_back(al1);

            // Segunda luz triangular (compartilha um vértice com a primeira)
            v1 = { distance * i, 508, distance * j };
            v2 = { distance * i + triangle_size, 508, distance * j + triangle_size };
            v3 = { distance * i, 508, distance * j + triangle_size };

            auto* al2 = new AreaLight(power, v1, v2, v3, n);
            scene.lights.push_back(al2);
        }
    }
    scene.numLights = n_lights;*/
    scene.printSummary();
       

    // Image resolution
    const int W = 1024;
    const int H = 1024;

    img = new ImagePPM(W,H);

    // Camera parameters 
    const Point Eye = { 280,265,-300 }, At = { 280,265,0 };
    const Vector Up={0.f,1.f,0.f};
    const float fovW = 60.f;
    const float fovH = fovW * (float)H / (float)W;  // in degrees
    const float fovWrad = fovW * 3.141592f / 180.f, fovHrad = fovH * 3.141592f / 180.f;    // to radians
    cam = new Perspective(Eye, At, Up, W, H, fovWrad, fovHrad); 

    // create the shader
    RGB background(0.05f, 0.05f, 0.55f);
    //shd = new AmbientShader(&scene, background);
    //shd = new WhittedShader(&scene, background);
    shd = new DistributedShader(&scene, background);    
    //shd = new PathTracerShader(&scene, background);


    int spp = 16;
    StandardRenderer myRender (cam, &scene, img, shd, spp);
    

    
    start = clock();
    myRender.Render();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    // save the image
    std::string out_path = "/Users/marco/Desktop/RepoGit/VI-RT/ImageOut/";    
    //std::string out_path = "C:/Users/paulo/Desktop/Visualizacao e Iluminacao/VI-RT/ImageOut/";

    img->Save(out_path+"MyImage.ppm");

    fprintf(stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);

    std::cout << "That's all, folks!" << std::endl;
    return 0;
}
