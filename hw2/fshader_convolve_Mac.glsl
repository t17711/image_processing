#version 120

varying	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform int       u_Wsize;	// blur width value
uniform int       u_Hsize;	// blur height value
uniform float	  u_WStep;
uniform float	  u_HStep;
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform	float[1000] u_Weight; // uniform variable for the texture image

void main() {
	vec3 avg = vec3(0.0);
	vec2 tc  = v_TexCoord;
	int  w2  = u_Wsize / 2;
	int  h2  = u_Hsize / 2;

	int sz = (u_Wsize*u_Hsize);
	float wt_hstep = 1.0/u_Hsize;
	float wt_wstep = 1.0/u_Wsize;

	int ii =0;
	for(int j=-h2; j<=h2; ++j){
		for(int i=-w2; i<=w2; ++i){
			avg += (texture2D(u_Sampler, vec2(tc.x + i*u_WStep, tc.y +j*u_HStep )).rgb*u_Weight[ii]);
			ii++;
		}
	}

	gl_FragColor = vec4(avg, 1.0);
	
}