//////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm> 
#include "StandardRenderer.hpp"
#include "../Camera/perspective.hpp"

struct Tile {
    int startX, endX;
    int startY, endY;
};

std::vector<Tile> createTiles(int width, int height, int tileSize) {
    std::vector<Tile> tiles;
    for (int y = 0; y < height; y += tileSize) {
        for (int x = 0; x < width; x += tileSize) {
            tiles.push_back(Tile{
                x,
                std::min(x + tileSize, width),
                y,
                std::min(y + tileSize, height)
                });
        }
    }
    return tiles;
}

void StandardRenderer::Render() {
    int W = 0, H = 0;
    cam->getResolution(&W, &H);
    int tileSize = 32; // Tamanho do tile
    auto tiles = createTiles(W, H, tileSize);
    std::queue<Tile> tileQueue;
    for (const auto& tile : tiles) {
        tileQueue.push(tile);
    }
    const bool jitter = true;

    std::atomic<int> completedPixels(0);
    int totalPixels = W * H;
    std::mutex queueMutex;

    auto renderTask = [&]() {
        std::mt19937 gen(std::random_device{}());  // Gerador de números aleatórios
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        while (true) {
            Tile tile;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (tileQueue.empty()) break;
                tile = tileQueue.front();
                tileQueue.pop();
            }
            for (int y = tile.startY; y < tile.endY; ++y) {
                for (int x = tile.startX; x < tile.endX; ++x) {
                    RGB color(0.f, 0.f, 0.f);
                    for (int ss = 0; ss < spp; ss++) {
                        RGB this_color;
                        Ray primary;
                        float jitterV[2] = { dist(gen), dist(gen) };
                        cam->GenerateRay(x, y, &primary, jitter ? jitterV : nullptr);

                        Intersection isect;
                        bool intersected = this->scene->trace(primary, &isect);
                        this_color = this->shd->shade(intersected, isect, 0);
                        color += this_color;
                    }
                    color = color / spp;
                    img->set(x, y, color);
                }
            }
            int pixelsThisTile = (tile.endX - tile.startX) * (tile.endY - tile.startY);
            int oldPixelCount = completedPixels.fetch_add(pixelsThisTile);
            int newPixelCount = oldPixelCount + pixelsThisTile;

            if (newPixelCount / (totalPixels / 100) > oldPixelCount / (totalPixels / 100)) {
                std::cout << "\rRendering progress: " << newPixelCount * 100 / totalPixels << "% complete." << std::flush;
            }
        }
        };

    std::vector<std::thread> threads;
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        threads.emplace_back(renderTask);
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // Ensure 100% progress is shown
    std::cout << "\rRendering progress: [##################################################] 100%" << std::endl;
}

































/*
void StandardRenderer::Render() {
    int W = 0, H = 0;
    this->cam->getResolution(&W, &H);
    int totalPixels = W * H;
    std::atomic<int> completedPixels(0);
    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::mutex coutMutex;
    const bool jitter = true;

    auto renderTask = [&](int startRow, int endRow) {
        std::mt19937 gen(std::random_device{}());  // Cria um gerador de números aleatórios
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);  // Distribuição uniforme entre 0 e 1

        for (int y = startRow; y < endRow; ++y) {
            for (int x = 0; x < W; ++x) {
                RGB color(0.f, 0.f, 0.f);
                for (int ss = 0; ss < spp; ss++) {
                    RGB this_color;
                    Ray primary;
                    float jitterV[2] = { dist(gen), dist(gen) };
                    cam->GenerateRay(x, y, &primary, jitter ? jitterV : nullptr);

                    Intersection isect;
                    bool intersected = this->scene->trace(primary, &isect);
                    this_color = this->shd->shade(intersected, isect, 0);
                    color += this_color;
                }
                color = color / spp;
                img->set(x, y, color);

                // Safe increment
                int oldCount = completedPixels.fetch_add(1);
                // Update progress bar under lock
                std::lock_guard<std::mutex> guard(coutMutex);
                if ((oldCount + 1) % (totalPixels / 100) == 0) {
                    int progress = (oldCount + 1) * 100 / totalPixels;
                    std::cout << "\rRendering progress: [" << std::string(progress / 2, '#') << std::string(50 - progress / 2, ' ') << "] " << progress << "%" << std::flush;
                }
            }
        }
        };

    // Divide work among threads
    int rowsPerThread = H / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? H : (i + 1) * rowsPerThread;
        threads.emplace_back(renderTask, startRow, endRow);
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Ensure 100% progress is shown
    std::cout << "\rRendering progress: [##################################################] 100%" << std::endl;
}
?

































//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
#include <random>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "StandardRenderer.hpp"
#include "../Camera/perspective.hpp"

void StandardRenderer::Render () {
    int W=0,H=0;  // resolution
    // resolution from the camera
    Perspective* perspCam = dynamic_cast<Perspective*>(cam);
    this->cam->getResolution(&W,&H);
    // number of samples per pixel

    int totalPixels = W * H; // Total number of pixels
    int completedPixels = 0; // Track completed pixels

    const bool jitter = true;
       

        for (int y = 0 ; y< H ; y++) {  // loop over rows
            for (int x=0 ; x< W ; x++) { // loop over columns
                
                completedPixels++;

                // Update progress bar (every 1% or adjust based on preference)
                if (completedPixels % (totalPixels / 100) == 0) {
                    int progress = completedPixels * 100 / totalPixels;
                    std::cout << "\rRendering progress: [" << std::string(progress / 2, '#') << std::string(50 - progress / 2, ' ') << "] " << progress << "%" << std::flush;
                }
                
                
                
                
                RGB color(0.f, 0.f,0.f);
               
                //iterate through the pixel samples
                for (int ss = 0; ss < spp; ss++){
                    RGB this_color;
                    Ray primary;

                    if (jitter) {
                        float jitterV[2];
                        jitterV[0] = ((float)rand()) / ((float)RAND_MAX);
                        jitterV[1] = ((float)rand()) / ((float)RAND_MAX);
                        cam->GenerateRay(x, y, &primary, jitterV);
                    }
                    else {
                        cam->GenerateRay(x, y, &primary);
                    }

                    // trace ray (scene), type of intersection
                    Intersection isect;
                    bool intersected = this->scene->trace(primary,&isect);

                    // shade this intersection (shader)
                    this_color = this->shd->shade(intersected,isect,0);
                    color += this_color;
                }
                color = color / spp;
                // write the result into the image frame buffer (image)
                img->set(x,y,color);
            }
        }
    };*/