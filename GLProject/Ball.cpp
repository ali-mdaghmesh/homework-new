#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include "stb_image.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

using namespace std;
inline const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(inPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * inNormal;
    TexCoord = inTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";



inline const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ballTexture;
uniform vec3 lightPos;   
uniform vec3 lightColor;  

void main()
{
    vec3 textureColor = texture(ballTexture, TexCoord).rgb;


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = 0.2 * textureColor;
    vec3 diffuse = 0.8 * diff * textureColor;

    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
)";



inline const char* sunFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D ballTexture;

void main()
{
    vec4 texColor = texture(ballTexture, TexCoord);
    FragColor = texColor * vec4(1.3, 1.3, 1.3, 1.0); 
}

)";



class Ball {
public:

    unsigned int VAO, VBO, EBO;
    unsigned int texture;
    int indexCount;
    float radius;
    glm::vec3 position = glm::vec3(0.0f);
    unsigned int shaderProgram;

    Ball(float r, int stacks, int slices,bool isSun)
    {
        radius = r;
        

        vector<float> vertices;
        vector<unsigned int> indices;

        for (int i = 0; i <= stacks; i++) {
            float v = (float)i / stacks;
            float v_angle = v * glm::pi<float>();

            for (int j = 0; j <= slices; j++) {
                float u = (float)j / slices;
                float u_angle = u * glm::two_pi<float>();

                float x = radius * sin(v_angle) * cos(u_angle);
                float y = radius * cos(v_angle);
                float z = radius * sin(v_angle) * sin(u_angle);

                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);

                vertices.push_back(u);
                vertices.push_back(v);

            }
        }

        for (int i = 0; i < stacks; i++) {
            for (int j = 0; j < slices; j++) {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        indexCount = indices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


      
        createShader(isSun);  
        glUseProgram(shaderProgram);
    }

    void setTexture(const char* image)
    {
        int width, height, channels;
        unsigned char* data = stbi_load(image, &width, &height, &channels, 0);

        if (!data) {
            cout << "unfortunatly failed to load the texture";
            return;
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
       
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }


    void createShader(bool isSun)
    {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        if (isSun)
            glShaderSource(fragmentShader, 1, &sunFragmentShaderSource, NULL);
        else
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }



    
};
