# LLM Console Application

This C++ console application demonstrates integration with Hugging face API to process user input and provide AI-generated responses.

## Prerequisites

- C++ compiler (C++17 or later)
- CMake (3.15 or later)
- libcurl library
- nlohmann/json library
- Huggingface API key

## Setup Instructions

1. Clone this repository
2. Install the required dependencies:
   ```bash
   vcpkg install curl:x64-windows
   vcpkg install nlohmann-json:x64-windows
   ```
3. Set your Huggingface API key as an environment variable:
   ```bash
   set HUGGING_FACE_API_KEY=your_api_key_here
   ```
4. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
5. Run the application:
   ```bash
   cd c:/Users/MATRIX/Desktop/-NLP-/build/Debug 
   .\llm_console.exe
   
   ```

## Usage

1. Launch the application
2. Enter your question or prompt when prompted
3. The application will send your input to the Huggingface API
4. The AI-generated response will be displayed
5. Type 'exit' to quit the application

## Project Structure

- `main.cpp`: Entry point of the application
- `llm_client.hpp`: Header file containing the LLMClient class declaration
- `llm_client.cpp`: Implementation of the LLMClient class
- `CMakeLists.txt`: CMake configuration file

## Note

Make sure to keep your API key secure and never commit it to version control.
