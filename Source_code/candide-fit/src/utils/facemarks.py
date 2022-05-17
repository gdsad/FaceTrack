"""
Utility library for facial landmarks detection.
"""

import cv2
import dlib
import numpy as np


def rect_to_bb(rect):
    """
    Converts a bounding rect predicted by dlib to the format (x, y, w, h)
    Parameters
    ----------
        rect(dlib.rectangle): A dlib rectangle.
    Returns:
    --------
        (x, y, w, h): a tuple representation in OpenCV format.
    """
    x = rect.left()
    y = rect.top()
    w = rect.right() - x
    h = rect.bottom() - y
    return (x, y, w, h)


def dlib_detection_to_np(detection, dtype="int"):
    """
    A dlib shape (list of points) to a numpy array of shape (2, detection.num_parts).
    Parameters
    ----------
        detection(dlib.full_object_detection): a dlib object detection.
    Returns:
    --------
        A numpy array of shape (2, detection.num_parts), each column is a 2D point.
    """
    # Initialize the matrix of (x, y) coordinates.
    coords = np.zeros((2, detection.num_parts), dtype=dtype)
    # Loop over the part of the detection.
    for i in range(detection.num_parts):
        coords[:, i] = np.array([detection.part(i).x, detection.part(i).y]).T
    return coords


def detect_landmarks(image, predictor_path):
    """
    Detects a face and its facial landmarks in the given image, using the given predictor model.
    Parameters
    ----------
        image(numpy.array): the input BGR image.
        predictor_path(str): the path to the predictor model file.
    Returns:
    --------
        tuple(landarmarks, bounding_rect)
            landmarks(numpy.array):  a matrix in which each column is a landmark location.
            bounding_rect(tuple): the bounding rect of the detected face in format (x, y, w, h).
    """
    # Initialize dlib's face detector (HOG-based) and then create the facial landmark predictor.
    detector = dlib.get_frontal_face_detector()
    predictor = dlib.shape_predictor(predictor_path)

    # Convert image to grayscale.
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    # Fetect faces in the grayscale image.
    rects = detector(gray, 1)

    # Here we assume there is only one face so we return the first result.
    rect = rects[0]
    # Detect landmarks only in the face region.
    detection = predictor(gray, rect)
    landmarks = dlib_detection_to_np(detection)
    return landmarks, rect_to_bb(rect)


def draw_landmarks(image, landmarks, draw_indices = False):
    """
    Draw landmarks in the given image.
    Parameters
    ----------
        image(numpy.array): the target image.
        landmarks(numpy.array): the landmarks to be drawn.
    """
    for idx in range(landmarks.shape[1]):
        (x, y) = (landmarks[0, idx], landmarks[1, idx])
        if draw_indices:
            cv2.putText(image, "#{}".format(idx), (x - 10, y - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.3, (0, 255, 0), 1)
        cv2.circle(image, (x, y), 1, (0, 0, 255), -1)
