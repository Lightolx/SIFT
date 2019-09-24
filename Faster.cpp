//
// Created by lightol on 2019/9/29.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include <eigen3/Eigen/Eigen>

// 直接数25领域内两种点的数量的差异，如果一样多说明是在边缘上，如果一种远远多于另一种说明是角点


using std::cout;
using std::endl;

int main() {
    cv::Mat image0 = cv::imread("/data/hz/second_batch/0001678.png", CV_LOAD_IMAGE_UNCHANGED);
    cv::Mat image;
    cv::cvtColor(image0, image, CV_BGR2GRAY);
    int r = 5;
    int rows = image.rows;
    int cols = image.cols;

    /*
    int num = 0;
    for (int m = -4; m <= 4; ++m) {
        for (int n = -4; n <= 4; ++n) {
            if (sqrt(m*m + n*n) < 5 && sqrt(m*m + n*n) >= 4) {
                cout << "[" << m << ", " << n << "]\n";
                num++;
            }
        }
    }
    cout << "num = " << num << endl;
    cv::imshow("image0", image);
    cv::waitKey();
     */

    // Step1: 先提轮廓
    /*
    std::vector<std::vector<cv::Point> > vvContourPts;
    std::vector<cv::Vec4i> vHierarchy;
    cv::findContours(image, vvContourPts, vHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    std::vector<cv::Point> vCorners;
    for (const auto &vContourPts : vvContourPts) {
        for (const auto &pt : vContourPts) {
            cv::circle(image0, pt, 1, cv::Scalar(0, 0, 255));

//            int numBackground = 0;
//            int numFrontground = 0;
//            for (int m = -4; m <= 4; ++m) {
//                for (int n = -4; n <= 4; ++n) {
//                    double dist = sqrt(m*m + n*n);
//                    if (dist < 5 && dist >= 4) {
//                        if (image.at<uchar>(pt.x+m, pt.y+n) == 0) {
//                            numBackground++;
//                        } else {
//                            numFrontground++;
//                        }
//                    }
//                }
//            }
//
//            cout << "numBack = " << numBackground << endl;
//            cout << "numFront = " << numFrontground << endl;
//            if (std::abs(numBackground - numFrontground) > 6) {
//                vCorners.emplace_back(pt);
//            }
        }
    }

    cv::imshow("image0", image0);
    cv::waitKey();
    */

    std::vector<cv::Point> vCorners;
    for (int i = r; i < rows-r; ++i) {
        for (int j = r; j < cols-r; ++j) {

            if (image.at<uchar>(i, j) == 0) {  // 背景中一个角点也不提
                continue;
            }

            int numBackground = 0;
            int numFrontground = 0;
            for (int m = -4; m <= 4; ++m) {
                for (int n = -4; n <= 4; ++n) {
                    double dist = sqrt(m*m + n*n);
                    if (dist < 5 && dist >= 4) {
                        if (image.at<uchar>(i+m, j+n) == 0) {
                            numBackground++;
                        } else {
                            numFrontground++;
                        }
                    }
                }
            }

            cout << "numBack = " << numBackground << endl;
            cout << "numFront = " << numFrontground << endl;
            if (std::abs(numBackground - numFrontground) > 6 && std::abs(numBackground - numFrontground) < 10) {
                vCorners.emplace_back(j, i);
            }
        }
    }

    for (const auto &pt : vCorners) {
        cv::circle(image0, pt, 1, cv::Scalar(0, 0, 255));
    }

    cv::imshow("image0", image0);
    cv::waitKey();

};