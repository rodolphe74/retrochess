#version 150 core

in vec3 fPos;

in vec3 fMaterial_ambient;
in vec3 fMaterial_diffuse;
in vec3 fMaterial_specular;
in float fMaterial_shininess;

in vec2 fUv;
in float fTid;
in float fWhich;
in vec3 fNorm;
out vec4 outColor;

uniform vec3 camera_position;

uniform float lights[7*5];
uniform int lights_count;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform sampler2D texture_0;

vec3 calcPointLight(vec3 ambient, vec3 viewPos, vec3 lightPos, vec3 lightColor, vec3 normal, vec3 fragPos,  vec3 materialDiffuse, vec3 materialSpecular, float shininess)
{
    // diffuse shading
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * materialDiffuse);

    // specular shading
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = lightColor * (spec * materialSpecular);

    return (ambient + diffuse + specular);
}


void main()
{

    // https://learnopengl.com/Lighting/Basic-Lighting
    
    // utilisé pour diffuse et specular dans calcPointLight
    vec3 fragPos = vec3(model_matrix * vec4(fPos, 1.0));
    vec3 normal = mat3(transpose(inverse(model_matrix))) * fNorm;

    int which = int(fWhich);
    if (which == 0) {
        vec3 result = vec3(0, 0, 0);
        for (int i = 0; i < lights_count; i++) {
            vec3 lightColor = vec3(lights[i*7+3], lights[i*7+4], lights[i*7+5]);
            vec3 lightPos = vec3(lights[i*7], lights[i*7+1], lights[i*7+2]);
            vec3 ambient = lightColor * fMaterial_ambient;
            result += calcPointLight(ambient, camera_position, lightPos, lightColor, normal, fragPos, fMaterial_diffuse, fMaterial_specular, fMaterial_shininess);
        }
        outColor = vec4(result, 1.0);
    } else if (which == 1) {
        // Affichage du texte - plaquage texture sur rectangle
        vec3 objectColor = texture2D(texture_0, fUv).xyz;

        if (objectColor == vec3(0,0,0)) {
            // le fond noir est plus ou moins transparent
            outColor = vec4(objectColor, 0.3);
        } else {
            outColor = vec4(objectColor, 1.0);
        }

    } else if (which == 2) {

    } else {
        outColor = vec4(0.0, 1.0, 1.0, 1.0);
    }

};
