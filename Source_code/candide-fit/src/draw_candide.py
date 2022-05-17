from utils.candidelib import load_model
import argparse
import numpy as np
import cv2

'''
Run with:
    python3 ./src/draw_candide.py --image ./resources/candide3.wfm
'''
MODEL_SCALE=2000

# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required=True,
    help="path to input image")
args = vars(ap.parse_args())

model = load_model(args["image"])
vertex_matrix = (model["vertex_matrix"][0:2,:] * np.array([[MODEL_SCALE],[MODEL_SCALE]]) + MODEL_SCALE * 1.3).astype(int)
image = np.zeros((int(MODEL_SCALE*2.6),int(MODEL_SCALE*2.6)), dtype=float)
image[vertex_matrix[1,:],vertex_matrix[0,:]] = 0.5

for vertex_idx in range(vertex_matrix.shape[1]):
    x = vertex_matrix[0,vertex_idx]
    y = vertex_matrix[1,vertex_idx]
    cv2.putText(image, "#{}".format(vertex_idx), (x, y), cv2.FONT_HERSHEY_SIMPLEX, 1, 1, 1)

faces_matrix = model["faces_matrix"]
for face_idx in range(faces_matrix.shape[1]):
    p0_index = faces_matrix[0,face_idx]
    p1_index = faces_matrix[1,face_idx]
    p2_index = faces_matrix[2,face_idx]

    p0 = vertex_matrix[:,p0_index]
    p1 = vertex_matrix[:,p1_index]
    p2 = vertex_matrix[:,p2_index]

    cv2.line(image,(p0[0], p0[1]),(p1[0], p1[1]),0.5,1)
    cv2.line(image,(p1[0], p1[1]),(p2[0], p2[1]),0.5,1)
    cv2.line(image,(p2[0], p2[1]),(p0[0], p0[1]),0.5,1)

cv2.imwrite("candide.png", image* 200)