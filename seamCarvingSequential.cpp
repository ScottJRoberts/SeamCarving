#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <iostream>
#include <vector>
#include <fstream>

#include <sstream>      /*string stream*/
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "CImg.h"

using namespace cimg_library;
using namespace std;

int printVector(vector<vector<int> > matrix){
  cout<<"PRINTING VECTOR";
    int size = static_cast<int>(matrix.size());
    for( int col = 0; col < size; col ++) {
          int innerSize = static_cast<int>(matrix[0].size());
            for( int row = 0; row < innerSize; row ++) {
                cout << matrix[col][row];
                cout << " ";
            } 
        cout << "\n";
   }
   return 0;

}

void write_pgm(vector<vector<int> > greyscale){ 
    int i, j, temp = 0; 
    int width = static_cast<int>(greyscale.size());
    int height = static_cast<int>(greyscale[0].size());

  
    FILE* pgmimg; 
    pgmimg = fopen("pgmimg.pgm", "wb"); 
  
    // Writing Magic Number to the File 
    fprintf(pgmimg, "P2\n");  
  
    // Writing Width and Height 
    fprintf(pgmimg, "%d %d \n", width, height);  
  
    // Writing the maximum gray value 
    fprintf(pgmimg, " 255\n");  
    int count = 0; 
    for (i = 0; i < width; i++) { 
        for (j = 0; j < height; j++) {
            temp = greyscale[j][i];
  
            // Writing the gray values in the 2D array to the file 
            fprintf(pgmimg, "%d\n", temp); 
        } 
    } 
    fclose(pgmimg); 
} 

int getLowestBelow(vector<int> below, int col){
  int left = col-1;
  int right = col+1;
  if (left>=0){
    if (right<static_cast<int>(below.size())){
      if (below[right]<below[left]){
        if (below[right]< below[col]){
          return below[right];
        } else {
          return below[col];
        }
      } else{
        if (below[left]< below[col]){
          return below[left];
        } else{
          return below[col];
        }
      }
    }
  }
  else if (right < static_cast<int>(below.size())){
    if (below[right] < below[col]){
      return below[right];
    } else {
      return below[col];
    }
  } else {
    return below[col];
  }

}

vector<vector<int> > accumulations_creator(vector<vector<int> > energies) {
  int width = static_cast<int>(energies.size());
  int height = static_cast<int>(energies[0].size());

  vector<vector<int> > results(width, vector<int>(height, 0));
  
  for (int row = (width -2); row> -1; row --){
    vector<int> prevRow = energies[row+1];
    for (int col= (height-1); col>-1; col--){
      results[row][col]= getLowestBelow(prevRow,col)+ energies[row][col];
    }
  }

  return results;

}

vector<vector<int> > energy_data(CImg<unsigned char> image){
  
  int row, col, diffx, diffy, diffxy, width, height;
  int tempPixel;
  width = static_cast<int>(image.width());
  height = static_cast<int>(image.height());

  vector<vector<int> > results(width, vector<int>(height));
  
  for(row = 0;row<width; row ++) {
    for(col =0; col<height; col++){
      //cout <<"pixel is " <<static_cast<int>(image.atXY(row,col))<<"\n";
      diffx = abs(image.atXY(col, row) - image.atXY(col, row+1));
      diffy = abs(image.atXY(col, row) - image.atXY(col+1, row));
      diffxy = abs(image.atXY(col, row) - image.atXY(col+1, row+1));
      tempPixel = diffx +diffy + diffxy;

      if (tempPixel>255)
        tempPixel =255;

      results[row][col] = tempPixel;
      //row_vec.push_back(holder[row][col]);
    }
    //results.push_back(row_vec);
  }

  return results;
}

vector<vector<int> > writeIntoVector(CImg<unsigned char> image){
  
  int width, height, row, col;
  int tempPixel;
  width = static_cast<int>(image.width());
  height = static_cast<int>(image.height());
  vector<vector<int> > results(width, vector<int>(height));
  for(row = 0;row<width; row ++) {
    for(col =0; col<height; col++){
      tempPixel = image.atXY(col,row);
      if (tempPixel>255)
        tempPixel =255;

      results[row][col] = tempPixel;
    }
  }

  return results;
}


int main() {
  CImg<unsigned char> bwImage("tester.pgm");
  // CImg<unsigned char> image("test.pgm");
  vector<vector<int> > test = energy_data(bwImage);
  printVector(test);
  vector<vector<int> > test2 = accumulations_creator(test);
  printVector(test2);
  // write_pgm(test);

  // CImg<unsigned char>output("pgmimg.pgm");

  // CImgDisplay main_disp(output,"energies"), other_dip(bwImage, "original");
  // while (!main_disp.is_closed()) {
  //   main_disp.wait();
  //   }
  return 0;
}