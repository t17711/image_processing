// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoStretch:
//
// Apply histogram stretching to I1. Output is in I2.
// Stretch intensity values between t1 and t2 to fill the range [0,255].
//
// Written by: ADD YOUR NAMES HERE, 2016
//
void
HW_histoStretch(ImagePtr I1, int t1, int t2, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);  // copys width height and other properties from i1 to i2
	int w = I1->width();  // input image
	int h = I1->height();
	int total = w * h; // 

	int lut[MXGRAY];
	int diff = t2 - t1;
	if (diff < 0) return;
	// get look up table
	for (int i = 0; i < MXGRAY; ++i)
		// just use the formula
		lut[i] = CLIP(((MXGRAY - 1)*(i - t1) / diff), 0, (MXGRAY - 1));
	// create image

	int type;
	ChannelPtr<uchar> p1, p2, endd;

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type); // gets channle 0 1 or 2 (r, g ,b) array 
		for (endd = p1 + total; p1 < endd;) *p2++ = lut[*p1++];  // set rgb to 0 below threshold and 255 above
	}
}
