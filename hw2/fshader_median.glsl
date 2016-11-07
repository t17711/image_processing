#version 330

#define MXGRAYF 255.0
#define MXGRAY 256

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform int       u_Wsize;	// blur width value
uniform int       u_Hsize;	// blur height value
uniform float	  u_WStep;
uniform float	  u_HStep;
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
ivec3 hist[MXGRAY];

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
	
	for(int i=0;i<MXGRAY;i++){
		hist[i] = ivec3(0,0,0);
	}
	
	
	// create histogram
	for(int i=-w1; i<=w1; i++)
			for(int j=-w2; j<=w2; j++){
				tmp = texture2D(u_Sampler, vec2(tc.x + i*u_WStep, tc.y + j*u_HStep)).rgb;
				r = int (tmp.r * MXGRAYF);
				g =	int (tmp.g * MXGRAYF);
				b =	int (tmp.b * MXGRAYF);

				hist[r].r+=1;
				hist[g].g+=1;
				hist[b].b+=1;		
			}
		
	int m = 0;
	int k = 0;
	for(k = 0; m<med && k<MXGRAY; k++){
		m+= hist[k].r;
	}
	tmp.r = float(k/MXGRAYF);
	
	m = 0;
	for(k = 0; m<med && k<MXGRAY; k++){
		m+= hist[k].g;
	}
	tmp.g= float(k/MXGRAYF);
	
	m = 0;
	for(k = 0; m<med && k<MXGRAY; ++k){
		m+= hist[k].b;
	}
	tmp.b= float(k/MXGRAYF);
	
	gl_FragColor = vec4(tmp, 1.0);
	//gl_FragColor = texture2D(u_Sampler, v_TexCoord);

}