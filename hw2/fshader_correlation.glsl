#version 330

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

// for sample get its step and colors
uniform float u_WStep_s;
uniform float u_HStep_s;

// for kernel
uniform int       u_Wsize_k;	
uniform int       u_Hsize_k;
uniform float	  u_WStep_k;
uniform float	  u_HStep_k;
uniform	sampler2D u_Sampler;
uniform	sampler2D u_Kernel; 

void main() {
	vec2 tc  = v_TexCoord;
	float sum1 = 1.0f;
	float sum2 = 1.0f;
	float image=0.0f;
	float kernel = 0.0f;
	vec3 pt1 = vec3(0.0);
	vec3 pt2 = vec3(0.0);

	//vec3 s1= vec3(0.0);
	//vec3 s2= vec3(0.0);

	for(int i = 0; i< u_Hsize_k; ++i){
		for (int j =0; j < u_Wsize_k; ++j){
		//!		CROSS_CORR (cross correlation):
		//!		<pre>
		//!		C(u,v) = sum of {T(x,y) * I(x-u,y-v)}
		//!			 --------------------------------
		//!			 sqrt{ sum of {I(x-u,y-v)^2}}

		pt1 = texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j *u_HStep_s)).rgb;
		pt2 = texture2D(u_Kernel,vec2(i*u_WStep_k, j *u_HStep_k)).rgb;

			// get grey value
		image = 0.3*pt1.r + 0.6* pt1.g + 0.1 *pt1.b;
		kernel=  0.3*pt2.r + 0.6* pt2.g + 0.1 *pt2.b;
		sum1 += (image*kernel);
			sum2 += (image*image);
				
			//s1+= pt1*pt2;
			//s2+= pt1*pt1;
		}
	}
	
	if (sum2 == 0) sum2 =1.0f;
	else sum2 = sqrt(sum2);
	float c = (sum1/ sum2);

	#if 0
		gl_FragColor = vec4(clamp(c,0.0,1.0),0.0,0.0,1.0);
	#else
		gl_FragColor = vec4(texture2D(u_Kernel,tc));
	#endif

}