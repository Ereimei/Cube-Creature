#version 330 core

uniform sampler2D ourTexture; 
uniform sampler2D shadowMap;

uniform vec3 l_p;
uniform vec3 c_p;

out vec4 color;

in vec3 nrm;
in vec3 fpo;
in vec2 uv;
in vec4 fpoLightSpace;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(nrm);
    vec3 lightDir = normalize(l_p - fpo);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
	vec4 col = texture(ourTexture, uv);
	vec3 light_color = vec3(1,1,1);
	vec3 ambient = 0.3 * light_color;

	vec3 ld = normalize(l_p - fpo);
	vec3 n = normalize(nrm);
	float angle = dot(n, ld);
	float diff = max(0.0, angle);
	vec3 diffuse = 0.8 * diff * light_color;

	float specularStrength = 0.8;
	vec3 viewDir = normalize(c_p - fpo);
	vec3 reflectDir = reflect(-ld, n);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * light_color;

	float shadow = ShadowCalculation(fpoLightSpace); 
	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular));
	color = vec4(result, 1.0)* col;
}