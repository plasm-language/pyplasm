attribute vec3 tangent;
varying vec3 eyeVec;

void main() 
{ 
     gl_TexCoord[0] = gl_MultiTexCoord0; 
     
     vec3 binormal=cross(gl_Normal,tangent);
 
     mat3 TBN_Matrix;// = mat3(tangent, binormal, gl_Normal);  
     TBN_Matrix[0] =  gl_NormalMatrix * tangent; 
     TBN_Matrix[1] =  gl_NormalMatrix * binormal; 
     TBN_Matrix[2] =  gl_NormalMatrix * gl_Normal; 
     vec4 Vertex_ModelView = gl_ModelViewMatrix * gl_Vertex; 
     eyeVec = vec3(-Vertex_ModelView) * TBN_Matrix ;      
     // Vertex transformation 
     gl_Position = ftransform(); 
}
