#ifndef ARRAY_UPDATER_H
#define ARRAY_UPDATER_H

#include <random>
#include <cmath>

// Global variables for smooth random generation
static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

// 2D noise field for smooth transitions
static float** noiseField = nullptr;
static float timeOffset = 0.0f;

// Initialize the noise field
void initializeNoiseField(int width, int height) {
    if (noiseField == nullptr) {
        noiseField = new float*[height];
        for (int i = 0; i < height; i++) {
            noiseField[i] = new float[width];
            for (int j = 0; j < width; j++) {
                noiseField[i][j] = dist(rng);
            }
        }
    }
}

// Function to update the array with smooth random values
void updateArray(unsigned char** array, int width, int height) {
    // Initialize noise field if not already done
    initializeNoiseField(width, height);
    
    // Update time offset for animation
    timeOffset += 0.03f;
    
    // Slowly evolve the noise field
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Perturb noise field slightly for organic movement
            noiseField[i][j] += dist(rng) * 0.05f;
            
            // Constrain values
            if (noiseField[i][j] > 1.0f) noiseField[i][j] = 1.0f;
            if (noiseField[i][j] < -1.0f) noiseField[i][j] = -1.0f;
            
            // Generate smooth value based on position and time
            float xCoord = (float)j / width;
            float yCoord = (float)i / height;
            
            // Create a wave pattern
            float wave = sin(xCoord * 10 + timeOffset) * cos(yCoord * 10 + timeOffset) * 0.5f;
            
            // Combine noise and wave
            float combined = (noiseField[i][j] * 0.3f + wave * 0.7f);
            
            // Map to 0-255 range
            array[i][j] = (unsigned char)(((combined + 1.0f) / 2.0f) * 255);
        }
    }
}

#endif // ARRAY_UPDATER_H