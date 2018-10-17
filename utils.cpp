#include <fstream>

using namespace std;

bool fileExists(string filename) {
  ifstream ifile(filename);
  return (bool)ifile;
}
