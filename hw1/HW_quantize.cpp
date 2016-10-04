// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
// Written by: ADD YOUR NAMES HERE, 2016
//
void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
	// add bias 1/ of the level, biased towards lwer end of interval
	// any value below thr is 0.
	IP_copyImageHeader(I1, I2);  // copys width height and other properties from i1 to i2
	int w = I1->width();  // input image
	int h = I1->height();
	int scale = MXGRAY / levels;
	int bias = scale / 2; // assign random error first
	int total = w * h; // 
	// compute lut[]

	int i, lut[MXGRAY];  // size 256
	for (i = 0; i < MXGRAY; ++i) lut[i] = scale * (int)(i / scale) + bias;

	int type;
	ChannelPtr<uchar> p1, p2, endd;

	if (dither){
		int k = 0;
		int j = 0;
		int ttttt = 0;
		int osc = 1;
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type); // gets channle 0 1 or 2 (r, g ,b) array 
			for (endd = p1 + total; p1 < endd; p1++){
				ttttt = ((rand() & 0x7FFF));
				j = ttttt / 32767. * bias * osc; //bias = scale / 2;
				k = CLIP(lut[*p1] + j, 0, MXGRAY);
				*p2++ = lut[k];
				osc *= -1;
			}

		}
	}

	else{
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type); // gets channle 0 1 or 2 (r, g ,b) array 
			for (endd = p1 + total; p1 < endd;){
				*p2++ = (lut[*p1++]);
			}
		}
	}
}
