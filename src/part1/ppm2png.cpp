#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

// Function to read a PPM file
std::vector<unsigned char> readPPM(const std::string &filename, int &width, int &height) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Could not open PPM file: " + filename);
    }

    // Read PPM header
    std::string magicNumber;
    file >> magicNumber;
    if (magicNumber != "P3") {
        throw std::runtime_error("Unsupported PPM format: " + magicNumber);
    }

    file >> width >> height;
    int maxVal;
    file >> maxVal;
    file.ignore(1); // Skip the single whitespace after the maxVal

    if (maxVal != 255) {
        throw std::runtime_error("Only 8-bit PPM files are supported.");
    }

    // Read pixel data
    std::vector<unsigned char> imageData(width * height * 3); // RGB
    int r, g, b;
    for (int i = 0; i < width * height; ++i) {
        file >> r >> g >> b;
        imageData[i * 3] = static_cast<unsigned char>(r);
        imageData[i * 3 + 1] = static_cast<unsigned char>(g);
        imageData[i * 3 + 2] = static_cast<unsigned char>(b);
    }

    if (!file) {
        throw std::runtime_error("Error reading pixel data.");
    }

    return imageData;
}

// Function to convert PPM to PNG
void convertPPMtoPNG(const std::string &ppmFile, const std::string &pngFile) {
    int width, height;
    auto imageData = readPPM(ppmFile, width, height);

    // Write the PNG file using stb_image_write
    if (!stbi_write_png(pngFile.c_str(), width, height, 3, imageData.data(), width * 3)) {
        throw std::runtime_error("Failed to write PNG file " + pngFile);
    }

    std::cout << "Successfully converted " << ppmFile << " to " << pngFile << std::endl;
}

// int main() {
//     try {
//         convertPPMtoPNG("input.ppm", "output.png");
//     } catch (const std::exception &e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }
