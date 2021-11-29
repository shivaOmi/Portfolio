#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos.rgb, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D texture_tree;

uniform vec3 lightPositionOnScreen;

const float exposure = 1.0f;
const float decay = 1.0f; 
const float density = 1.0f;
const float weight = 0.01;
const int NUM_SAMPLES = 100;

void main()
{
    vec2 deltaTexCoord = vec2(TexCoords.xy - (lightPositionOnScreen.xy));
    vec2 texCoord = TexCoords;

    deltaTexCoord *= 1.0 / NUM_SAMPLES; // deltaTexCoord = ((deltaTexCoord * 1.0) / NUM_SAMPLES) * density

    //vec3 color = vec3(1.0,0.0,0.0);
    vec3 color = texture(texture_tree, texCoord).rgb;

    float illuminationDecay = 1.0f;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        texCoord -= deltaTexCoord;

        vec3 sample = texture(texture_tree, texCoord).rgb;

        sample *= illuminationDecay * weight;

        color += sample;

        illuminationDecay *= decay;
    }

    FragColor = vec4(color * exposure, 1.0);
}