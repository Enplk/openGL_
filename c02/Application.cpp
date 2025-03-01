#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <string>
#include <fstream>
#include <sstream>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
   
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
            
        }
        else
        {
            ss[(int)type] << line << '\n'; 
        }
    }
    return { ss[0].str(), ss[1].str() };
}


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
    * 传入的数据实际上是一个源码 用string去接收
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

    // 需要注意的是，在使用glewinit的时候，要确保glewinit是联系上下文的。否则它返回的就不是一个glew_ok
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

    // Step1：创建一个顶点缓冲区(Vertex Buffer)
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    // 需要指定有多少个缓冲区。我们这里只需要一个缓冲区
    unsigned int buffer;
    // 生成缓冲区
    glGenBuffers(1, &buffer);
    // 将缓冲区和数据进行绑定
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);

    // 在使用这些vertex之前，我们要先使用这个函数来启用顶点
    glEnableVertexAttribArray(0);
    // glVertexAttribPointer(index, size, type, normalized, stride, pointer)
    /*
    * index: 我们要注意到，我们给的顶点vertex（在这一例子中，我们专门指positions）是由不同的属性组成的。我们这一个例子中vertex只有一个属性：
    *        顶点。index指的是在一个可以容纳不同类型的数组中找出对应属性的索引，在这一例子我们只需要找到坐标对应的将索引。放置为0即可。
    * size：是指组件数量。它不是指一些数据所占据的内存，它只有四个值1，2，3，4.默认为4。我们在这一例子中华将它设置为2，因为它是一个二维坐标
    * stride：简单而言，就是到达第二个顶点所需要的字节总量
    * pointer：一个比较让人难以理解的是，我们需要接收的是一个指针，但我们还是向里面传递了一个数字。这里需要学习一下。
    *          回到指针上，pointer是指向实际属性的指针。第一个属性在0字节，那就把它置为0.第二个属性在8字节，那就把它置为8（const void*）
    *          到后期，我们要使用struct来定义我们的vertex
    */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    
    ShaderProgramSource source = ParseShader("Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}