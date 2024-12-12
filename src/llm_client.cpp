#include "llm_client.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <Winhttp.h>
#include <fstream>
#include <filesystem>

LLMClient::LLMClient() : hSession(NULL) {}

LLMClient::~LLMClient() {
    if (hSession) {
        WinHttpCloseHandle(hSession);
    }
}

bool loadApiKeyFromEnvFile(std::string& apiKey) {
    // Use absolute path
    std::string envPath = "c:/Users/MATRIX/Desktop/-NLP-/.env";
    std::ifstream file(envPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open .env file at: " << envPath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "HUGGING_FACE_API_KEY") {
            apiKey = value;
            std::cout << "Loaded Hugging Face API Key: " << value.substr(0, 5) << "...\n";
            return true;
        }
    }

    std::cerr << "Error: HUGGING_FACE_API_KEY not found in .env file\n";
    return false;
}

bool LLMClient::initialize() {
    // Load API key from .env file
    if (!loadApiKeyFromEnvFile(apiKey)) {
        std::cerr << "Failed to load Hugging Face API key from .env file" << std::endl;
        return false; // Return false if API key loading fails
    }

    // Create a WinHTTP session
    hSession = WinHttpOpen(L"Hugging Face Client", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "Error: Failed to open WinHTTP session" << std::endl;
        return false;
    }

    return true; // Return true if initialization is successful
}

std::wstring LLMClient::stringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

std::string LLMClient::wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

std::string LLMClient::makeRequest(const std::string& prompt) {
    // Create connection
    HINTERNET hConnect = WinHttpConnect(hSession, 
                                      stringToWString(API_HOST).c_str(),
                                      INTERNET_DEFAULT_HTTPS_PORT,
                                      0);
    if (!hConnect) {
        return "Error: Failed to connect to server";
    }

    // Create request
    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
                                          L"POST",
                                          stringToWString(MODEL_PATH).c_str(),
                                          NULL,
                                          WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES,
                                          WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        return "Error: Failed to create request";
    }

    // Prepare headers
    std::wstring headers = L"Content-Type: application/json\r\n";
    headers += stringToWString("Authorization: Bearer " + apiKey + "\r\n");

    // Prepare JSON payload with proper escaping
    std::string escapedPrompt = prompt;
    size_t pos = 0;
    while ((pos = escapedPrompt.find("\"", pos)) != std::string::npos) {
        escapedPrompt.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    // Format the prompt for the model
    std::string jsonPayload = "{\"inputs\": \"" + escapedPrompt + "\", \"parameters\": {\"max_new_tokens\": 250}}";

    // Send request
    if (!WinHttpSendRequest(hRequest,
                           headers.c_str(),
                           -1,
                           (LPVOID)jsonPayload.c_str(),
                           jsonPayload.length(),
                           jsonPayload.length(),
                           0)) {
        DWORD error = GetLastError();
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        return "Error: Failed to send request (Error code: " + std::to_string(error) + ")";
    }

    // Receive response
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        DWORD error = GetLastError();
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        return "Error: Failed to receive response (Error code: " + std::to_string(error) + ")";
    }

    // Read response
    std::string response;
    DWORD bytesAvailable;
    while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
        char* buffer = new char[bytesAvailable + 1];
        DWORD bytesRead;
        if (WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead)) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
        delete[] buffer;
    }

    // Clean up
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);

    // Return the actual response content, not just the similarity score
    return response;
}

std::string LLMClient::getResponse(const std::string& prompt) {
    // Maximum number of retries
    const int MAX_RETRIES = 5;
    const int RETRY_DELAY_MS = 10000; // 10 seconds

    for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
        std::string response = makeRequest(prompt);
        
        // Check for loading or rate limit errors
        if (response.find("Model") != std::string::npos && 
            response.find("is currently loading") != std::string::npos) {
            
            if (attempt < MAX_RETRIES - 1) {
                std::cout << "Model is loading. Waiting 10 seconds before retry..." << std::endl;
                Sleep(RETRY_DELAY_MS);
                continue;
            }
        } else if (response.find("rate limit exceeded") != std::string::npos) {
            if (attempt < MAX_RETRIES - 1) {
                std::cout << "Rate limit exceeded. Waiting 10 seconds before retry..." << std::endl;
                Sleep(RETRY_DELAY_MS);
                continue;
            }
        }
        
        return response;
    }
    
    return "Error: Maximum retries reached. The model is still loading or rate limit exceeded.";
}
