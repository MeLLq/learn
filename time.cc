#include <fstream>
#include <iostream>
#include <string>

void WriteFile(std::string filename, std::string str);
void WriteFile(std::string filename, std::string str) {
  std::ofstream file(filename, std::ios::binary | std::ios::app);
  file.write(str.c_str(), str.size());
  file.put('\n');
  file.close();
}
int main() {
  std::string s;
  std::ifstream file("filetest");
  while (getline(file, s)) {
    std::cout << s << std::endl;
    s += "dumb";
    std::cout << s << std::endl;
    WriteFile("filetest1.bin", s);
  }
  file.close();
  return 0;
}