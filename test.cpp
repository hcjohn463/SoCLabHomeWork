#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <regex>

class Computer {
public:
    bool openFile(std::string filename);
    bool readFile(std::ifstream& file);
    void storeVariable(std::string line, bool type);
    void processExpression(std::string line);
    std::map<std::string, int> intMap;
    std::map<std::string, float> floatMap;
    std::string spaceGap(std::string line);
    std::string changeMathSymbol(std::string expression);
    float extractNumber(std::string s);
};

bool Computer::openFile(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return 1;
    }
    readFile(file);
    file.close();
}

bool Computer::readFile(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            if (word == "int") {
                std::cout << "Define int: " << line << std::endl;
                storeVariable(line, 1); //int
                break;
            }
            else if (word == "float") {
                std::cout << "Define float: " << line << std::endl;
                storeVariable(line, 0); //float
                break;
            }
        }
        if (iss.eof()) { // expression
            std::cout << "Sentence: " << line << std::endl;
            line = spaceGap(line); //間隔+去除分號
            processExpression(line);
        }
    }
    return true;
}
std::string Computer::spaceGap(std::string line) {
    std::string result = "";
    bool lastWasOp = true;
    for (int i = 0; i < line.length(); i++) {
        char c = line[i];
        if (c == '=') {
            result += " = ";
            lastWasOp = true;
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
            if (!lastWasOp) {
                result += " ";
            }
            result += c;
            result += " ";
            lastWasOp = true;
        }
        else if (c == ';') {
            break;
        }
        else if (c == ' ') {
            continue;
        }
        else {
            result += c;
            lastWasOp = false;
        }
    }
    // Replace math functions with space-separated versions
    std::vector<std::string> mathFunctions = { "sin", "cos", "exp", "log" };
    for (const auto& func : mathFunctions) {
        std::string searchStr = func + "(";
        std::string replaceStr = " " + func + " (";
        size_t pos = 0;
        while ((pos = result.find(searchStr, pos)) != std::string::npos) {
            result.replace(pos, searchStr.length(), replaceStr);
            pos += replaceStr.length();
        }
    }
    return result;
}

std::string Computer::changeMathSymbol(std::string expression) {
    std::vector<std::string> words;
    std::istringstream iss(expression);
    std::string word;
    std::string after;
    while (iss >> word) {
        words.push_back(word);
    }
    for (const auto& w : words) {
        switch (w[0]) {
            case 's': {
                // sin
                float degrees = this->extractNumber(w);
                float radians = degrees * M_PI / 180;
                float result = sin(radians);
                after += std::to_string(result);
                after += " ";
                break;
            }
            case 'c': {
                // cos
                float degrees = this->extractNumber(w);
                float radians = degrees * M_PI / 180;
                float result = cos(radians);
                after += std::to_string(result);
                after += " ";
                break;
            }
            case 't': {
                // tan
                float degrees = this->extractNumber(w);
                float radians = degrees * M_PI / 180;
                float result = tan(radians);
                after += std::to_string(result);
                after += " ";
                break;
            }
            case 'e': {
                // exp
                float n = this->extractNumber(w);
                float result = exp(n);
                after += std::to_string(result);
                after += " ";
                break;
            }
            case 'l': {
                // log
                float n = this->extractNumber(w);
                float result = log(n);
                after += std::to_string(result);
                after += " ";
                break;
            }
            default: {
                after += w;
                after += " ";
                break;
            }
        }
    }
    std::cout << "After: " << after << std::endl;
    return after;
}
float Computer::extractNumber(std::string s)
{
    std::regex re("\\d+\\.?\\d*");
    std::smatch match;
    if (std::regex_search(s, match, re)) {
        // 如果匹配成功，將匹配到的字符串轉換為浮點數並返回
        return std::stof(match.str());
    }
    else {
        // 如果匹配失敗，返回 0
        return 0;
    }
}
void Computer::processExpression(std::string line) {
    std::istringstream iss(line);
    std::string varName;
    char equalSign;
    float varValue;

    if (iss >> varName >> equalSign >> varValue) {
        if (equalSign == '=') {
            auto itInt = this->intMap.find(varName);
            auto itFloat = this->floatMap.find(varName);
            if (itInt != this->intMap.end()) {
                this->intMap[varName] = varValue;
            }
            else if (itFloat != this->floatMap.end()) {
                this->floatMap[varName] = varValue;
            }
        }
        else {
            std::cerr << "Invalid input: " << line << std::endl;
        }
    }
    else {
        std::string expr;
        std::size_t pos = line.find('=');
        if (pos != std::string::npos && pos < line.length() - 1) {
            expr = line.substr(pos + 1);
        }
        else {
            std::cerr << "Invalid input: " << line << std::endl;
            return;
        }
        std::cout << "Expression: " << expr << std::endl;
        std::string expression = changeMathSymbol(expr);
    }
}

void Computer::storeVariable(std::string line, bool type) {
    std::vector<std::string> variables;
    size_t pos;
    pos = type ? line.find("int ") : line.find("float ");
    if (pos != std::string::npos) {
        pos += type ? 4 : 6;
        size_t end_pos = line.find("//");
        if (end_pos == std::string::npos) {
            end_pos = line.size();
        }
        while (pos < end_pos) {
            size_t next_pos = line.find_first_of(",;", pos);
            if (next_pos == std::string::npos || next_pos >= end_pos) {
                next_pos = end_pos;
            }
            std::string variable_name = line.substr(pos, next_pos - pos);
            variable_name.erase(std::remove_if(variable_name.begin(), variable_name.end(), [](unsigned char c) { return std::isspace(c); }), variable_name.end());
            variables.push_back(variable_name);
            pos = next_pos + 1;
        }
    }
    variables.pop_back();
    if (type) {
        for (const auto& var : variables) {
            this->intMap[var] = 0;
        }
    }
    else {
        for (const auto& var : variables) {
            this->floatMap[var] = 0.0;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl;
        return 1;
    }
    Computer cm;
    cm.openFile(argv[1]);
    for (auto it = cm.intMap.begin(); it != cm.intMap.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    for (auto it = cm.floatMap.begin(); it != cm.floatMap.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    return 0;
}