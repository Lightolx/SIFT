#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include <fstream>

using std::cout;
using std::endl;

int main() {
    std::vector<cv::String> pngFilenames;
    cv::String filepath = cv::String("/data/STCC_HZ/2019-10-08-11-07-43/pics2/*.png");
    cv::glob(filepath, pngFilenames);

    int numImage = 0;
    for (const auto &filename : pngFilenames) {
//        if (filename != "/data/STCC_HZ/2019-10-08-11-07-43/pics2/749.png") {
//            continue;
//        }

        {
            cout << filename << endl;
            cv::Mat image0 = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);
            std::string strID = filename.substr(filename.find_last_of('/') + 1, 3);
            std::ifstream fin("/data/STCC_HZ/2019-10-08-11-07-43/txts3/" + strID + ".txt");
            std::string ptline;

            std::vector<cv::Point2f> vCorners;
            cv::Point2f pt;
            while (getline(fin, ptline)) {
                std::stringstream ss(ptline);

                ss >> pt.x >> pt.y;
                vCorners.push_back(pt);
            }


            for (const auto &pt : vCorners) {
//            cv::circle(image_gray, pt, 1, cv::Scalar(255));
                cv::circle(image0, pt, 3, cv::Scalar(255, 0, 0));
            }

            cv::namedWindow("re", cv::WINDOW_NORMAL);
            cv::imshow("re", image0);
            cv::waitKey();
            continue;
        }
        cout << filename << endl;
        cv::Mat image0 = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);
        cv::Mat image(image0.size(), CV_8UC1);
        image.setTo(0);
        for (int i = 0; i < image0.rows; ++i) {
            for (int j = 0; j < image0.cols; ++j) {
                if (image0.at<cv::Vec3b>(i,j) == cv::Vec3b(0, 0, 255)) {
                    image.at<uchar>(i, j) = 255;
                }
            }
        }

        if (cv::countNonZero(image) == 0) {
            continue;
        }

//        cv::namedWindow("image", cv::WINDOW_NORMAL);
//        cv::imshow("image", image);

        // Step1: 对整幅图像做闭运算，消除里面的孔洞
        cv::Mat image1;  // 对canny的结果也膨胀一下，免得从一些孔的地方漏出去
        cv::Mat element1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(image, image1, cv::MORPH_CLOSE, element1);

//        cv::namedWindow("image1", cv::WINDOW_NORMAL);
//        cv::imshow("image1", image1);
//        cv::waitKey();
//        continue;

        cv::Mat image_gray = image1;
        // GFTT
        std::vector<cv::Point2f> vKeypoints;
        int max_corners = 50;
        double quality_level = 0.005;
        double min_distance = 15;
        int block_size = 5;
        bool use_harris = false;
        double k = 0.04;
        cv::goodFeaturesToTrack(image_gray,
                                vKeypoints,
                                max_corners,
                                quality_level,
                                min_distance,
                                cv::Mat(),
                                block_size,
                                use_harris,
                                k);

        //指定亚像素计算迭代标注
        cv::TermCriteria criteria = cv::TermCriteria(
                cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
                40,
                0.01);

        //亚像素检测
        cv::cornerSubPix(image_gray, vKeypoints, cv::Size(5, 5), cv::Size(-1, -1), criteria);

//        for (int i = 0; i < vKeypoints.size(); ++i) {
//            cv::circle(image0, vKeypoints[i], 3, cv::Scalar(255, 0, 0));
//        }

//        std::vector<cv::Point> vCorners;
//        cv::imshow("gftt", image0);
//        cv::waitKey(100);
//        continue;

        std::vector<cv::Point2f> vCorners;
        for (const auto &pt : vKeypoints) {
            int numBackground = 0;
            int numFrontground = 0;
//            if (std::fabs(pt.x - 1143) < 1 && std::fabs(pt.y - 450) < 1) {
            // cout << "pt is " << pt << endl;
            // cout << "pt.value is " << image_gray.at<cv::Vec3b>(pt.y, pt.x) << endl;
            for (int m = -5; m <= 5; ++m) {
                for (int n = -5; n <= 5; ++n) {
                    double dist = sqrt(m*m + n*n);
                    if (dist < 6 && dist >= 5) {
//                         cout << m << ", " << n << ", " << endl;
                        // cout << "pt.y+m = " << std::round(pt.y + m)  << ", pt.x+n = " << std::round(pt.x+n) << endl;
                        // cout << "value = " << image_gray.at<cv::Vec3b>(std::round(pt.y + m), std::round(pt.x+n)) << endl << endl;
                        int u = pt.x + n;
                        int v = pt.y + m;
                        if (u < 0 || u > image.cols || v < 0 || v > image.rows) {
                            continue;
                        }
                        // 这里没有写灰度等于0是因为有些边缘区域在RGB2GRAY转换的时候变成了灰度为5, 8等比较小的数值，这些过渡区域不太好判断到底是背景还是前景
                        if (image_gray.at<uchar>(std::round(pt.y + m), std::round(pt.x+n)) <= 20) {
                            numBackground++;
                        } else {

                            numFrontground++;
                        }
                    }
                }
            }

//            cout << endl;

            // cout << "pt is " << pt << endl;
//             cout << "numBack = " << numBackground << endl;
//             cout << "numFront = " << numFrontground << endl;
             if (numBackground + numFrontground < 40) {
                 continue;
             }
            if (std::abs(numBackground - numFrontground) >= 7) {
//            if (std::abs(numBackground - numFrontground) > 6 && std::abs(numBackground - numFrontground) < 10) {
                vCorners.emplace_back(pt);
            }
//            }
        }

        std::string strID = filename.substr(filename.find_last_of('/') + 1, 3);
        std::ofstream fout("/data/STCC_HZ/2019-10-08-11-07-43/txts/" + strID + ".txt");
        for (const auto &pt : vCorners) {
//            cv::circle(image_gray, pt, 1, cv::Scalar(255));
            cv::circle(image0, pt, 3, cv::Scalar(255, 0, 0));
            fout << pt.x << " " << pt.y << endl;
        }
        fout.close();

        cv::namedWindow("fast", cv::WINDOW_NORMAL);
        cv::imshow("fast", image0);
        cv::waitKey();

//        cv::namedWindow("gftt", cv::WINDOW_FULLSCREEN);
//        cv::imwrite("/data/hz/corners/image" + std::to_string(numImage++) + ".png", image);
//        cv::imshow("gftt", image);
//        char c = cv::waitKey(1500);
//        if (c == ' ') {
//            cv::waitKey();
//        }

//        cv::waitKey(1500);
    }

    return 0;
}