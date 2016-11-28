// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// IPconvolve.cpp - Convolution and correlation functions.
//
// Written by: George Wolberg, 2016
// ======================================================================

//! \file	IPconvolve.cpp
//! \brief	Convolution and correlation functions.
//! \author	George Wolberg, 2016

#include "IP.h"

namespace IP {

#define MAG(a, b)	(sqrt(a*a + b*b))

//! \addtogroup filtnbr
//@{

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IP_convolve:
//
// Convolve I1 with the 2D kernel Ikernel. Output is in I2.
//! \brief	Convolve I1 with the 2D kernel Ikernel. Output is in I2.
//! \param[in]	I1 - ImagePtr.
//! \param[in]	Ikernel - 2D kernel.
//! \param[out]	I2 - ImagePtr.
//
void
IP_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2)
{
	// kernel dimensions
	int ww = Ikernel->width ();
	int hh = Ikernel->height();

	// error checking: must use odd kernel dimensions
	if(!(ww%2 && hh%2)) {
		fprintf(stderr, "IP_convolve: kernel size must be odd\n");
		return;
	}

	// input image dimensions
	IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();

	// clear offset; restore later
	int xoffset = I1->xoffset();
	int yoffset = I1->yoffset();
	I1->setXOffset(0);
	I1->setYOffset(0);

	// pad image to handle border problems
	int	 padnum[4];
	ImagePtr Isrc;
	padnum[0] = padnum[2] = ww/2;		// left and right  padding
	padnum[1] = padnum[3] = hh/2;		// top  and bottom padding
	IP_pad(I1, padnum, REPLICATE, Isrc);	// replicate border

	// restore offsets
	I1->setXOffset(xoffset);
	I1->setYOffset(yoffset);

	// cast kernel into array weight (of type float)
	ImagePtr Iweights;
	IP_castChannelsEq(Ikernel, FLOAT_TYPE, Iweights);
	ChannelPtr<float> wts = Iweights[0];

	ImagePtr I1f, I2f;
	if(I1->maxType() > UCHAR_TYPE) {
		I1f = IP_allocImage(w+ww-1, h+hh-1, FLOATCH_TYPE);
		I2f = IP_allocImage(w, h, FLOATCH_TYPE);
	}

	int	t;
	float	sum;
	ChannelPtr<uchar> p1, p2, in, out;
	ChannelPtr<float> f1, f2, wt;
	for(int ch=0; IP_getChannel(Isrc, ch, p1, t); ch++) {
		IP_getChannel(I2, ch, p2, t);
		if(t == UCHAR_TYPE) {
			out = p2;
			for(int y=0; y<h; y++) {	// visit rows
			   for(int x=0; x<w; x++) {	// slide window
				sum = 0;
				in  = p1 + y*(w+ww-1) + x;
				wt  = wts;
				for(int i=0; i<hh; i++) {	// convolution
					for(int j=0; j<ww; j++)
						sum += (wt[j]*in[j]);
					in += (w+ww-1);
					wt +=  ww;
				}
				*out++ = (int) (CLIP(sum, 0, MaxGray));
			   }
			}
			continue;
		}
		IP_castChannel(Isrc, ch, I1f, 0, FLOAT_TYPE);
		f2 = I2f[0];
		for(int y=0; y<h; y++) {		// visit rows
		   for(int x=0; x<w; x++) {		// slide window
			sum = 0;
			f1 = I1f[0];
			f1 += y*(w+ww-1) + x;
			wt = wts;
			for(int i=0; i<hh; i++) {	// convolution
				for(int j=0; j<ww; j++)
					sum += (wt[j]*f1[j]);
				f1 += (w+ww-1);
				wt +=  ww;
			}
			*f2++ = sum;
		   }
		}
		IP_castChannel(I2f, 0, I2, ch, t);
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IP_correlation:
//
// Cross-correlation of image I1 with template I2.
// Correlation is performed on the first channel of I1 and I2 only.
//
// Multiresolution correlation is used if multires=1.
// This makes use of image pyramids for I1 and I2 and is significantly
// faster than direct correlation (multires=0).
//
// Return (dx,dy) offset of I2 for which there exists best match.
// Return the correlation number computed by method.
//
// The correlation methods are specified by mtd:
//	CROSS_CORR (cross correlation):
//		C(u,v) = sum of {T(x,y) * I(x-u,y-v)}
//			 --------------------------------
//			 sqrt{ sum of {I(x-u,y-v)^2}}
//	SSD (sum of squared differences):
//		C(u,v) = sum of {T(x,y)-I(x-u,y-v)}^2
//			 ------------------------------
//			 sqrt{ sum of {I(x-u,y-v)^2}}
//	CORR_COEFF (correlation coefficient):
//		C(u,v) = sum of {(T(x,y)-Tavg) * (I(x-u,y-v)-Iavg)}
//			 -----------------------------------------------------
//			 sqrt{sum{(T(x,y)-Tavg)^2} * sum{(I(x-u,y-v)-Iavg)^2}}
//	PHASE_CORR (phase correlation):
//				     {  FFT(T(x,y) * FFT*(I(x-u,y-v))  }
//		C(u,v) = inverse FFT { ------------------------------- }
//				     { |FFT(T(x,y) * FFT*(I(x-u,y-v))| }
//! \brief	Cross-correlation of image I1 with template I2.
//! \details	Correlation is performed on the first channel of I1 and I2 only.
//!
//!		Multiresolution correlation is used if multires=1.
//!		This makes use of image pyramids for I1 and I2 and is significantly
//!		faster than direct correlation (multires=0).
//!
//!		Return (dx,dy) offset of I2 for which there exists best match.
//!
//!		The correlation methods are specified by mtd:
//!
//!		CROSS_CORR (cross correlation):
//!		<pre>
//!		C(u,v) = sum of {T(x,y) * I(x-u,y-v)}
//!			 --------------------------------
//!			 sqrt{ sum of {I(x-u,y-v)^2}}
//!		</pre>
//!
//!		SSD (sum of squared differences):
//!		<pre>
//!		C(u,v) = sum of {T(x,y)-I(x-u,y-v)}^2
//!			 ------------------------------
//!			 sqrt{ sum of {I(x-u,y-v)^2}}
//!		</pre>
//!
//!		CORR_COEFF (correlation coefficient):
//!		<pre>
//!		C(u,v) = sum of {(T(x,y)-Tavg) * (I(x-u,y-v)-Iavg)}
//!			 -----------------------------------------------------
//!			 sqrt{sum{(T(x,y)-Tavg)^2} * sum{(I(x-u,y-v)-Iavg)^2}}
//!		</pre>
//!
//!		PHASE_CORR (phase correlation):
//!		<pre>
//!				     {  FFT(T(x,y) * FFT*(I(x-u,y-v))  }
//!		C(u,v) = inverse FFT { ------------------------------- }
//!				     { |FFT(T(x,y) * FFT*(I(x-u,y-v))| }
//!		</pre>
//! \return	The correlation number computed by method.
//
float
IP_correlation(ImagePtr I1, ImagePtr I2, int mtd, int multires, int &xx, int &yy)
{
	// init vars to suppress compiler warnings
	int	  dx = 0;
	int	  dy = 0;
	int   lowres = 0;
	float	mag  = 0;
	float	corr = 0;
	double	xsz, ysz;

	// image dimensions
	int w = I1->width ();
	int h = I1->height();

	// template dimensions
	int ww = I2->width ();
	int hh = I2->height();

	// error checking: size of image I1 must be >= than template I2
	if(!(ww<=w && hh<=h)) {
		fprintf(stderr, "Correlation: image is smaller than template\n");
		return 0.;
	}

	// cast image into buffer of type float
	ImagePtr II1;
	if(I1->channelType(0) != FLOAT_TYPE) {
		II1 = IP_allocImage(I1->width(), I1->height(), FLOATCH_TYPE);
		IP_castChannel(I1, 0, II1, 0, FLOAT_TYPE);
	} else	II1 = I1;

	// cast template into buffer of type float
	ImagePtr II2;
	if(I2->channelType(0) != FLOAT_TYPE) {
		II2 = IP_allocImage(I2->width(), I2->height(), FLOATCH_TYPE);
		IP_castChannel(I2, 0, II2, 0, FLOAT_TYPE);
	} else	II2 = I2;

	// create image and template pyramids with original images at base;
	// if no multiresolution is used, pyramids consist of only one level.
	int mxlevel;
	ImagePtr pyramid1[8], pyramid2[8];
	pyramid1[0] = II1;		// base: original image
	pyramid2[0] = II2;		// base: original template
	if(multires) {
		// set lowest resolution for pyramid1 (empirically tested)
		switch(mtd) {
		case CROSS_CORR:
		case SSD:
			lowres = 64;
			break;
		case CORR_COEFF:
			lowres = 128;
			break;
		case PHASE_CORR:
			lowres = MAX(w,h);	// disable multires
			break;
		}

		// update lowres if pyramid2 will become too small
		int k;
		for(k=1; (w>>k) >= lowres   &&  (h>>k) >= lowres; k++);
		for(k--; (ww>>k) < 4 || (hh>>k) < 4; k--) lowres *= 2 ;

		// compute lower-res versions for remaining pyramid levels
		for(k=1; (w>>k) >= lowres && (h>>k) >= lowres; k++) {
		  IP_resize(pyramid1[k-1],  w>>k,  h>>k, TRIANGLE, pyramid1[k]);
		  IP_resize(pyramid2[k-1], ww>>k, hh>>k, TRIANGLE, pyramid2[k]);
		}
		mxlevel = k-1;
	} else	mxlevel = 0;

	// init search window
	int x1 = 0;
	int y1 = 0;
	int x2 = (w-ww)>>mxlevel;
	int y2 = (h-hh)>>mxlevel;

	// declarations
	int		  total;
	float		  sum1, sum2, avg, tmpl_pow;
	ChannelPtr<float> image, templ;
	ImagePtr	  Iblur, Ifft1, Ifft2;

	// multiresolution correlation: use results of lower-res correlation 
	// (at the top of the pyramid) to narrow the search in the higher-res
	// correlation (towards the base of the pyramid).
	for(int n=mxlevel; n>=0; n--) {
	    // init vars based on pyramid at level n
	    w  = pyramid1[n]->width(); h  = pyramid1[n]->height();
	    ww = pyramid2[n]->width(); hh = pyramid2[n]->height();

	    // pointers to image and template data
	    ChannelPtr<float> p1 = pyramid1[n][0];	// image    ptr
	    ChannelPtr<float> p2 = pyramid2[n][0];	// template ptr

	    // init min and max
	    float min = 10000000.;
	    float max = 0.;

	    switch(mtd) {
	    case CROSS_CORR:				// cross correlation
		for(int y=y1; y<=y2; y++) {		// visit rows
		    for(int x=x1; x<=x2; x++) {		// slide window
			sum1  = sum2 = 0;
			image = p1 + y*w + x;
			templ = p2;
			for(int i=0; i<hh; i++) {	// convolution
		   		for(int j=0; j<ww; j++) {
					sum1 += (templ[j] * image[j]);
					sum2 += (image[j] * image[j]);
				}
				image += w;
				templ += ww;
			}
			if(sum2 == 0) continue;

			corr = sum1 / sqrt(sum2);
			if(corr > max) {
		   		max = corr;
		   		dx  = x;
		   		dy  = y;
			}
		    }
		}

		// update search window or normalize final correlation value
		if(n) {		// set search window for next pyramid level
			x1 = MAX(0,   2*dx - n);
			y1 = MAX(0,   2*dy - n);
			x2 = MIN(2*w, 2*dx + n);
			y2 = MIN(2*h, 2*dy + n);
		} else {	// normalize correlation value at final level
			tmpl_pow = 0;
			total	 = ww * hh;
			for(int i=0; i<total; i++)
				tmpl_pow += (p2[i] * p2[i]);
			corr =	max / sqrt(tmpl_pow);
		}
		break;

	    case SSD:				// sum of squared differences
		float diff;
		for(int y=y1; y<=y2; y++) {		// visit rows
		    for(int x=x1; x<=x2; x++) {		// slide window
			sum1  = sum2 = 0;
			image = p1 + y*w + x;
			templ = p2;
			for(int i=0; i<hh; i++) {	// convolution
		   		for(int j=0; j<ww; j++) {
					diff  = templ[j] - image[j];
					sum1 += (diff * diff);
					sum2 += (image[j] * image[j]);
				}
				image += w;
				templ += ww;
			}
			if(sum2 == 0) continue;

			corr = sum1 / sqrt(sum2);
			if(corr < min) {
		   		min = corr;
		   		dx  = x;
		   		dy  = y;
			}
		     }
		}

		// update search window or normalize final correlation value
		if(n) {		// set search window for next pyramid level
			x1 = MAX(0,   2*dx - n);
			y1 = MAX(0,   2*dy - n);
			x2 = MIN(2*w, 2*dx + n);
			y2 = MIN(2*h, 2*dy + n);
		} else {	// normalize correlation value at final level
			total = ww * hh;
			float	tmpl_pow = 0;
			for(int i=0; i<total; i++)
				tmpl_pow += (p2[i] * p2[i]);
			corr =	min / sqrt(tmpl_pow);
		}
		break;

	    case CORR_COEFF:			// correlation coefficient
		// compute template average
		total = ww * hh;
		avg   = 0;
		for(int i=0; i<total; i++) avg += p2[i];
		avg /= total;

		// subtract average from template
		for(int i=0; i<total; i++) p2[i] -= avg;

		// compute template power
		tmpl_pow = 0;
		for(int i=0; i<total; i++)
			tmpl_pow += (p2[i] * p2[i]);

		// compute local image average: blur with box filter
		xsz = ww + !(ww%2);	// make filter width  odd
		ysz = hh + !(hh%2);	// make filter height odd
		IP_blur(pyramid1[n], xsz, ysz, Iblur); 

		/* subtract local image averages from pixels */
		IP_subtractImage(pyramid1[n], Iblur, pyramid1[n]);

		for(int y=y1; y<=y2; y++) {		// visit rows
		    for(int x=x1; x<=x2; x++) {		// slide window
			sum1  = sum2 = 0;
			image = p1 + y*w + x;		// avgs were subtracted
			templ = p2;			// from image & template

			float image_pow = 0;
			image = p1 + y*w + x;
			for(int i=0; i<hh; i++) {	// convolution
		   		for(int j=0; j<ww; j++) {
					sum1	  += (templ[j] * image[j]);
					image_pow += (image[j] * image[j]);
				}
				image    += w;
				templ += ww;
			}

			sum2 = image_pow * tmpl_pow;
			corr = sum1 / sqrt(sum2);
			if(corr > max) {
		   		max = corr;
		   		dx  = x;
		   		dy  = y;
			}
		    }
		}

		// update search window or set final correlation value
		if(n) {		// set search window for next pyramid level
			x1 = MAX(0,   2*dx - n);
			y1 = MAX(0,   2*dy - n);
			x2 = MIN(2*w, 2*dx + n);
			y2 = MIN(2*h, 2*dy + n);
		} else {	// set correlation value at final level
			corr = max;
		}
		break;

	    case PHASE_CORR:			// Fourier phase correlation
		IP_crop(I2, 0, 0, w, h, II2);		// pad template
		IP_fft2D(II1, 1, Ifft1);		// image    FFT (F1)
		IP_fft2D(II2, 1, Ifft2);		// template FFT (F2)
		IP_complexConjugate(Ifft2, Ifft2);	// F2* is cmplx conj
		IP_multiplyCmplx(Ifft1, Ifft2, Ifft1);	// F1 x F2*

		// divide F1 x F2* by the magnitude of (F1 x F2*)
		total = w * h;
		p1 = Ifft1[0];
		p2 = Ifft1[1];
		for(int i=0; i<total; i++) {
			mag    = MAG(*p1, *p2);
			*p1++ /= mag;
			*p2++ /= mag;
		}

		// find position of maximum in inverse FFT
		IP_fft2D(Ifft1, -1, Ifft2);
		p1 = Ifft2[0];
		p2 = Ifft2[1];
		for(int y=0; y<h; y++) {
			for(int x=0; x<w; x++) {
				mag = MAG(*p1, *p2);
				if(mag > max) {
					max = mag;
					dx  = x;
					dy  = y;
				}
				p1++;
				p2++;
			}
		}

		// update position to avoid wrap-around; allow for -dx or -dy
		if(dx > w/2) dx -= w;
		if(dy > h/2) dy -= h;
		corr = mag;
		break;

	    default:
		fprintf(stderr, "Correlation: Bad mtd %d\n", mtd);
		return 0.;
	    }
	}

	xx = dx;
	yy = dy;
	return corr;
}
//@}

}	// namespace IP
