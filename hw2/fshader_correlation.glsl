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

	float sum =0.0f;
	
	if (u_passthrough == 0 ){
		vec3 s= vec3(0.2126f,0.7152f,0.0722f);

		for(int j= 0; j< u_Hsize_k; j++){
			for (int i =0; i < u_Wsize_k; i++){


				//	CROSS_CORR (cross correlation):
				//		C(u,v) = sum of {T(x,y) * I(x-u,y-v)}
				//			 --------------------------------
				//			 sqrt{ sum of {I(x-u,y-v)^2}}

				pt1 = (texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j*u_HStep_s)).rgb);
				pt2 = (texture2D(u_Kernel,vec2(i*u_WStep_k, j*u_HStep_k)).rgb);
			
				float t = dot(pt2,s);
				sum1 += pt1*t; // multiply with grey template
				sum2 += (pt1*pt1);	

				sum+=pow(t,2);
			}
		}


		
		float out1 = 0.0f;
		if(u_Color == 0){
			float t1 = dot(sum1,s);
			float t2 = dot(sum2,s);

			out1 = t1/sqrt(t2)/sqrt(sum);
			
		}

		else{
			out1 = sum1.r/sqrt(sum2.r)/sqrt(sum);
		}

		gl_FragColor = vec4(out1);
	}
	else{
			gl_FragColor = (texture2D(u_Sampler,v_TexCoord));
	}

}