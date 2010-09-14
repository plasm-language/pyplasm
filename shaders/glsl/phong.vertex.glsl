// Giorgio Scorzelli Dic 2009

varying vec3 normal, lightDir, eyeVec;
varying vec4 vPos;



void main()
{	
	normal = gl_NormalMatrix * gl_Normal;

	vPos = gl_ModelViewMatrix * gl_Vertex;

	lightDir = gl_LightSource[0].position.xyz - vPos.xyz;

	eyeVec = -vPos.xyz;

	gl_Position = ftransform();	

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;	
}

