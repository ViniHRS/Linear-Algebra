/*
   This program creates the moment field of a force
   Compiling in terminal:
   gcc moment.c src/glad.c -Iinclude -lglfw -ldl -lm -o moment
*/
#include <glad/glad.h>
#include <KHR/khrplatform.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

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

   //Leitura de vertexShaderSource e fragmentShaderSource   
   const char* vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = vec4(aPos, 1.0);\n"
      "}\n";

   const char* fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
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
      0.0f, 0.0f,  0.8f
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

   //Laço de abertura da janela
   while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(shaderProgram);

      glBindVertexArray(VAO);

      glDrawArrays(GL_LINES, 0, 6);

      glfwSwapBuffers(window);
   }

   //Encerrando a janela
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}