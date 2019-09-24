//
// Created by lightol on 2019/10/9.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>

#include <eigen3/Eigen/Eigen>

using std::cout;
using std::endl;

int main() {
    std::vector<cv::String> pngFilenames;
    cv::String filepath = cv::String("/data/hz/second_batch/*.png");
    cv::glob(filepath, pngFilenames);

    int numImage = 0;
    for (const auto &filename : pngFilenames) {
        // cout << filename << endl;
        cv::Mat image = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);
        cv::Mat image_gray;
        cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
        if (cv::countNonZero(image_gray) == 0) {
            continue;
        }
        // Canny提取边界
        cv::Mat image_edges;
        cv::Canny(image_gray, image_edges, 100, 200, 3);
        cv::imshow("canny", image_edges);
//        cv::waitKey();

        // Hough变换提取直线
//        cv::findContours();
//        cv::convexHull();
        std::vector<cv::Vec4f> vLines;
        cv::HoughLinesP(image_edges, vLines, 1, CV_PI/180, 20, 15);
        cout << "line size = " << vLines.size() << endl;
//        for (const auto &line : vLines) {
//            cv::line(image, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(0, 0, 255));
//        }

//        cv::imshow("hough", image);
//        cv::waitKey();
        // 计算凸包
        // 首先对image_edges进行聚类
        std::vector<Eigen::Vector2d> vPts;
        for (int i = 0; i < image_edges.rows; ++i) {
            for (int j = 0; j < image_edges.cols; ++j) {
                if (image_edges.at<uchar>(i, j) > 0) {
                    vPts.emplace_back(i, j);
                }
            }
        }
        std::vector<std::vector<int> > vClusters;
        {
            int numPts = vPts.size();
            std::vector<bool> vbProcessed(numPts, false);  // 每个元素存储对应索引的点是否已经被处理过

            for (int i = 0; i < numPts; ++i) {
                if (vbProcessed[i]) {  // 如果这个点q已经被加入到了之前某个点p的临域内，则这次就不处理了
                    continue;
                }

                Eigen::Vector2d q = vPts[i];
                std::vector<int> Q;  // 该序列存储以点q为种子点的cluster的所有点的id
                int slow_idx = 0;
                Q.push_back(i);
                vbProcessed[i] = true;  // 每当一个点被push_back到cluster里面，它就被判定为已经处理过了

                // 从头至尾遍历这个vector，有新元素加进来的话就往后push_back
                while (slow_idx < Q.size())  // 快慢指针，当慢指针追上快指针时，说明快指针已经停下了
                {
                    std::vector<int> P;  // 该序列存储vPts中所有与点q的euclidean距离在thres以内的点
                    for (int j = 0; j < numPts; ++j) {
                        if (i == j) {  // 自己跟自己就不要比了，距离肯定是0，也不用push_back进来，否则死循环了
                            continue;
                        }

                        if (vbProcessed[j]) {
                            // 如果这个点p已经被处理过了，也就是它已经被归到某个cluster里面了，那么有两种可能：
                            // 1. p就是在以Q为种子点的cluster里，那么肯定就不能重复push_back了
                            // 2. p不在以Q为种子点的cluster里，那么p跟q的距离肯定大于thres，否则在构建p_cluster时肯定把q给包进来了
                            continue;
                        }

                        if ((vPts[Q[slow_idx]] - vPts[j]).norm() <= 2) {
                            Q.push_back(j);
                            vbProcessed[j] = true;
                        }
                    }

                    slow_idx++;
                }

                vClusters.push_back(Q);
            }
        }

        cv::Mat image_canny = image.clone();
        image_canny.setTo(cv::Scalar(0, 0, 0));
        for (const auto &ids : vClusters) {
            std::vector<cv::Point> vEdgePts;
            for (int id : ids) {
                Eigen::Vector2d pt = vPts[id];
                vEdgePts.emplace_back(pt.x(), pt.y());
            }

            Eigen::Vector3i rgb = Eigen::Vector3i::Random();
            int r = rgb[0] % 255;
            int g = rgb[1] % 255;
            int b = rgb[2] % 255;

            for (auto pt : vEdgePts) {
                image_canny.at<cv::Vec3b>(pt.x, pt.y) = cv::Vec3b(r, g, b);
            }

//            std::vector<cv::Point> vCorners;
//            cv::convexHull(vEdgePts, vCorners);
//            for (int i = 0; i < vCorners.size(); ++i) {
////                cv::line(image, vCorners[i], vCorners[i+1], cv::Scalar(0, 0, 255));
//                cv::circle(image, vCorners[i], 4, cv::Scalar(r, g, b));
//            }
        }
        cv::imshow("hull", image_canny);
        cv::waitKey();


        /*
        std::vector<cv::Point> vCorners;
//    cv::imshow("gftt", image);
//    cv::waitKey();
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
                        cout << m << ", " << n << ", " << endl;
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

            cout << endl;

            // cout << "pt is " << pt << endl;
            cout << "numBack = " << numBackground << endl;
            cout << "numFront = " << numFrontground << endl;
            if (numBackground + numFrontground < 40) {
                continue;
            }
            if (std::abs(numBackground - numFrontground) >= 6) {
//            if (std::abs(numBackground - numFrontground) > 6 && std::abs(numBackground - numFrontground) < 10) {
                vCorners.emplace_back(pt);
            }
//            }
        }

        for (const auto &pt : vCorners) {
//            cv::circle(image_gray, pt, 1, cv::Scalar(255));
            cv::circle(image, pt, 3, cv::Scalar(0, 0, 255));
        }

        cv::namedWindow("gftt", cv::WINDOW_FULLSCREEN);
        cv::imwrite("/data/hz/corners/image" + std::to_string(numImage++) + ".png", image);
        cv::imshow("gftt", image);
        char c = cv::waitKey(1500);
        if (c == ' ') {
            cv::waitKey();
        }

//        cv::waitKey(1500);
    }
    */
    }
    return 0;
}