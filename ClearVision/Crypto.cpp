#include "Crypto.h"
#include "GrayscaleImage.h"


// Extract the least significant bits (LSBs) from SecretImage, calculating x, y based on message length
std::vector<int> Crypto::extract_LSBits(SecretImage& secret_image, int message_length) {
    std::vector<int> LSB_array;
    // Reconstruct the image and get the size of it
    GrayscaleImage image = secret_image.reconstruct();
    int total_bits = message_length * 7;
    int size_of_image = image.get_height() * image.get_width();

    // Throw error if we dont have enough space
    if (total_bits > size_of_image){
        throw std::runtime_error("Image does not have enough pixels for the message!");
    }

    // Calculating starting point
    int start = size_of_image - total_bits;
    int x = start / image.get_width();
    int y = start % image.get_width();
    int ind = 0;

    for (int i = x; i < image.get_height(); i++){
        // If i = x, start from coordinate y, else start j from 0. 
        for (int j = (i == x) ? y : 0; j < image.get_width(); j++){
            int pixel = image.get_pixel(i, j);
            LSB_array.push_back(pixel & 1);
        }
    }
    

    return LSB_array;
}


// Decrypt message by converting LSB array into ASCII characters
std::string Crypto::decrypt_message(const std::vector<int>& LSB_array) {
    std::string message;
    if (LSB_array.size() % 7 != 0){
        throw std::runtime_error("Incorrect LSB array size!");
    }

    for (int i = 0; i < LSB_array.size(); i += 7){
        std::bitset<7> bits;
        for (int j = 0; j < 7; j++){
            // Assign the bits in reverse order, since bitset stores LSB at index 0. (Realising that took me hours :D)
            bits[6-j] = LSB_array[i+j];
        }
        message += static_cast<char>(bits.to_ulong());
    }

    return message;
}

// Encrypt message by converting ASCII characters into LSBs
std::vector<int> Crypto::encrypt_message(const std::string& message) {
    std::vector<int> LSB_array;
    for (char ch : message){
        std::bitset<7> bits(ch);
        // Reverse itarate because bitset stores the LSB at index 0.
        for (int i = 6; i >= 0; --i){
            LSB_array.push_back(bits[i]);
        }
    }

    return LSB_array;
}

// Embed LSB array into GrayscaleImage starting from the last bit of the image
SecretImage Crypto::embed_LSBits(GrayscaleImage& image, const std::vector<int>& LSB_array) {
    int size_of_image = image.get_height() * image.get_width();

    if (LSB_array.size() > size_of_image){
        throw std::runtime_error("Image does not have enough pixels.");
    }

    // Calculating starting point
    int start = size_of_image - LSB_array.size();
    int x = start / image.get_width();
    int y = start % image.get_width();
    int ind = 0;

    for (int i = x; i < image.get_height(); i++){
        // If i = x, start from coordinate y, else start j from 0.
        for (int j = (i == x) ? y : 0; j < image.get_width(); j++){
            int pixel = image.get_pixel(i, j);
            // Bitwise operation. Get the LSB and take the OR with bit from the array.
            pixel = (pixel & ~1) | LSB_array[ind];
            image.set_pixel(i, j, pixel);
            ind++;
        }
    }

    SecretImage secret_image(image);
    return secret_image;
}
