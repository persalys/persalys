//                                               -*- C++ -*-
// basic file operations
#include <iostream>
#include <fstream>
using namespace std;

int main ()
{
  ofstream myfile;
  myfile.open ("output0.csv", std::ios::trunc);
  myfile << "# \n";
  myfile << "# 12/16/2021 15:18:38\n";
  myfile << "# The parameters defined in the project are:\n";
  myfile << "# ,P1 - L,P2 - b [m],P3 - h [m],P6 - Module de Young [Pa],P7 - Force Composante Y [N],P5 - Déplacement directionnel Maximum [m]\n";
  myfile << "#\n";
  myfile << "# The following header line defines the name of the columns by reference to the parameters.\n";
  myfile << "Name,P1,P2,P3,P6,P7,P5\n";
  myfile << "DP 0,1,2,3,5,4,0.10662037134170532\n";
  myfile.close();
  return 0;
}
