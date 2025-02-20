#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include "array_updater.h"

// Global constants
const int w = 100;          // Width of array
const int h = 200;          // Height of array
const int s = 5;            // Scaling factor
const int FPS = 60;         // Frames per second

int main() {
    // Create the window
    sf::RenderWindow window(sf::VideoMode(w * s, h * s), "2D Array Visualizer");
    window.setFramerateLimit(FPS);
    
    // Initialize array
    unsigned char **array = new unsigned char*[h];
    for (int i = 0; i < h; i++) {
        array[i] = new unsigned char[w];
        for (int j = 0; j < w; j++) {
            array[i][j] = 0;
        }
    }
    
    // Create an SFML texture and sprite for display
    sf::Texture texture;
    if (!texture.create(w, h)) {
        std::cerr << "Failed to create texture!" << std::endl;
        return 1;
    }
    
    sf::Sprite sprite(texture);
    sprite.setScale(s, s);
    
    // Pixel array for updating the texture
    sf::Uint8* pixels = new sf::Uint8[w * h * 4]; // 4 components: R, G, B, A
    
    // Main loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        // Update array
        updateArray(array, w, h);
        
        // Convert array to pixel data
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                int pixelIndex = (i * w + j) * 4;
                unsigned char value = array[i][j];
                pixels[pixelIndex] = value;     // R
                pixels[pixelIndex + 1] = value; // G
                pixels[pixelIndex + 2] = value; // B
                pixels[pixelIndex + 3] = 255;   // A (fully opaque)
            }
        }
        
        // Update the texture
        texture.update(pixels);
        
        // Draw the sprite
        window.clear();
        window.draw(sprite);
        window.display();
    }
    
    // Clean up
    delete[] pixels;
    for (int i = 0; i < h; i++) {
        delete[] array[i];
    }
    delete[] array;
    
    return 0;
}