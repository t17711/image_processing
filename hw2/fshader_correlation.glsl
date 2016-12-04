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

void main() {

	vec2 tc  = v_TexCoord;
	vec3 sm1 =vec3(0.0f);
	vec3 sm2 =vec3(0.0f);
	vec3 sm3 =vec3(0.0f);
	float sum1 = 1.0f;
	float sum2 = 1.0f;
	vec3 image=	vec3(0.0f);
	vec3 kernel = vec3(0.0f);
	vec3 pt1 = vec3(0.0);
	vec3 pt2 = vec3(0.0);

	vec3 tavg = vec3(0.0);
	vec3 iavg = vec3(0.0);

	if (u_passthrough == 0 ){
		vec3 s= vec3(0.3f,0.6f,0.1f);
		//vec3 s2= vec3(0.0);

		int factor = 1; // because gpu hanged

		for(int i = 0; i<= u_Hsize_k; i+=factor){
			for (int j =0; j <= u_Wsize_k; j+=factor){
				pt1 = (texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j *u_HStep_s)).rgb);
				pt2 = (texture2D(u_Kernel,vec2(i*u_WStep_k, j *u_HStep_k)).rgb);
				tavg+= pt1;
				iavg+= pt2;
				}
			}

			tavg/=(u_Hsize_k*u_Wsize_k);
			iavg/=(u_Hsize_k*u_Wsize_k);

			tavg*=factor;
			iavg*=factor;

		for(int i = 0; i<= u_Hsize_k; i+=factor){
			for (int j =0; j <= u_Wsize_k; j+=factor){
				
			//	CORR_COEFF (correlation coefficient):
			//		C(u,v) = sum of {(T(x,y)-Tavg) * (I(x-u,y-v)-Iavg)}
			//			 -----------------------------------------------------
			//			 sqrt{sum{(T(x,y)-Tavg)^2} * sum{(I(x-u,y-v)-Iavg)^2}}

				pt1 = (texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j *u_HStep_s)).rgb);
				pt2 = (texture2D(u_Kernel,vec2(i*u_WStep_k, j *u_HStep_k)).rgb);
				vec3 a = (pt2-tavg);
				vec3 b = (pt1-iavg);

				sm1 += a*b;
				sm2 += a*a;	
				sm3 += b*b;	
			}
		}

		sm2= inversesqrt(sm2*sm3);
		
		sm1 = sm1*sm2*s;

		if(u_Color ==1){
				sum1 = sm1.r+sm1.g+sm1.b;
		}
		else 
			sum1 = sm1.r;
		gl_FragColor = vec4(sum1,sum1,sum1,sum1);
	}
	else{
			gl_FragColor = (texture2D(u_Sampler,v_TexCoord));
	}

}