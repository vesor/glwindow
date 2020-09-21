//========================================================================
// Simple multi-window test
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
//
// This test creates four windows and clears each in a different color
//
//========================================================================

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "linmath.h"

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};
 
static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


static const char* titles[] =
{
    "Red",
    "Green",
    "Blue",
    "Yellow"
};

static const struct
{
    float r, g, b;
} colors[] =
{
    { 0.95f, 0.32f, 0.11f },
    { 0.50f, 0.80f, 0.16f },
    {   0.f, 0.68f, 0.94f },
    { 0.98f, 0.74f, 0.04f }
};

static void usage(void)
{
    printf("Usage: windows [-h] [-b] [-f] \n");
    printf("Options:\n");
    printf("  -b create decorated windows\n");
    printf("  -f set focus on show off for all but first window\n");
    printf("  -h show this help\n");
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
        case GLFW_KEY_SPACE:
        {
            int xpos, ypos;
            glfwGetWindowPos(window, &xpos, &ypos);
            glfwSetWindowPos(window, xpos, ypos);
            break;
        }

        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
    }
}

int main(int argc, char** argv)
{
    int i, ch;
    int decorated = GLFW_FALSE;
    int focusOnShow = GLFW_TRUE;
    int running = GLFW_TRUE;
    GLFWwindow* windows[4];

    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    while ((ch = getopt(argc, argv, "bfh")) != -1)
    {
        switch (ch)
        {
            case 'b':
                decorated = GLFW_TRUE;
                break;
            case 'f':
                focusOnShow = GLFW_FALSE;
                break;
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_DECORATED, decorated);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    for (i = 0;  i < 4;  i++)
    {
        int left, top, right, bottom;
        if (i)
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, focusOnShow);

        windows[i] = glfwCreateWindow(200, 200, titles[i], NULL, NULL);
        if (!windows[i])
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(windows[i], key_callback);

        glfwMakeContextCurrent(windows[i]);
        gladLoadGL(glfwGetProcAddress);
        glClearColor(colors[i].r, colors[i].g, colors[i].b, 1.f);

        glfwGetWindowFrameSize(windows[i], &left, &top, &right, &bottom);
        glfwSetWindowPos(windows[i],
                         100 + (i & 1) * (200 + left + right),
                         100 + (i >> 1) * (200 + top + bottom));
    }

    for (i = 0;  i < 4;  i++)
        glfwShowWindow(windows[i]);

    // NOTE: OpenGL error checks have been omitted for brevity
 
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
 
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
 
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
 
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));

    while (running)
    {
        for (i = 0;  i < 4;  i++)
        {
            glfwMakeContextCurrent(windows[i]);

            // if (i == 0) 
            {
                float ratio;
                int width, height;
                mat4x4 m, p, mvp;
        
                glfwGetFramebufferSize(windows[i], &width, &height);
                ratio = width / (float) height;
        
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT);
        
                mat4x4_identity(m);
                mat4x4_rotate_Z(m, m, (float) glfwGetTime());
                mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
                mat4x4_mul(mvp, p, m);
        
                glUseProgram(program);
                glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            } 
            // else {
            //     glClear(GL_COLOR_BUFFER_BIT);
            // }

            glfwSwapBuffers(windows[i]);
            glfwPollEvents();

            if (glfwWindowShouldClose(windows[i]))
                running = GLFW_FALSE;
        }

        
        // glfwWaitEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

