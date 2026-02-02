#include "Security/AntiCheat.h"
#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

std::string AntiCheat::CalculateSHA256(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbFile[1024];
    DWORD cbRead = 0;
    BYTE rgbHash[32]; // SHA-256 is 32 bytes
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
    std::string hexHash = "";

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        return "";
    }

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }

    while (file.read((char*)rgbFile, sizeof(rgbFile))) {
        CryptHashData(hHash, rgbFile, (DWORD)file.gcount(), 0);
    }
    // Hash remaining bytes
    if (file.gcount() > 0) {
        CryptHashData(hHash, rgbFile, (DWORD)file.gcount(), 0);
    }

    cbHash = 32;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        std::stringstream ss;
        for (DWORD i = 0; i < cbHash; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)rgbHash[i];
        }
        hexHash = ss.str();
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    return hexHash;
}

bool AntiCheat::CheckFileIntegrity(const std::string& path, const std::string& expectedHash) {
    std::string actualHash = CalculateSHA256(path);
    std::cout << "[AntiCheat] Checking " << path << ": " << actualHash << std::endl;
    return actualHash == expectedHash;
}

void AntiCheat::ScanDirectory(const std::string& dir) {
    // Placeholder for recursive scan
    // In real implementation, this would iterate std::filesystem::directory_iterator
    // and check every DLL/EXE against a server-provided manifest.
    std::cout << "[AntiCheat] Scanning directory: " << dir << std::endl;
}
