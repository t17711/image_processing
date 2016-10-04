// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_contrast:
//
// Apply contrast enhancement to I1. Output is in I2.
// Stretch intensity difference from reference value (128) by multiplying
// difference by "contrast" and adding it back to 128. Shift result by
// adding "brightness" value.
//
// Written by: ADD YOUR NAMES HERE, 2016
//
void
HW_contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);  // copys width height and other properties from i1 to i2
	int w = I1->width();  // input image
	int h = I1->height();
	int total = w * h; // 

	int lut[MXGRAY];
	int shift = 128 + brightness;
	// apply brightness or contrast
	for (int i = 0; i < MXGRAY; ++i){
		// value is always 0 to 255
		lut[i] = (int)CLIP((i - 128)*contrast + shift, 0, 255);
	}
	// point operation
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type); // gets channle 0 1 or 2 (r, g ,b) array 
		for (endd = p1 + total; p1 < endd;) *p2++ = lut[*p1++];  // set rgb to 0 below threshold and 255 above
	}
}
