// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:

//
/*this does one dimensional blur. it reads input of 'width, count by taking 'steps' step
then it pads row or column by 'size-1 / 2 pixels left and right and takes blur of all concurrend size pixels*/
void
getBlur_1D2(IP::ChannelPtr<uchar> p1, int width, int steps, int size, IP::ChannelPtr<uchar> p2) {
	size_t buffer_size = width + size - 1; // so buffer size is width plus size of neighborhood - 1 
	int extra = (size - 1) / 2;
	// if size is 1 no need to blur
	if (size == 1) {
		for (int i = 0; i < width; i++, p2 += steps, p1 += steps) {
			*p2 = *p1;
		}
		return;
	}

	// save max row size
	uint16_t * buffer;

	// allocate memory
	buffer = (uint16_t *)malloc(sizeof(uint16_t)*buffer_size);
	if (buffer == NULL) exit(0);

	// add val to front buffer, it is low(half (neighborehood size/2))
	int i = 0;
	for (; i < extra; ++i) 	buffer[i] = (*p1);

	// copy row or column, step point by steps
	width += extra;
	for (; i < width; ++i) {
		buffer[i] = (*p1);
		p1 += steps;
	}

	p1 -= steps; // go back to pointer end

				 // add val to end
	extra += width;
	for (; i < extra; ++i) buffer[i] = (*p1);

	// now find sum
	int sum = 0;

	// do front 1st
	i = 0;
	for (; i < size; i++) {
		sum += buffer[i];
	}
	*p2 = sum / size;

	// go to next step
	p2 += steps;

	//goes from i+size to buffer end, deletes left adds new
	// here add incoming val subtract outgoing value and update output pointer
	// do for whole row i.e. input image + padd element
	for (; i < buffer_size; i++, p2 += steps) {
		sum += (buffer[i] - buffer[i - size]); // delete left end item  add right end of neighborhood

											   // put in output
		*p2 = sum / size;
	}
	free(buffer);

}

void
HW_blur2(ImagePtr I1, int xsz, int ysz, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);  // copys width height and other properties from i1 to i2
	int w = I1->width();  // input image
	int h = I1->height();
	if (xsz >= w) xsz = 2 * ((int)w / 2) - 1; //use whole width -1
	if (ysz >= h) ysz = 2 * ((int)h / 2) - 1; //use whole width -1

	int total = w * h; 
	
	int type;
	ChannelPtr<uchar> p1, p2,p3, endd;
	

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++){

		IP_getChannel(I2, ch, p2, type); // gets channel 0 1 or 2 (r, g ,b) array 
		
		// for row blur
		for (int i = 0; i < h;++i){ // go from top row to bottom

			// now get 1d blur of pixel width w, step 1, and neighborhood xsz
			getBlur_1D2(p1, w, 1, xsz, p2);
			p1+=w; // go to next row
			p2+=w;
		}
		
		// reinitialize pointer to point to output picture to do columns
		p2 -= (total-1); // sent temp image to front

		// get pointer for output
		IP_getChannel(I2, ch, p3, type); // gets channle 0 1 or 2 (r, g ,b) array 

		// for column blur
		for (int i = 0; i < w; ++i){ // go from 1st col to last
		// now get 1d blur of pixel width h, step w, and neighborhood ysz
			getBlur_1D2(p2, h, w, ysz, p3); 
			p2++; // go to next row
			p3++;
		}
		
	}
}

