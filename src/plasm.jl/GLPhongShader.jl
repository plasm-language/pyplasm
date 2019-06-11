# /////////////////////////////////////////////////////////////////////
vertex_source="""
uniform mat4 u_modelview_matrix;
uniform mat4 u_projection_matrix;
uniform vec4 u_color;

attribute  vec4 a_position;

#if LIGHTING_ENABLED
attribute  vec3 a_normal;
#endif

#if COLOR_ATTRIBUTE_ENABLED
attribute vec4 a_color;
#endif

#if LIGHTING_ENABLED
uniform mat3 u_normal_matrix;
uniform vec3 u_light_position;
varying vec3 v_normal;
varying vec3 v_light_dir;
varying vec3 v_eye_vec;
#endif

#if COLOR_ATTRIBUTE_ENABLED
  varying vec4 v_color;
#endif

void main() 
{
	vec4 eye_pos= u_modelview_matrix * a_position;
	
#if LIGHTING_ENABLED	
	v_normal = u_normal_matrix * a_normal;
	vec3 vVertex = vec3(u_modelview_matrix * a_position);
	v_light_dir  = normalize(u_light_position - vVertex);
	v_eye_vec    = normalize(-vVertex);
#endif	

#if COLOR_ATTRIBUTE_ENABLED
	v_color=a_color;
#endif
	
	gl_Position = u_projection_matrix * eye_pos;
}
"""


# /////////////////////////////////////////////////////////////////////
fragment_source="""
uniform vec4 u_color;

#if LIGHTING_ENABLED
varying vec3 v_normal;
varying vec3 v_light_dir;
varying vec3 v_eye_vec;
#endif

#if COLOR_ATTRIBUTE_ENABLED
  varying vec4 v_color;
#endif

void main() 
{
	vec4 frag_color=u_color; 
	
  #if LIGHTING_ENABLED
	vec3 N = normalize(v_normal   );
	vec3 L = normalize(v_light_dir);
	vec3 E = normalize(v_eye_vec  );

	vec4  u_material_ambient  = vec4(0.2,0.2,0.2,1.0);
	vec4  u_material_diffuse  = vec4(0.8,0.8,0.8,1.0);
	vec4  u_material_specular = vec4(0.1,0.1,0.1,1.0);
	float u_material_shininess=100.0;	
	
	if(gl_FrontFacing)
	{
		frag_color = u_material_ambient;
		float NdotL = abs(dot(N,L));
		if (NdotL>0.0)
			{
			vec3 R = reflect(-L, N);
			float NdotHV = abs(dot(R, E));
			frag_color += u_material_diffuse * NdotL;
			frag_color += u_material_specular * pow(NdotHV,u_material_shininess);
		}
	}
	else
	{
		frag_color = u_material_ambient;
		float NdotL = abs(dot(-N,L));
		if (NdotL>0.0);
		{
			vec3 R = reflect(-L, -N);
			float NdotHV=abs(dot(R, E));
			frag_color += u_material_diffuse * NdotL;
			frag_color += u_material_specular * pow(NdotHV,u_material_shininess);
		}
	}
#endif

#if COLOR_ATTRIBUTE_ENABLED
	frag_color =v_color;
#endif

	gl_FragColor = frag_color;
}
"""


# /////////////////////////////////////////////////////////////////////
function GLPhongShader(lighting_enabled,color_attribute_enabled)
	
	defines=""
	if lighting_enabled
		defines=string(defines , "#define LIGHTING_ENABLED 1\n")
	end	
		
	if color_attribute_enabled
		defines=string(defines , "#define COLOR_ATTRIBUTE_ENABLED 1\n")
	end
	
	return GLShader(
		string(defines , vertex_source),
		string(defines , fragment_source))	
end

