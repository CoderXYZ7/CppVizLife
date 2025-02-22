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

// Function to update the array with smooth Conway's Game of Life rules
void updateArray(unsigned char** array, int width, int height) {
    // Configuration parameters
    static float birthLowerThreshold = 0.5f;    // Lower threshold for birth (percentage of max neighbors)
    static float birthUpperThreshold = 0.6f;    // Upper threshold for birth
    static float survivalLowerThreshold = 0.6f; // Lower threshold for survival
    static float survivalUpperThreshold = 0.8f; // Upper threshold for survival
    static int neighborhoodRadius = 20;         // Radius to check for neighbors
    static float randomizationThreshold = 0.1f; // Max average value below which we randomize
    static float transitionSpeed = 0.15f;       // How quickly cells transition between states
    
    // Create a temporary array for the next state
    unsigned char** nextState = new unsigned char*[height];
    for (int i = 0; i < height; i++) {
        nextState[i] = new unsigned char[width];
    }
    
    // Check if the grid is nearly empty
    float totalValue = 0.0f;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            totalValue += array[i][j];
        }
    }
    float averageValue = totalValue / (width * height * 255.0f);
    
    // Randomize if grid is nearly empty
    if (averageValue < randomizationThreshold) {
        static std::uniform_int_distribution<int> randDist(0, 255);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                array[i][j] = randDist(rng);
            }
        }
    }
    
    // Apply smooth Conway's Game of Life rules
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Calculate the "aliveness" of neighborhood
            float neighborSum = 0.0f;
            int neighborCount = 0;
            
            for (int ni = -neighborhoodRadius; ni <= neighborhoodRadius; ni++) {
                for (int nj = -neighborhoodRadius; nj <= neighborhoodRadius; nj++) {
                    // Skip the center cell
                    if (ni == 0 && nj == 0) continue;
                    
                    // Calculate wrapped coordinates for toroidal grid
                    int neighborI = (i + ni + height) % height;
                    int neighborJ = (j + nj + width) % width;
                    
                    neighborSum += array[neighborI][neighborJ] / 255.0f;
                    neighborCount++;
                }
            }
            
            float neighborRatio = neighborSum / neighborCount;
            float currentCellValue = array[i][j] / 255.0f;
            float targetValue = 0.0f;
            
            // Apply modified Conway rules with smooth transitions
            if (currentCellValue < 0.5f) {
                // Currently "dead" - check if it should be born
                if (neighborRatio >= birthLowerThreshold && neighborRatio <= birthUpperThreshold) {
                    targetValue = 1.0f;  // Should become alive
                }
            } else {
                // Currently "alive" - check if it should survive
                if (neighborRatio >= survivalLowerThreshold && neighborRatio <= survivalUpperThreshold) {
                    targetValue = 1.0f;  // Should stay alive
                }
            }
            
            // Apply smooth transition toward target state
            float newValue = currentCellValue + (targetValue - currentCellValue) * transitionSpeed;
            nextState[i][j] = (unsigned char)(newValue * 255.0f);
        }
    }
    
    // Copy the next state back to the original array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            array[i][j] = nextState[i][j];
        }
    }
    
    // Clean up temporary array
    for (int i = 0; i < height; i++) {
        delete[] nextState[i];
    }
    delete[] nextState;
}

#endif // ARRAY_UPDATER_H