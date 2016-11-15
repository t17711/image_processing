// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve
#include <deque>
#include <algorithm>

// get row buffer to add to quq=eue
uchar*
getRowBuff_convolve(ChannelPtr<uchar> p1, int width, int pad) {
	uchar* temp = (uchar*)malloc(sizeof(uchar*)*(width + 2 * pad));
	if (temp == NULL) exit(1);

	// copy leftmost pixel left pad
	int i = 0;
	while (i < pad) {
		*(temp + i) = *p1;
		++i;
	}

	// copy row
	width += i;
	while (i < width) {
		*(temp + i) = (*p1++);
		++i;
	}

	// copy rightmost pixel for right pad
	pad += i;
	while (i < pad) {
		*(temp + i) = (*p1);
		++i;
	}

	return temp;
}

// get convolve alue
void get_convolve(ChannelPtr<uchar> p2, ChannelPtr<float> kernel, std::deque <uchar* > buffer, int xsz, int ysz, int w) {
	int sz = xsz*ysz;
	// there are w neighborhood in a buffer
	for (int i = 0; i < w; i++) {
		// get weighed convlve value
		float val = 0;
		for (int j = 0; j < ysz; j++) {
			for (int k = i; k < xsz+i; k++) {
				float s = *kernel++;
				val += (buffer[j][k] * (s));
			}
			
			
		}
		kernel -= sz;
		*p2 = CLIP(round(val), 0, 255);
		p2++;
	}
}

// this function creates buffer that we apply convolve value to
void
HW_convolve(ImagePtr I1, ImagePtr kernel, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);  // copys width height and other properties from i1 to i2

	int w = I1->width();  // input image
	int h = I1->height();
	int total = w * h; // 

	int xsz = kernel->width();
	int ysz = kernel->height();

	std::deque<uchar*> buffer;

	int xpad = (xsz) / 2; // padding for left and right of w
	int ypad = (ysz) / 2; // padding for top and bottom

	int type;
	ChannelPtr<uchar> p1, p2, endd, endd2;
	ChannelPtr<float> ker;
	IP_getChannel(kernel, 0, ker, type); // gets channle 0 1 or 2 (r, g ,b) array 

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type); // gets channle 0 1 or 2 (r, g ,b) array 
		endd = p1 + total;
		endd2 = p2 + total;

		// create top pad 
		for (int i = 0; i < ypad; ++i) {

			//xpad is number of pixels to be added to left and right of w 
			buffer.push_back(getRowBuff_convolve(p1, w, xpad));
		}
		// add 1st ysz - ypad  rows
		// i used ysz since it determines neighborhood size of column
		for (int i = 0; i < ysz - ypad; ++i) {
			buffer.push_back(getRowBuff_convolve(p1, w, xpad));
			p1 += w;
		}
		// traverse 
		// do until last  row
		//	int end_l = total - (sz - pad)*w;
		for (; p1 < endd; p1 += w, p2 += w) {

			// get median and put it on p2
			get_convolve(p2,ker, buffer, xsz, ysz, w);
			free(buffer.front());

			// remove top row
			buffer.pop_front();

			// add new padded row to bottom
			buffer.push_back(getRowBuff_convolve(p1, w, xpad));
		}
		///////
		//for end
		////

		// add last rows

		// since i already copied last pixel it has gone over it so go back
		p1 -= w;

		// get median and pop out values
		// need to do for last + pad row
		while (p2 < endd2) {
			get_convolve(p2,ker, buffer, xsz, ysz, w);
			p2 += w;
			free(buffer.front());
			buffer.pop_front();
			buffer.push_back(getRowBuff_convolve(p1, w, xpad));
		}


	}
	for (auto i : buffer) {
		free(i);
	}
}
