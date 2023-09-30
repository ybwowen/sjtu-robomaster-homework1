#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>
#include <string>
#include <iostream>

// #define DEBUG
#define ROTATED
// #define HSV

const int eps = 7;

cv::Mat apple_src;
cv::Mat result;

void showImage(std::string windowname, cv::Mat img){
    cv::imshow(windowname, img);
    cv::waitKey(0);
}

namespace RGBChannels{
    
int thres_red = 72; int coef_green = 2589; int coef_blue = 2038;
const double MAX_RANGE = 1000.0;
const double START_POSITION = 2000.0;

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

cv::Mat threshBinary(cv::Mat apple_src){
    TrackerData data = (TrackerData){apple_src, result, thres_red, coef_green, coef_blue};

    #ifdef DEBUG
    cv::namedWindow("result");
    cv::createTrackbar("thres_red", "result", &thres_red, 255, OnChange_thres, &data);
    cv::createTrackbar("coef_green", "result", &coef_green, 4 * MAX_RANGE, OnChange_coefgreen, &data);
    cv::createTrackbar("coef_blue", "result", &coef_blue, 4 * MAX_RANGE, OnChange_coefblue, &data);
    cv::waitKey(0);
    // showImage("apple", data.dir);
    #else
    cv::Mat apple_channels[3];
    cv::split(apple_src, apple_channels);
    apple_channels[2] -= (coef_green - START_POSITION) / MAX_RANGE * apple_channels[1] - (coef_blue - START_POSITION) / MAX_RANGE * apple_channels[0];
    cv::normalize(apple_channels[2], apple_channels[2], 0, 255, cv::NORM_MINMAX);
    // thres_red = value;
    // std::cout << apple_channels[2] << std::endl;
    cv::threshold(apple_channels[2], data.dir, thres_red, 255, cv::THRESH_BINARY);
    // cv::imshow("result", dir);
    #endif
    return data.dir;
}

}

namespace HSVColors{

    int hmin = 171; int hmax = 35;
    int smin = 99; int smax = 255;
    int vmin = 43; int vmax = 246;

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
    cv::Mat hsv_red1, hsv_red2, hsv_result;
    user_data.hmin = value;
    cv::inRange(apple_hsv, cv::Scalar(0, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(180, user_data.smax, user_data.vmax), hsv_red2);
    hsv_result = hsv_red1 | hsv_red2;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_hmax(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.hmax = value;
    cv::inRange(apple_hsv, cv::Scalar(0, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(180, user_data.smax, user_data.vmax), hsv_red2);
    hsv_result = hsv_red1 | hsv_red2;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_smin(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.smin = value;
    cv::inRange(apple_hsv, cv::Scalar(0, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(180, user_data.smax, user_data.vmax), hsv_red2);
    hsv_result = hsv_red1 | hsv_red2;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_smax(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    cv::inRange(apple_hsv, cv::Scalar(0, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(180, user_data.smax, user_data.vmax), hsv_red2);
    hsv_result = hsv_red1 | hsv_red2;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_vmin(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.vmin = value;
    cv::inRange(apple_hsv, cv::Scalar(0, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(180, user_data.smax, user_data.vmax), hsv_red2);
    hsv_result = hsv_red1 | hsv_red2;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

void OnChange_vmax(int value, void* data){
    TrackerData &user_data = *(TrackerData*) data;
    cv::Mat apple_hsv = user_data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    user_data.vmax = value;
    cv::inRange(apple_hsv, cv::Scalar(0, user_data.smin, user_data.vmin), cv::Scalar(user_data.hmax, user_data.smax, user_data.vmax), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(user_data.hmin, user_data.smin, user_data.vmin), cv::Scalar(180, user_data.smax, user_data.vmax), hsv_red2);
    hsv_result = hsv_red1 | hsv_red2;
    user_data.dir = hsv_result;
    cv::imshow("result", hsv_result);
}

cv::Mat threshBinary(cv::Mat apple_src){
    cv::Mat apple_hsv, apple_result;
    cv::cvtColor(apple_src, apple_hsv, cv::COLOR_BGR2HSV);

    TrackerData data = (TrackerData){apple_hsv, apple_result, hmin, hmax, smin, smax, vmin, vmax};
    #ifdef DEBUG
    cv::namedWindow("result");
    cv::createTrackbar("hmin", "result", &hmin, 180, OnChange_hmin, &data);
    cv::createTrackbar("hmax", "result", &hmax, 180, OnChange_hmax, &data);
    cv::createTrackbar("smin", "result", &smin, 255, OnChange_smin, &data);
    cv::createTrackbar("smax", "result", &smax, 255, OnChange_smax, &data);
    cv::createTrackbar("vmin", "result", &vmin, 255, OnChange_vmin, &data);
    cv::createTrackbar("vmax", "result", &vmax, 255, OnChange_vmax, &data);
    cv::waitKey(0);
    #else
    apple_hsv = data.src;
    cv::Mat hsv_red1, hsv_red2, hsv_green, hsv_result;
    cv::inRange(apple_hsv, cv::Scalar(0, data.smin, data.vmin), cv::Scalar(data.hmax, data.smax, data.vmax), hsv_red1);
    cv::inRange(apple_hsv, cv::Scalar(data.hmin, data.smin, data.vmin), cv::Scalar(180, data.smax, data.vmax), hsv_red2);
    hsv_result = hsv_red1 | hsv_red2;
    data.dir = hsv_result;
    #endif

    return data.dir;
}

}

namespace Solver{
    bool cmp(std::pair <std::vector<cv::Point>, cv::Point> &tmp1, std::pair <std::vector<cv::Point>, cv::Point> &tmp2){
    return tmp1.second.x < tmp2.second.x;
}

bool cmp2(cv::Point tmp1, cv::Point tmp2){
    return tmp1.x < tmp2.x;
}

int sign(int x){
    return x > 0 ? 1 : -1;
}

bool cmpPoints(cv::Point2f tmp1, cv::Point2f tmp2){
    if(tmp1.x == tmp2.x) return tmp1.y < tmp2.y;
    return tmp1.x < tmp2.x;
}

bool cmpRects(cv::RotatedRect &tmp1, cv::RotatedRect &tmp2){
    return tmp1.center.x < tmp2.center.x;
}

void solve(){
    // cv::cvtColor(apple_src, apple, cv::COLOR_BGR2RGB);
    #ifndef HSV
    cv::Mat apple_preres = RGBChannels::threshBinary(apple_src);
    #else
    cv::Mat apple_preres = HSVColors::threshBinary(apple_src);
    #endif
    cv::medianBlur(apple_preres, apple_preres, 3);
    // showImage("apple_preres", apple_preres);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierachy;
    cv::findContours(apple_preres, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::Mat drawer = cv::Mat::zeros(cv::Size(apple_src.cols, apple_src.rows), CV_8UC3);
    cv::Mat drawerPoly = cv::Mat::zeros(cv::Size(apple_src.cols, apple_src.rows), CV_8UC3);
    cv::Mat drawerDetect = cv::Mat::zeros(cv::Size(apple_src.cols, apple_src.rows), CV_8UC3);
    std::vector<std::vector<cv::Point> > contoursPoly(contours.size());
    std::vector<std::pair <std::vector<cv::Point>, cv::Point> > lights;
    std::vector<cv::RotatedRect> rotated_rects;
    for(int i = 0; i < contours.size(); i++){
        #ifndef DEBUG
        if(cv::contourArea(contours[i]) > 300 * 4) continue;
        if(cv::contourArea(contours[i]) < 5 * 4) continue;
        if(cv::arcLength(contours[i], true) > 80 * 2) continue;
        #else
        #ifndef HSV
        if(cv::contourArea(contours[i]) > 300) continue;
        if(cv::contourArea(contours[i]) < 5) continue;
        if(cv::arcLength(contours[i], true) > 80) continue;
        #else
        if(cv::contourArea(contours[i]) > 250) continue;
        if(cv::contourArea(contours[i]) < 19) continue;
        if(cv::arcLength(contours[i], true) > 80) continue;
        #endif
        #endif
        int minn = 0x3f3f3f3f;
        for(auto p: contours[i]) minn = std::min(minn, p.y);
        if(minn < apple_src.rows / 2 - 30) continue;
        // cv::drawContours(drawer, contours, i, cv::Scalar(255, 255, 255), 1);
        // if(cv::arcLength(contoursPoly[i], true) < 10 * 2) continue;
        cv::Rect rect = cv::boundingRect(contours[i]);
        if(rect.width * 1.3 > rect.height) continue;  
        cv::RotatedRect rotated_rect = cv::minAreaRect(contours[i]);
        if(rotated_rect.size.area() < 15) continue;
        #ifdef ROTATED
        cv::Point2f pots[4];
        rotated_rect.points(pots);
        for(int j = 0; j < 4; j++){
            cv::line(drawer, pots[j], pots[(j + 1) % 4], cv::Scalar(255, 255, 255));
            // std::cout << pots[j] << std::endl;
        }
        // showImage("drawer", drawer);
        rotated_rects.push_back(rotated_rect);
        #else
        cv::approxPolyDP(contours[i], contoursPoly[i], 10 * 2, true);
        cv::drawContours(drawerPoly, contoursPoly, i, cv::Scalar(0, 0, 255), 1);
        showImage("approxPoly", drawerPoly);
        cv::Point center;
        center.x = 0; center.y = 0;
        for(auto p: contoursPoly[i]) center.x += p.x, center.y += p.y;
        center.x /= contoursPoly[i].size(); center.y /= contoursPoly[i].size();
        lights.push_back(std::make_pair(contoursPoly[i], center));
        #endif
    }
    #ifndef ROTATED
    std::sort(lights.begin(), lights.end(), cmp);
    for(int i = 1; i < lights.size(); i++){
        std::vector<cv::Point> leftline = lights[i - 1].first;
        cv::Point leftcenter = lights[i - 1].second;
        std::vector<cv::Point> rightline = lights[i].first;
        cv::Point rightcenter = lights[i].second;
        if(abs(leftcenter.y - rightcenter.y) < eps){
            // std::cout << leftline.size() << std::endl;
            std::sort(leftline.begin(), leftline.end(), cmp2);
            std::sort(rightline.begin(), rightline.end(), cmp2);
            float slopel = (1.0 * (leftline.back().y - leftcenter.y) / (leftline.back().x - leftcenter.x) + \
                            1.0 * (leftcenter.y - leftline.front().y) / (leftcenter.x - leftline.front().x)) / 2.0;
            float sloper = (1.0 * (rightline.back().y - rightcenter.y) / (rightline.back().x - rightcenter.x) + \
                            1.0 * (rightcenter.y - rightline.front().y) / (rightcenter.x - rightline.front().x)) / 2.0;
            // std::cout << slopel << ' ' << sloper << std::endl;
            float lengthl = cv::arcLength(leftline, true) / 2.2;
            float lengthr = cv::arcLength(rightline, true) / 2.2;
            if(std::isinf(slopel)) slopel = sign(slopel) * (1e4 + 7);
            if(std::isinf(sloper)) sloper = sign(sloper) * (1e4 + 7);
            // cv::circle(drawer, leftcenter, 3, cv::Scalar(0, 255, 0), -1);
            // cv::circle(drawer, leftline.front(), 3, cv::Scalar(0, 255, 0), -1);
            // cv::circle(drawer, leftline.back(), 3, cv::Scalar(0, 255, 0), -1);
            // cv::circle(drawer, rightcenter, 3, cv::Scalar(0, 255, 0), -1);
            // std::cout << lengthl << ' ' << lengthr << std::endl;
            cv::Point2f pots[4];
            pots[0] = (cv::Point2f){leftcenter.x - lengthl * 1.0 / sqrt(1.0 + slopel * slopel), leftcenter.y - lengthl * slopel / sqrt(1.0 + slopel * slopel)};
            pots[1] = (cv::Point2f){rightcenter.x - lengthr * 1.0 / sqrt(1.0 + sloper * sloper), rightcenter.y - lengthr * sloper / sqrt(1.0 + sloper * sloper)};
            pots[2] = (cv::Point2f){rightcenter.x + lengthr * 1.0 / sqrt(1.0 + sloper * sloper), rightcenter.y + lengthr * sloper / sqrt(1.0 + sloper * sloper)};
            pots[3] = (cv::Point2f){leftcenter.x + lengthl * 1.0 / sqrt(1.0 + slopel * slopel), leftcenter.y + lengthl * slopel / sqrt(1.0 + slopel * slopel)};
            if(pots[1].y < pots[2].y && sign(sloper) != sign(slopel)) swap(pots[1], pots[2]);
            // for(int j = 0; j < 4; j++)
            //     std::cout << pots[j] << std::endl;
            for(int j = 0; j < 4; j++){
                cv::line(drawerDetect, pots[j], pots[(j + 1) % 4], cv::Scalar(255, 255, 255));
            }
            // showImage("armor", drawer);
            result = apple_src.clone();
            drawerDetect.copyTo(result, drawerDetect);
            
        }
    }
    #else
    std::sort(rotated_rects.begin(), rotated_rects.end(), cmpRects);
    for(int i = 1; i < rotated_rects.size(); i++){
        cv::RotatedRect leftrect = rotated_rects[i - 1];
        cv::RotatedRect rightrect = rotated_rects[i];
        cv::Point2f leftcenter, rightcenter;
        leftcenter = leftrect.center;
        rightcenter = rightrect.center;
        if(std::abs(leftcenter.y - rightcenter.y) < eps){
            std::vector<cv::Point2f> leftpots, rightpots;
            leftrect.points(leftpots); rightrect.points(rightpots);
            std::sort(leftpots.begin(), leftpots.end(), cmpPoints);
            std::sort(rightpots.begin(), rightpots.end(), cmpPoints);
            cv::Point2f leftpos, rightpos;
            leftpos = (leftpots[2] + leftpots[3]) / 2;
            rightpos = (rightpots[0] + rightpots[1]) / 2;
            float slopel = 1.0 * (leftpots[2].y - leftpots[3].y) / (leftpots[2].x - leftpots[3].x);
            float sloper = 1.0 * (rightpots[0].y - rightpots[1].y) / (rightpots[0].x - rightpots[1].x);
            if(std::isinf(slopel)) slopel = sign(slopel) * (1e4 + 7);
            if(std::isinf(sloper)) sloper = sign(sloper) * (1e4 + 7);
            // std::cout << leftpots << std::endl;
            // std::cout << rightpots << std::endl;
            std::vector<cv::Point2f> pots(4);
            float lengthl = sqrt((leftpots[2].y - leftpots[3].y) * (leftpots[2].y - leftpots[3].y) + (leftpots[2].x - leftpots[3].x) * (leftpots[2].x - leftpots[3].x));
            float lengthr = sqrt((rightpots[0].y - rightpots[1].y) * (rightpots[0].y - rightpots[1].y) + (rightpots[0].x - rightpots[1].x) * (rightpots[0].x - rightpots[1].x));
            // std::cout << lengthl << ' ' << lengthr << std::endl;
            pots[0] = (cv::Point2f){leftpos.x - lengthl * 1.0 / sqrt(1.0 + slopel * slopel), leftpos.y - lengthl * slopel / sqrt(1.0 + slopel * slopel)};
            pots[1] = (cv::Point2f){rightpos.x - lengthr * 1.0 / sqrt(1.0 + sloper * sloper), rightpos.y - lengthr * sloper / sqrt(1.0 + sloper * sloper)};
            pots[2] = (cv::Point2f){rightpos.x + lengthr * 1.0 / sqrt(1.0 + sloper * sloper), rightpos.y + lengthr * sloper / sqrt(1.0 + sloper * sloper)};
            pots[3] = (cv::Point2f){leftpos.x + lengthl * 1.0 / sqrt(1.0 + slopel * slopel), leftpos.y + lengthl * slopel / sqrt(1.0 + slopel * slopel)};
            // if(pots[0].y < pots[1].y && pots[2].y < pots[3].y && sign(sloper) != sign(slopel)) swap(pots[2], pots[3]);
            std::sort(pots.begin(), pots.end(), cmpPoints);
            if(sign(slopel) < 0 && sign(sloper) > 0){

            }else if(sign(slopel) < 0 && sign(sloper) < 0){
                swap(pots[2], pots[3]);
            }else if(sign(slopel) > 0 && sign(sloper) > 0){
                swap(pots[2], pots[3]);
            }else{
            }
            // for(int j = 0; j < 4; j++)
            //     std::cout << pots[j] << std::endl;
            for(int j = 0; j < 4; j++){
                cv::line(drawerDetect, pots[j], pots[(j + 1) % 4], cv::Scalar(255, 255, 255));
            }
            // showImage("armor", drawerDetect);
            result = apple_src.clone();
            drawerDetect.copyTo(result, drawerDetect);
        }
    }
    #endif
    // showImage("armoresult", result);
    cv::destroyAllWindows();
}
}

int main(){
    cv::VideoCapture capture("../armor.mp4");
    capture >> apple_src;
    cv::VideoWriter writer("../armor_result.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), capture.get(cv::CAP_PROP_FPS), cv::Size(apple_src.cols, apple_src.rows), true);
    while(true){
        #ifdef DEBUG
        cv::resize(apple_src, apple_src, cv::Size(apple_src.cols / 2, apple_src.rows / 2));
        // showImage("apple_src", apple_src);
        #endif

        Solver::solve();
        // HSVColors::solve();
        #ifndef DEBUG
        cv::resize(result, result, cv::Size(apple_src.cols, apple_src.rows));
        writer << result;
        #endif
        capture >> apple_src;
        if(apple_src.empty()) break;
    }
    return 0;
}
