#define _USE_MATH_DEFINES
#include "Filter.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include <math.h>
#include <iostream>

// Mean Filter
void Filter::apply_mean_filter(GrayscaleImage& image, int kernelSize) {
    GrayscaleImage copy = image;

    int offset = kernelSize / 2;
    // Iterate over each pixel
    for (int i = 0; i < image.get_height(); i++){
        for (int j = 0; j < image.get_width(); j++){
            int sum = 0;
            // Calculate the sum of all the pixels in the kernel
            for (int a = i - offset; a <= i + offset; a++){
                for (int b = j - offset; b <= j + offset; b++){
                    // Add to sum if it is within the boundries
                    if (a >= 0 && a < image.get_height() && b >= 0 && b < image.get_width()){
                        sum += copy.get_pixel(a, b);
                    }
                }
            }
            // Set the pixel to the mean of the kernel
            int res = static_cast<int>(sum / (kernelSize * kernelSize));
            image.set_pixel(i, j, res);
        }
    }
}

// Gaussian Smoothing Filter
void Filter::apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize, double sigma) {
    // Creating the kernel
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));

    GrayscaleImage copy = image;
    int offset = kernelSize/2;
    double sum = 0.0;

    // Calculate via Gaussian function
    for (int i = 0; i < kernelSize; i++){
        for (int j = 0; j < kernelSize; j++){
            int x = j - offset;
            int y = i - offset;
            kernel[i][j] = (1.0/(2.0 * M_PI * sigma * sigma)) * std::exp(-(x*x + y*y)/(2*sigma*sigma));
            sum += kernel[i][j];
        }
    }

    // Normalizing the kernel
    for (int i = 0; i < kernelSize; i++){
        for (int j = 0; j < kernelSize; j++){
            kernel[i][j] /= sum;
        }
    }

    // Apply the filter on each pixel
    for (int i = 0; i < image.get_height(); i++){
        for (int j = 0; j < image.get_width(); j++){
            sum = 0.0;

            // Apply the kernel and sum them
            for (int a = 0; a < kernelSize; a++){
                for (int b = 0; b < kernelSize; b++){
                    int x = i + a - offset;
                    int y = j + b - offset;

                    // Check boundry
                    if (x >= 0 && x < image.get_height() && y >= 0 && y < image.get_width()){
                        sum += copy.get_pixel(x, y) * kernel[a][b];
                    }
                }
            }

            // Set new pixel val (casting and clamping)
            int new_val = static_cast<int>(sum);
            if (new_val < 0) new_val = 0;
            if (new_val > 255) new_val = 255;
            image.set_pixel(i, j, new_val);
        }
    }

}

// Unsharp Masking Filter
void Filter::apply_unsharp_mask(GrayscaleImage& image, int kernelSize, double amount) {
    // Get the gaussian smoothed image
    GrayscaleImage smoothed = image;
    apply_gaussian_smoothing(smoothed, kernelSize, 1.0);

    for (int i = 0; i < image.get_height(); i++){
        for (int j = 0; j < image.get_width(); j++){
            // Subtract the blurred/smoothed image from the original
            int res = image.get_pixel(i, j) + amount * (image.get_pixel(i, j) - smoothed.get_pixel(i, j));

            // Clamping
            if (res < 0) res = 0;
            if (res > 255) res = 255;
            image.set_pixel(i, j, res);
        }
    }
}
