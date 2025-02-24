/*
 * How to read this file :
 * First : Utilities function (to get images from videos, calibrate camera...)
 * Second : main function : where all the intersting code is
 */
#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

#define REF_WIDTH 0.137
#define REF_HEIGHT 0.205

std::vector<cv::Point2f> xref;
std::vector<cv::Point3f> w_axis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Utils ////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mouseCallback(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {

        xref.emplace_back(x, y);
        std::cout << "Selected point : (" << x << ", " << y << ")" << std::endl;

        auto *image = static_cast<cv::Mat *>(userdata);
        cv::circle(*image, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1);
        cv::imshow("Image", *image);
    }
}

void saveImageFromVideosForCalibration(const std::string &src_path, const std::string &dst_path)
{
    std::vector<cv::String> videos;
    //cv::glob(R"(resources\videos\*.mov)", videos);
    cv::glob(src_path, videos);
    //std::string base_image_path = R"(resources\images\calibration)";
    for (int i = 0; i < videos.size(); i++)
    {
        cv::VideoCapture cap(videos.at(i));
        if (!cap.isOpened())
        {
            std::cerr << "Error while opening the video " << i << std::endl;
            return;
        }
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error while reading the frame " << i << std::endl;
            return;
        }
        std::string image_path = dst_path + R"(\)" + std::to_string(i) + ".jpg";
        std::cout << "path : " << image_path << std::endl;

        cv::imwrite(image_path, frame);
        std::cout << "Frame " << i << " extracted and saved : " << image_path << std::endl;
    }
}

void saveImageFromVideo(const std::string &src_path, const std::string &dst_path)
{
    cv::VideoCapture cap(src_path);
    if (!cap.isOpened())
    {
        std::cerr << "Error while opening the video " << std::endl;
        return;
    }
    cv::Mat frame;
    cap >> frame;
    if (frame.empty())
    {
        std::cerr << "Error while reading the frame " << std::endl;
        return;
    }
    std::cout << "path : " << dst_path << std::endl;
    cv::imwrite(dst_path, frame);

}

void extractFpsAndDimFromVideo(const std::string &videoPath, double & fps, int & videoWidth, int & videoHeight)
{
    cv::VideoCapture video(videoPath);
    fps = video.get(cv::CAP_PROP_FPS);
    videoWidth = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
    videoHeight = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));
    video.release();
}

bool extractImageFromVideo(const std::string &videoPath, const int frame_index, cv::Mat &extracted_frame)
{
    // Open the video file
    cv::VideoCapture video(videoPath);
    if (!video.isOpened())
    {
        std::cerr << "Error: Could not open the video file: " << videoPath << std::endl;
        return false;
    }

    // Check if the frame index is valid
    int total_frames = static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
    if (frame_index < 0 || frame_index >= total_frames)
    {
        std::cerr << "Error: Frame index " << frame_index << " is out of bounds. Total frames: " << total_frames
                  << std::endl;
        return false;
    }

    // Read the frame
    video.set(cv::CAP_PROP_POS_FRAMES, frame_index);
    if (!video.read(extracted_frame))
    {
        std::cerr << "Error: Could not read the frame at index " << frame_index << std::endl;
        return false;
    }

    // Verify that the frame was extracted successfully
    if (extracted_frame.empty())
    {
        std::cerr << "Error: Extracted frame is empty." << std::endl;
        return false;
    } else
    {
        std::cout << "Frame " << frame_index << " successfully extracted." << std::endl;
    }

    // Release the video (optional, as it auto-releases when out of scope)
    video.release();
    return true;
}

//Code found on https://learnopencv.com/camera-calibration-using-opencv/
void calibrateCamera(const int board_width, const int board_height, cv::Mat &cameraMatrix, cv::Mat &distCoeffs, cv::Mat &R, cv::Mat &T)
{
    // Storing the checkerboard size
    const int CHECKERBOARD[]{board_width, board_height};

    // Init of the different arrays
    std::vector<std::vector<cv::Point3f> > objpoints;
    std::vector<std::vector<cv::Point2f> > imgpoints;
    std::vector<cv::Point3f> objp;

    for (int i{0}; i < CHECKERBOARD[1]; i++)
    {
        for (int j{0}; j < CHECKERBOARD[0]; j++)
            objp.emplace_back(j, i, 0);
    }

    std::vector<cv::String> images;
    std::string calibration_path = "resources/images/calibration/*.jpg";
    cv::glob(calibration_path, images);
    cv::Mat frame, gray;
    std::vector<cv::Point2f> corner_pts;

    for (int i{0}; i < images.size(); i++)
    {
        frame = cv::imread(images[i]);
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        bool success = cv::findChessboardCorners(
                gray,
                cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]),
                corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
        if (success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.001);
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);
            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }
        cv::imwrite("resources/images/calibration/" + std::to_string(i) + "_final.png", frame);
        //cv::imshow("Image",frame);
        //cv::waitKey(0);
    }
    cv::destroyAllWindows();
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

    std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    std::cout << "distCoeffs : " << distCoeffs << std::endl;
    std::cout << "Rotation vector : " << R << std::endl;
    std::cout << "Translation vector : " << T << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Main /////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    // ---------------------
    //    Initialization
    // ---------------------
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    //cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);

    //read ref image (first image of video)
    saveImageFromVideo(R"(resources\videos\livre2.mov)", R"(resources\images\first_image.png)");
    cv::Mat refFrame = cv::imread(R"(resources\images\first_image.png)");

    // --- Letting the user click on the four points
    // /!\ WARNING : clicking order is important :
    //            - first  : up-left corner
    //            - second : up-right corner
    //            - third  : down-right corner
    //            - fourth : down-left corner
    // xref has to be in the same order of Xref
    cv::Mat displayRefFrame = refFrame.clone();
    cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Image",  mouseCallback, &displayRefFrame); // Setting what have to do mouse actions

    // looping while the user didn't click 4 times
    while (xref.size() < 4) // Four points are needed
    {
        cv::imshow("Image", displayRefFrame);
        char key = (char)cv::waitKey(1);
        if (key == 27)
        {
            std::cout << "Anticipated exit." << std::endl;
            break;
        }
    }

    cv::destroyAllWindows();

    displayRefFrame = refFrame.clone();
    line(displayRefFrame,xref[0],xref[1],cv::Scalar(255, 255, 0),2); // last line
    line(displayRefFrame,xref[1],xref[2],cv::Scalar(255, 255, 0),2); // last line
    line(displayRefFrame,xref[2],xref[3],cv::Scalar(255, 255, 0),2); // last line
    line(displayRefFrame,xref[3],xref[0],cv::Scalar(255, 255, 0),2); // last line
    cv::imshow("Image", displayRefFrame);
    cv::waitKey(1);
    cv::imwrite(R"(resources\images\first_image_detouree.png)", displayRefFrame);

    // --- Calibration
    // read calib videos to extract calib images
    saveImageFromVideosForCalibration(R"(resources\videos\calib\*.mov)", R"(resources\images\calibration)");

    if (refFrame.empty()) // Verifying that images exist
    {
        std::cerr << "Error : no image !" << std::endl;
        return -1;
    }

    cv::Mat K; // calibration matrix of camera
    cv::Mat distCoeffs; // distortion coefficients
    cv::Mat R; // Rotation matrix
    cv::Mat T; // Translation matrix
    calibrateCamera(6, 9, K, distCoeffs , R, T);

    // --- Initializing the 3D points
    std::vector<cv::Point3f> Xref;
    Xref.emplace_back(0,0,0);
    Xref.emplace_back(REF_WIDTH, 0, 0);
    Xref.emplace_back(REF_WIDTH, REF_HEIGHT, 0);
    Xref.emplace_back(0, REF_HEIGHT, 0);

    // --- Init axis
    // in the up-left corner
    w_axis.emplace_back(0.0,0.0,0.0);    // Origin
    w_axis.emplace_back(0.050,0.0,0.0); // X (red)
    w_axis.emplace_back(0.0,0.050,0.0); // Y (green)
    w_axis.emplace_back(0.0,0.0, -0.050); // Z (blue)

    // in the middle of the object
    /*
    w_axis.emplace_back(REF_WIDTH / 2, REF_HEIGHT / 2, 0);    // Origin
    w_axis.emplace_back(REF_WIDTH / 2 + 0.05,REF_HEIGHT / 2, 0); // X (red)
    w_axis.emplace_back(REF_WIDTH / 2,REF_HEIGHT / 2 + 0.05, 0); // Y (green)
    w_axis.emplace_back(REF_WIDTH / 2,REF_HEIGHT / 2, - 0.05); // Z (blue)
    */

    // --- Creating descriptors in ref image
    // Creating mask to keep only keypoints that are in that mask
    cv::Mat mask = cv::Mat::zeros(refFrame.size(), CV_8UC1);
    std::vector<cv::Point> xref_int;
    xref_int.reserve(xref.size());

    // Convert xref into int for compatibility issues
    for (const auto& pt : xref)
    {
        xref_int.emplace_back(cvRound(pt.x), cvRound(pt.y)); // float → int
    }
    std::vector<std::vector<cv::Point>> contours = { xref_int };
    cv::fillPoly(mask, contours, cv::Scalar(255));

    // Making descriptors
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create(500);
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    sift->detectAndCompute(refFrame, mask, keypoints, descriptors);

    // --- Writting images
    cv::imwrite(R"(resources\images\mask.png)", mask);
    cv::drawKeypoints(refFrame, keypoints, displayRefFrame, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DEFAULT);
    cv::imwrite(R"(resources\images\keypointsRef.png)", displayRefFrame);

    // ----------
    //    Loop
    // ----------
    std::string videoPath = R"(resources\videos\livre2.mov)"; // Path of the video
    //std::string outPath = R"(resources\videos\outLivre2_good.avi)"; // Path of the output video

    // Init output writer
    double fps; int videoWidth, videoHeight;
    extractFpsAndDimFromVideo(videoPath, fps, videoWidth, videoHeight);
    //cv::VideoWriter output(outPath, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, cv::Size(videoWidth, videoHeight));
    int frame_index = 1; // Index starting at 1, because image 0 is for init
    cv::Mat frame; // Where the frame will be stored

    // Looping while there is an image in the video at index i
    while (extractImageFromVideo(videoPath, frame_index, frame)) {

        frame_index++;

        // ------- New keypoints -------
        // Computing the new keypoints in the new frame
        std::vector<cv::KeyPoint> new_keypoints;
        cv::Mat new_descriptors;
        sift->detectAndCompute(frame, cv::noArray(), new_keypoints, new_descriptors);

        // Selecting points in keypoints that matches with keypoints in image 0
        std::vector<int> ids;
        cv::BFMatcher matcher(cv::NORM_L2, true);
        std::vector<cv::DMatch> matches;
        matcher.match(descriptors, new_descriptors, matches);
        std::sort(matches.begin(), matches.end(), [](const cv::DMatch& a, const cv::DMatch& b)
        { return a.distance < b.distance; });
        std::cout << " MATCHER > matching number : " << matches.size() << std::endl;

        // Saving points that match
        std::vector<cv::Point2f> k_src;
        std::vector<cv::Point2f> k_dst;
        k_dst.reserve(matches.size());
        k_src.reserve(matches.size());
        for(auto match: matches)
        {
            k_src.push_back(keypoints.at(match.queryIdx).pt);
            k_dst.push_back(new_keypoints.at(match.trainIdx).pt);
        }

        // ------- Homography -------
        // Computing homography, using a RANSAC
        cv::Mat homography = cv::findHomography(k_src, k_dst, cv::RANSAC);

        // ------- 4 new points -------
        // Computing the new 4 points in the new pose
        std::vector<cv::Point2f> xnew;
        xnew.reserve(xref.size());
        cv::perspectiveTransform(xref,xnew,homography);

        // ------- Pose computing -------
        // Solving PnP to have rvec and tvec to accuratly project world axis on image plane
        cv::Mat rvec, tvec;
        cv::solvePnP(Xref, xnew, K, distCoeffs, rvec, tvec);

        // ------- Drawing -------
        //  --- Drawing axis
        // Projecting world axis in image plane
        std::vector<cv::Point2f> i_axis;
        cv::projectPoints(w_axis, rvec, tvec, K, distCoeffs, i_axis);

        // Drawing line to represent the axis
        arrowedLine(frame, i_axis[0], i_axis[1], cv::Scalar(255, 0, 0), 3); // X in red
        arrowedLine(frame, i_axis[0], i_axis[3], cv::Scalar(0, 0, 255), 3); // Z in blue
        arrowedLine(frame, i_axis[0], i_axis[2], cv::Scalar(0, 255, 0), 3); // Y in green

        // --- Drawing rectangle lines
        line(frame,xnew[0],xnew[1],cv::Scalar(255, 255, 0),2);
        line(frame,xnew[1],xnew[2],cv::Scalar(255, 255, 0),2);
        line(frame,xnew[2],xnew[3],cv::Scalar(255, 255, 0),2);
        line(frame,xnew[3],xnew[0],cv::Scalar(255, 255, 0),2);

        // --- Drawing key points, for fanciness
        // Making a mask, with correct 4 points
        mask = cv::Mat::zeros(refFrame.size(), CV_8UC1);
        std::vector<cv::Point> xnew_int;
        xnew_int.reserve(xnew.size());

        // Convert xnew into int, for compatibility issues
        for (const auto& pt : xnew)
        {
            xnew_int.emplace_back(cvRound(pt.x), cvRound(pt.y)); // float → int
        }
        contours = { xnew_int };
        cv::fillPoly(mask, contours, cv::Scalar(255));

        // Filtering the key points that are in that mask
        std::vector<cv::KeyPoint> display_keypoints;
        for (const auto& kp : new_keypoints)
        {
            if (mask.at<uchar>(cv::Point((int)kp.pt.x, (int)kp.pt.y)) > 0) // If in mask
            {
                display_keypoints.push_back(kp);
            }
        }
        cv::drawKeypoints(frame, display_keypoints, frame, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DEFAULT);

        // --- Finalize drawing
        // Displaying the frame
        imshow("Frame", frame);

        // Saving the frame
        //output.write(frame);

        cv::waitKey(10);

    }
    //output.release();
    return 0;
}


