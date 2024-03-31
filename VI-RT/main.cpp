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



int main(int argc, const char * argv[]) {
    Scene scene;
    Perspective *cam; // Camera
    ImagePPM *img;    // Image 
    Shader *shd;
    bool success;
    clock_t start, end;
    double cpu_time_used;

    //change for specific path in local machine
    std::string my_path = "C:/Users/marco/Desktop/Trabalho_Vi/Visualizacao-e-Iluminacao-master/VI-RT/Scene/tinyobjloader/models/cornell_box_VI.obj";

   

    //createAreaLights(scene);
    success = scene.Load(my_path); 

    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }        
    std::cout << "Scene Load Success!:-)\n";
    

    // add an ambient light to the scene
    AmbientLight ambient(RGB(0.9, 0.9, 0.9));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    scene.printSummary();
       

    // Image resolution
    const int W = 640;
    const int H = 480;

    img = new ImagePPM(W,H);

    // Camera parameters 
    const Point Eye = { 280,275,-490 }, At = { 280,265,0 };
    const Vector Up={0.f,1.f,0.f};
    const float fovW = 60.f;
    const float fovH = fovW * (float)H / (float)W;  // in degrees
    const float fovWrad = fovW * 3.14f / 180.f, fovHrad = fovH * 3.14f / 180.f;    // to radians
    cam = new Perspective(Eye, At, Up, W, H, fovW, fovH); 

    // create the shader
    RGB background(0.05f, 0.05f, 0.55f);
    shd = new AmbientShader(&scene, background);
    //shd = new DistributedShader(&scene, background);
    //shd = new WhittedShader(&scene, background);
    shd = new PathTracerShader(&scene, background);


    int spp = 1;
    StandardRenderer myRender (cam, &scene, img, shd, spp);
    

    
    start = clock();
    myRender.Render();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    // save the image
    std::string out_path = "/Users/marco/Desktop/Trabalho_Vi/Visualizacao-e-Iluminacao-master/VI-RT/ImageOut/";    

    img->Save(out_path+"MyImage.ppm");

    fprintf(stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);

    std::cout << "That's all, folks!" << std::endl;
    return 0;
}
