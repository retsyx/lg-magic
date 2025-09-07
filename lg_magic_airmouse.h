#ifndef LG_MAGIC_AIRMOUSE_H
#define LG_MAGIC_AIRMOUSE_H

struct lg_magic_airmouse_calib
{
	float gyro_bias[3];
	float gyro_scale[3];
	float alpha;
	float mouse_k;
};

#endif
