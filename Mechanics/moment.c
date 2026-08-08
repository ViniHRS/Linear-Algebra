/*
   This program creates the moment field of a force
   Compiling in terminal:
   gcc moment.c src/glad.c -Iinclude -lglfw -ldl -lm -o moment
*/
#include <glad/glad.h>
#include <KHR/khrplatform.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <math.h>

//Criando um struct para vetores
typedef struct {
   float x;
   float y;
   float z;
} Vec3;

//Protótipos de função
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 v, float scalar);
float vec3_length(Vec3 v);
Vec3 vec3_normalize(Vec3 v);

int main() {
   if (!glfwInit()) {
      fprintf(stderr, "Falha ao inicializar GLFW\n");
      return -1;
   }

   //Criando a janela
   GLFWwindow* window = glfwCreateWindow(1000, 600, "Campo de Momentos de uma Força", NULL, NULL);
   if (!window) {
      fprintf(stderr, "Falha ao criar a janela\n");
      glfwTerminate();
      return -1;
   }
   //Criando um contexto da janela
   glfwMakeContextCurrent(window);

   //Inicializando o GLAD
   if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
      fprintf(stderr, "Falha ao inicializar glad\n");
      glfwDestroyWindow(window);
      glfwTerminate();
      return -1;
   }
   glEnable(GL_DEPTH_TEST);

   //Leitura de vertexShaderSource e fragmentShaderSource   
   const char* vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "uniform mat4 view;\n"
      "uniform mat4 projection;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = projection * view * vec4(aPos, 1.0);\n"
      "}\n";

   const char* fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
      "}\n";

   //Criando e compilando vertexShader
   unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

   glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

   glCompileShader(vertexShader);

   //Criando e compilando vertexShader
   unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

   glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

   glCompileShader(fragmentShader);

   //Juntando ambos os shaders
   unsigned int shaderProgram = glCreateProgram();

   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);

   glLinkProgram(shaderProgram);

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   //Obtendo a localização das matrizes no shader
   int viewLoc = glGetUniformLocation(shaderProgram, "view");
   int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

   //Configuração da câmera
   mat4 view;
   mat4 projection;

   vec3 cameraPos = {3.0f, 3.0f, 3.0f};
   vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
   vec3 cameraUp = {0.0f, 1.0f, 0.0f};

   glm_lookat(cameraPos, cameraTarget, cameraUp, view);

   glm_perspective(
      glm_rad(45.0f),   // Campo de visão
      1000.0f / 600.0f, // Proporção da tela
      0.1f,             // Distância mínima
      100.0f,           // Distância máxima
      projection        // Matriz resultante
   );

   //Criando o ponto P a força F (valores de teste)
   Vec3 P = {0.5f, 1.5f, 1.0f};
   Vec3 F = {1.0f, 0.2f, 0.7f};

   //Obtendo o versor de F
   Vec3 F_direction = vec3_normalize(F);

   //Criando os pontos de ação
   Vec3 A = vec3_add(P, vec3_scale(F_direction, -2.0f));
   Vec3 B = vec3_add(P, vec3_scale(F_direction,  2.0f));

   //Criação dos eixos
   float vertices[] = {
      // X
      -0.8f, 0.0f, 0.0f,
      0.8f, 0.0f, 0.0f,

      // Y
      0.0f, -0.8f, 0.0f,
      0.0f,  0.8f, 0.0f,

      // Z
      0.0f, 0.0f, -0.8f,
      0.0f, 0.0f,  0.8f,

      // Linha de ação
      A.x, A.y, A.z,
      B.x, B.y, B.z
   };

   //Criando o VBO
   unsigned int VBO;

   glGenBuffers(1, &VBO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);

   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   //Criando o VAO
   unsigned int VAO;

   glGenVertexArrays(1, &VAO);

   glBindVertexArray(VAO);

   //Como interpretar o VBO
   glVertexAttribPointer(
      0,                  // índice do atributo
      3,                  // quantidade de componentes
      GL_FLOAT,           // tipo
      GL_FALSE,           // normalização
      3 * sizeof(float),  // distância entre vértices
      (void*)0            // deslocamento inicial
   );

   glEnableVertexAttribArray(0);

   //Enviando matrizes para o shader
   glUseProgram(shaderProgram);

   glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);

   glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection);

   //Laço de abertura da janela
   while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(shaderProgram);

      glBindVertexArray(VAO);

      glDrawArrays(GL_LINES, 0, 8);

      glfwSwapBuffers(window);
   }

   //Encerrando a janela
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}

//Funções
Vec3 vec3_add(Vec3 a, Vec3 b) {
   Vec3 result = {
      a.x + b.x,
      a.y + b.y,
      a.z + b.z
   };

   return result;
}

Vec3 vec3_scale(Vec3 v, float scalar) {
   Vec3 result = {
      v.x * scalar,
      v.y * scalar,
      v.z * scalar
   };

   return result;
}

float vec3_length(Vec3 v) {
   return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 vec3_normalize(Vec3 v) {
   float length = vec3_length(v);

   Vec3 result = {
        v.x / length,
        v.y / length,
        v.z / length
   };

   return result;
}