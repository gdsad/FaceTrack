"""
Utility library with miscelaneous helper methods.
"""

import cv2
import numpy as np

def resize(image, target_width):
    """
    Resizes an image to match the target width, preserving aspect ratio.
    Parameters
    ----------
        image(numpy.array): the input image.
        target_width(int): the target width.
    Returns:
    --------
        resized_image(numpy.array): the resized image.
    """
    width = int(target_width)
    height = int(image.shape[0] * target_width / image.shape[1])
    dim = (width, height)

    # resize image
    resized_image = cv2.resize(image, dim, interpolation=cv2.INTER_AREA)
    return resized_image
