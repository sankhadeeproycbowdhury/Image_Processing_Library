# Image Processing Library

## Overview
The **Image Processing Library** is a C++-based project that provides functionalities to manipulate and process images. This library is designed for ease of use and includes several image operations such as brightness adjustment, contrast adjustment, Gaussian blur, Sobel edge detection, and color space conversion. It also integrates with OpenCV for image loading and saving.

## Features
- **Image Loading and Saving**: Load images from disk and save processed images back to disk.
- **Brightness Adjustment**: Increase or decrease the brightness of an image.
- **Contrast Adjustment**: Modify the contrast of an image.
- **Gaussian Blur**: Apply a Gaussian blur to smooth the image.
- **Sobel Edge Detection**: Perform edge detection using the Sobel operator.
- **RGB to Grayscale Conversion**: Convert color images to grayscale.

## Requirements
- **C++ Compiler**: Supports C++17 or higher.
- **Libraries**:
  - [OpenCV](https://opencv.org/) (for image I/O operations).
  - Standard C++ Libraries (`<vector>`, `<cmath>`, etc.).

## Installation
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/sankhadeeproycbowdhury/Image_Processing_Library.git
   cd Image_Processing_Library
   ```

2. **Install Dependencies**:
   Ensure OpenCV is installed on your system. For example:
   - **Ubuntu**:
     ```bash
     sudo apt update
     sudo apt install libopencv-dev
     ```
   - **macOS**:
     ```bash
     brew install opencv
     ```
   - **Windows**:
     Download and install OpenCV from [opencv.org](https://opencv.org/).

3. **Build the Project**:
   Use the provided Makefile to build the project.
   ```bash
   make
   ```

## Usage
1. **Run the Application**:
   After building, run the executable:
   ```bash
   ./program
   ```

2. **Example Input File**:
   Select an image file to upload via the HTML frontend or directly test endpoints.

3. **Endpoints**:
   - **Upload Image**:
     Endpoint: `/uploadImage`
     - Method: `POST`
     - Description: Upload an image to the server.
     - Example cURL Command:
       ```bash
       curl -X POST http://localhost:18080/uploadImage --data-binary @example.jpg
       ```
   - **Process Image**:
     Additional endpoints for operations like blur, brightness adjustment, etc., can be tested via REST clients or the frontend.

## API Endpoints
### 1. `/uploadImage`
- **Method**: `POST`
- **Description**: Upload an image to the server.
- **Request Body**: Binary image data.
- **Response**: Confirmation of successful upload.

### 2. `/getImage`
- **Method**: `GET`
- **Description**: Get the uploaded or Processed Image as per the situation.
- **Request Body**: None.
- **Response**: Successful Download.

### 2. `/processImage`
- **Method**: `POST`
- **Description**: Request an operation to the server.
- **Request Body**: None.
- **Response**: Confirmation of successful operation.

## Code Structure
- **Header Files**:
  - `src/image_processing.hpp`: Defines the `Image` class and its methods.
- **Implementation Files**:
  - `src/image_processing.cpp`: Implementation of image processing methods.
- **Main Application**:
  - `app.cpp`: Contains the Crow server and API endpoints.
- **Frontend**:
  - `index.html`: Web interface for uploading and processing images.

## Example Operations
### Brightness Adjustment
Increase brightness by 50:
```cpp
Image img = convertToImageClass("example.jpg");
img.brightnessAdjust(50);
```

### Gaussian Blur
Apply a 5x5 Gaussian blur:
```cpp
img.applyGaussianBlur(5);
```

### Sobel Edge Detection
Perform edge detection:
```cpp
img.sobelEdgeDetection();
```

### Convert to Grayscale
Convert RGB image to grayscale:
```cpp
img.rgbToGrayscale();
```

## Frontend
The project includes an `index.html` file to provide a user interface for uploading images and selecting operations.
- Open the `index.html` file in your browser.
- Choose an image file and select an operation from the dropdown menu.
- Click "Upload" to send the image to the server for processing.

## Future Enhancements
- Add support for more image filters and transformations.
- Implement advanced edge detection algorithms (e.g., Canny).
- Enable real-time processing through WebSocket integration.
- Add unit tests for better reliability.

## License
This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).

## Author
[Sankhadeep Roy Choudhury](https://github.com/sankhadeeproycbowdhury)

