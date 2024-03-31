//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include <random>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "StandardRenderer.hpp"

void StandardRenderer::Render () {
    int W=0,H=0;  // resolution
    // resolution from the camera
    this->cam->getResolution(&W,&H);
    // number of samples per pixel

    int totalPixels = W * H; // Total number of pixels
    int completedPixels = 0; // Track completed pixels
       

        for (int y = 0 ; y< H ; y++) {  // loop over rows
            for (int x=0 ; x< W ; x++) { // loop over columns
                
                completedPixels++;

                // Update progress bar (every 1% or adjust based on preference)
                if (completedPixels % (totalPixels / 100) == 0) {
                    int progress = completedPixels * 100 / totalPixels;
                    std::cout << "\rRendering progress: [" << std::string(progress / 2, '#') << std::string(50 - progress / 2, ' ') << "] " << progress << "%" << std::flush;
                }
                
                Ray primary;
                Intersection isect;
                bool intersected = false;
                RGB color(0.f, 0.f,0.f);
                //iterate through the pixel samples
                for (int ss = 0; ss < spp; ss++){
                    RGB this_color;
                    // taking multiple random samples within each pixel and averaging their color contributions
                    float jitterV[2];

                    // Generate Ray (camera)
                    this->cam->GenerateRay(x,y,&primary,jitterV);

                    // trace ray (scene), type of intersection
                    intersected = this->scene->trace(primary,&isect);

                    // shade this intersection (shader)
                    this_color = this->shd->shade(intersected,isect,0);
                    color += this_color;
                }
                color = color / spp;
                // write the result into the image frame buffer (image)
                img->set(x,y,color);
            }
        }
    };

