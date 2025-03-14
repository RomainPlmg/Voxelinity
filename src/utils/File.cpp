#include "File.h"

#include "Logger.h"
#include "pch.h"

std::string File::ReadFromFile(const std::string& filePath) {
    std::ifstream stream(filePath);

    if (!stream.is_open()) {
        LOG_ERROR("File \'{0}\' not found.", filePath);
        return "";
    }

    std::string line;
    std::stringstream ss;
    while (getline(stream, line)) {
        ss << line << '\n';
    }
    return ss.str();
}

std::unique_ptr<std::ifstream> File::LoadFile(const std::string& filePath) {
    auto stream = std::make_unique<std::ifstream>(filePath);
    if (!stream->is_open()) {
        LOG_ERROR("File \'{0}\' not found.", filePath);
        return nullptr;
    } else {
        return stream;
    }
}
