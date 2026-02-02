#pragma once
#include <string>

// Forward declare to avoid including miniaudio in header everywhere
struct ma_engine;

class AudioEngine {
public:
    static bool Init();
    static void Shutdown();
    static void PlaySound(const std::string& path);
    
    // Voice Chat
    static bool InitMicrophone();
    static void StartVoiceCapture();
    static void StopVoiceCapture();

private:
   static void* pEngine; // Void* to hide impl details (ma_engine)
};
