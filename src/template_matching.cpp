/**
 * @brief  This file is part of Template Matching Implementation using OpenCV C++ Library
 * @file  template_matching.cpp
 * @date  3/12/19 -- Revised 18/01/2021
 * @copyright
 * @author  Arslan Ali  <marslanali800@gmail.com>
 */

#include <armadillo>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief Template function for converting openCV Mat to Armadillo Mat
 * @tparam T
 * @param opencv_mat
 * @return arma::Mat<T>
 */
template <typename T>
arma::Mat<T> opencv_to_arma2(const cv::Mat& opencv_mat)
{
  arma::Mat<T> arma_mat(opencv_mat.rows, opencv_mat.cols);

  for (size_t i = 0; i < opencv_mat.rows; ++i)
  {
    for (size_t j = 0; j < opencv_mat.cols; ++j)
    {
      arma_mat(i, j) = (T)opencv_mat.at<uchar>(i, j);
    }
  }

  return arma_mat;
}

/**
 * main function
 * @param argc  Total number of command line arguments
 * @param argv  Pointer to store command line arguments
 * @return 0
 */
int main(int argc, char** agrv)
{
  cv::Mat input_image, input_filter;
  cv::Mat gray_scale_image(input_image.rows, input_image.cols, CV_8UC1);
  cv::Mat gray_scale_filter(input_filter.rows, input_filter.cols, CV_8UC1);

  input_image = cv::imread("../img/clutter.png");
  input_filter = cv::imread("../img/waldo.png");

  cv::imshow("Input Image", input_image);
  cv::imshow("Input Template", input_filter);

  if (input_image.empty() || input_filter.empty())
  {
    std::cout << "Can't find image" << std::endl;
    return -1;
  }

  cv::cvtColor(input_image, gray_scale_image, cv::COLOR_BGR2GRAY);
  cv::cvtColor(input_filter, gray_scale_filter, cv::COLOR_BGR2GRAY);

  cv::imshow("GrayScale Image", gray_scale_image);
  cv::imshow("GrayScale Filter", gray_scale_filter);

  cv::Mat final_image(
      gray_scale_image.rows - gray_scale_filter.rows + 1, gray_scale_image.cols - gray_scale_filter.cols + 1, CV_8UC1);
  cv::matchTemplate(gray_scale_image, gray_scale_filter, final_image, 3);
  cv::normalize(final_image, final_image, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  /// Localizing the best match with minMaxLoc
  double min_val, max_val;
  cv::Point min_loc, max_loc, match_loc;

  minMaxLoc(final_image, &min_val, &max_val, &min_loc, &max_loc, cv::Mat());

  // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
  match_loc = max_loc;
  std::cout << match_loc << std::endl;

  /// Show what you got
  cv::rectangle(input_image,
                match_loc,
                cv::Point(match_loc.x + gray_scale_filter.cols, match_loc.y + gray_scale_filter.rows),
                cv::Scalar::all(0),
                2,
                8,
                0);
  cv::rectangle(final_image,
                match_loc,
                cv::Point(match_loc.x + gray_scale_filter.cols, match_loc.y + gray_scale_filter.rows),
                cv::Scalar::all(0),
                2,
                8,
                0);

  cv::imshow("Final Image", input_image);

  cv::imwrite("../img/Final-Image.png", input_image);

  // Convert OpenCV Mat to Armadillo Mat<double>
  arma::Mat<double> arma_gray_scale_image(gray_scale_image.rows, gray_scale_image.cols);
  arma::Mat<double> arma_gray_scale_filter(gray_scale_filter.rows, gray_scale_filter.cols);

  // arma_gray_scale_filter = opencv_to_arma2<double>(gray_scale_filter);

  for (size_t i = 0; i < gray_scale_image.rows; i++)
  {
    for (size_t j = 0; j < gray_scale_image.cols; j++)
    {
      // cv::Vec3b pixelColor = testData.at<cv::Vec3b>(i);
      arma_gray_scale_image(i, j) = (double)gray_scale_image.at<uchar>(i, j);
    }
  }

  for (size_t i = 0; i < gray_scale_filter.rows; i++)
  {
    for (size_t j = 0; j < gray_scale_filter.cols; j++)
    {
      arma_gray_scale_filter(i, j) = (double)gray_scale_filter.at<uchar>(i, j);
    }
  }

  // arma_gray_scale_image += 50;
  // arma_gray_scale_filter += 50;

  double value = arma::sqrt(arma::sum(arma::sum(arma::pow(arma_gray_scale_filter, 2), 0), 1)).eval()(0, 0);
  arma_gray_scale_filter = arma_gray_scale_filter / value;

  // convert Armadillo Mat <double> to openCV Mat
  cv::Mat image(arma_gray_scale_image.n_rows, arma_gray_scale_image.n_cols, CV_8UC1);
  cv::Mat filter(arma_gray_scale_filter.n_rows, arma_gray_scale_filter.n_cols, CV_8UC1);

  // final_image = arma_to_opencv<uchar>(arma_mat_filter);

  for (size_t i = 0; i < gray_scale_image.rows; ++i)
  {
    for (size_t j = 0; j < gray_scale_image.cols; ++j)
    {
      image.at<uchar>(i, j) = (uchar)arma_gray_scale_image(i, j);
    }
  }

  for (size_t i = 0; i < gray_scale_filter.rows; ++i)
  {
    for (size_t j = 0; j < gray_scale_filter.cols; ++j)
    {
      filter.at<uchar>(i, j) = (uchar)arma_gray_scale_filter(i, j);
    }
  }

  // imshow("Final Image 2 ", image);
  // imshow("Final Filter", filter);

  cv::waitKey();
  return 0;
}
