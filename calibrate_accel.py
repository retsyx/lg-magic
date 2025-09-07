import numpy as np
import csv
from scipy.optimize import least_squares
import json

# -----------------------------
# Load accelerometer CSV
# Assume first three columns are ax, ay, az
# CSV format: ax, ay, az, ... (other columns ignored)
def load_accel_csv(filename):
    data = []
    with open(filename, newline='') as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 3:
                continue
            try:
                a = [float(row[0]), float(row[1]), float(row[2])]
                data.append(a)
            except ValueError:
                continue  # skip invalid lines
    return np.array(data)


def save_calibration_json(bias, matrix, filename="calibration.json"):
    bias_list = [float(f"{v:.6f}") for v in bias]
    matrix_list = [[float(f"{v:.6f}") for v in row] for row in matrix]
    calib_dict = {
        "bias": bias_list,
        "matrix": matrix_list
    }
    with open(filename, "w") as f:
        json.dump(calib_dict, f, indent=4)
    print(f"Calibration saved to {filename}")

# ---------------------------------------
# Define residuals for least squares
# x = [b_x, b_y, b_z, M00, M01, M02, M10, M11, M12, M20, M21, M22]
def residuals(x, accel):
    b = x[0:3]                  # bias
    M = x[3:].reshape(3, 3)     # 3x3 scale/misalignment
    res = []
    for a_raw in accel:
        a_corr = M @ (a_raw - b)
        res.append(np.linalg.norm(a_corr) - 9.80665)  # want |a_corr| = 1g
    return res

# -----------------------------
def calibrate_accel_from_csv(filename):
    accel_samples = load_accel_csv(filename)
    if len(accel_samples) < 6:
        raise ValueError("Need at least 6 samples in different orientations")

    # Initial guess: bias=mean, M=identity
    b0 = np.mean(accel_samples, axis=0)
    M0 = np.eye(3).flatten()
    x0 = np.hstack([b0, M0])

    # Solve
    result = least_squares(residuals, x0, args=(accel_samples,))
    b_calib = result.x[0:3]
    M_calib = result.x[3:].reshape(3, 3)
    return b_calib, M_calib

# -----------------------------
# Example usage
csv_file = "accel_calib.csv"  # replace with your file
b_calib, M_calib = calibrate_accel_from_csv(csv_file)
save_calibration_json(b_calib, M_calib)

print("Bias (counts):", b_calib)
print("Scale/Misalignment matrix:")
print(M_calib)
