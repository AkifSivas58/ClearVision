#include "SecretImage.h"


// Constructor: split image into upper and lower triangular arrays
SecretImage::SecretImage(const GrayscaleImage& image) {
    width = image.get_width();
    height = image.get_height();

    // Alloc memory
    upper_triangular = new int[(width * (width+1))/2];
    lower_triangular = new int[(width * (width-1))/2];

    // Indexes to keep track of the each array
    int upper_index = 0;
    int lower_index = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // If j >= i fill the upper triangular, else lower triangular
            if (j >= i) {
                upper_triangular[upper_index] = image.get_pixel(i, j);
                upper_index++;
            } else {
                lower_triangular[lower_index] = image.get_pixel(i, j);
                lower_index++;
            }
        }
    }
}

// Constructor: instantiate based on data read from file
SecretImage::SecretImage(int w, int h, int* upper, int* lower) {
    width = w;
    height = h;
    upper_triangular = upper;
    lower_triangular = lower;
}


// Destructor: free the arrays
SecretImage::~SecretImage() {
    delete[] upper_triangular;
    delete[] lower_triangular;
}

// Reconstructs and returns the full image from upper and lower triangular matrices.
GrayscaleImage SecretImage::reconstruct() const {
    GrayscaleImage image(width, height);

    // Indexes to keep track of the each array
    int upper_index = 0;
    int lower_index = 0;

    for (int i = 0; i < image.get_height(); i++) {
        for (int j = 0; j < image.get_width(); j++) {
            // If j >= i set from the upper triangular, else from the lower triangular
            if (j >= i) {
                image.set_pixel(i, j, upper_triangular[upper_index]);
                upper_index++;
            } else {
                image.set_pixel(i, j, lower_triangular[lower_index]);
                lower_index++;
            }
        }
    }

    return image;
}

// Save the filtered image back to the triangular arrays
void SecretImage::save_back(const GrayscaleImage& image) {
    // Indexes to keep track of each array
    int upper_index = 0;
    int lower_index = 0;

    for (int i = 0; i < image.get_height(); i++){
        for (int j = 0; j < image.get_width(); j++){
            // Update upper if j >= i, else lower
            if (j >= i){
                upper_triangular[upper_index] = image.get_pixel(i, j);
                upper_index++;
            }
            else {
                lower_triangular[lower_index] = image.get_pixel(i, j);
                lower_index++;
            }
        }
    }

}

// Save the upper and lower triangular arrays to a file
void SecretImage::save_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening output file!" << std::endl;
        return;
    }
    
    // First clear the output file because why not
    file.clear();
    // Write width and height on the first line
    file << width << " " << height << "\n";
    // Write the data of upper triangular array in the second line
    for (int i = 0; i < (width * (width+1)) / 2; i++){
        file << upper_triangular[i];
        if (i < (width * (width+1)) / 2 - 1){
            file << " ";
        }
    }
    file << "\n";
    // Write the data of lower triangular array in the second line
    for (int i = 0; i < (width * (width-1)) / 2; i++){
        file << lower_triangular[i];
        if (i < (width * (width-1)) / 2 - 1){
            file << " ";
        }
    }
    file << "\n";

    file.close();
}

// Static function to load a SecretImage from a file
SecretImage SecretImage::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
    }

    // Extract the width and height
    int w, h;
    file >> w >> h;
    // Create the arrays
    int* upper = new int[(w * (w+1)) / 2];
    int* lower = new int[(w * (w-1)) / 2];

    // Extract the data for upper and lower arrays
    for (int i = 0; i < (w * (w+1)) / 2; i++){
        file >> upper[i];
    }

    for (int i = 0; i < (w * (w-1)) / 2; i++){
        file >> lower[i];
    }
    
    SecretImage secret_image(w, h, upper, lower);

    return secret_image;
}

// Returns a pointer to the upper triangular part of the secret image.
int * SecretImage::get_upper_triangular() const {
    return upper_triangular;
}

// Returns a pointer to the lower triangular part of the secret image.
int * SecretImage::get_lower_triangular() const {
    return lower_triangular;
}

// Returns the width of the secret image.
int SecretImage::get_width() const {
    return width;
}

// Returns the height of the secret image.
int SecretImage::get_height() const {
    return height;
}
