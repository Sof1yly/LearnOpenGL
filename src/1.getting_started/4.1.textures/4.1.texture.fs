#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform float sepia; 

void main()
{
	vec4 color = texture(texture1, TexCoord);
	vec3 sepiaColor = vec3(
		dot(color.rgb, vec3(0.393, 0.769, 0.189)),
		dot(color.rgb, vec3(0.349, 0.686, 0.168)),
		dot(color.rgb, vec3(0.272, 0.534, 0.131)));
	color.rgb = mix(color.rgb, sepiaColor, sepia);
	FragColor = color;
}
