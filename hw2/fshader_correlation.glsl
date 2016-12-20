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
uniform int u_Color;
uniform int u_filter;
uniform float u_normalizor;

void main() {

	vec2 tc  = v_TexCoord;
	
	vec3 sum1 = vec3(0.0f);
	vec3 sum2 = vec3(0.0f);

	
	vec3 pt1 = vec3(0.0);
	vec3 pt2 = vec3(0.0);

	int  w2  = u_Wsize_k/2;
    int  h2  = u_Hsize_k/2;
	
	if (u_filter == 1){
		vec3 s= vec3(0.2126f,0.7152f,0.0722f);

		for(int j= -h2; j< h2; j++){
			for (int i =-w2; i < w2; i++){


				//	CROSS_CORR (cross correlation):
				//		C(u,v) = sum of {T(x,y) * I(x-u,y-v)}
				//			 --------------------------------
				//			 sqrt{ sum of {I(x-u,y-v)^2}}

				pt1 = (texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j*u_HStep_s)).rgb);
				pt2 = (texture2D(u_Kernel,vec2(0.5+i*u_WStep_k,0.5+ j*u_HStep_k)).rgb);
			
				
				sum1 += pt1*pt2;
				sum2 += pt1*pt1;
			}
		}


		
		float out1 = (dot(sum1/sqrt(sum2)/u_normalizor,s));
		

		gl_FragColor = vec4(vec3(out1),1.0f);	
		//gl_FragColor = (texture2D(u_Kernel,v_TexCoord));

	}
	else{
			gl_FragColor = (texture2D(u_Sampler,v_TexCoord));
	}

}