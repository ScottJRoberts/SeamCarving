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
    int temp = 0; 
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
    //@TODO parallel this 
    for (int y = 0; y<image.height(); y++){
      for (int x = 0; x< image.width(); x++){
        temp = greyscale[x][y];
        fprintf(pgmimg, "%d\n", temp);
    }
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
  //@TODO parallel this (RACE CONDITION POSSIBLE)


  for (int row =0; row < (int) data.size(); row++){
    data[row].erase(data[row].begin()+path[count]);
    count+=1;
  }
  return data;
}

//TODO erase data vertically
vector<vector<int> > removeSeam_vertical(vector<int> path, vector<vector<int> > data){
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
    vector<int> currentRow(height);
    #pragma omp parallel for collapse(1)
    for (int col= (height-1); col>-1; col--){
      int newBit =getLowestBelow(prevRow,col);
      currentRow[col]=newBit+energies[row][col];
      //results[col][row] = newBit;
      
    }
    prevRow = currentRow;
    results.insert(results.begin(),prevRow);
  }

  return results;

}


//TODO: build a horizontal check needs to check left upper, left, left lower and move across
vector<vector<int> > accumulations_creator_horizontal(vector<vector<int> > energies) {
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
  
  int tempPixel,diffx, diffy, diffxy, width, height;
  
  width = static_cast<int>(image.width());
  height = static_cast<int>(image.height());

  //@TODO parallel this, RACE CONDITIONS
  vector<vector<int> > results(width, vector<int>(height,0));
  // #pragma omp parallel private(tempPixel)
  // #pragma omp for collapse(2)
  for (int y = 0; y<image.height(); y++){
    for (int x = 0; x< image.width(); x++){
      diffx = abs((int)image.atXY(x, y) - (int)image.atXY(x, y+1));
    diffy = abs((int)image.atXY(x, y) - (int)image.atXY(x+1, y));
    diffxy = abs((int)image.atXY(x, y) - (int)image.atXY(x+1, y+1));
    tempPixel = diffx +diffy + diffxy;

    if (tempPixel>255)
        tempPixel =255;

      results[x][y] = tempPixel;
    }
  }

  return results;
}

vector<vector<int> > writeIntoVector(CImg<unsigned char> image){
  
  int width, height;
  width = static_cast<int>(image.width());
  height = static_cast<int>(image.height());

  vector<vector<int> > results(width, vector<int>(height));
  //@TODO parallel this (how can we parallel the for work with this??)
  for (int y = 0; y<image.height(); y++){
    for (int x = 0; x< image.width(); x++){
  results[x][y]= image.atXY(x,y);
  }
  }

  return results;
}


CImg<unsigned char> carveSeam(CImg<unsigned char> image, int cuts){
  vector<vector<int> > data = writeIntoVector(image);
  write_pgm(data, image);
  while (cuts>0){
    CImg<unsigned char> editableImage("pgmimg.pgm");
    //read energies
    vector<vector<int> > energy = energy_data(editableImage);
    //find accumulations
    vector<vector<int> > accumulations = accumulations_creator(energy);
    //build path
    vector<int> path = seamPathfinder(accumulations);
    //remove seam
    data = removeSeam(path, data);
    cuts--;
    //save image
    write_pgm(data, editableImage);
  }
  CImg<unsigned char> fixedImage("pgmimg.pgm");
  return fixedImage;
}

int main(int argc, char *argv[]) {
  CImg<unsigned char> bwImage(argv[1]);
  double start = omp_get_wtime();
  int numSeams;
  cin >> numSeams;
  double start = omp_get_wtime();
  carveSeam(bwImage, numSeams);
  double end = omp_get_wtime();

  cout<<"Total time for " << numSeams <<" seams is " << end-start <<"\n";
  CImg<unsigned char>output("pgmimg.pgm");
  CImgDisplay main_disp(output,"Carved Image"), other_dip(bwImage, "Original Image");
  while (!main_disp.is_closed()) {
     main_disp.wait();
   }
  return 0;
}
