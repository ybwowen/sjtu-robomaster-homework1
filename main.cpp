#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>
#include <string>
#include <iostream>

const double MAX_RANGE = 1000.0;
const double START_POSITION = 2000.0;

cv::Mat apple;

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
    cv::Mat result;
    int thres_red = 100; int coef_green = START_POSITION + MAX_RANGE; int coef_blue = START_POSITION;
    TrackerData data = (TrackerData){apple, result, thres_red, coef_green, coef_blue};
    cv::namedWindow("result");
    cv::createTrackbar("thres_red", "result", &thres_red, 255, OnChange_thres, &data);
    cv::createTrackbar("coef_green", "result", &coef_green, 4 * MAX_RANGE, OnChange_coefgreen, &data);
    cv::createTrackbar("coef_blue", "result", &coef_blue, 4 * MAX_RANGE, OnChange_coefblue, &data);
}

}

void showImage(std::string windowname, cv::Mat image){
    cv::imshow(windowname, image);
    cv::waitKey(0);
}   

namespace HSVColors{

class TrackerData{
public:
    cv::Mat src;
    cv::Mat dir;
    int hmin;
    int hmax;
    int smin;
    int smax;
    int vmin;
    int vmax;
    TrackerData(cv::Mat x1, cv::Mat x2, int _hmin, int _hmax, int _smin, int _smax, int _vmin, int _vmax){
        src = x1; dir = x2;
        hmin = _hmin; hmax = _hmax;
        smin = _smin; smax = _smax;
        vmin = _vmin; vmax = _vmax;
    }
};

void OnChange_hmin(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.hmin = value;
    cv::inRange(apple_hsv, cv::Scalar(0, 43, 46), cv::Scalar(10, 255, 255), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(168, 43, 46), cv::Scalar(180, 255, 255), hsv_red2);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_green);
    hsv_result = hsv_red1 | hsv_red2 | hsv_green;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_hmax(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.hmax = value;
    cv::inRange(apple_hsv, cv::Scalar(0, 43, 46), cv::Scalar(10, 255, 255), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(168, 43, 46), cv::Scalar(180, 255, 255), hsv_red2);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_green);
    hsv_result = hsv_red1 | hsv_red2 | hsv_green;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_smin(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.smin = value;
    cv::inRange(apple_hsv, cv::Scalar(0, 43, 46), cv::Scalar(10, 255, 255), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(168, 43, 46), cv::Scalar(180, 255, 255), hsv_red2);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_green);
    hsv_result = hsv_red1 | hsv_red2 | hsv_green;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_smax(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.smax = value;
    cv::inRange(apple_hsv, cv::Scalar(0, 43, 46), cv::Scalar(10, 255, 255), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(168, 43, 46), cv::Scalar(180, 255, 255), hsv_red2);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_green);
    hsv_result = hsv_red1 | hsv_red2 | hsv_green;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_vmin(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.vmin = value;
    cv::inRange(apple_hsv, cv::Scalar(0, 43, 46), cv::Scalar(10, 255, 255), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(168, 43, 46), cv::Scalar(180, 255, 255), hsv_red2);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_green);
    hsv_result = hsv_red1 | hsv_red2 | hsv_green;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_vmax(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.vmax = value;
    cv::inRange(apple_hsv, cv::Scalar(0, 43, 46), cv::Scalar(10, 255, 255), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(168, 43, 46), cv::Scalar(180, 255, 255), hsv_red2);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_green);
    hsv_result = hsv_red1 | hsv_red2 | hsv_green;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void solve(){
    cv::Mat apple_hsv, apple_result;
    cv::cvtColor(apple, apple_hsv, cv::COLOR_BGR2HSV);
    int hmin = 0; int hmax = 27;
    int smin = 160; int smax = 255;
    int vmin = 75; int vmax = 255;
    TrackerData data = (TrackerData){apple_hsv, apple_result, hmin, hmax, smin, smax, vmin, vmax};
    cv::namedWindow("result");
    cv::createTrackbar("hmin", "result", &hmin, 255, OnChange_hmin, &data);
    cv::createTrackbar("hmax", "result", &hmax, 255, OnChange_hmax, &data);
    cv::createTrackbar("smin", "result", &smin, 255, OnChange_smin, &data);
    cv::createTrackbar("smax", "result", &smax, 255, OnChange_smax, &data);
    cv::createTrackbar("vmin", "result", &vmin, 255, OnChange_vmin, &data);
    cv::createTrackbar("vmax", "result", &vmax, 255, OnChange_vmax, &data);
    // cv::waitKey(0);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(data.dir, apple_result, cv::MORPH_ERODE, kernel, cv::Point(-1, -1), 1);
    // showImage("morph_img", apple_result);
    cv::medianBlur(apple_result, apple_result, 7);
    // showImage("blured_img", apple_result);
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierachy;
    cv::findContours(apple_result, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::Mat drawer = cv::Mat::zeros(cv::Size(apple_result.cols, apple_result.rows), CV_8UC3);
    cv::Rect box;
    for(int i = 0; i < contours.size(); i++){
        if(cv::contourArea(contours[i]) < 2000) continue;
        cv::drawContours(drawer, contours, i, cv::Scalar(255, 255, 255), 1);
        box = cv::boundingRect(contours[i]);
    }
    cv::rectangle(drawer, cv::Point(box.x, box.y), cv::Point(box.x + box.width, box.y + box.height), cv::Scalar(255, 255, 255), 2);
    drawer.copyTo(apple, drawer);
    showImage("apple_result", apple);
    cv::imwrite("../apple_result.png", apple);
}

}

int main(){
    apple = cv::imread("../apple.png");
    assert(apple.channels() == 3);
    // RGBChannels::solve();
    HSVColors::solve();
}
