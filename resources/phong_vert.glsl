#version 120

attribute vec4 vertPos;
attribute vec3 vertNor;
uniform mat4 P;
uniform mat4 MV;
varying vec3 fragPos;
varying vec3 fragNor;

void main()
{
	vec4 posCam = MV * vertPos;
	gl_Position = P * posCam;
	fragPos = posCam.xyz;
	fragNor = (MV * vec4(vertNor, 0.0)).xyz;
}
