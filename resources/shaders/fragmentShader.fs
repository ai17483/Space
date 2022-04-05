#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //vec3 lightPos;
    //vec3 direction;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{

    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intesity = clamp((theta - light.outerCutOff)/ epsilon, 0, 1);

    vec3 ambient = texture(material.diffuse, TexCoords).rgb * light.ambient;

    vec3 norm = normalize(Normal);

    //vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = texture(material.diffuse, TexCoords).rgb * diff * light.diffuse;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = texture(material.specular, TexCoords).rgb * spec * light.specular;

    float distance = length(light.position - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation * intesity;
    specular *= attenuation * intesity;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);


}