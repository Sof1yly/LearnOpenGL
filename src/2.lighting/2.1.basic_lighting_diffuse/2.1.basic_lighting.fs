#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

#define NR_LIGHTS 2

uniform vec3 lightPos[NR_LIGHTS];   // 0 = sun, 1 = lamp inside the house
uniform vec3 lightColor[NR_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D texture1;   // diffuse
uniform sampler2D specularMap;

void main()
{
    vec3 objectColor = texture(texture1, TexCoord).rgb;
    float specularMask = texture(specularMap, TexCoord).r;

    // ambient (a bit of light everywhere so faces turned away from the light are not black)
    vec3 result = 0.3 * lightColor[0] * objectColor;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    for (int i = 0; i < NR_LIGHTS; i++)
    {
        vec3 toLight = lightPos[i] - FragPos;
        float dist = length(toLight);
        vec3 lightDir = toLight / dist;

        // diffuse
        float diff = max(dot(norm, lightDir), 0.0);

        // specular (strength comes from the specular map)
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

        // the sun is not attenuated, the local lamps fade with distance
        float att = (i == 0) ? 1.0 : 1.0 / (1.0 + 0.14 * dist + 0.07 * dist * dist);

        result += att * lightColor[i] * (diff * objectColor + specularMask * spec);
    }

    FragColor = vec4(result, 1.0);
}
