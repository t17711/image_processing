#version 120

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
uniform int u_Color;

void main() {
	vec2 tc  = v_TexCoord;
	vec3 sm1 =vec3(0.0f);
	vec3 sm2 =vec3(1.0f);
	float sum1 = 1.0f;
	float sum2 = 1.0f;
	vec3 image=	vec3(0.0f);
	vec3 kernel = vec3(0.0f);
	vec3 pt1 = vec3(0.0);
	vec3 pt2 = vec3(0.0);

	#if 1
	vec3 s1= vec3(0.3f,0.6f,0.1f);
	//vec3 s2= vec3(0.0);

	for(int i = 0; i<= u_Hsize_k; i+=8){
		for (int j =0; j <= u_Wsize_k; j+=8){
		//!		CROSS_CORR (cross correlation):
		//!		<pre>
		//!		C(u,v) = sum of {T(x,y) * I(x-u,y-v)}
		//!			 --------------------------------
		//!			 sqrt{ sum of {I(x-u,y-v)^2}}
			pt1 = texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j *u_HStep_s)).rgb;
			pt2 = texture2D(u_Kernel,vec2(i*u_WStep_k, j *u_HStep_k)).rgb;
			sm1 += pt1*pt2;
			sm2 += pt1*pt1;		
		}
	}

	sm1 = sm1/sqrt(sm2);

	sm1 = s1*sm1/75.0;;

	sum1 = sm1.r+sm1.g+sm1.b;
	gl_FragColor = vec4(sum1,sum1,sum1,1.0);
	#else
			pt1 = (texture2D(u_Sampler,v_TexCoord)).rgb;
			pt2 = (texture2D(u_Kernel,v_TexCoord)).rgb;

			gl_FragColor = vec4(pt1+pt2,1.0);
		
	#endif

}