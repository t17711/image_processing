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

	for(int i=-h2, ii =0; i<=h2; ++i){
		for(int j=-w2; j<=w2; ++j,++ii){
			if (ii==sz) ii = 0;// just for security
			avg += (texture2D(u_Sampler, vec2(tc.x + i*u_WStep, tc.y + j*u_HStep)).rgb*u_Weight[ii]);
		}
	}
	//avg = avg / (u_Wsize * u_Hsize);
	gl_FragColor = vec4(avg, 1.0);
	
}