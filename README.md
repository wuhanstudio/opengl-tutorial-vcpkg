[Modern OpenGL Tutorial](#modern-opengl-tutorial)

  * [Install vcpkg](#install-vcpkg)
  * [01 Hello OpenGL](#01-hello-opengl)
  * [02 Hello Triangle](#02-hello-triangle)
  * [03 Hello Shader](#03-hello-shader)
  * [04 Hello Texture](#04-hello-texture)
  * [05 Hello Camera](#05-hello-camera)
  * [06 Hello Object](#06-hello-object)
  * [07 Hello Skybox](#07-hello-skybox)
  * [08 Hello Lighting](#08-hello-lighting)
  * [09 Hello Shadow](#09-hello-shadow)
  * [10 Hello ImGUI](#10-hello-imgui)



# Modern OpenGL Tutorial

> vcpkg is a free and open-source C/C++ package manager maintained by Microsoft and the C++ community.

- Cross Platform (Windows / Linux)
- Automatic library management via vcpkg



## Install vcpkg

```
$ sudo apt install build-essential pkg-config cmake curl zip unzip tar
$ sudo apt install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev

$ git clone https://github.com/microsoft/vcpkg.git
$ cd vcpkg
$ ./bootstrap-vcpkg.sh
$ echo 'export VCPKG_ROOT=$HOME/vcpkg' >> ~/.bashrc
$ echo 'export PATH=$PATH:$VCPKG_ROOT' >> ~/.bashrc
$ source ~/.bashrc
$ vcpkg integrate install
```



## Create new projects

```
$ vcpkg new --application
$ vcpkg add port fmt glfw3 glad
$ vcpkg x-update-baseline --add-initial-baseline
```

C++ dependencies are defined in `vcpkg.json`:

```
{
  "dependencies": [
    "fmt",
    "glfw3",
    "glad"
  ]
}
```



## 01 Hello OpenGL

The first example creates a window using `glfw` and  `glad`.

```
$ cd hello-opengl
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-opengl
```

![](docs/hello-opengl.png)

```
glfwInit();

glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

GLFWwindow* window = glfwCreateWindow(800, 800, "Hello", NULL, NULL);

if (window == NULL) {
    fmt::print("Failed to create a window\n");
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);

glfwSetKeyCallback(window, glfw_onKey);

gladLoadGL();
```



## 02 Hello Triangle

This example creates a triangle and displays the FPS.

```
$ cd hello-triangle
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-triangle
```

![](docs/hello-triangle.png)

You'll learn how to compile and link vertex shader and fragment shader.

```
GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);

GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
glCompileShader(fragmentShader);

GLuint shaderProgram = glCreateProgram();

glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
```

And bind the data to VAO, VBO.

```
GLuint VAO, VBO;

glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);

glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);

glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);
```



## 03 Hello Shader

This example applies animations to the triangle by changing the position offset in vertex shader:

```
$ cd hello-shader
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-shader
```

![](docs/hello-shader.gif)

```
GLfloat time = (GLfloat)glfwGetTime();
GLfloat blueColor = (sin(time) / 2) + 0.5f;
glm::vec2 pos;
pos.x = sin(time) / 2;
pos.y = cos(time) / 2;
shaderProgram.setUniform("vertColor", glm::vec4(0.0f, 0.0f, blueColor, 1.0f));
shaderProgram.setUniform("posOffset", pos);
```

Vertex Shader:

```
#version 330 core

layout (location = 0) in vec3 pos;

uniform vec2 posOffset;

void main()
{
	gl_Position = vec4(pos.x + posOffset.x, pos.y + posOffset.y, pos.z, 1.0);
}
```



## 04 Hello Texture

This example loads a texture using `stbi` image library. 

```
$ cd hello-texture
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-texture
```

![](docs/hello-texture.png)

```
// Use stbi image library to load our image
int width, height, components;
stbi_set_flip_vertically_on_load(true);
unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);

if (imageData == NULL)
{
    fmt::println("Error loading texture '{}'", fileName);
    return false;
}

glGenTextures(1, &mTexture);
glBindTexture(GL_TEXTURE_2D, mTexture); // all upcoming GL_TEXTURE_2D operations will affect our texture object (mTexture)

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

if (generateMipMaps)
    glGenerateMipmap(GL_TEXTURE_2D);

stbi_image_free(imageData);

glBindTexture(GL_TEXTURE_2D, 0); // unbind texture when done so we don't accidentally mess up our mTexture
```



## 05 Hello Camera

This example adds a camera to the scene (Key: WASD / QE).

```
$ cd hello-camera
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-camera
```

![](docs/hello-camera.gif)

We first enable the depth buffer to enable a 3D world.

```
glEnable(GL_DEPTH_TEST);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

Then we can change the view in vertex shader.

```
//-----------------------------------------------------------------------------
// basic.vert by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Vertex shader
//-----------------------------------------------------------------------------
#version 330 core

layout (location = 0) in vec3 pos;  // in local coords
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 model;			// model matrix
uniform mat4 view;			// view matrix
uniform mat4 projection;	// projection matrix

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	TexCoord = texCoord;
}
```



## 06 Hello Object

```
$ cd hello-object
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-object
```



## 07 Hello Skybox

- Coming Soon



## 08 Hello Lighting

```
$ cd hello-lighting
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-lighting
```





## 09 Hello Shadow

- Coming Soon



## 10 Hello ImGUI

```
$ cd hello-imgui
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-imgui
```





## Resources

- https://www.udemy.com/course/learn-modern-opengl-programming/
- https://learnopengl.com/Getting-started/OpenGL
