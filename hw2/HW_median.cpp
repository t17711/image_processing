// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_Median:
//
#include <deque>
#include <algorithm>
/* this function gets padded row of width w and pad size pad in each side*/

uchar*
getRowBuff(ChannelPtr<uchar> p1, int width, int pad){
	uchar* temp = (uchar*)malloc(sizeof(uchar*)*(width + 2 * pad));
	if (temp == NULL) exit(1);

	// copy leftmost pixel left pad
	int i = 0;
	while( i < pad){
		*(temp + i)=*p1;
		++i;
	}

	// copy row
	width += i;
	while( i < width){
		*(temp + i)=(*p1++);
		++i;
	}

	// copy rightmost pixel for right pad
	pad += i;
	while (i < pad){
		*(temp + i)=(*p1);
		++i;
	}

	return temp;
}

/* this is for histogram method
it creates histogram of neighbor hood and cummulates the histogram until the cummulative
is msdian then index where it reaches median is median*/
void
get_med_Hist(ChannelPtr<uchar> p2, std::deque <uchar* > buffer, int xsz, int ysz, int w, int avg) {
	int size = xsz*ysz;

	if (avg > size) avg = size;

	int sum[MXGRAY];
	for (int i = 0; i < MXGRAY; i++) sum[i] = 0;

	// 1st get hist of 1st neighbor hood
	for (int j = 0; j < ysz; j++) {

		// so iterate theough buffer

		// get histogram of 1st xsz items of each item in buffer
		for (int k = 0; k < xsz; k++) {
			sum[buffer[j][k]]++;
		}
	}

	// get median value and set output value, increment pointer
	int med = (size - 1) / 2 + 1; // median of 3 is 3/2 +1  = 1+1 =2
	int m = 0;
	int k = 0;

	// there are w neighborhood in a buffer
	for (int i = 0; i < w; i++) {
		m = 0;
		for (k = 0; m < med && k < MXGRAY; ++k) {
			m += sum[k];
		}

		// get median value and set output value, increment pointer
		*p2 = k;
		p2++;

		// remove leftmost item from hist
		for (int b = 0; b < ysz; ++b) {
			// delete last item
			sum[buffer[b][i]]--;
			// add new item
			sum[buffer[b][i + xsz]]++;
		}
	}
}


/* this function is called to get median from buffer and set it to output image
there are two types of median method that it can call*/
void
get_med(ChannelPtr<uchar> p2, std::deque <uchar* > buffer, int xsz, int ysz, int w, int avg){
		get_med_Hist(p2, buffer, xsz, ysz, w,avg);	
	
}


void
HW_median(ImagePtr I1, int xsz, int ysz, int avg, ImagePtr I2){
	/*clock_t t;
	t = clock();*/

	IP_copyImageHeader(I1, I2);  // copys width height and other properties from i1 to i2

	int w = I1->width();  // input image
	int h = I1->height();
	int total = w * h; // 


	std::deque<uchar*> buffer;

	int xpad = (xsz) / 2; // padding for left and right of w
	int ypad = (ysz) / 2; // padding for top and bottom

	int type;
	ChannelPtr<uchar> p1, p2, endd, endd2;

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type); // gets channle 0 1 or 2 (r, g ,b) array 
		endd = p1 + total;
		endd2 = p2 + total;

		// create top pad 
		for (int i = 0; i < ypad; ++i){

			//xpad is number of pixels to be added to left and right of w 
			buffer.push_back(getRowBuff(p1, w, xpad));
		}
		// add 1st ysz - ypad  rows
		// i used ysz since it determines neighborhood size of column
		for (int i = 0; i < ysz - ypad; ++i){
			buffer.push_back(getRowBuff(p1, w, xpad));
			p1 += w;
		}
		// traverse 
		// do until last  row
		//	int end_l = total - (sz - pad)*w;
		for (; p1 < endd; p1 += w, p2 += w) {

			// get median and put it on p2
			get_med(p2, buffer, xsz, ysz, w, avg);
			free(buffer.front());

			// remove top row
			buffer.pop_front();

			// add new padded row to bottom
			buffer.push_back(getRowBuff(p1, w, xpad));
		}
		///////
		//for end
		////

		// add last rows

		// since i already copied last pixel it has gone over it so go back
		p1 -= w;

		// get median and pop out values
		// need to do for last + pad row
		while (p2 < endd2){
			get_med(p2, buffer, xsz, ysz, w, avg);
			p2 += w;
			free(buffer.front());
			buffer.pop_front();
			buffer.push_back(getRowBuff(p1, w, xpad));
		}


	}
	for (auto i : buffer){
		free(i);
	}

}
