import numpy as np

class GyroLPF:
    def __init__(self, alpha=0.2):
        self.alpha = alpha
        self.prev = np.zeros(3)  # [gx, gy, gz]

    def filter(self, gyro):
        """
        Apply low-pass filter to gyro vector.
        gyro: np.array([gx, gy, gz]) in rad/s
        """
        self.prev = self.alpha * gyro + (1 - self.alpha) * self.prev
        return self.prev
