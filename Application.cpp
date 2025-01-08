#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // TODO: ERROR handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            <<"shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    /*
    * ���������ʵ������һ��Դ�� ��stringȥ����
    */
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    glewInit();

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // ��Ҫע����ǣ���ʹ��glewinit��ʱ��Ҫȷ��glewinit����ϵ�����ĵġ����������صľͲ���һ��glew_ok
    //  if (glewInit() != GLEW_OK)
    //      std::cout << "Error!" << std::endl;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //glewInit();
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    // �����ǶԴ�ͳ��opengl���и���

    // Step1������һ�����㻺����(Vertex Buffer)
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    // ��Ҫָ���ж��ٸ�����������������ֻ��Ҫһ��������
    unsigned int buffer;
    // ���ɻ�����
    glGenBuffers(1, &buffer);
    // �������������ݽ��а�
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);

    // ��ʹ����Щvertex֮ǰ������Ҫ��ʹ��������������ö���
    glEnableVertexAttribArray(0);
    // glVertexAttribPointer(index, size, type, normalized, stride, pointer)
    /*
    * index: ����Ҫע�⵽�����Ǹ��Ķ���vertex������һ�����У�����ר��ָpositions�����ɲ�ͬ��������ɵġ�������һ��������vertexֻ��һ�����ԣ�
    *        ���㡣indexָ������һ���������ɲ�ͬ���͵��������ҳ���Ӧ���Ե�����������һ��������ֻ��Ҫ�ҵ������Ӧ�Ľ�����������Ϊ0���ɡ�
    * size����ָ���������������ָһЩ������ռ�ݵ��ڴ棬��ֻ���ĸ�ֵ1��2��3��4.Ĭ��Ϊ4����������һ�����л���������Ϊ2����Ϊ����һ����ά����
    * stride���򵥶��ԣ����ǵ���ڶ�����������Ҫ���ֽ�����
    * pointer��һ���Ƚ��������������ǣ�������Ҫ���յ���һ��ָ�룬�����ǻ��������洫����һ�����֡�������Ҫѧϰһ�¡�
    *          �ص�ָ���ϣ�pointer��ָ��ʵ�����Ե�ָ�롣��һ��������0�ֽڣ��ǾͰ�����Ϊ0.�ڶ���������8�ֽڣ��ǾͰ�����Ϊ8��const void*��
    *          �����ڣ�����Ҫʹ��struct���������ǵ�vertex
    */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "void main()\n"
        "{\n"
        "gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";


    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);



        // ������ʹ�ô�ͳopengl������
        //glBegin(GL_TRIANGLES);
        //glVertex2d(-0.5f, -0.5f);
        //glVertex2d( 0.0f,  0.5f);
        //glVertex2d( 0.5f, -0.5f);
        //glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}