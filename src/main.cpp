#include "llm_client.hpp"
#include <iostream>
#include <string>

int main() {
    // Initialize LLM client
    LLMClient client;
    if (!client.initialize()) {
        std::cerr << "Failed to initialize LLM client. Please check your API key.\n";
        return 1;
    }

    std::cout << "Welcome to the LLM Console Application!\n";
    std::cout << "Type 'exit' to quit the application.\n\n";

    std::string input;
    while (true) {
        // Get user input
        std::cout << "\nEnter your prompt: ";
        std::getline(std::cin, input);

        // Check for exit command
        if (input == "exit") {
            break;
        }

        // Skip empty input
        if (input.empty()) {
            continue;
        }

        try {
            // Get response from LLM
            std::cout << "\nProcessing...\n";
            std::string response = client.getResponse(input);
            
            // Display response
            std::cout << "\nAI Response:\n";
            std::cout << "------------\n";
            std::cout << response << "\n";
            std::cout << "------------\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    std::cout << "\nThank you for using the LLM Console Application!\n";
    return 0;
}
