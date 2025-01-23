#include "src/image_processing.hpp"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>


Image convertToImageClass(const std::string& imagePath) {
    cv::Mat img = cv::imread(imagePath, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        throw std::runtime_error("Failed to load image: " + imagePath);
    }

    int height = img.rows;
    int width = img.cols;
    int channels = img.channels();

    Image image(width, height, channels);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                image.at(y, x, c) = img.at<cv::Vec3b>(y, x)[c];
            }
        }
    }

    return image;
}

int main() {
    try {
        std::string imagePath = "example.jpeg";
        
        // Load image using OpenCV
        cv::Mat img = cv::imread(imagePath);
        if (img.empty()) {
            throw std::runtime_error("Failed to load image: " + imagePath);
        }

        // Convert to custom Image class
        Image myImage = convertToImageClass(imagePath);

        // Perform operations
        // myImage.brightnessAdjust(20);
        // myImage.contrastAdjust(1.2f);
        // myImage.rgbToGrayscale();
        // myImage.applyGaussianBlur(25);
        // myImage.sobelEdgeDetection();
        // myImage.reflectHorizontally();
        // myImage.invert();
        myImage.compressImage(0.8);

        std::cout << "Image processed successfully!\n";

        cv::Mat processedImg;
        if (myImage.getChannels() == 1) {
            processedImg = cv::Mat(myImage.getHeight(), myImage.getWidth(), CV_8UC1);
            for (int y = 0; y < myImage.getHeight(); y++) {
                for (int x = 0; x < myImage.getWidth(); x++) {
                    processedImg.at<uint8_t>(y, x) = myImage.at(y, x, 0);
                }
            }
        } else {
            processedImg = cv::Mat(myImage.getHeight(), myImage.getWidth(), CV_8UC3);
            for (int y = 0; y < myImage.getHeight(); y++) {
                for (int x = 0; x < myImage.getWidth(); x++) {
                    for (int c = 0; c < myImage.getChannels(); c++) {
                        processedImg.at<cv::Vec3b>(y, x)[c] = myImage.at(y, x, c);
                    }
                }
            }
        }

        // Save processed image
        cv::imwrite("processed_image.jpeg", processedImg);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}





