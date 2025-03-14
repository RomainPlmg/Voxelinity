#ifndef __FILE_H__
#define __FILE_H__

#include <fstream>
#include <memory>
#include <string>

class File {
   public:
    static std::string ReadFromFile(const std::string& filePath);
    static std::unique_ptr<std::ifstream> LoadFile(const std::string& filePath);
};

#endif  // __FILE_H__
