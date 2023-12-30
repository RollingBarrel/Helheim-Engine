#include "ModuleRenderTest.h"
#include "Application.h"
#include "glew.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "MathGeoLib.h"



unsigned ModuleRenderTest::CreateProgram(unsigned vtx_shader, unsigned frg_shader)
{
    unsigned program_id = glCreateProgram();
    glAttachShader(program_id, vtx_shader);
    glAttachShader(program_id, frg_shader);
    glLinkProgram(program_id);
    int res;
    glGetProgramiv(program_id, GL_LINK_STATUS, &res);
    if (res == GL_FALSE)
    {
        int len = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int written = 0;
            char* info = (char*)malloc(len);
            glGetProgramInfoLog(program_id, len, &written, info);
            LOG("Program Log Info: %s", info);
            free(info);
        }
    }
    glDeleteShader(vtx_shader);
    glDeleteShader(frg_shader);
    return program_id;
}

unsigned ModuleRenderTest::CompileShader(unsigned type, const char* source)
{
    unsigned shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &source, 0);
    glCompileShader(shader_id);
    int res = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE)
    {
        int len = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int written = 0;
            char* info = (char*)malloc(len);
            glGetShaderInfoLog(shader_id, len, &written, info);
            LOG("Log Info: %s", info);
            free(info);
        }
    }
    return shader_id;
}

char* ModuleRenderTest::LoadShaderSource(const char* shader_file_name)
{
    char* data = nullptr;
    FILE* file = nullptr;
    fopen_s(&file, shader_file_name, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        data = (char*)malloc(size + 1);
        fseek(file, 0, SEEK_SET);
        fread(data, 1, size, file);
        data[size] = 0;
        fclose(file);
    }
    return data;
}

void ModuleRenderTest::RenderVBO(unsigned vbo, unsigned program)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    // size = 3 float per vertex
    // stride = 0 is equivalent to stride = sizeof(float)*3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glUseProgram(program);
    // 1 triangle to draw = 3 vertices
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

unsigned ModuleRenderTest::CreateTriangleVBO()
{
    float vtx_data[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
    unsigned vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // set vbo active
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtx_data), vtx_data, GL_STATIC_DRAW);

    return vbo;

}

void ModuleRenderTest::DestroyTriangleVBO()
{
    glDeleteBuffers(1, &triangleVBO);

}

void ModuleRenderTest::RenderTriangleTest()
{
    float4x4 viewproj = App->GetCamera()->GetViewProjMatrix();
    float4x4 model = float4x4::identity;
    glUseProgram(program_id);
    glUniformMatrix4fv(glGetUniformLocation(program_id, "model"), 1, GL_TRUE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program_id, "viewproj"), 1, GL_TRUE, &model[0][0]);

    // TODO: bind buffer and vertex attributes
    RenderVBO(triangleVBO, program_id);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

ModuleRenderTest::ModuleRenderTest()
{
}

ModuleRenderTest::~ModuleRenderTest()
{
}

bool ModuleRenderTest::Init()
{
    triangleVBO = CreateTriangleVBO();

    vertex_id = CompileShader(GL_VERTEX_SHADER, LoadShaderSource("Shaders/basic.vs"));
    fragment_id = CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource("Shaders/hello_world.fs"));
    program_id = CreateProgram(vertex_id, fragment_id);

    return true;
}

update_status ModuleRenderTest::PreUpdate()
{
    return UPDATE_CONTINUE;
}

update_status ModuleRenderTest::Update()
{
    return UPDATE_CONTINUE;
}

update_status ModuleRenderTest::PostUpdate()
{
    return UPDATE_CONTINUE;

}

bool ModuleRenderTest::CleanUp()
{
    DestroyTriangleVBO();
    return true;
}
