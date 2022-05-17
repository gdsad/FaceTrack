'''
Fits a Candide-3 Model into a facial image.
To install required dependencies run:
    pip3 install --no-cache-dir numpy opencv-python dlib tensorflow
Example usage:
    python3 ./src/fit_candide.py --image ./dataset/jobs.jpeg
'''

import math
import os
import sys

# Reduce tensorflow verbosity
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

import time
import numpy as np
import argparse
import cv2
import utils.facemarks as facemarks
import utils.imutils as imutils
import utils.candidelib as candidelib

start_time = time.time()

PARENT_DIR = os.path.dirname(__file__)

print("Processing...")

PREDICTOR_PATH = PARENT_DIR + "/resources/shape_predictor_68_face_landmarks.dat"
CANDIDE_MODEL_PATH = PARENT_DIR + "/resources/candide3.wfm"
CORRESPONDENCES_PATH = PARENT_DIR + "/resources/correspondences.txt"

HELP_STRING = "Fits a Candide-3 model into an image with a face.\n\
    It expects to find exactly one face in the image.\n\
    The projection of the fitted model will be saved as ./result.png"

# Parse arguments.
ap = argparse.ArgumentParser(description=HELP_STRING)
ap.add_argument("-i", "--image", required=True,
                help="path to input image")
ap.add_argument('--export-video', action='store_true',
                help="Export video of the step-by-step fitting process.")
args = vars(ap.parse_args())

# Load the input image, resize it, and convert it to grayscale
image = cv2.imread(args["image"])

# image = imutils.resize(image, 1270)

print("Detecting landmarks...")
# Detect landmarks.
landmarks, bounding_box = facemarks.detect_landmarks(image, PREDICTOR_PATH)

# Load the candide model.
candide_model = candidelib.load_model(CANDIDE_MODEL_PATH)
vertex_matrix = candide_model["vertex_matrix"]
shape_units = candide_model["shape_units"]
animation_units = candide_model["animation_units"]

# Load correspondences
landmarks_indexes = list()
vertex_indexes = list()
correlations_file = open(CORRESPONDENCES_PATH,
                         "r", encoding='utf-8', errors='replace')

# Parse the correspondences file.
for line in correlations_file:
    if "#" in line:
        continue
    nums = line.split()
    landmarks_indexes.append(int(nums[0]))
    vertex_indexes.append(int(nums[1]))

# Filter the model to contain only the matched vertices.
filtered_vertex = vertex_matrix[:, vertex_indexes]
filtered_shape_units = shape_units[:, vertex_indexes, :]
filtered_animation_units = animation_units[:, vertex_indexes, :]
filtered_landmarks = landmarks[:, landmarks_indexes]

# Draw filtered landarks.
facemarks.draw_landmarks(image, filtered_landmarks)

# Initialize the transformation matrix as a translation to the bounding box center,
# and a scaling to fit the face bounding box.

initial_r_vector = np.array([
    [0.00001],
    [0.00001],
    [0.00001]
])

initial_t_vector = np.array([
    [0],
    [0],
    [5]
])

# First compute the bounding box and center of the candide model without any scaling.
candide_left = np.min(vertex_matrix[0, :])
candide_right = np.max(vertex_matrix[0, :])
candide_top = np.max(vertex_matrix[1, :])
candide_bottom = np.min(vertex_matrix[1, :])
cadide_width = candide_right - candide_left
candide_height = candide_top - candide_bottom
candide_cx = (candide_left + candide_right) / 2
candide_cy = (candide_top + candide_bottom) / 2

# Compute the center of the bounding box
bb_cx = int(bounding_box[0] + bounding_box[2] / 2)
bb_cy = int(bounding_box[1] + bounding_box[3] / 2)

# Compute scaling factors
fl = 2 * bounding_box[3] / candide_height

print(fl)

initial_fl = 1000
initial_cx = 1200 / 2
initial_cy = 896 / 2

# Initialize shape and animation coefficients with zeros.
shape_coefs = np.zeros((1, 1, shape_units.shape[2]))
animation_coefs = np.zeros((1, 1, animation_units.shape[2]))

# candidelib.draw_model_projection(
#     image, candide_model, shape_coefs, animation_coefs, initial_r_vector, initial_t_vector,initial_cx, initial_cy, initial_fl,
#     [255, 255, 255], draw_indices=False, draw_lines=True)

# cv2.imshow("initial", image)
# cv2.waitKey()

print("Fitting candide model...")
# Estimate the projection matrix and coefficients.
r_vector, t_vector, shape_coefs, animation_coefs, cx, cy, fl, cost, partial_results = candidelib.estimate_projection(
    filtered_vertex,
    filtered_landmarks,
    filtered_shape_units,
    filtered_animation_units,
    initial_r_vector=initial_r_vector,
    initial_t_vector=initial_t_vector,
    initial_shape_coef=shape_coefs,
    initial_animation_coef=animation_coefs,
    initial_fl= initial_fl,
    initial_cy= initial_cy,
    initial_cx= initial_cx
)

elapsed_time = time.time() - start_time

print("Processing time: " + str(round(elapsed_time, 2)) + " seconds")

if args["export_video"]:
    print("Exporting video...")
    # Configure video output
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('fit-animation.avi', fourcc, 10.0,
                          (image.shape[1], image.shape[0]))
    for partial in partial_results:
        frame = image.copy()
        candidelib.draw_model_projection(
            frame, candide_model, partial["shape_coefs"], partial["animation_coefs"], partial["r_vector"], partial["t_vector"],
            partial["cx"], partial["cy"], partial["fl"],
            [255, 255, 255], draw_indices=False, draw_lines=True)
        out.write(frame)

# Draw the result and save.
candidelib.draw_model_projection(
    image, candide_model, shape_coefs, animation_coefs, r_vector, t_vector, cx, cy, fl,
    [255, 255, 255], draw_indices=False, draw_lines=True)
cv2.imwrite("result.png", image)

candidelib.export_transform("output.shape", r_vector, t_vector, shape_coefs, animation_coefs, cx, cy, fl)
