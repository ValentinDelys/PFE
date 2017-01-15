#include "preprocessing.h"
#include "gpxerrors.h"
#include "math.h"
#include <opencv.hpp>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <qmath.h>

PreProcessing::PreProcessing()
{
    this->filename = QFileDialog::getOpenFileName(this,
                                                  QString("Open Image"),
                                                  QDir::currentPath(),
                                                  QString("Files (*.jpg)")).toStdString();
}

void PreProcessing::houghCircles(){
    cv::Mat src, src_gray;

    src = imread(this->filename, cv::IMREAD_COLOR);

    if(!src.empty()){
        cv::imshow("Src", src);

        cv::Mat drawing;
        src.copyTo(drawing);

        cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

        qDebug() << cv::countNonZero(src_gray);

        cv::blur(src_gray, src_gray, cv::Size(9, 9));

        cv::imshow("Scr Blur", src_gray);

        std::vector<cv::Vec3f> circles;

        cv::HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 1, 35, 30, 5, 15, 17);

        cv::Mat result (src.cols, src.rows, CV_8UC1, cv::Scalar(0));

        for(size_t i = 0; i < circles.size(); i++){
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

            int radius = cvRound(circles[i][2]) + 2;

            // Center
            cv::circle(drawing, center, 3, cv::Scalar(0, 255 ,0), -1, 8, 0);

            // Circle
            cv::circle(drawing, center, radius, cv::Scalar(0, 0, 255), 1, 8, 0);

            cv::Point p1((center.x - radius), (center.y - radius));
            cv::Point p2((center.x + radius), (center.y + radius));

            if(center.x - radius < 0) p1.x = 0;
            if(center.y - radius < 0) p1.y = 0;
            if(center.x + radius > src.cols) p2.x = src.cols;
            if(center.y + radius > src.rows) p2.y = src.rows;

            // Rectangle
            cv::rectangle(drawing, p1, p2, cv::Scalar(255, 0, 0), 1, 8, 0);

            cv::Mat sub_result(src.colRange(p1.x, p2.x).rowRange(p1.y, p2.y));
            cv::cvtColor(sub_result, sub_result, cv::COLOR_BGR2GRAY);

            sub_result.copyTo(result(cv::Rect(p1.x, p1.y, sub_result.cols, sub_result.rows)));
            cv::imshow("drawing", result);
        }

        cv::imshow("Scr Hough", drawing);
        cv::waitKey(0);
    }
}

void PreProcessing::fitEllipse(){
    cv::Mat src, src_gray;

    int thresh = 100;
    int max_thresh = 255;
    size_t num_of_ellipses = 24;
    cv::RNG rng(12345);

    src = imread(this->filename, cv::IMREAD_COLOR);

    if(!src.empty()){
        cv::imshow("Src", src);

        cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

        cv::blur(src_gray, src_gray, cv::Size(3, 3));

        cv::imshow("Blur", src_gray);

        std::vector<cv::Rect> prev_Rect;
        std::vector<cv::RotatedRect> prev_Ellipse;

        cv::Mat result (src.cols, src.rows, CV_8UC3, cv::Scalar(0));

        do{
            /// Detect edges using Threshold
            cv::Mat threshold_output;
            std::vector<cv::Vec4i> hierarchy;
            cv::threshold( src_gray, threshold_output, thresh, max_thresh, cv::THRESH_BINARY);
            cv::imshow("Threshold", threshold_output);

            /// Find contours
            std::vector<std::vector<cv::Point> > contours;
            cv::findContours( threshold_output, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

            /// Find the rotated rectangles and ellipses for each contour
            std::vector<cv::Rect> Rect;
            std::vector<cv::RotatedRect> Ellipse;

            for(size_t i = 0; i < contours.size(); i++){

                double area = cv::contourArea(contours[i]);

                if (area >= 150 && area < 1000){
                    Rect.push_back(cv::boundingRect(cv::Mat(contours[i])));

                    if(contours[i].size() > 5){
                        Ellipse.push_back(cv::fitEllipse(cv::Mat(contours[i])));
                    }
                }
            }

            if((Ellipse.size() >= num_of_ellipses) || (Ellipse.size() < prev_Ellipse.size()) || thresh == 1){

                if(Ellipse.size() < prev_Ellipse.size()){
                    Ellipse.swap(prev_Ellipse);
                    Rect.swap(prev_Rect);
                }

                /// Draw contours + rotated rects + ellipses
                cv::Mat drawing = src.clone();
                for( size_t i = 0; i < Ellipse.size(); i++)
                {
                    cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
                    // contour
//                    cv::drawContours(drawing, contours, (int)i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
                    // ellipse
                    cv::ellipse(drawing, Ellipse[i], color, 1, 8);
                    // bounding rectangle
                    cv::rectangle(drawing, Rect[i], color, 1, 8);

                    src(Rect[i]).copyTo(result(Rect[i]));
                }

                cv::namedWindow("Result", cv::WINDOW_AUTOSIZE);
                cv::imshow("Result", result);

                /// Show in a window
                cv::namedWindow("Contours", cv::WINDOW_AUTOSIZE);
                cv::imshow("Contours", drawing);
                qDebug() << "Break, thresh = " << thresh;
                qDebug() << Ellipse.size();
                break;
            } else {
                prev_Ellipse.swap(Ellipse);
                prev_Rect.swap(Rect);
                thresh--;
            }
        } while(thresh != 0);
    }
}

cv::Mat img; cv::Mat templ; cv::Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";
int match_method;
int max_Trackbar = 5;

void  MatchingMethod( int, void* );

int PreProcessing::matchTemplate(){

    img = cv::imread(this->filename, cv::IMREAD_COLOR);

    std::string template_path = QFileDialog::getOpenFileName( this,
                                                  QString("Open Image"),
                                                  QDir::currentPath(),
                                                  QString("Files (*.jpg)")).toStdString();

    templ = imread(template_path, cv::IMREAD_COLOR);

    cv::namedWindow( image_window, cv::WINDOW_AUTOSIZE );
    cv::namedWindow( result_window, cv::WINDOW_AUTOSIZE );
    const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
    cv::createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
    MatchingMethod( 0, 0 );
    cv::waitKey(0);

    return 0;
}

void MatchingMethod( int, void* )
{
  cv::Mat img_display;
  img.copyTo( img_display );

  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  result.create( result_rows, result_cols, CV_32FC1 );

  cv::matchTemplate( img, templ, result, match_method );
  cv::normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

  double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
  cv::Point matchLoc;

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

  if( match_method  == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED ){
      matchLoc = minLoc;
  } else {
      matchLoc = maxLoc;
  }

  cv::rectangle( img_display, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar(0, 255, 0), 2, 8, 0 );
  cv::rectangle( result, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 );
  cv::imshow( image_window, img_display );
  cv::imshow( result_window, result );

  return;
}
