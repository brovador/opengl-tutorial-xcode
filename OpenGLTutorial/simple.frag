#version 330 core
uniform vec3 ViewPos;
uniform vec3 LightPos;

in vec3 Normal;
in vec3 FragPos;
in vec3 ObjectColor;

out vec3 FragColor;

void main() {
    vec3 ambient = vec3(0.01, 0.01, 0.01);
    vec3 lightColor = vec3(1, 1, 1);
    float specularStrength = 3.0;
    
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * ObjectColor;
    FragColor = result;
}
