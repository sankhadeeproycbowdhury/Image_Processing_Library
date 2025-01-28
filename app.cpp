#include "crow.h"
#include "src/image_processing.hpp"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <filesystem>
#include <string>


std::string uploadedImagePath = "uploaded_image.jpg"; // Path for the uploaded image
std::string processedImagePath = "processed_image.jpg"; // Path for the processed image


// Middleware for CORS
struct CORS {
    struct context {}; // Required context structure (can be empty if unused)

    // Before handling a request
    void before_handle(crow::request& req, crow::response& res, context& /*ctx*/) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }

    // After handling a request
    void after_handle(crow::request& /*req*/, crow::response& res, context& /*ctx*/) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};


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



void saveImage(Image& myImage, const std::string& outputPath) {
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
    cv::imwrite(outputPath, processedImg);
}



int main(){
    //define your crow application
    crow::App<CORS> app; 

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Image Processing API";
    });

    // Define POST endpoint for image upload
    CROW_ROUTE(app, "/uploadImage").methods(crow::HTTPMethod::Post)([] (const crow::request& req) {
        try {
            // Parse the request body
            auto body = req.body;

            // Write the image to the file
            std::ofstream out(uploadedImagePath, std::ios::binary);
            if (!out) {
                return crow::response(500, "Failed to save the image.");
            }
            out.write(body.data(), body.size());
            out.close();

            return crow::response(200, "Image uploaded successfully.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

     // Define GET endpoint for fetching the uploaded image
    CROW_ROUTE(app, "/getImage").methods(crow::HTTPMethod::Get)([]() {
        std::ifstream file(processedImagePath, std::ios::binary);

        if (!file) {
            file.open(uploadedImagePath, std::ios::binary);
            if (!file) {
                return crow::response(404, "Image not found.");
            }
        }

        // Read the image file into a string
        std::string image_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Set the response with the image data and proper content type
        crow::response res;
        res.set_header("Content-Type", "image/jpeg"); // Adjust content type as per your image format
        res.write(image_data);
        return res;
    });

    // Adjust brightness
    CROW_ROUTE(app, "/brightness/<int>").methods(crow::HTTPMethod::Post)([](int adjustment) {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.brightnessAdjust(adjustment);
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Brightness adjusted and image saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Adjust contrast
    CROW_ROUTE(app, "/contrast/<float>").methods(crow::HTTPMethod::Post)([](float factor) {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.contrastAdjust(factor);
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Contrast adjusted and image saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Adjust saturation
    CROW_ROUTE(app, "/saturation/<float>").methods(crow::HTTPMethod::Post)([](float factor) {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.adjustSaturation(factor);
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Saturation adjusted and image saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Invert
    CROW_ROUTE(app, "/invert").methods(crow::HTTPMethod::Post)([]() {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.invert();
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Image inverted and saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Apply GaussianBlur
    CROW_ROUTE(app, "/gaussianblur/<int>").methods(crow::HTTPMethod::Post)([](int kernelSize) {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.applyGaussianBlur(kernelSize);
            saveImage(myImage, processedImagePath);
            return crow::response(200, "GaussianBlur applied and image saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Apply VignetteEffect
    CROW_ROUTE(app, "/vignetteffect/<float>").methods(crow::HTTPMethod::Post)([](float strength) {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.addVignetteEffect(strength);
            saveImage(myImage, processedImagePath);
            return crow::response(200, "VignetteEffect applied and image saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Reflect Horizontally
    CROW_ROUTE(app, "/reflectHorizontally").methods(crow::HTTPMethod::Post)([]() {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.reflectHorizontally();
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Image Reflected Horizontally and saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Reflect Vertically
    CROW_ROUTE(app, "/reflectVertically").methods(crow::HTTPMethod::Post)([]() {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.reflectVertically();
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Image Reflected Vertically and saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Edge Detection
    CROW_ROUTE(app, "/detectEdge").methods(crow::HTTPMethod::Post)([]() {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.sobelEdgeDetection();
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Edge Detection Complete");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Convert to grayscale
    CROW_ROUTE(app, "/grayscale").methods(crow::HTTPMethod::Post)([]() {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.rgbToGrayscale();
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Image converted to grayscale and saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Convert to sepia
    CROW_ROUTE(app, "/sepia").methods(crow::HTTPMethod::Post)([]() {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.convertToSepia();
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Image converted to sepia and saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });

    // Image Compression
    CROW_ROUTE(app, "/compress/<float>").methods(crow::HTTPMethod::Post)([](float quality) {
        try {
            std::string pathToLoad = std::filesystem::exists(processedImagePath) ? processedImagePath : uploadedImagePath;
            Image myImage = convertToImageClass(pathToLoad);
            myImage.compressImage(quality);
            saveImage(myImage, processedImagePath);
            return crow::response(200, "Image compressed and saved.");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Error: ") + e.what());
        }
    });


    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();
    return 0;
}


