#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// Small helpers for reading console input and parsing text.

inline std::string trim(const std::string& text) {
    std::size_t start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    std::size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

inline bool parseInt(const std::string& text, int& value) {
    std::istringstream stream(text);
    char extra;
    return (stream >> value) && !(stream >> extra);
}

inline bool parseDouble(const std::string& text, double& value) {
    std::istringstream stream(text);
    char extra;
    return (stream >> value) && !(stream >> extra);
}

// Splits a comma-separated line into at most maxFields trimmed fields.
inline int splitFields(const std::string& line, std::string fields[], int maxFields) {
    std::istringstream stream(line);
    std::string field;
    int count = 0;
    while (count < maxFields && std::getline(stream, field, ',')) {
        fields[count++] = trim(field);
    }
    return count;
}

// Throws if the input stream has ended, so main() can exit cleanly.
inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("input ended");
    return trim(line);
}

inline std::string readText(const std::string& prompt) {
    while (true) {
        std::string text = readLine(prompt);
        if (!text.empty()) return text;
        std::cout << "This field cannot be empty.\n";
    }
}

inline int readInt(const std::string& prompt, int low, int high) {
    while (true) {
        int value;
        if (parseInt(readLine(prompt), value) && value >= low && value <= high) return value;
        std::cout << "Please enter a whole number from " << low << " to " << high << ".\n";
    }
}

inline double readPositiveDouble(const std::string& prompt) {
    while (true) {
        double value;
        if (parseDouble(readLine(prompt), value) && value > 0) return value;
        std::cout << "Please enter a number greater than 0.\n";
    }
}

#endif
