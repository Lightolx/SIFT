//
// Created by lightol on 2019/9/29.
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

    cv::imshow("image0", image);
    cv::waitKey(2000);
    cv::destroyAllWindows();

    // Harris
    cv::Mat feature_image0(image.size(), CV_32FC1);
    cv::cornerHarris(image, feature_image0, 2, 3, 0.04);
    cout << feature_image0 << endl;
    cv::Mat feature_image = feature_image0.clone();
    cv::normalize(feature_image0, feature_image, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
//    cout << feature_image << endl;
    cv::Mat corner_image;
    cv::convertScaleAbs(feature_image, corner_image);

    for (int i = 0; i < feature_image.rows; ++i) {
        for (int j = 0; j < feature_image.cols; ++j) {
            if ((int)feature_image.at<float>(i, j) > 120) {
                cv::circle(corner_image, cv::Point(j, i), 5, cv::Scalar(255), 2, 8, 0);
            }
        }
    }

    cv::imshow("feature_map_sift", corner_image);
    cv::waitKey();

    return 0;
}