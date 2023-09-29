#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>
#include <string>
#include <iostream>

const double MAX_RANGE = 1000.0;
const double START_POSITION = 2000.0;

cv::Mat apple;
cv::Mat apple_src;

void showImage(std::string windowname, cv::Mat img){
    cv::imshow(windowname, img);
    cv::waitKey(0);
}

namespace RGBChannels{
    
class TrackerData{
public:
    cv::Mat src;
    cv::Mat dir;
    int thres_red;
    int coef_green;
    int coef_blue;
    TrackerData(cv::Mat x1, cv::Mat x2, int _thres_red, int _coef_green, int _coef_blue){
        src = x1; dir = x2;
        thres_red = _thres_red;
        coef_green = _coef_green;
        coef_blue = _coef_blue;
    }
};

void OnChange_thres(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_channels[3];
    cv::split(user_data.src, apple_channels);
    apple_channels[2] -= (user_data.coef_green - START_POSITION) / MAX_RANGE * apple_channels[1] - (user_data.coef_blue - START_POSITION) / MAX_RANGE * apple_channels[0];
    cv::normalize(apple_channels[2], apple_channels[2], 0, 255, cv::NORM_MINMAX);
    user_data.thres_red = value;
    // std::cout << apple_channels[2] << std::endl;
    cv::threshold(apple_channels[2], user_data.dir, user_data.thres_red, 255, cv::THRESH_BINARY);
    cv::imshow("result", user_data.dir);
}

void OnChange_coefgreen(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_channels[3];
    cv::split(user_data.src, apple_channels);
    user_data.coef_green = value;
    apple_channels[2] -= (user_data.coef_green - START_POSITION) / MAX_RANGE * apple_channels[1] - (user_data.coef_blue - START_POSITION) / MAX_RANGE * apple_channels[0];
    cv::normalize(apple_channels[2], apple_channels[2], 0, 255, cv::NORM_MINMAX);
    cv::threshold(apple_channels[2], user_data.dir, user_data.thres_red, 255, cv::THRESH_BINARY);
    cv::imshow("result", user_data.dir);
}

void OnChange_coefblue(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_channels[3];
    cv::split(user_data.src, apple_channels);
    user_data.coef_blue = value;
    apple_channels[2] -= (user_data.coef_green - START_POSITION) / MAX_RANGE * apple_channels[1] - (user_data.coef_blue - START_POSITION) / MAX_RANGE * apple_channels[0];
    cv::normalize(apple_channels[2], apple_channels[2], 0, 255, cv::NORM_MINMAX);
    cv::threshold(apple_channels[2], user_data.dir, user_data.thres_red, 255, cv::THRESH_BINARY);
    cv::imshow("result", user_data.dir);
}

void solve(){
    cv::cvtColor(apple_src, apple, cv::COLOR_BGR2RGB);
    cv::Mat result;
    int thres_red = 150; int coef_green = 2850; int coef_blue = 2243;
    TrackerData data = (TrackerData){apple, result, thres_red, coef_green, coef_blue};
    cv::namedWindow("result");
    cv::createTrackbar("thres_red", "result", &thres_red, 255, OnChange_thres, &data);
    cv::createTrackbar("coef_green", "result", &coef_green, 4 * MAX_RANGE, OnChange_coefgreen, &data);
    cv::createTrackbar("coef_blue", "result", &coef_blue, 4 * MAX_RANGE, OnChange_coefblue, &data);
    cv::waitKey(0);
    std::vector<std::vector<cv::Point> > contours;

    std::vector<cv::Vec4i> hierachy;
    cv::findContours(data.dir, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::Mat drawer = cv::Mat::zeros(cv::Size(apple.cols, apple.rows), CV_8UC3);
    cv::Mat drawerPoly = cv::Mat::zeros(cv::Size(apple.cols, apple.rows), CV_8UC3);
    std::vector<std::vector<cv::Point> > contoursPoly(contours.size());
    for(int i = 0; i < contours.size(); i++){
        if(cv::contourArea(contours[i]) < 3000) continue;
        // if(cv::arcLength(contours[i], true) > 1000) continue;
        cv::drawContours(drawer, contours, i, cv::Scalar(255, 255, 255), 1);
        cv::approxPolyDP(contours[i], contoursPoly[i], 30, true);
        cv::drawContours(drawerPoly, contoursPoly, i, cv::Scalar(0, 0, 255), 3);
        showImage("drawer", drawer);
        showImage("approxPoly", drawerPoly);
    }
    
    drawerPoly.copyTo(apple_src, drawerPoly);
    showImage("apple_result", apple_src);
    cv::imwrite("../005_result.jpg", apple_src);
}

}

int main(){
    apple_src = cv::imread("../005.jpg");


    // cv::resize(apple_src, apple_src, {1.0 * apple_src.size[0] / 2, 1.0 * apple_src.size[1] / 2});
    showImage("apple_src", apple_src);
    // cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    // cv::Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    // cv::Mat src_moprh;
    // cv::morphologyEx(src, src_moprh, cv::MORPH_GRADIENT, kernel);
    // showImage("src_morph", src_moprh);
    // cv::Mat src_gray;
    // cv::cvtColor(src_moprh, src_gray, cv::COLOR_BGR2GRAY);
    // showImage("src_gray", src_gray);
    // // cv::medianBlur(src_gray, src_gray, 3);
    // // cv::morphologyEx(src_gray, src_gray, cv::MORPH_ERODE, kernel_small, cv::Point(-1, -1), 1);
    // cv::threshold(src_gray, src_gray, 55, 255, cv::THRESH_BINARY);
    // // cv::medianBlur(src_gray, src_gray, 3);
    // showImage("src_gray", src_gray);
    // // cv::Mat src_fixed;
    // // cv::morphologyEx(src_gray, src_fixed, cv::MORPH_ERODE, kernel_small, cv::Point(-1, -1), 1);
    // // cv::medianBlur(src_fixed, src_fixed, 3);
    // // showImage("src_fixed", src_fixed);
    RGBChannels::solve();
    return 0;
}
