#include "PointState.h"

void PointState::Init()
{
    m_kalman.transitionMatrix = (cv::Mat_<double>(4, 4) <<
        1, 0, m_deltaTime, 0,
        0, 1, 0, m_deltaTime,
        0, 0, 1, 0,
        0, 0, 0, 1);

    m_kalman.statePre.at<double>(0) = m_point.x; // x
    m_kalman.statePre.at<double>(1) = m_point.y; // y

    m_kalman.statePre.at<double>(2) = 1; // init velocity x
    m_kalman.statePre.at<double>(3) = 1; // init velocity y

    m_kalman.statePost.at<double>(0) = m_point.x;
    m_kalman.statePost.at<double>(1) = m_point.y;

    cv::setIdentity(m_kalman.measurementMatrix);

    m_kalman.processNoiseCov = (cv::Mat_<double>(4, 4) <<
        pow(m_deltaTime, 4.0) / 4.0, 0, pow(m_deltaTime, 3.0) / 2.0, 0,
        0, pow(m_deltaTime, 4.0) / 4.0, 0, pow(m_deltaTime, 3.0) / 2.0,
        pow(m_deltaTime, 3.0) / 2.0, 0, pow(m_deltaTime, 2.0), 0,
        0, pow(m_deltaTime, 3.0) / 2.0, 0, pow(m_deltaTime, 2.0));


    m_kalman.processNoiseCov *= m_accelNoiseMag;

    cv::setIdentity(m_kalman.measurementNoiseCov, cv::Scalar::all(0.1));

    cv::setIdentity(m_kalman.errorCovPost, cv::Scalar::all(.1));
}



void PointState::Update(cv::Point2f point)
{
    cv::Mat measurement(2, 1, CV_64FC1);
    if (point.x < 0 || point.y < 0)
    {
        Predict();
        measurement.at<double>(0) = m_point.x;  //update using prediction
        measurement.at<double>(1) = m_point.y;

        m_isPredicted = true;
    }
    else
    {
        measurement.at<double>(0) = point.x;  //update using measurements
        measurement.at<double>(1) = point.y;

        m_isPredicted = false;
    }

    // Correction
    cv::Mat estimated = m_kalman.correct(measurement);
    m_point.x = static_cast<float>(estimated.at<double>(0));   //update using measurements
    m_point.y = static_cast<float>(estimated.at<double>(1));

    Predict();
}


cv::Point2f PointState::Predict()
{
    cv::Mat prediction = m_kalman.predict();
    m_point.x = static_cast<float>(prediction.at<double>(0));
    m_point.y = static_cast<float>(prediction.at<double>(1));
    return m_point;
}


cv::Point2f PointState::GetPoint() 
{
    return m_point;
}

bool PointState::IsPredicted() 
{
    return m_isPredicted;
}



