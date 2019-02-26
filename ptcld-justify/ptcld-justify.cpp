#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> 
#include <cctype>
#include <locale>

using namespace cv;

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
    int pointnum = 0;
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
        POINT.resize(4);
        line_split(line, POINT, delim);

        for (int j = 0; j < 4; j++) {
            POINTS[rowcnt].resize(4);
            POINTS[rowcnt][j] = std::stof(trim_copy(POINT[j]));
            //std::cout << POINTS[rowcnt][j] << std::endl;
        }
        //std::cout << std::endl;
    }
    fin.close();
    return POINTS;
}

void savepointcloud(std::string outputfilename, std::vector<std::vector<float>>POINTS, const char delim) {
    std::ofstream fout(outputfilename);
    for (int i = 0; i < POINTS.size(); i++) {
        fout << POINTS[i][0] << delim << POINTS[i][1] << delim << POINTS[i][2] << delim << POINTS[i][3];
        if (i < POINTS.size() - 1){
            fout << std::endl;
        }
    }
    fout.close();
}

float compute_horizontal_width(std::vector<std::vector<float>> POINTS) {
    //// compute mean point
    //std::vector<float> _meanpoint;
    //_meanpoint.resize(3);
    //for (int i = 0; i < POINTS.size(); i++) {
    //    _meanpoint[0] += POINTS[i][0];
    //    _meanpoint[1] += POINTS[i][1];
    //    _meanpoint[2] += POINTS[i][2];
    //}
    //_meanpoint[0] = _meanpoint[0] / POINTS.size();
    //_meanpoint[1] = _meanpoint[1] / POINTS.size();
    //_meanpoint[2] = _meanpoint[2] / POINTS.size();
    //std::cout << _meanpoint[0] << ", " << _meanpoint[1] << ", " << _meanpoint[2] << "\n";

    //for (int i = 0; i < POINTS.size(); i++) {
    //    POINTS[i][0] = POINTS[i][0] - _meanpoint[0];
    //    POINTS[i][1] = POINTS[i][1] - _meanpoint[1];
    //    POINTS[i][2] = POINTS[i][2] - _meanpoint[2];
    //}

    Mat data_pts = Mat((int)POINTS.size(), 3, CV_64F);
    for (int i = 0; i < data_pts.rows; i++) {
        data_pts.at<float>(i, 0) = POINTS[i][0];
        data_pts.at<float>(i, 1) = POINTS[i][1];
        data_pts.at<float>(i, 2) = POINTS[i][2];
    }

    //Perform PCA analysis
    PCA pca_analysis(data_pts, Mat(), PCA::DATA_AS_ROW);

    //Store the center of the object
	std::cout << "center of the object:\n";
    Point3d cntr = Point3d(pca_analysis.mean.at<double>(0, 0),
                           pca_analysis.mean.at<double>(0, 1),
                           pca_analysis.mean.at<double>(0, 2));
    std::cout << cntr << "\n\n";

    //Store the eigenvalues and eigenvectors
    std::vector<Point3d> eigen_vecs(3);
    std::vector<double> eigen_val(3);
    for (int i = 0; i < 3; i++) {
        eigen_vecs[i] = Point3d(pca_analysis.eigenvectors.at<double>(i, 0),
                                pca_analysis.eigenvectors.at<double>(i, 1),
                                pca_analysis.eigenvectors.at<double>(i, 2));
        eigen_val[i] = pca_analysis.eigenvalues.at<double>(i);
    }

    std::cout << "eigen vectors:\n";
    for (int i = 0; i < 3; i++)
        std::cout << eigen_vecs[i] << "\n";
	std::cout << "\n";
	std::cout << "eigen valus:\n";
    for (int i = 0; i < 3; i++)
        std::cout << eigen_val[i] << " ";
    std::cout << "\n\n";

    return 0.0f;
}

int main()
{
    std::string filename = "";

    std::cout << "input filename:" << std::endl;
    filename = ".\\whiteboxscan\\Serial_numbe_1100118528610\\Front.csv";
    
    // reading the point cloud
    std::cout << "reading " << filename << " ..." << std::endl;
    std::vector<std::vector<float>> POINTS = readpointcloud(filename, ',');
    std::cout << "finish reading !" << std::endl;

    std::cout << "point cloud contains " << POINTS.size() << " points !" << std::endl;
    
    // find point cloud within ROI
    int activePointNum = 0;
    std::vector<float> POINT;
    std::vector<std::vector<float>> POINTS_ROI;
    POINT.resize(4);
    for (unsigned int pointcnt = 0; pointcnt < POINTS.size(); pointcnt++) {
        POINT.clear();
        POINT = POINTS[pointcnt];
        if (
            POINT[0] >= 1.38 && POINT[0] <= 1.89 &&
            POINT[1] >= 2.22 && POINT[1] <= 2.65 &&
            POINT[2] >=-0.45 && POINT[2] <= 1.70
            ) {
            POINTS_ROI.push_back(POINT);
            activePointNum++;
        }
    }
    //std::cout << "point cloud contains " << activePointNum << " active points !" << std::endl;
    std::cout << "point cloud contains " << POINTS_ROI.size() << " active points !" << std::endl;

    filename = ".\\whiteboxscan\\Serial_numbe_1100118528610\\Front-roi.csv";
    savepointcloud(filename, POINTS_ROI, ',');

    compute_horizontal_width(POINTS_ROI);
    return 0;
}
