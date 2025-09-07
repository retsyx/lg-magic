#include <linux/types.h>
#include "lg_magic_airmouse.h"

void lgmagic_calc_mouse(struct lg_magic_airmouse_calib *calib, float *gyro_acc, s16 *gyro, s16 *mouse);

static inline void lgmagic_lpf(float *acc, float alpha, float value)
{
	*acc = alpha * value + (1.0 - alpha) * (*acc);
}

void lgmagic_calc_mouse(struct lg_magic_airmouse_calib *calib, float *gyro_acc, s16 *gyro, s16 *mouse)
{
	for (size_t i = 0; i < 3; i++)
	{
		float gyro_corr = ((float)gyro[i] - calib->gyro_bias[i]) * calib->gyro_scale[i];
		gyro_acc[i] = calib->alpha * gyro_corr + (1.0 - calib->alpha) * gyro_acc[i];
	}

	mouse[0] = gyro_acc[2] * calib->mouse_k;
	mouse[1] = gyro_acc[0] * calib->mouse_k;
}
