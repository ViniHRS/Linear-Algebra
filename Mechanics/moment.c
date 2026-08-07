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

   GLFWwindow* window = glfwCreateWindow(1000, 600, "Campo de Momentos de uma Força", NULL, NULL);
   if (!window) {
      fprintf(stderr, "Falha ao criar a janela\n");
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
      fprintf(stderr, "Falha ao inicializar glad\n");
      glfwDestroyWindow(window);
      glfwTerminate();
      return -1;
   }

   while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glfwSwapBuffers(window);
   }

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}