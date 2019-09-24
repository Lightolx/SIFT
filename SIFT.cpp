//
// Created by lightol on 2019/9/27.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

using std::cout;
using std::endl;

int main() {
    cv::Mat image0 = cv::imread("road.png", CV_LOAD_IMAGE_UNCHANGED);
    cv::Mat imageX = image0.clone();
    for (int i = 0; i < imageX.rows; ++i) {
        for (int j = 0; j < imageX.cols; ++j) {
            if (imageX.at<cv::Vec3b>(i, j) == cv::Vec3b(100, 100, 150) ||
                imageX.at<cv::Vec3b>(i, j) == cv::Vec3b(255,   0,   0) ||
                imageX.at<cv::Vec3b>(i, j) == cv::Vec3b(  0, 255,   0) ||
                imageX.at<cv::Vec3b>(i, j) == cv::Vec3b(225,  55, 100) ||
                imageX.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 255,   255)) {
                continue;
            }

            imageX.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
        }
    }
    cv::Mat image1;
    cv::resize(imageX, image1, cv::Size(), 1, 1);
//    cv::resize(imageX, image1, cv::Size(), 0.25, 0.25);
    cv::Mat image = image1;
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            if (image.at<cv::Vec3b>(i, j) == cv::Vec3b(100, 100, 150) ||
                image.at<cv::Vec3b>(i, j) == cv::Vec3b(255, 0, 0) ||
                image.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 255, 0) ||
                image.at<cv::Vec3b>(i, j) == cv::Vec3b(225, 55, 100) ||
                image.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 255, 255)) {
                continue;
            }

            image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
        }
    }

    std::vector<cv::KeyPoint> vKeypoints;

    // SIFT
    {
//        auto detector = cv::xfeatures2d::SIFT::create(150);
//        detector->detect(image, vKeypoints);
    }

    // SURF
    {
//        auto detector = cv::xfeatures2d::SURF::create(100);
//        detector->detect(image, vKeypoints);
    }

    // FAST
    {
//        auto detector = cv::FastFeatureDetector::create();
//        detector->detect(image, vKeypoints);
    }

    // Harris
    {
        cv::Mat feature_image(image.size(), CV_32FC1);
        cv::cornerHarris(image, feature_image, 2, 3, 0.05);
//        auto detector = cv::xfeatures2d::HarrisLaplaceFeatureDetector::create();
//        detector->detect(image, vKeypoints);
        cv::imwrite("fast.png", feature_image);
        cv::imshow("feature_map_sift", feature_image);
        cv::waitKey();
    }

    cv::Mat feature_image;
    cv::drawKeypoints(image, vKeypoints, feature_image);
//    cv::namedWindow("feature_map_sift", cv::WINDOW_NORMAL);
    cv::imwrite("fast.png", feature_image);
    cv::imshow("feature_map_sift", feature_image);
    cv::waitKey();

    return 0;
}