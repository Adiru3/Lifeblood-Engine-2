#pragma once
#include <string>
#include <vector>

class AntiCheat {
public:
    static bool CheckFileIntegrity(const std::string& path, const std::string& expectedHash);
    static std::string CalculateSHA256(const std::string& path);
    static void ScanDirectory(const std::string& dir);
};
