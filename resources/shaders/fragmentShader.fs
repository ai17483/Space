#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 lightPos;
};

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
    vec3 ambient = material.ambient * light.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.diffuse;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);
    //Texture1 = texture(t0, outTexCords);

}