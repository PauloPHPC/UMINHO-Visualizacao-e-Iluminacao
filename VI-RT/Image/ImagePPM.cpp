//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <exception>
#include <setjmp.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfRgba.h>
#include <turbojpeg.h>
 
void ImagePPM::ToneMap() {
    imageToSave = new PPM_pixel[W * H];
    // loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            imageToSave[j * W + i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].R) * 255);
            imageToSave[j * W + i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].G) * 255);
            imageToSave[j * W + i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].B) * 255);
        }
    }
}


bool ImagePPM::Save(std::string filename) {
    if (W == 0 || H == 0) {
        std::cerr << "Can't save an empty image" << std::endl;
        return false;
    }
    // convert from float to {0,1,..., 255}

     ToneMap();

    std::ofstream ofs;
    try {
        ofs.open(filename, std::ios::binary); //need to spec. binary mode for windows
        if (ofs.fail()) {
            throw std::runtime_error("Can't open output file");
        }
        ofs << "P6\n"
            << this->W << " " << this->H << "\n255\n";

        for (int j = 0; j < H; j++) {
            for (int i = 0; i < W; i++) {
                unsigned char r = imageToSave[j * W + i].val[0];
                unsigned char g = imageToSave[j * W + i].val[1];
                unsigned char b = imageToSave[j * W + i].val[2];
                ofs.write(reinterpret_cast<const char*>(&r), sizeof(r));
                ofs.write(reinterpret_cast<const char*>(&g), sizeof(g));
                ofs.write(reinterpret_cast<const char*>(&b), sizeof(b));
            }
        }

        ofs.close();
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        ofs.close();
        return false;
    }

    return true;
}

bool ImagePPM::SaveJPG(std::string filename, int quality) {
    tjhandle tjInstance = tjInitCompress();
    if (!tjInstance) {
        std::cerr << "Error initializing TurboJPEG compressor" << std::endl;
        return false;
    }

    unsigned char* jpegBuf = nullptr;
    unsigned long jpegSize = 0;
    int pixelFormat = TJPF_RGB;

    if (tjCompress2(tjInstance, (unsigned char*)imageToSave, W, 0, H, pixelFormat, &jpegBuf, &jpegSize, TJSAMP_444, quality, TJFLAG_FASTDCT) < 0) {
        std::cerr << "Error compressing JPEG image: " << tjGetErrorStr() << std::endl;
        tjDestroy(tjInstance);
        return false;
    }

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error opening output file: " << filename << std::endl;
        tjFree(jpegBuf);
        tjDestroy(tjInstance);
        return false;
    }

    ofs.write(reinterpret_cast<const char*>(jpegBuf), jpegSize);
    ofs.close();

    tjFree(jpegBuf);
    tjDestroy(tjInstance);
    return true;
}

bool ImagePPM::SavePFM(std::string filename) {
    std::ofstream ofs(filename, std::ios::binary);

    if (!ofs) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }

    ofs << "PF\n";
    ofs << this->W << " " << this->H << "\n";
    ofs << "-1.0\n"; // Negative value indicates Little Endian

    // Write the image data in binary format in reverse order (top-left to bottom-right)
    for (int j = this->H - 1; j >= 0; --j) {
        for (int i = 0; i < this->W; ++i) {
            const RGB& rgb = imagePlane[j * this->W + i];
            ofs.write(reinterpret_cast<const char*>(&rgb.R), sizeof(float));
            ofs.write(reinterpret_cast<const char*>(&rgb.G), sizeof(float));
            ofs.write(reinterpret_cast<const char*>(&rgb.B), sizeof(float));
        }
    }

    ofs.close();
    return true;
}

bool ImagePPM::SaveEXR(std::string filename) {
    Imf::Rgba* pixels = new Imf::Rgba[W * H];

    float exposure = 2.0f;  // Fator de exposição para simular HDR
    for (int j = 0; j < H; ++j) {
        for (int i = 0; i < W; ++i) {
            int index = j * W + i;
            pixels[index].r = imagePlane[index].R * exposure;  // Red channel
            pixels[index].g = imagePlane[index].G * exposure;  // Green channel
            pixels[index].b = imagePlane[index].B * exposure;  // Blue channel
            pixels[index].a = 1.0f;  // No alpha channel, set to 1.0
        }
    }

    try {
        Imf::RgbaOutputFile file(filename.c_str(), W, H, Imf::WRITE_RGBA);
        file.setFrameBuffer(pixels, 1, W);
        file.writePixels(H);
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing EXR file: " << e.what() << std::endl;
        delete[] pixels;
        return false;
    }

    delete[] pixels;
    return true;
}
