// Giorgio Scorzelli Doc 2009
#version 120
#extension GL_EXT_texture_array : enable


uniform bool           skin_enabled;
uniform sampler2D      skin;

uniform bool           texture1_enabled;
uniform sampler2D      texture1;


uniform bool           shadow_enabled;
uniform sampler2DArray shadow_texture;
uniform vec4           shadow_bounds;

varying vec4 vPos;
varying vec3 normal, lightDir, eyeVec;


//-----------------------------------------------------------------
uniform vec4 offset0 = vec4(0.000000, 0.000000, 0.0, 0.0);
uniform vec4 offset1 = vec4(0.079821, 0.165750, 0.0, 0.0);
uniform vec4 offset2 = vec4(-0.331500, 0.159642, 0.0, 0.0);
uniform vec4 offset3 = vec4(-0.239463, -0.497250, 0.0, 0.0);
uniform vec4 offset4 = vec4(0.662999, -0.319284, 0.0, 0.0);
uniform vec4 offset5 = vec4(0.399104, 0.828749, 0.0, 0.0);
uniform vec4 offset6 = vec4(-0.994499, 0.478925, 0.0, 0.0);
uniform vec4 offset7 = vec4(-0.558746, -1.160249, 0.0, 0.0);


//-----------------------------------------------------------------
float getOccCoef(vec4 shadow_coord)
{
	// get the stored depth
	float shadow_d = texture2DArray(shadow_texture, shadow_coord.xyz).x;
	
	// get the difference of the stored depth and the distance of this fragment to the light
	float diff = shadow_d - shadow_coord.w;
	
	// smoothen the result a bit, so that we don't get hard shadows
	return clamp( diff*250.0 + 1.0, 0.0, 1.0);
}

//-----------------------------------------------------------------
float shadowCoef()
{
	const float scale = 2.0/4096.0;
	
	// find the appropriate depth map to look up in based on the depth of this fragment
	int index = 3;
	
	     if(gl_FragCoord.z < shadow_bounds.x) index = 0;
	else if(gl_FragCoord.z < shadow_bounds.y) index = 1;
	else if(gl_FragCoord.z < shadow_bounds.z) index = 2;
	
	// transform this fragment's position from world space to scaled light clip space
	// such that the xy coordinates are in [0;1]
	vec4 shadow_coord = gl_TextureMatrix[index+2]*vPos; //note, the texture matrices begins from 2
	shadow_coord.w=shadow_coord.z;
	shadow_coord.z=float(index);
	
    // sum shadow samples	 
	return
		(
			  getOccCoef(shadow_coord + scale*offset0)
			+ getOccCoef(shadow_coord + scale*offset1)
			+ getOccCoef(shadow_coord + scale*offset2)
			+ getOccCoef(shadow_coord + scale*offset3)
			+ getOccCoef(shadow_coord + scale*offset4)
			+ getOccCoef(shadow_coord + scale*offset5)
			+ getOccCoef(shadow_coord + scale*offset6)
			+ getOccCoef(shadow_coord + scale*offset7)
		)
		/8.0;
}



//-----------------------------------------------------------------
void main (void)
{

	float alpha=1.0;

	
	vec3 rgb_color;

	
	if (texture1_enabled) 
	{
		vec4 texture1_color = texture2D(texture1, gl_TexCoord[1].st);
		rgb_color = texture1_color.rgb;
	}
	else
	{
		//********** ambient term **********
		rgb_color = (gl_FrontLightModelProduct.sceneColor.rgb * gl_FrontMaterial.ambient.rgb) + (gl_LightSource[0].ambient.rgb * gl_FrontMaterial.ambient.rgb);
	

		//********** diffuse term **********		
		vec3 N = normalize(normal);
		vec3 L = normalize(lightDir);
		float lambertTerm = dot(N,L);

		if(lambertTerm > 0.0)
		{
			rgb_color += gl_LightSource[0].diffuse.rgb * gl_FrontMaterial.diffuse.rgb * lambertTerm;
			alpha*=gl_FrontMaterial.diffuse.a;

			//********** specular term **********	
			vec3 E = normalize(eyeVec);
			vec3 R = reflect(-L, N);
			float specular_quantity = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess);
			rgb_color += gl_LightSource[0].specular.rgb * gl_FrontMaterial.specular.rgb * specular_quantity;	
		}
	}	
				

	//skin (modulate)
	if (skin_enabled) 
	{
		vec4 skin_color = texture2D(skin, gl_TexCoord[0].st);
		rgb_color *= skin_color.rgb;
		alpha *= skin_color.a;

	}

	//shadow
	if (shadow_enabled)
	{
		float shadow_quantity=(0.8+0.2*shadowCoef());
		rgb_color *=vec3(shadow_quantity,shadow_quantity,shadow_quantity);
	}


	gl_FragColor = vec4(rgb_color,alpha);			
}

