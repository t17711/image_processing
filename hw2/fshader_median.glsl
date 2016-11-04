#version 330

#define MXGRAY 256
in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform int       u_Wsize;	// blur width value
uniform int       u_Hsize;	// blur height value
uniform float	  u_WStep;
uniform float	  u_HStep;
uniform	sampler2D u_Sampler;	// uniform variable for the texture image

void main() {
	vec3 tmp = vec3(0.0);
	vec2 tc  = v_TexCoord;

	int  w1  = u_Wsize / 2;
	int  w2  = u_Hsize / 2;
	int r 	=0;
	int g	=0;
	int b 	=0;
	int size = u_Wsize*u_Hsize; // size of buffer
	int med = ((size - 1)/ 2) +1; //median of 3 is 3/2 +1  = 1+1 =2
	
	int hist_r[MXGRAY];
	int hist_g[MXGRAY];
	int hist_b[MXGRAY];
	
	// create histogram
	for(int i=-w1; i<=w1; ++i)
			for(int j=-w2; j<=w2; ++j)
			{
			tmp = texture2D(u_Sampler, vec2(tc.x + i*u_WStep, tc.y + j*u_HStep)).rgb;
			r = int (tmp.r * MXGRAY);
			g =	int (tmp.g * MXGRAY);
			b =	int (tmp.b * MXGRAY);

			hist_r[r]++;
			hist_g[g]++;
			hist_b[b]++;
			
			}
		
	int m = 0;
	int k = 0;
	for(k = 0; m<med && k<MXGRAY; ++k){
		m+= hist_r[k];
	}
	tmp.r = float(k/MXGRAY);
	
	m = 0;
	for(k = 0; m<med && k<MXGRAY; ++k){
		m+= hist_g[k];
	}
	tmp.g= float(k/MXGRAY);
	
	m = 0;
	for(k = 0; m<med && k<MXGRAY; ++k){
		m+= hist_b[k];
	}
	tmp.b= float(k/MXGRAY);
	
	gl_FragColor = vec4(tmp, 1.0);
	//gl_FragColor = texture2D(u_Sampler, v_TexCoord);

}