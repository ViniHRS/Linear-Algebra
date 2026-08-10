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

#define HEXAGON_RADIUS 2.0f
#define HEXAGON_POINTS 6

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
Vec3 vec3_cross(Vec3 a, Vec3 b);
Vec3 calculate_moment(Vec3 P, Vec3 Q, Vec3 F);
void add_arrow_vertices(float *vertices, int index, Vec3 start, Vec3 direction, float length, float arrowLength, float arrowWidth);

int main() {
   if (!glfwInit()) {
      fprintf(stderr, "Falha ao inicializar GLFW\n");
      return -1;
   }

   //Criando a janela
   GLFWwindow* window = glfwCreateWindow(1200, 800, "Campo de Momentos de uma Força", NULL, NULL);
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
      "uniform vec3 color;\n"
      "out vec4 FragColor;\n"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(color, 1.0);\n"
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

   //Obtendo a localização das matrizes no shader e de color
   int viewLoc = glGetUniformLocation(shaderProgram, "view");
   int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
   int colorLoc = glGetUniformLocation(shaderProgram, "color");

   //Configuração da câmera
   mat4 view;
   mat4 projection;

   vec3 cameraPos = {5.0f, 5.0f, 5.0f};
   vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
   vec3 cameraUp = {0.0f, 0.0f, 1.0f};

   glm_lookat(cameraPos, cameraTarget, cameraUp, view);

   glm_perspective(
      glm_rad(45.0f),   // Campo de visão
      1200.0f / 800.0f, // Proporção da tela
      0.1f,             // Distância mínima
      100.0f,           // Distância máxima
      projection        // Matriz resultante
   );

   //Criando o ponto P a força F (valores de teste)
   Vec3 P = {0.5f, 1.5f, 1.0f};
   Vec3 F = {0.8f, -0.6f, 1.0f};

   //Ponto onde queremos calcular o momento
   Vec3 Q = {2.0f, -0.8f, -0.5f};

   //Vetor posição de P até Q
   Vec3 r = vec3_add(P, vec3_scale(Q, -1.0f));

   //Momento da força em relação a O
   float momentVertices[HEXAGON_POINTS * 18];
   int vertexIndex = 0;

   for (int i = 0; i < HEXAGON_POINTS; i++) {

    float angle = 2.0f * M_PI * i / HEXAGON_POINTS;

    Vec3 Q_field = {
        HEXAGON_RADIUS * cosf(angle),
        HEXAGON_RADIUS * sinf(angle),
        0.0f
    };

    Vec3 M_field = calculate_moment(P, Q_field, F);

    float M_length = vec3_length(M_field);

    if (M_length > 0.001f) {

        float arrowLengthField = 0.25f;

        add_arrow_vertices(momentVertices, vertexIndex, Q_field, M_field, arrowLengthField, 0.06f, 0.025f);

        vertexIndex += 18;
    }
}

   //Obtendo o versor de F
   Vec3 F_direction = vec3_normalize(F);

   Vec3 reference = {0.0f, 0.0f, 1.0f};
   Vec3 arrowSide = vec3_normalize(vec3_cross(F_direction, reference));

   float forceLength = 1.2f;
   float arrowLength = 0.15f;
   float arrowWidth = 0.1f;
   
   Vec3 F_end = vec3_add(P, vec3_scale(F_direction, forceLength));
   Vec3 arrowBase = vec3_add(F_end, vec3_scale(F_direction, -arrowLength));

   Vec3 arrowLeft = vec3_add(arrowBase, vec3_scale(arrowSide, arrowWidth));
   Vec3 arrowRight = vec3_add(arrowBase, vec3_scale(arrowSide, -arrowWidth));

   //Criando os pontos de ação
   float actionLenght = 3.0f;
   Vec3 A = vec3_add(P, vec3_scale(F_direction, -actionLenght));
   Vec3 B = vec3_add(P, vec3_scale(F_direction,  actionLenght));

   //Criação dos eixos
   float axisLenght = 2.0f;
   float vertices[] = {
      // X
      -axisLenght, 0.0f, 0.0f,
      axisLenght, 0.0f, 0.0f,

      // Y
      0.0f, -axisLenght, 0.0f,
      0.0f,  axisLenght, 0.0f,

      // Z
      0.0f, 0.0f, -axisLenght,
      0.0f, 0.0f,  axisLenght,

      // Linha de ação
      A.x, A.y, A.z,
      B.x, B.y, B.z,

      // Força
      P.x, P.y, P.z,
      F_end.x, F_end.y, F_end.z,

      // Ponta da força
      F_end.x, F_end.y, F_end.z,
      arrowLeft.x, arrowLeft.y, arrowLeft.z,

      F_end.x, F_end.y, F_end.z,
      arrowRight.x, arrowRight.y, arrowRight.z
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

   //Criando VBO e VAO para os momentos
   unsigned int momentVBO;
   unsigned int momentVAO;

   glGenBuffers(1, &momentVBO);
   glGenVertexArrays(1, &momentVAO);

   glBindVertexArray(momentVAO);

   glBindBuffer(GL_ARRAY_BUFFER, momentVBO);

   glBufferData(GL_ARRAY_BUFFER, sizeof(momentVertices), momentVertices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

   glEnableVertexAttribArray(0);

   //Laço de abertura da janela
   while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(shaderProgram);

      glBindVertexArray(VAO);

      //Elementos pretos
      glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f);

      glDrawArrays(GL_LINES, 0, 6);

      //Linha de ação vermelha
      glUniform3f(colorLoc, 1.0f, 0.35f, 0.35f);

      glDrawArrays(GL_LINES, 6, 2);

      //Força vermelha
      glDisable(GL_DEPTH_TEST);

      glUniform3f(colorLoc, 0.68f, 0.0f, 0.0f);

      glDrawArrays(GL_LINES, 8, 6);

      glEnable(GL_DEPTH_TEST);
      
      //Campo de momentos
      glDisable(GL_DEPTH_TEST);

      glUniform3f(colorLoc, 0.0f, 0.0f, 0.8f);

      glBindVertexArray(momentVAO);

      glDrawArrays(GL_LINES, 0, vertexIndex / 3);

      glEnable(GL_DEPTH_TEST);

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

Vec3 vec3_cross(Vec3 a, Vec3 b) {
   Vec3 result = {
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x
   };

   return result;
}

Vec3 calculate_moment(Vec3 P, Vec3 Q, Vec3 F) {
    Vec3 QP = vec3_add(P, vec3_scale(Q, -1.0f));
    return vec3_cross(QP, F);
}

void add_arrow_vertices(float *vertices, int index, Vec3 start, Vec3 direction, float length, float arrowLength, float arrowWidth) {
    Vec3 dir = vec3_normalize(direction);

    Vec3 reference = {0.0f, 0.0f, 1.0f};

    // Evita produto vetorial nulo caso direction seja paralelo a Z
    if (fabsf(dir.z) > 0.99f) {
        reference.x = 1.0f;
        reference.z = 0.0f;
    }

    Vec3 arrowSide = vec3_normalize(vec3_cross(dir, reference));

    Vec3 end = vec3_add(start, vec3_scale(dir, length));

    Vec3 base = vec3_add(
        end,
        vec3_scale(dir, -arrowLength)
    );

    Vec3 left = vec3_add(
        base,
        vec3_scale(arrowSide, arrowWidth)
    );

    Vec3 right = vec3_add(
        base,
        vec3_scale(arrowSide, -arrowWidth)
    );

    // Corpo da seta
    vertices[index++] = start.x;
    vertices[index++] = start.y;
    vertices[index++] = start.z;

    vertices[index++] = end.x;
    vertices[index++] = end.y;
    vertices[index++] = end.z;

    // Ponta esquerda
    vertices[index++] = end.x;
    vertices[index++] = end.y;
    vertices[index++] = end.z;

    vertices[index++] = left.x;
    vertices[index++] = left.y;
    vertices[index++] = left.z;

    // Ponta direita
    vertices[index++] = end.x;
    vertices[index++] = end.y;
    vertices[index++] = end.z;

    vertices[index++] = right.x;
    vertices[index++] = right.y;
    vertices[index++] = right.z;
}