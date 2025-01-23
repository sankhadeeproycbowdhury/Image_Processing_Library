// image_processing.hpp
#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>


class Image {
private:
    std::vector<std::vector<std::vector<uint8_t>>> data; // Height x Width x Channels
    int width;
    int height;
    int channels;


public:
    // Constructor
    Image(int width, int height, int channels = 3) 
        : width(width), height(height), channels(channels) {
        data.resize(height, std::vector<std::vector<uint8_t>>(
            width, std::vector<uint8_t>(channels, 0)));
    }


    // Basic getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }

    // Pixel access
    uint8_t& at(int y, int x, int channel) {
        if (x < 0 || x >= width || y < 0 || y >= height || channel < 0 || channel >= channels) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[y][x][channel];
    }


    // Basic Operations
    void brightnessAdjust(int delta) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < channels; c++) {
                    int newVal = std::clamp(static_cast<int>(data[y][x][c]) + delta, 0, 255);
                    data[y][x][c] = static_cast<uint8_t>(newVal);
                }
            }
        }
    }

    void contrastAdjust(float factor) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < channels; c++) {
                    float pixel = data[y][x][c];
                    float adjusted = 128 + (pixel - 128) * factor;
                    data[y][x][c] = static_cast<uint8_t>(std::clamp(adjusted, 0.0f, 255.0f));
                }
            }
        }
    }

    void invert() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < channels; c++) {
                    data[y][x][c] = 255 - data[y][x][c];
                }
            }
        }
    }

    void adjustSaturation(float factor) {
        if (channels < 3) return;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float r = data[y][x][0];
                float g = data[y][x][1];
                float b = data[y][x][2];

                float gray = 0.299f * r + 0.587f * g + 0.114f * b;
                
                r = gray + (r - gray) * factor;
                g = gray + (g - gray) * factor;
                b = gray + (b - gray) * factor;

                data[y][x][0] = std::clamp(static_cast<int>(r), 0, 255);
                data[y][x][1] = std::clamp(static_cast<int>(g), 0, 255);
                data[y][x][2] = std::clamp(static_cast<int>(b), 0, 255);
            }
        }
    }


    // Filters and Transformations
    void applyGaussianBlur(int kernelSize = 3) {
        std::vector<std::vector<std::vector<uint8_t>>> temp = data;
        std::vector<std::vector<float>> kernel = createGaussianKernel(kernelSize);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < channels; c++) {
                    float sum = 0.0f;
                    float weightSum = 0.0f;
                    
                    for (int ky = -kernelSize/2; ky <= kernelSize/2; ky++) {
                        for (int kx = -kernelSize/2; kx <= kernelSize/2; kx++) {
                            int ny = std::clamp(y + ky, 0, height - 1);
                            int nx = std::clamp(x + kx, 0, width - 1);
                            float weight = kernel[ky + kernelSize/2][kx + kernelSize/2];
                            
                            sum += data[ny][nx][c] * weight;
                            weightSum += weight;
                        }
                    }
                    temp[y][x][c] = static_cast<uint8_t>(sum / weightSum);
                }
            }
        }
        data = temp;
    }

    void addVignetteEffect(float strength = 0.5) {
        float centerX = width / 2.0f;
        float centerY = height / 2.0f;
        float maxDist = std::sqrt(centerX * centerX + centerY * centerY);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float distFromCenter = std::sqrt(
                    std::pow(x - centerX, 2) + 
                    std::pow(y - centerY, 2)
                );
                
                float vignetteMultiplier = 1.0f - (distFromCenter / maxDist) * strength;
                vignetteMultiplier = std::max(0.0f, vignetteMultiplier);

                for (int c = 0; c < channels; c++) {
                    data[y][x][c] = std::clamp(
                        static_cast<int>(data[y][x][c] * vignetteMultiplier), 
                        0, 255
                    );
                }
            }
        }
    }

    void reflectHorizontally() {
        std::vector<std::vector<std::vector<uint8_t>>> temp = data;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width / 2; x++) {
                for (int c = 0; c < channels; c++) {
                    std::swap(temp[y][x][c], temp[y][width - 1 - x][c]);
                }
            }
        }
        data = temp;
    }

    void reflectVertically() {
        std::vector<std::vector<std::vector<uint8_t>>> temp = data;
        for (int y = 0; y < height / 2; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < channels; c++) {
                    std::swap(temp[y][x][c], temp[height - 1 - y][x][c]);
                }
            }
        }
        data = temp;
    }

    // Edge Detection
    void sobelEdgeDetection() {
        std::vector<std::vector<std::vector<uint8_t>>> temp = data;
        
        // Sobel kernels
        const std::vector<std::vector<int>> kernelX = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };
        
        const std::vector<std::vector<int>> kernelY = {
            {-1, -2, -1},
            {0, 0, 0},
            {1, 2, 1}
        };

        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                for (int c = 0; c < channels; c++) {
                    float gx = 0.0f, gy = 0.0f;
                    
                    // Apply kernels
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            float pixel = data[y + i][x + j][c];
                            gx += pixel * kernelX[i + 1][j + 1];
                            gy += pixel * kernelY[i + 1][j + 1];
                        }
                    }
                    
                    // Calculate gradient magnitude
                    float magnitude = std::sqrt(gx * gx + gy * gy);
                    temp[y][x][c] = static_cast<uint8_t>(std::clamp(magnitude, 0.0f, 255.0f));
                }
            }
        }
        data = temp;
    }

    // Color Space Conversions
    void rgbToGrayscale() {
        if (channels < 3) return;
        
        std::vector<std::vector<std::vector<uint8_t>>> temp(
            height, std::vector<std::vector<uint8_t>>(
                width, std::vector<uint8_t>(1, 0)));

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Using luminosity method: 0.299R + 0.587G + 0.114B
                float gray = 0.299f * data[y][x][0] + 
                           0.587f * data[y][x][1] + 
                           0.114f * data[y][x][2];
                temp[y][x][0] = static_cast<uint8_t>(std::clamp(gray, 0.0f, 255.0f));
            }
        }
        
        data = temp;
        channels = 1;
    }

    void convertToSepia() {
        if (channels < 3) return;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float r = data[y][x][0];
                float g = data[y][x][1];
                float b = data[y][x][2];

                // Sepia tone calculation
                float newR = std::min(255.0f, r * 0.393f + g * 0.769f + b * 0.189f);
                float newG = std::min(255.0f, r * 0.349f + g * 0.686f + b * 0.168f);
                float newB = std::min(255.0f, r * 0.272f + g * 0.534f + b * 0.131f);

                data[y][x][0] = static_cast<uint8_t>(newR);
                data[y][x][1] = static_cast<uint8_t>(newG);
                data[y][x][2] = static_cast<uint8_t>(newB);
            }
        }
    }

    // Image Compression
    void compressImage(float quality = 0.5) {
        // Validate quality factor
        quality = std::max(0.0f, std::min(1.0f, quality));

        // Reduce color depth based on quality
        int colorReductionFactor = static_cast<int>(256 * (1 - quality));

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < channels; c++) {
                    // Quantize pixel values
                    data[y][x][c] = 
                        (data[y][x][c] / colorReductionFactor) * colorReductionFactor;
                }
            }
        }
    }



private:
    std::vector<std::vector<float>> createGaussianKernel(int size) {
        std::vector<std::vector<float>> kernel(
            size, std::vector<float>(size));
        float sigma = size / 6.0f;
        float sum = 0.0f;
        
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int nx = x - (size / 2);
                int ny = y - (size / 2);
                float value = std::exp(-(nx * nx + ny * ny) / (2 * sigma * sigma));
                kernel[y][x] = value;
                sum += value;
            }
        }
        
        // Normalize kernel
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                kernel[y][x] /= sum;
            }
        }
        
        return kernel;
    }
};

#endif // IMAGE_PROCESSING_H