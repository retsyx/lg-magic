#ifndef LG_MAGIC_AIRMOUSE_H
#define LG_MAGIC_AIRMOUSE_H

struct lg_magic_airmouse_calib
{
	float gyro_bias[3];
	float gyro_scale[3];
	float alpha;
	float mouse_k;
};

void lgmagic_calc_mouse(struct lg_magic_airmouse_calib *calib, float *gyro_acc, s16 *gyro, s16 *mouse);
int lgmagic_validate_calib(struct lg_magic_airmouse_calib *calib);

#endif
