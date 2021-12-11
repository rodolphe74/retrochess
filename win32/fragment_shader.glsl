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
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform sampler2D texture_4;
uniform sampler2D texture_5;
uniform sampler2D texture_6;
uniform sampler2D texture_7;
uniform sampler2D texture_8;
uniform sampler2D texture_9;
uniform sampler2D texture_10;
uniform sampler2D texture_11;
uniform sampler2D texture_12;
uniform sampler2D texture_13;
uniform sampler2D texture_14;
uniform sampler2D texture_15;
uniform sampler2D texture_16;
uniform sampler2D texture_17;
uniform sampler2D texture_18;
uniform sampler2D texture_19;
uniform sampler2D texture_20;
uniform sampler2D texture_21;
uniform sampler2D texture_22;
uniform sampler2D texture_23;
uniform sampler2D texture_24;
uniform sampler2D texture_25;
uniform sampler2D texture_26;
uniform sampler2D texture_27;
uniform sampler2D texture_28;
uniform sampler2D texture_29;
uniform sampler2D texture_30;
uniform sampler2D texture_31;

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

    int tid = int(fTid);
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
        vec3 result = vec3(0, 0, 0);
        for (int i = 0; i < lights_count; i++) {
            vec3 lightColor = vec3(lights[i*7+3], lights[i*7+4], lights[i*7+5]);
            vec3 lightPos = vec3(lights[i*7], lights[i*7+1], lights[i*7+2]);
            vec3 ambient = lightColor * fMaterial_ambient;
            result += calcPointLight(ambient, camera_position, lightPos, lightColor, normal, fragPos, fMaterial_diffuse, fMaterial_specular, fMaterial_shininess);
        }
        vec3 objectColor;
        vec3 phong;
        switch (tid)
        {
        case 0:
            objectColor = texture2D(texture_0, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 1:
            objectColor = texture2D(texture_1, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 2:
            objectColor = texture2D(texture_2, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 3:
            objectColor = texture2D(texture_3, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 4:
            objectColor = texture2D(texture_4, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 5:
            objectColor = texture2D(texture_5, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 6:
            objectColor = texture2D(texture_6, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 7:
            objectColor = texture2D(texture_7, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 8:
            objectColor = texture2D(texture_8, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 9:
            objectColor = texture2D(texture_9, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 10:
            objectColor = texture2D(texture_10, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 11:
            objectColor = texture2D(texture_11, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 12:
            objectColor = texture2D(texture_12, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 13:
            objectColor = texture2D(texture_13, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 14:
            objectColor = texture2D(texture_14, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 15:
            objectColor = texture2D(texture_15, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 16:
            objectColor = texture2D(texture_16, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 17:
            objectColor = texture2D(texture_17, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 18:
            objectColor = texture2D(texture_18, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 19:
            objectColor = texture2D(texture_19, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 20:
            objectColor = texture2D(texture_20, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 21:
            objectColor = texture2D(texture_21, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 22:
            objectColor = texture2D(texture_22, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 23:
            objectColor = texture2D(texture_23, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 24:
            objectColor = texture2D(texture_24, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 25:
            objectColor = texture2D(texture_25, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 26:
            objectColor = texture2D(texture_26, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 27:
            objectColor = texture2D(texture_27, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 28:
            objectColor = texture2D(texture_28, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 29:
            objectColor = texture2D(texture_29, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 30:
            objectColor = texture2D(texture_30, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        case 31:
            objectColor = texture2D(texture_31, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        default:
            objectColor = texture2D(texture_0, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            outColor = vec4(phong, 1.0);
            break;
        }



} else if (which == 2) {
        vec4 firstColor = vec4(fMaterial_ambient, 1.0);
        vec4 secondColor;
// 		vec3 result = (ambient + diffuse);
        vec3 result = vec3(0, 0, 0);
        for (int i = 0; i < lights_count; i++) {
            vec3 lightColor = vec3(lights[i*7+3], lights[i*7+4], lights[i*7+5]);
            vec3 lightPos = vec3(lights[i*7], lights[i*7+1], lights[i*7+2]);
            vec3 ambient = lightColor * fMaterial_ambient;
            result += calcPointLight(ambient, camera_position, lightPos, lightColor, normal, fragPos, fMaterial_diffuse, fMaterial_specular, fMaterial_shininess) /** fMaterial_ambient*/;
        }
        vec4 lighting = vec4(result, 1.0);
        vec3 objectColor;
        vec3 phong;
        switch (tid)
        {
        case 0:
            objectColor = texture2D(texture_0, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 1:
            objectColor = texture2D(texture_1, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 2:
            objectColor = texture2D(texture_2, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 3:
            objectColor = texture2D(texture_3, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 4:
            objectColor = texture2D(texture_4, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 5:
            objectColor = texture2D(texture_5, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 6:
            objectColor = texture2D(texture_6, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 7:
            objectColor = texture2D(texture_7, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 8:
            objectColor = texture2D(texture_8, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 9:
            objectColor = texture2D(texture_9, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 10:
            objectColor = texture2D(texture_10, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 11:
            objectColor = texture2D(texture_11, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 12:
            objectColor = texture2D(texture_12, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 13:
            objectColor = texture2D(texture_13, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 14:
            objectColor = texture2D(texture_14, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 15:
            objectColor = texture2D(texture_15, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 16:
            objectColor = texture2D(texture_16, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 17:
            objectColor = texture2D(texture_17, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 18:
            objectColor = texture2D(texture_18, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 19:
            objectColor = texture2D(texture_19, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 20:
            objectColor = texture2D(texture_20, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 21:
            objectColor = texture2D(texture_21, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 22:
            objectColor = texture2D(texture_22, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 23:
            objectColor = texture2D(texture_23, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 24:
            objectColor = texture2D(texture_24, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 25:
            objectColor = texture2D(texture_25, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 26:
            objectColor = texture2D(texture_26, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 27:
            objectColor = texture2D(texture_27, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 28:
            objectColor = texture2D(texture_28, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 29:
            objectColor = texture2D(texture_29, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 30:
            objectColor = texture2D(texture_30, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        case 31:
            objectColor = texture2D(texture_31, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        default:
            objectColor = texture2D(texture_0, fUv).xyz;
            phong = result * objectColor /*+ specular*/;
            secondColor = vec4(phong, 1.0);
            break;
        }
        outColor = mix(firstColor, secondColor, 0.5);
    } else {
        outColor = vec4(0.0, 1.0, 1.0, 1.0);
    }

};
