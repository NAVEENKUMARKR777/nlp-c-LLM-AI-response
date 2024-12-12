#pragma once

#include <string>
#include <memory>
#include <windows.h>
#include <winhttp.h>

class LLMClient {
public:
    LLMClient();
    ~LLMClient();

    // Initialize the client with API key
    bool initialize();

    // Send a prompt to the LLM and get response
    std::string getResponse(const std::string& prompt);

private:
    // WinHTTP handle
    HINTERNET hSession;
    
    // API key
    std::string apiKey;
    
    // Helper functions
    std::string makeRequest(const std::string& prompt);
    std::wstring stringToWString(const std::string& str);
    std::string wstringToString(const std::wstring& wstr);
    
    // Constants
    const std::string API_HOST = "api-inference.huggingface.co";
    const std::string MODEL_PATH = "/models/HuggingFaceH4/zephyr-7b-beta";
};
