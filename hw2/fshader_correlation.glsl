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
uniform int u_passthrough;
uniform float u_normalizor;

void main() {

	vec2 tc  = v_TexCoord;
	
	vec3 sum1 = vec3(0.0f);
	vec3 sum2 = vec3(0.0f);

	
	vec3 pt1 = vec3(0.0);
	vec3 pt2 = vec3(0.0);
	
	if (u_passthrough == 0 ){
		vec3 s= vec3(0.3f,0.6f,0.1f);

		for(int j= 0; j< u_Hsize_k; j++){
			for (int i =0; i < u_Wsize_k; i++){


				//	CROSS_CORR (cross correlation):
				//		C(u,v) = sum of {T(x,y) * I(x-u,y-v)}
				//			 --------------------------------
				//			 sqrt{ sum of {I(x-u,y-v)^2}}

				pt1 = (texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j*u_HStep_s)).rgb);
				pt2 = (texture2D(u_Kernel,vec2(i*u_WStep_k, j*u_HStep_k)).rgb);
			
				
				sum1 += pt1*pt2; // multiply with grey template
				sum2 += (pt1*pt1);	
			}
		}



		sum1 = sum1/sqrt(sum2);
		
		float out1 = 0.0f;
		if(u_Color == 0){
			out1 = dot(sum1, s);
		}

		else{
			out1 = sum1.r;
		}

		out1 = clamp(out1/u_normalizor,0.0,1.0);

		gl_FragColor = vec4(vec3(out1),1.0);
	}
	else{
			gl_FragColor = (texture2D(u_Sampler,v_TexCoord));
	}

}