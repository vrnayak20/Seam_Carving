#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using std::cout, std::endl, std::string;

void initializeImage(Pixel image[][MAX_HEIGHT]) {
  // iterate through columns
  for (unsigned int col = 0; col < MAX_WIDTH; col++) {
    // iterate through rows
    for (unsigned int row = 0; row < MAX_HEIGHT; row++) {
      // initialize pixel
      image[col][row] = {0, 0, 0};
    }
  }
}

void loadImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height) {
  std::ifstream inFS;
  inFS.open(filename);
  if (!inFS.is_open()) {
    throw std::runtime_error("Failed to open " + filename);
  }
  string fileNum;
  unsigned int count = 0;
  int end = -2;
  short red, green, blue;
  inFS >> fileNum; // preamble
  if (fileNum != "p3" && fileNum != "P3") {
    throw std::runtime_error("Invalid type " + fileNum);
  }
  
  inFS >> width;
  inFS >> height;
  if (width < 1 || width > MAX_WIDTH || height < 1 || height > MAX_HEIGHT)
    throw std::runtime_error("Invalid dimensions");
  inFS >> fileNum; // max color value always 255
  for (unsigned int i=0; i<height; i++) {
    for (unsigned int j=0; j<width; j++) {
      if (!inFS.fail()) {
        inFS >> red;
      }
      else {inFS >> end;}
      if (!inFS.fail()) {
        inFS >> green;
      }
      else {inFS >> end;}
      if (!inFS.fail()) {
        inFS >> blue;
      }
      else {inFS >> end;}
      
      if (inFS.fail() && (i+1)*(j+1) != count) {
        throw std::runtime_error("Too few values");
      }
      if (red < 0 || green < 0 || blue < 0 || red > 255 || green > 255 || blue > 255)
        throw std::runtime_error("Invalid color value");
      count++;
      image[j][i].r = red;
      image[j][i].g = green;
      image[j][i].b = blue;
    }
  }
  inFS >> end;
  if (end != -2) {
    throw std::runtime_error("Too many values");
  }

  // Done with file, so close it
  inFS.close();
}

void outputImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height) {
  std::ofstream outFS;
  outFS.open(filename);
  if (!outFS.is_open()) {
    throw std::runtime_error("Failed to open " + filename);
  }
  outFS << "P3" << endl;
  outFS << width << " " << height << endl;
  outFS << "255" << endl;
  for (unsigned int i=0; i<height; i++) {
    for (unsigned int j=0; j<width; j++) {
      outFS << image[j][i].r << " ";
      outFS << image[j][i].g << " ";
      outFS << image[j][i].b << " ";
    }
    outFS << endl;
  }

  outFS.close();
}

unsigned int energy(Pixel image[][MAX_HEIGHT], unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  int above=y-1, below=y+1, right=x+1, left=x-1;
  if (y == 0)
    above = height-1;
  if (x == 0)
    left = width-1;
  if (y == height-1)
    below = 0;
  if (x == width-1)
    right = 0;

  int energy = pow((image[x][above].r-image[x][below].r),2) + pow((image[x][above].g-image[x][below].g),2) + pow((image[x][above].b-image[x][below].b),2) + pow((image[right][y].r-image[left][y].r),2) + pow((image[right][y].g-image[left][y].g),2) + pow((image[right][y].b-image[left][y].b),2);
  return energy;
}

// uncomment functions as you implement them (part 2)

unsigned int loadVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int start_col, unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  seam[0] = start_col;
  unsigned int col = start_col;
  unsigned int total = energy(image, start_col, 0, width, height);
  unsigned int spot;
  for (unsigned int i=1; i < height; i++) {
    spot = energy(image, start_col, i, width, height);
    if (start_col < width-1 && energy(image, start_col+1, i, width, height) < spot) {
      spot = energy(image, start_col+1, i, width, height);
      col = start_col+1;
    }
    if (start_col > 0 && energy(image, start_col-1, i, width, height) < spot) {
      spot = energy(image, start_col-1, i, width, height);
      col = start_col-1;
    }
    total += spot;
    seam[i] = col;
    spot = 0;
    start_col = col;
  }
  return total;
}

unsigned int loadHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int start_row, unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  seam[0] = start_row;
  unsigned int col = start_row;
  unsigned int total = energy(image, start_row, 0, width, height);
  unsigned int spot;
  for (unsigned int i=1; i < height; i++) {
    spot = energy(image, start_row, i, width, height);
    if (start_row < width-1 && energy(image, start_row+1, i, width, height) < spot) {
      spot = energy(image, start_row+1, i, width, height);
      col = start_row+1;
    }
    if (start_row > 0 && energy(image, start_row-1, i, width, height) < spot) {
      spot = energy(image, start_row-1, i, width, height);
      col = start_row-1;
    }
    total += spot;
    seam[i] = col;
    spot = 0;
    start_row = col;
  }
  return total;
}

void findMinVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  unsigned int * min = new unsigned int[height];
  unsigned int * compare = new unsigned int[height];
  unsigned int minEnergy = loadVerticalSeam(image, width-1, width, height, min);
  for (int i=int(width)-2; i >= 0; i--) {
    if (loadVerticalSeam(image, i, width, height, compare) <= minEnergy) {
      minEnergy = loadVerticalSeam(image, i, width, height, min);
    }
  }
  for(unsigned int i=0; i < height; i++) seam[i] = min[i];
}

// void findMinHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, unsigned int seam[]) {
//   // TODO: implement (part 2)
// }

void removeVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int height, unsigned int verticalSeam[]) {
  // TODO: implement (part 2)
  for (unsigned int i=0; i < height; i++) {
    for (unsigned int j=verticalSeam[i]; j < width-1; j++) {
      image[j][i] = image[j+1][i];
    }
  }
  width--;
}

// void removeHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int& height, unsigned int horizontalSeam[]) {
//   // TODO: implement (part 2)
// }
