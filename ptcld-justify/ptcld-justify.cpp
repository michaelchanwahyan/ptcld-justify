#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> 
#include <cctype>
#include <locale>

// =====================
// for string opteration
// =====================
// ref: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// #include <algorithm> 
// #include <cctype>
// #include <locale>

// trim from start (in place)
void ltrim(std::string &s) {s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {return !std::isspace(ch);}));}
// trim from end (in place)
void rtrim(std::string &s) {s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch);}).base(), s.end());}
// trim from both ends (in place)
void trim(std::string &s) {ltrim(s);rtrim(s);}
// trim from start (copying)
std::string ltrim_copy(std::string s) {ltrim(s);return s;}
// trim from end (copying)
std::string rtrim_copy(std::string s) {rtrim(s);return s;}
// trim from both ends (copying)
std::string trim_copy(std::string s) {trim(s);return s;}

// ============================
// END OF for string opteration
// ============================

void line_split(const std::string &inputstring, std::vector<std::string> &output, char delim)
{
    size_t pos = inputstring.find(delim);
    size_t initialPos = 0;
    output.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        output.push_back(inputstring.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = inputstring.find(delim, initialPos);
    }

    // Add the last one
    output.push_back(inputstring.substr(initialPos, std::min(pos, inputstring.size()) - initialPos + 1));

    return;
}

std::vector<std::vector<float>> readpointcloud(std::string inputfilename, const char delim) {
    std::ifstream fin;
    std::vector<std::string> POINT;
    std::vector<std::vector<float>> POINTS;

    // check number of points from input file
    fin.open(inputfilename, std::ios::in);
    std::string line;
    unsigned int pointnum = 0;
    line = "";
    while (getline(fin, line)) {
        pointnum++;
    }
    fin.close();

    // initialize point cloud buffer
    POINTS.resize(pointnum);

    // read points from input file
    fin.open(inputfilename, std::ios::in);

    line = "";
    for (int rowcnt = 0; rowcnt < pointnum; rowcnt++) {
        getline(fin, line);
        POINT.resize(3);
        line_split(line, POINT, delim);

        for (int j = 0; j < 3; j++) {
            POINTS[rowcnt].resize(3);
            POINTS[rowcnt][j] = std::stof(trim_copy(POINT[j]));
            //std::cout << POINTS[rowcnt][j] << std::endl;
        }
        std::cout << std::endl;
    }
    fin.close();
    return POINTS;
}

int main()
{
    std::string filename = "";

    std::cout << "input filename:" << std::endl;
    std::cin >> filename;
    std::cout << "reading " << filename << " ..." << std::endl;
    std::vector<std::vector<float>> POINTS = readpointcloud(filename, ',');
    std::cout << "finish reading !" << std::endl;

    for (unsigned int pointcnt = 0; pointcnt < POINTS.size(); pointcnt++) {
        for (uint8_t axiscnt = 0; axiscnt < 3; axiscnt++) {
            std::cout << ' ' << POINTS[pointcnt][axiscnt];
        }
        std::cout << std::endl;
    }
    return 0;
}
