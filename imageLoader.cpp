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


// 
//my modified version
vector<vector<int> > simpleEnergyCalculation(vector<vector<int> > photo_info) {
  int curr_x, curr_y;
  int x, y, xy;
  vector<vector<int> > results;
  //lets add a strip of blank on the bottom and right edge
  int width = static_cast<int>(photo_info.size());
  int height = static_cast<int>(photo_info[0].size());
  vector<int> padding(height, 0);
  photo_info.push_back(padding);
  width+=1;
  for (int row =0; row<width; row++){
    photo_info[row].push_back(0);
  }
  height +=1;

  int array[width][height];
  //check sides
for(int i=0;i<width-1;i++){
  vector<int> temp_vec;
  curr_x = i;
  int temp_energies;
  for (int j=0;j<height-1; j++){
    x = abs(photo_info[i][j]-photo_info[i][j+1]);
    y = abs(photo_info[i][j]-photo_info[i+1][j]);
    xy = abs(photo_info[i][j]-photo_info[i+1][j+1]);
    temp_energies= abs(x+y+xy);
    if(temp_energies>255){
      temp_energies = 255;
    }
    array[i][j] = temp_energies;
    temp_vec.push_back(array[i][j]);
    }
  results.push_back(temp_vec);
  }              
  return results;
}






vector<vector<int> > read_pgm_in(string fileName){
  int row = 0, col = 0, numrows = 0, numcols = 0;
  ifstream infile(fileName);
  stringstream ss;
  string inputLine = "";

  // First line : version
  getline(infile,inputLine);
  if(inputLine.compare("P5") != 0) cerr << "Version error" << endl;
  else cout << "Version : " << inputLine << endl;

  // Continue with a stringstream
  ss << infile.rdbuf();
  // Third line : size
  ss >> numcols >> numrows;
  cout << numcols << " columns and " << numrows << " rows" << endl;

  vector<vector<int> > vec;
  int array[numrows][numcols];
  // Following lines : data
  for(row = 0; row < numrows; ++row){
    vector<int> temp;
    
    
    for (col = 0; col < numcols; ++col) {
      
      ss >> array[row][col];
      // cout<< "vector position reads " << vec[row][col]
      temp.push_back(array[row][col]);
      
    }
    vec.push_back(temp);
  }
  infile.close();
return vec;}
// void simpleEnergyCPU(PGMData *input, PGMData *output, int new_width)
// {
// int i, j;
// int diffx, diffy, diffxy; 
// int tempPixel; 
// for(i=0; i<(input->height); i++) {
//   for(j=0; j<new_width; j++)
//   {
//     diffx = abs(getPixelCPU(input, i, j) - getPixelCPU(input, i, j+1));
//     diffy = abs(getPixelCPU(input, i, j) - getPixelCPU(input, i+1, j));
//     diffxy = abs(getPixelCPU(input, i, j) - getPixelCPU(input, i+1, j+1));
//     tempPixel = diffx + diffy + diffxy; if( tempPixel>255 )
//     output->image[i*output->width+j] = 255; else
//     output->image[i*(output->width)+j] = tempPixel;
//             }
//  }
// }

void write_pgm(vector<vector<int> > greyscale){ 
    int i, j, temp = 0; 
    int width = static_cast<int>(greyscale.size());
    int height = static_cast<int>(greyscale[0].size());
  
    FILE* pgmimg; 
    pgmimg = fopen("pgmimg.pgm", "wb"); 
  
    // Writing Magic Number to the File 
    fprintf(pgmimg, "P5\n");  
  
    // Writing Width and Height 
    fprintf(pgmimg, "%d %d\n", width, height);  
  
    // Writing the maximum gray value 
    fprintf(pgmimg, "255\n");  
    int count = 0; 
    for (i = 0; i < height; i++) { 
        for (j = 0; j < width; j++) { 
            temp = (unsigned char)greyscale[i][j];
  
            // Writing the gray values in the 2D array to the file 
            fprintf(pgmimg, "%d ", temp); 
        } 
        fprintf(pgmimg, "\n"); 
    } 
    fclose(pgmimg); 
} 



vector<vector<int> > energy_data(CImg<unsigned char> image){
  
  vector<vector<int> > results;
  int row, col, diffx, diffy, diffxy, width, height;
  int tempPixel;
  width = static_cast<int>(image.width());
  height = static_cast<int>(image.height());
  int holder[width][height];
  for(row = 0;row<width; row ++) {
    vector<int> row_vec;
    for(col =0; col<height; col++){
      //cout <<"pixel is " <<static_cast<int>(image.atXY(row,col))<<"\n";
      diffx = abs(image.atXY(row, col) - image.atXY(row, col+1));
      diffy = abs(image.atXY(row, col) - image.atXY(row+1, col));
      diffxy = abs(image.atXY(row, col) - image.atXY(row+1, col+1));
      tempPixel = diffx + diffy + diffxy;
      if (tempPixel>255)
        tempPixel =255;
      holder[row][col] = tempPixel;
      row_vec.push_back(holder[row][col]);
    }
    results.push_back(row_vec);
  }

  return results;
}


int main() {
  CImg<unsigned char> bwImage("testLandscape.pgm");
  CImg<unsigned char> image("test.pgm");
  cout <<"cImg size" << static_cast<int>(bwImage.size());
  vector<vector<int> > test = energy_data(bwImage);
  write_pgm(test);
  CImg<unsigned char>output("pgmimg.pgm");
  CImgDisplay main_disp(bwImage, "Testing"), main_display_output(output,"See anything?");
  while (!main_disp.is_closed()) {
    main_disp.wait();
    }
  return 0;
}