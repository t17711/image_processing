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

		for(int i = 0; i< u_Hsize_k; i++){
			for (int j =0; j < u_Wsize_k; j++){


//			SSD (sum of squared differences):
//!		<pre>
//!		C(u,v) = sum of {T(x,y)-I(x-u,y-v)}^2
//!			 ------------------------------
//!			 sqrt{ sum of {I(x-u,y-v)^2}}
//!		</pre>

				pt1 = (texture2D(u_Sampler,vec2(tc.x + i*u_WStep_s, tc.y + j *u_HStep_s)).rgb);
				pt2 = (texture2D(u_Kernel,vec2(i*u_WStep_k, j *u_HStep_k)).rgb);
				
				vec3 a = (pt2-pt1);
				//vec3 b = (pt1-iavg);

				sm1 += a*a;
				sm2 += pt1*pt1;	
				//sm3 += b*b;	
			}
		}

		sm1= s*normalize(sm1/sqrt(sm2));
		
		sum1 = (sm1.r+sm1.g+sm1.b);
	
		gl_FragColor = vec4(sum1,sum1,sum1,sum1);
	}
	else{
			gl_FragColor = (texture2D(u_Sampler,v_TexCoord));
	}

}