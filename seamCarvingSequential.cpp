#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <iostream>
#include <vector>
#include <fstream>
#include<tuple>
#include <algorithm> 
#include <sstream>      /*string stream*/
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "CImg.h"
#include "omp.h"

using namespace cimg_library;
using namespace std;

int printCImg(CImg<unsigned char> img){
  cimg_forXY(img,x,y){
    cout << (int)img.atXY(x,y) << " ";
  }
  return 1;
}

int printVec(vector<int> vec){
  for (int col = 0; col < static_cast<int>(vec.size()); col ++){
    cout << vec[col] << " ";

  } cout <<"\n";
  return 0;
}

int printVector(vector<vector<int> > matrix){
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

void write_pgm(vector<vector<int> > greyscale, CImg<unsigned char> image){ 
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
    cimg_forXY(image,x,y){
      temp = greyscale[x][y];
      fprintf(pgmimg, "%d\n", temp);
    }
    fclose(pgmimg); 
} 

vector<int> chaseBack(vector<vector<int> > accums, int col){

  vector<int> path;
  int max = static_cast<int>(accums.size());
  
  int count = 1;
  path.push_back(col);
  int width = static_cast<int>(accums[0].size());
  int current = col;

  while (max>count){
    vector<int> row = accums[count];
    // printVec(row);
    // cout <<"current is " <<current <<"\n";
    int left = INT_MAX;
    int right = INT_MAX;
    if (current>0){
      left = row[current-1];
    }
    if (current< (width-1)){
      right = row[current+1];
    }
    if (right<left){
      if (right<row[current]){
        current+=1;
        path.push_back(current);
      } else {
        path.push_back(current);
      }
    } else if (left< row[current]){
      current-=1;
      path.push_back(current);
    } else{
      path.push_back(current);
    }
    count +=1;
  
  }
  return path;
}



int getLowestBelow(vector<int> below, int col){
  int left = INT_MAX;
  int right = INT_MAX;

  if (col > 0){
    left = below[col-1];
  }
  if ((col+1) < static_cast<int>(below.size())){
    right = below[col+1];
  }

  return min({left, right, below[col]});

}

vector<vector<int> > removeSeam(vector<int> path, vector<vector<int> > data){
  int count = 0;
  for (int row =0; row < (int) data.size(); row++){
    data[row].erase(data[row].begin()+path[count]);
    count+=1;
  }
  return data;
}

vector<vector<int> > accumulations_creator(vector<vector<int> > energies) {
  int width = static_cast<int>(energies.size());
  int height = static_cast<int>(energies[0].size());

  vector<vector<int> > results;
  vector<int> prevRow(width, 0);
  results.insert(results.begin(),prevRow);

  for (int row = (width -2); row> -1; row --){
    vector<int> currentRow;
    for (int col= (height-1); col>-1; col--){
      int newBit =getLowestBelow(prevRow,col);
      currentRow.push_back(newBit+energies[row][col]);
      
    }
    prevRow = currentRow;
    results.insert(results.begin(),prevRow);
  }

  return results;

}

vector<int> seamPathfinder(vector<vector<int> > accumulations){
  vector<int> topRow = accumulations[0];
  int lowest = topRow[0];
  int col = 0;
  vector<int> results;

  for (int i =1; i< static_cast<int>(topRow.size()); i++){
    if (topRow[i]<lowest){
      lowest = topRow[i];
      col = i;
    }
  }

  results.push_back(col);
  return chaseBack(accumulations, col);

}

vector<vector<int> > energy_data(CImg<unsigned char> image){
  
  int row, col, diffx, diffy, diffxy, width, height;
  int tempPixel;
  width = static_cast<int>(image.width());
  height = static_cast<int>(image.height());

  vector<vector<int> > results(width, vector<int>(height));
  cimg_forXY(image,x,y){
    diffx = abs((int)image.atXY(x, y) - (int)image.atXY(x, y+1));
    diffy = abs((int)image.atXY(x, y) - (int)image.atXY(x+1, y));
    diffxy = abs((int)image.atXY(x, y) - (int)image.atXY(x+1, y+1));
    tempPixel = diffx +diffy + diffxy;

    if (tempPixel>255)
        tempPixel =255;

      results[x][y] = tempPixel;
  }

  return results;
}

vector<vector<int> > writeIntoVector(CImg<unsigned char> image){
  
  int width, height, row, col;
  int tempPixel;
  width = static_cast<int>(image.width());
  height = static_cast<int>(image.height());

  vector<vector<int> > results(width, vector<int>(height));
  // for(row = 0;row<height; row ++) {
  //   for(col =0; col<width; col++){
  //     tempPixel = image.atXY(col,row);
  //     if (tempPixel>255)
  //       tempPixel =255;

  //     results[row][col] = tempPixel;
  //   }
  // }
  cimg_forXY(image,x,y) {
  results[x][y]= image.atXY(x,y);
  }

  return results;
}

CImg<unsigned char> carveSeam(CImg<unsigned char> image, int cuts){
  vector<vector<int> > data = writeIntoVector(image);
  write_pgm(data, image);
  while (cuts>0){
    CImg<unsigned char> editableImage("pgmimg.pgm");
    vector<vector<int> > energy = energy_data(editableImage);
    
    vector<vector<int> > accumulations = accumulations_creator(energy);
    vector<int> path = seamPathfinder(accumulations);
    data = removeSeam(path, data);
    cuts--;
    write_pgm(data, editableImage);
  }
  CImg<unsigned char> fixedImage("pgmimg.pgm");
  return fixedImage;
}

int main() {
  CImg<unsigned char> bwImage("testMountain.pgm");
  double start = omp_get_wtime();
  carveSeam(bwImage, 100);
  double end = omp_get_wtime();

  cout<<"Total time for 100 seams is " << end-start <<"\n";
  CImg<unsigned char>output("pgmimg.pgm");
  CImgDisplay main_disp(output,"energies"), other_dip(bwImage, "original");
  while (!main_disp.is_closed()) {
     main_disp.wait();
   }
  return 0;
}
