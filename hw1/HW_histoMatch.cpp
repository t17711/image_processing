// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
// Written by: ADD YOUR NAMES HERE, 2016
//
void
HW_histoMatch(ImagePtr I1, ImagePtr Ilut, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);  // copys width height and other properties from i1 to i2
	int w = I1->width();  // input image
	int h = I1->height();
	int total = w * h; // 

	int histogram_input[MXGRAY]; // image histogram
	int histogram_target[MXGRAY];

	int left[MXGRAY];
	int right[MXGRAY];
	int reserveLeft[MXGRAY];

	/*-----------------------------------------------------*/
	// this  calculates histogram of image

	for (int i = 0; i < MXGRAY; ++i) histogram_input[i] = 0;
	ChannelPtr<uchar> p1, endd;
	int type1;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type1); ch++){
		for (endd = p1 + total; p1 < endd;) histogram_input[*p1++]++;
	}
	/*---------------------------------------------------------------------------------------*/

	/*---------------------------------------------------------------------------------------*/
	/* normalize h2 to conform with dimensions of I1 */
	float Havg; // find level to equalize
	ChannelPtr<int> lut; 
	int type2;
	IP_getChannel(Ilut, 0, lut, type2);

	// first get average. sum all value and divide total by it
	for (int i = Havg = 0; i<MXGRAY; i++) Havg += lut[i];
	float scale = (float)total / Havg;

	// get integer histogram to match our input image histogram 	
	for (int i = 0; i < MXGRAY; i++) histogram_target[i] = (int)round(lut[i] * scale);

	/*---------------------------------------------------------------------------------------*/

	/*---------------------------------------------------------------------------------------*/
	/* normalize h2 to conform with dimensions of I1 */
	int R = 0;
	int Hsum = 0;
	int amount_to_fill = 0;

	//create temporary left to track overflow later
	int left2[MXGRAY];

	/// this is to get left reserve for pixel frequency. so this shows how much pixel i can add in histogram
	for (int i = 0; i<MXGRAY; i++) {
		left[i] = R; /* left end of interval */
		left2[i] = R; /* left end of interval */
		amount_to_fill = histogram_target[R] - Hsum;

		// now reserve space for max amount of pixel i to put in leftmost space of histogram
		reserveLeft[i] = (amount_to_fill > histogram_input[i]) ? 0 : amount_to_fill; // max amount of i on left

		Hsum += histogram_input[i]; /* cum. interval value */
		while (Hsum>histogram_target[R] && (MXGRAY - 1)>R) { /* make interval wider */
			Hsum -= histogram_target[R]; /* adjust Hsum */
			R++; /* update right end */
		}
		right[i] = R;
	}

	// create temporary empty histogram
	for (int i = 0; i<MXGRAY; i++) histogram_input[i] = 0;

	int type;
	ChannelPtr<uchar> p2;

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type); // gets channle 0 1 or 2 (r, g ,b) array 
		for (endd = p1 + total; p1 < endd;) {
			int p = left[*p1];

			//now compare how much of value can be added to the slot by comparing reserved place
			if (histogram_input[p] < histogram_target[p]){

				// if histogram output  value is less than avg then copy point
				// but 1st check if it is reserved space
				if (left2[*p1] != p) *p2 = p;

				// so check if left[*p1] is original left 
				else {

					//if it is so then check the reserved value, if
					if (reserveLeft[*p1] > 0){
						reserveLeft[*p1]--;
						*p2 = p;
					}

					//	reserved value is 0, then move to next place 
					else{
						left[*p1] = (p + 1 < right[*p1]) ? p + 1 : right[*p1];
						p = left[*p1];
						*p2 = p;
					}
				}
			}

			// if histogram target is met then simply set the left to be the tight bin unless it is already right
			else{
				left[*p1] = (p + 1 < right[*p1]) ? p + 1 : right[*p1];
				p = left[*p1];
				*p2 = p;
			}

			// fill output histogram and assigh value to output image
			histogram_input[p]++;
			*p1++;
			*p2++;
		}
	}
}
