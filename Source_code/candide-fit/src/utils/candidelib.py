"""
Utility library with helper functions to handle the candide-3 model.
"""

import cv2
import numpy as np
import tensorflow as tf
from torch import float32


def tf_rodrigues(rvec):
    tf.assert_equal(tf.shape(rvec), (3, 1))

    theta = tf.norm(rvec, axis=0)

    if (theta == 0):
        return tf.eye(3)

    u = rvec / theta

    Ks_1 = tf.stack([0, -u[2, 0],  u[1, 0]])  # row 1
    Ks_2 = tf.stack([u[2, 0],  0, -u[0, 0]])  # row 2
    Ks_3 = tf.stack([-u[1, 0],  u[0, 0],  0])  # row 3
    # pyformat: enable
    Ks = tf.stack([Ks_1, Ks_2, Ks_3], axis=0)         # stack rows

    Rs = tf.cos(theta) * tf.eye(3) + \
        tf.sin(theta) * Ks + \
        (1 - tf.cos(theta)) * tf.linalg.matmul(u, tf.transpose(u))

    tf.assert_equal(tf.shape(Rs), (3, 3))

    return Rs


def triplet_to_np(string):
    """
    Converts a string with three numbers into a numpy array (a column vector).
    """
    nums = string.split()
    return np.array(nums, dtype=float).reshape((3, 1))


def load_model(path):
    """
    Loads a candide model from its .wfm file.
    Parameters
    ----------
        path(str): the path to the .wfm file.
    Returns:
    --------
        model(dict): the loaded model:
            model["vertex_matrix"]: a matrix with all model vertices as column vectors.
            model["face_matrix"]: a matrix with all model faces as column vectors.
            model["shape_units"]: extended shape units to, indexed by [coordinate, vertex, unit].
    """
    model = {}
    f = open(path, "r", encoding='utf-8', errors='replace')
    line = f.readline()

    # Skip until the vertex list
    while line != "# VERTEX LIST:\n":
        line = str(f.readline())

    # Read all vertex
    vertex_count = int(str(f.readline()))
    vertex_matrix = None
    for _ in range(vertex_count):
        vertex = triplet_to_np(str(f.readline()))
        if vertex_matrix is None:
            vertex_matrix = vertex
        else:
            vertex_matrix = np.concatenate((vertex_matrix, vertex), axis=1)

    vertex_matrix[1] = -vertex_matrix[1]
    vertex_matrix[2] = -vertex_matrix[2]
    model["vertex_matrix"] = vertex_matrix

    # Skip until the face list
    while line != "# FACE LIST:\n":
        line = str(f.readline())

    # Read all faces
    faces_count = int(str(f.readline()))
    faces_matrix = None
    for _ in range(faces_count):
        face = triplet_to_np(str(f.readline()))
        if faces_matrix is None:
            faces_matrix = face
        else:
            faces_matrix = np.concatenate((faces_matrix, face), axis=1)

    model["faces_matrix"] = faces_matrix.astype(int)

    # Skip until the animation units list
    while line != "# ANIMATION UNITS LIST:\n":
        line = str(f.readline())

    animation_units_count = int(str(f.readline()))

    model["animation_units"] = np.zeros(
        (3, vertex_count, animation_units_count))
    for index in range(animation_units_count):
        line = str(f.readline())
        # Skip until unit name
        while line[0] != "#":
            line = str(f.readline())
        # Skip unit size
        while line[0] == "#":
            line = str(f.readline())
        action_unit_size = int(line)
        unit_matrix = np.zeros_like(model["vertex_matrix"])
        for _ in range(action_unit_size):
            line = str(f.readline())
            vertex_index, x, y, z = line.split()
            coef_vector = np.array([float(x), -float(y), -float(z)]).T
            unit_matrix[:, int(vertex_index)] = coef_vector

        model["animation_units"][:, :, index] = unit_matrix

    # Skip until the shape units list
    while line != "# SHAPE UNITS LIST:\n":
        line = str(f.readline())

    shape_units_count = int(str(f.readline()))

    model["shape_units"] = np.zeros((3, vertex_count, shape_units_count))
    for index in range(shape_units_count):
        line = str(f.readline())
        # Skip until unit name
        while line[0] != "#":
            line = str(f.readline())
        shape_unit_size = int(str(f.readline()))
        unit_matrix = np.zeros_like(model["vertex_matrix"])
        for _ in range(shape_unit_size):
            line = str(f.readline())
            vertex_index, x, y, z = line.split()
            coef_vector = np.array([float(x), -float(y), -float(z)]).T
            unit_matrix[:, int(vertex_index)] = coef_vector

        model["shape_units"][:, :, index] = unit_matrix

    return model


def draw_model_projection(
        image, model, shape_coefs, animation_coefs, r_vector, t_vector, cx, cy, fl, color=[255, 255, 255],
        draw_indices=False, draw_lines=True):
    """
    Draws a projection of the model in the given image, projected using the given projection matrix.
    Parameters
    ----------
        image(numpy.array): the target image.
        model(dict): the candide model.
        projection_matrix(numpy.array): matrix of shape (2,4) that maps the augmented vertex_matrix to the 2D plane.
        color(tuple): the target color.
        draw_indices(boolean): if True, vertex indices will be drawn.
        draw_indices(boolean): if True, face edges will be drawn.
    """
    camera_matrix = np.array([
        [fl, 0, cx],
        [0, fl, cy],
        [0, 0, 1]
    ])
    r_mat = np.eye(3)
    r_mat = tf_rodrigues(tf.constant(r_vector.astype('float32'))).numpy()
    transformation_matrix = np.hstack([r_mat, t_vector])
    projection_matrix = np.matmul(camera_matrix, transformation_matrix)
    # Compute the projected matrix.
    translated_vertex = model["vertex_matrix"] + \
        np.sum(shape_coefs * model["shape_units"][:, :, :], axis=2) + \
        np.sum(animation_coefs * model["animation_units"][:, :, :], axis=2)
    extended_vertex_matrix = np.concatenate(
        (translated_vertex, np.ones((1, translated_vertex.shape[1]))), axis=0)
    projected_vertex = np.matmul(projection_matrix, extended_vertex_matrix)
    projected_vertex = (projected_vertex[0:2] / projected_vertex[2]).astype(int)

    if draw_indices:
        for vertex_idx in range(projected_vertex.shape[1]):
            x = projected_vertex[0, vertex_idx]
            y = projected_vertex[1, vertex_idx]
            cv2.putText(image, "#{}".format(vertex_idx), (x, y),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, color, 1)

    if draw_lines:
        for face_idx in range(model["faces_matrix"].shape[1]):
            p0_index = model["faces_matrix"][0, face_idx]
            p1_index = model["faces_matrix"][1, face_idx]
            p2_index = model["faces_matrix"][2, face_idx]

            p0 = projected_vertex[:, p0_index]
            p1 = projected_vertex[:, p1_index]
            p2 = projected_vertex[:, p2_index]

            cv2.line(image, (p0[0], p0[1]), (p1[0], p1[1]), color, 1)
            cv2.line(image, (p1[0], p1[1]), (p2[0], p2[1]), color, 1)
            cv2.line(image, (p2[0], p2[1]), (p0[0], p0[1]), color, 1)


def estimate_projection(vertex_matrix, landmarks, shape_units, animation_units, initial_r_vector=None,
                        initial_t_vector=None, initial_shape_coef=None, initial_animation_coef=None,
                        initial_fl=None, initial_cx=None, initial_cy=None):
    """
    Estimates the projection matrix and shape coeficients minimizing the mean distance between the projected vertices and
    their corresponding landarks.
    Parameters
    ----------
        vertex_matrix(numpy.array): the vertices matrix.
        landmarks(numpy.array): the corresponding landmarks.
        shape_units(numpy.array): the shape units of the candide model.
        initial_matrix(numpy.array or None): i.
        draw_indices(boolean): if True, vertex indices will be drawn.
        draw_indices(boolean): if True, face edges will be drawn.
    """
    MAX_SHAPE_DEFORMATION = 1
    MAX_ANIMATION_DEFORMATION = 2
    if initial_r_vector is None:
        initial_r_vector = np.zeros((3, 1)) + 0.001

    if initial_t_vector is None:
        initial_t_vector = np.zeros((3, 1))

    if initial_shape_coef is None:
        initial_shape_coef = np.zeros((1, 1, shape_units.shape[2]))

    if initial_animation_coef is None:
        initial_animation_coef = np.zeros((1, 1, animation_units.shape[2]))

    focal_length = initial_fl
    center_x = initial_cx
    center_y = initial_cy

    # Add the unitary fourth coordinate so we can work in homogeneous coordinates
    vertex_matrix = np.concatenate(
        (vertex_matrix, np.ones((1, vertex_matrix.shape[1]))), axis=0)
    shape_units = np.concatenate((shape_units, np.zeros(
        (1, shape_units.shape[1], shape_units.shape[2]))), axis=0)
    animation_units = np.concatenate((animation_units, np.zeros(
        (1, animation_units.shape[1], animation_units.shape[2]))), axis=0)

    # Define inputs as constant tensors
    landmarks = tf.constant(landmarks.astype('float32'))
    vertex_matrix = tf.constant(vertex_matrix.astype('float32'))

    # Define camera parameters as variables to optimize.
    fl = tf.Variable(focal_length, dtype=tf.float32)
    cx = tf.Variable(center_x, dtype=tf.float32)
    cy = tf.Variable(center_y, dtype=tf.float32)

    # Define the transformation vectors as a variables (we'll optimize it).
    r_vector = tf.Variable(initial_r_vector.astype('float32'))
    t_vector = tf.Variable(initial_t_vector.astype('float32'))

    # Define shape and animation coefficients as vectors
    shape_coefs = tf.Variable(initial_shape_coef.astype('float32'))
    animation_coefs = tf.Variable(initial_animation_coef.astype('float32'))

    optimizer = tf.keras.optimizers.Adam(learning_rate=0.1)

    partial_results = list()
    delta_cost = None
    cost = None
    step = 0
    # Optimize the transformation matrix to minimize the mean square error.
    while cost == None or delta_cost/cost.numpy() > 1E-5:

        with tf.GradientTape() as tape:
            cm1 = tf.stack([fl, 0 , cx])
            cm2 = tf.stack([0 , fl, cy])
            cm3 = tf.stack([0 , 0 , 1.0 ])
            camera_matrix = tf.stack([cm1, cm2, cm3], axis=0)    
            r_mat = tf_rodrigues(r_vector)
            transformation_matrix = tf.concat((r_mat, t_vector), axis=1)
            transformation_matrix = tf.linalg.matmul(camera_matrix, transformation_matrix)
            shape_translations = tf.math.reduce_sum(
                MAX_SHAPE_DEFORMATION * tf.tanh(shape_coefs) * shape_units, axis=2)
            animation_translations = tf.math.reduce_sum(
                MAX_ANIMATION_DEFORMATION * tf.tanh(animation_coefs) * animation_units, axis=2)
            translated_vertex = vertex_matrix + shape_translations + animation_translations
            projected_matrix = tf.linalg.matmul(
                transformation_matrix, translated_vertex)
            projected_matrix = projected_matrix[0:2, :] / projected_matrix[2, :]
            cost = tf.math.reduce_mean(
                tf.math.square(projected_matrix - landmarks)) + \
                0.001 * tf.math.reduce_mean(tf.math.square(shape_coefs)) + \
                500 * tf.math.reduce_mean(tf.math.square(animation_coefs))

        print(cost.numpy())
        prev_cost = cost.numpy()

        if step > 1000:
            optimizer.minimize(cost, var_list=[
                r_vector, t_vector, shape_coefs, animation_coefs], tape=tape)
        else:
            optimizer.minimize(cost, var_list=[
                r_vector, t_vector, shape_coefs, animation_coefs], tape=tape)

        step += 1

        if delta_cost == None:
            delta_cost = cost.numpy()
        else:
            # Exponentially weighted average of delta_cost
            delta_cost = delta_cost * 0.99 + 0.01 * (prev_cost - cost.numpy())

        # Export the transformation matrix every 10 steps
        if step % 10 == 0:
            partial = dict()
            partial["r_vector"] = r_vector.numpy()
            partial["t_vector"] = t_vector.numpy()
            partial["shape_coefs"] = MAX_SHAPE_DEFORMATION * tf.tanh(shape_coefs).numpy()
            partial["animation_coefs"] = MAX_ANIMATION_DEFORMATION * tf.tanh(animation_coefs).numpy()
            partial["cx"] = cx.numpy()
            partial["cy"] = cy.numpy()
            partial["fl"] = fl.numpy()
            partial_results.append(partial)
    print("Adam optimization converged after " + str(step) + " steps.")
    return r_vector.numpy(), t_vector.numpy(),  MAX_SHAPE_DEFORMATION * tf.tanh(shape_coefs).numpy(), MAX_ANIMATION_DEFORMATION * tf.tanh(animation_coefs).numpy(), cx.numpy(), cy.numpy(), fl.numpy(), cost.numpy(), partial_results


def export_transform(path, r_vector, t_vector, shape_coefs, animation_coefs, cx, cy, fl):
    f = open(path, "w")

    f.write("# POSE:\n")
    f.write(str(t_vector[0, 0]) + " " + str(t_vector[1, 0]) + " " + str(t_vector[2, 0]) + "\n")
    f.write(str(r_vector[0, 0]) + " " + str(r_vector[1, 0]) + " " + str(r_vector[2, 0]) + "\n")

    f.write("\n")

    f.write("# ANIMATION:\n")

    print(shape_coefs.shape)
    print(animation_coefs.shape)

    for i in range(animation_coefs.shape[2]):
        f.write(str(animation_coefs[0, 0, i]) + "\n")

    f.write("\n")

    f.write("# SHAPE:\n")
    for i in range(shape_coefs.shape[2]):
        f.write(str(shape_coefs[0, 0, i]) + "\n")

    f.write("\n")

    # f.write("# CAMERA PARAMS:\n")
    # f.write(str(cx) + " " + str(cy) + " " + str(fl))

    # f.write("\n")

    f.write("# END:\n")

    f.write("\n")
