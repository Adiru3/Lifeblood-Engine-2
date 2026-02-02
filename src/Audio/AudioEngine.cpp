#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include "Audio/AudioEngine.h"
#include <iostream>

static ma_engine engine;
static ma_device captureDevice;
static bool voiceInitialized = false;

void* AudioEngine::pEngine = &engine;

// Capture callback
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    // In a real app, send 'pInput' (PCM data) to the UDP socket here!
    // For now, checks volume level to debug
    // float* samples = (float*)pInput;
}

bool AudioEngine::Init() {
    ma_result result;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
        return false;
    }
    return true;
}

void AudioEngine::Shutdown() {
    ma_engine_uninit(&engine);
    if(voiceInitialized) ma_device_uninit(&captureDevice);
}

void AudioEngine::PlaySound(const std::string& path) {
    ma_engine_play_sound(&engine, path.c_str(), NULL);
}

bool AudioEngine::InitMicrophone() {
    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = ma_format_f32;
    deviceConfig.capture.channels = 1;
    deviceConfig.sampleRate       = 44100;
    deviceConfig.dataCallback     = data_callback;

    if (ma_device_init(NULL, &deviceConfig, &captureDevice) != MA_SUCCESS) {
        std::cerr << "Failed to initialize capture device." << std::endl;
        return false;
    }
    voiceInitialized = true;
    return true;
}

void AudioEngine::StartVoiceCapture() {
    if(voiceInitialized) ma_device_start(&captureDevice);
}

void AudioEngine::StopVoiceCapture() {
    if(voiceInitialized) ma_device_stop(&captureDevice);
}
