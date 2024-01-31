#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void readPGM(const char *filename, int ***image, int *width, int *height) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }

    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }

    char magic[3];
    fscanf(file, "%2s", magic);
    if (magic[0] != 'P' || magic[1] != '2') {
        fprintf(stderr, "Invalid PGM format\n");
        exit(1);
    }

    fscanf(file, "%d %d", width, height);
    int maxVal;
    fscanf(file, "%d", &maxVal);

    *image = (int **)malloc(*height * sizeof(int *));
    for (int i = 0; i < *height; ++i) {
        (*image)[i] = (int *)malloc(*width * sizeof(int));
        for (int j = 0; j < *width; ++j) {
            fscanf(file, "%d", &(*image)[i][j]);
        }
    }

    fclose(file);
}

void rotateImage(int **original, int ***rotated, int originalWidth, int originalHeight, double angle) {
    // Convert angle to radians
    double radians = angle * -M_PI / 180.0;

    // Calculate rotated bounding box dimensions
    int rotatedWidth = (int)(fabs(originalWidth * cos(radians)) + fabs(originalHeight * sin(radians)));
    int rotatedHeight = (int)(fabs(originalHeight * cos(radians)) + fabs(originalWidth * sin(radians)));

    // Calculate centers for the original and rotated images
    int originalCenterX = originalWidth / 2;
    int originalCenterY = originalHeight / 2;
    int rotatedCenterX = rotatedWidth / 2;
    int rotatedCenterY = rotatedHeight / 2;

    // Allocate memory for the rotated image array
    *rotated = (int **)calloc(rotatedHeight, sizeof(int *));
    for (int i = 0; i < rotatedHeight; ++i) {
        (*rotated)[i] = (int *)calloc(rotatedWidth, sizeof(int));
        if ((*rotated)[i] == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }
    }


    // Initialize the rotated image with zeros
    for (int i = 0; i < rotatedHeight; ++i) {
        for (int j = 0; j < rotatedWidth; ++j) {
            (*rotated)[i][j] = 0;
        }
    }

    for (int i = 0; i < originalHeight; ++i) {
        for (int j = 0; j < originalWidth; ++j) {
            // Coordinates relative to the center of the original image
            int x = j - originalCenterX;
            int y = i - originalCenterY;

            // Apply rotation with floating-point precision
            double newXf = x * cos(radians) - y * sin(radians);
            double newYf = x * sin(radians) + y * cos(radians);

            // Round to the nearest integer and shift to the center of the rotated image
            int newX = (int)(round(newXf + rotatedCenterX));
            int newY = (int)(round(newYf + rotatedCenterY));

            // Check boundaries against rotated image dimensions
            if (newX >= 0 && newX < rotatedWidth && newY >= 0 && newY < rotatedHeight) {
                (*rotated)[newY][newX] = original[i][j];
            }
        }
    }
}

void freeImage(int **image, int height) {
    for (int i = 0; i < height; ++i) {
        free(image[i]);
    }
    free(image);

}

void printPGM(const char *filename, int **image, int width, int height) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }

    fprintf(file, "P2\n%d %d\n255\n", width, height);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(file, "%d ", image[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s {rotation_degree}\n", argv[0]);
        return 1;
    }

    // Get the time when the program starts execution
    clock_t start_time = clock();

    // Example image dimensions (replace with your actual dimensions)
    int width, height;

    // Example image data (replace with your actual image data)
    int **originalImage;

    // Read PGM file (assuming the PGM file is in the same directory as the executable)
    readPGM("im.pgm", &originalImage, &width, &height);

    // Pointer to store rotated image
    int **rotatedImage = NULL;

    // Convert the rotation degree argument to a double
    double rotationDegree = atof(argv[1]);

    // Perform image rotation with the specified angle
    rotateImage(originalImage, &rotatedImage, width, height, rotationDegree);

    // Print the dimensions of the rotated image
    printf("Original Image Dimensions: %d x %d\n", width, height);

    // Obtain the dimensions of the rotated image
    int rotatedWidth, rotatedHeight;
    rotatedWidth = (int)(fabs(width * cos(rotationDegree * M_PI / 180.0)) + fabs(height * sin(rotationDegree * M_PI / 180.0)));
    rotatedHeight = (int)(fabs(height * cos(rotationDegree * M_PI / 180.0)) + fabs(width * sin(rotationDegree * M_PI / 180.0)));

    printf("Rotated Image Dimensions: %d x %d\n", rotatedWidth, rotatedHeight);

    // Generate the output file name based on the rotation degree
    char outputFileName[256];
    sprintf(outputFileName, "rotate-image{%.0f}.pgm", rotationDegree);

    // Print the output filename
    printf("Output Filename: %s\n", outputFileName);

    // Save the rotated image to a new PGM file
    printPGM(outputFileName, rotatedImage, rotatedWidth, rotatedHeight);

    // Free allocated memory
    freeImage(originalImage, height);
    freeImage(rotatedImage, rotatedHeight);  // Free using rotatedHeight since the rotated image dimensions may be different

    // Get the time when the program ends execution
    clock_t end_time = clock();

    // Calculate and print the total execution time
    double total_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Total Execution Time: %f seconds\n", total_time);

    printf("Rotated image successfully generated and saved as: %s\n", outputFileName);

    return 0;
}
