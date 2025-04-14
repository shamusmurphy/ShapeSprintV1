#include "engine.h"
#include <iostream>

enum state {start, play, over};
state screen;

color originalFill, hoverFill, pressFill;

Engine::Engine() : keys() {
    this->initWindow();
    this->initShaders();
    this->initShapes();

    originalFill = {0, 0, 0, 1};
    hoverFill.vec = originalFill.vec + vec4{0.5, 0.5, 0.5, 0};
    pressFill.vec = originalFill.vec - vec4{0.5, 0.5, 1, 0};
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    // This creates the window using GLFW.
    // It's a C function, so we have to pass it a pointer to the window variable.
    window = glfwCreateWindow(width, height, "engine", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    // This sets the OpenGL context to the window we just created.
    glfwMakeContextCurrent(window);

    // Glad is an OpenGL function loader. It loads all the OpenGL functions that are defined by the driver.
    // This is required because OpenGL is a specification, not an implementation.
    // The driver is the implementation of OpenGL that is installed on your computer.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    // This defines the size of the area OpenGL should render to.
    glViewport(0, 0, width, height);
    // This enables depth testing which prevents triangles from overlapping.
    glEnable(GL_BLEND);
    // Alpha blending allows for transparent backgrounds.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert",
                                                  "../res/shaders/shape.frag",
                                                  nullptr, "shape");

    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);


    // Set uniforms that never change
    textShader.setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {

    //player
    player = make_unique<Rect>(shapeShader, vec2(width/ 4, height/4), vec2(50,50), color(0,.3,1,1));

    // No use for it yet. In the final project I am going to create multiple levels where this will be useful
    playButton = make_unique<Rect>(shapeShader, vec2(width/2, height/2), vec2(100, 50), color(0,0,0,0));


    // Level 1 Design
    // -----------------------------------------------
    // Single triangle
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 1.3, height/4), vec2(40,40), color(1,0,0,1)));


    // Two triangles
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 2, height/4), vec2(50,50), color(1,0,0,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 2.1, height/4), vec2(50,50), color(1,0,0,1)));

    // Tall triangle
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 3, height/4), vec2(40,70), color(1,0,0,1)));

    // Three triangles
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 4, height/4), vec2(40,40), color(1,0,0,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 4.1, height/4), vec2(40,40), color(1,0,0,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 4.2, height/4), vec2(40,40), color(1,0,0,1)));

    // four triangles
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 5, height/4), vec2(40,40), color(1,1,0,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 5.1, height/4), vec2(40,40), color(1,1,0,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 5.2, height/4), vec2(40,40), color(1,1,0,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 5.3, height/4), vec2(40,40), color(1,1,0,1)));

    // three spaced out
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 6, height/4), vec2(40,40), color(1,0,1,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 6.5, height/4), vec2(40,40), color(1,0,1,1)));
    triObstacles.push_back(make_unique<Triangle>(shapeShader, vec2(width * 7.2, height/4), vec2(40,40), color(1,0,1,1)));

    // -----------------------------------------------

}

void Engine::processInput() {

    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    // if on start screen, press s to play
    if (screen == start && keys[GLFW_KEY_S]) {
        initShapes();
        screen = play;
    }
    // Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    // go back to menu if m is pressed
    if (screen == over && keys[GLFW_KEY_M]) screen = start;

    // jump if space is pressed
    if (keys[GLFW_KEY_SPACE]) player->jump(270.0f);

}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (screen == play) {
        // Applying gravity for the user
        if (player->getIsJumping()) {
            player->setVelocityY(player->getVelocityY() + player->getGravity() * deltaTime);
            player->moveY(player->getVelocityY() * deltaTime);

            //check if landed
            if (player->getPosY() <= height/4) {
                player->setPosY(height/4);
                player->setVelocityY(0.0f);
                player->setIsJumping(false);
            }
        }
        // making the triangles move to the left then reset back to the right
        for (auto& tri : triObstacles) {
            tri->moveX(-4);
            if (tri->getPosX() < -(tri->getSize().x/2)) {
                tri->setPosX(width * 7 + (tri->getSize().x/2));
            }
        }

        // collision detection
        for (const auto& tri: triObstacles) {
            if (tri->isOverlapping(*player)) {
                triObstacles.clear();
                screen = over;
                break;
            }
        }
    }

    // float scale = (sin(currentFrame) + 1.0) / 2.0;  // Varies between 0 and 1
    // shapes[0]->setSize(vec2(300 * scale, 300 * scale));

    // Put this code in update in engine.cpp
    // shapes[0]->rotateShape(90.0f, 0.01);

    // Put this code in update in engine.cpp
    float phaseAngle = currentFrame * 2.0f; // You can adjust this value to make the colors change faster or slower
    float red   = sin(phaseAngle + 0) * 0.5f + .5f; // 0 degrees out of phase
    float green = sin(phaseAngle + 2.0f * M_PI / 3.0f) * 0.5f + .5f;  // 120 degrees out of phase
    float blue  = sin(phaseAngle + 4.0f * M_PI / 3.0f) * 0.5f + .5f;  // 240 degrees out of phase
    player->setColor(vec4(red, green, blue, 1.0f));

    for (int i = 0; i < triObstacles.size(); ++i) {
        red = sin(phaseAngle + 2.0f) * .5f + .5f;
        green = sin(phaseAngle + 4.0f * M_PI / 3.0f) * .5f + .5f;
        blue = sin(phaseAngle +0 * M_PI / 3.0f) * .5f + .5f;

        triObstacles[i]->setColor(vec4(red, green, blue, 1.0f));
    }


    // This function polls for events like keyboard input and mouse movement
    // It needs to be called every frame
    // Without this function, the window will freeze and become unresponsive
    glfwPollEvents();
}

void Engine::render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    // Render shapes
    // For each shape, call it's setUniforms() function and then call it's draw() function

    shapeShader.use();

    switch (screen) {
        case start: {
            string welcome = "Welcome to Shape Sprint!";
            string start = "Press S to Begin Level 1";
            // (12 * message.length()) is the offset to center text.
            // 12 pixels is the width of each character scaled by 1.
            // NOTE: This line changes the shader being used to the font shader.
            //  If you want to draw shapes again after drawing text,
            //  you'll need to call shapeShader.use() again first.
            this->fontRenderer->renderText(welcome, width/2 - (12 * welcome.length()), height/1.5, projection, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(start, width/2 - (12 * start.length()), height/1.9, projection, 1, vec3{1, 1, 1});
            break;
        }
        case play: {
            string title = "Shape Sprinttttttt";
            // draw shapes
            player->setUniforms();
            player->draw();
            for (auto& tri : triObstacles) {
                tri->setUniforms();
                tri->draw();
            }
            this->fontRenderer->renderText(title, width/2 - (12 * title.length()), height/1.7, projection, 1, vec3{1, 1, 1});
            break;
        }
        case over: {
            string boo = "You hit an obstacle.";
            string hoo = "Press M to return to the menu.";
            // Displayed the message on the screen
            this->fontRenderer->renderText(boo, width/2 - (12 * boo.length()), height/1.7, projection, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(hoo, width/2 - (12 * hoo.length()), height/2, projection, 1, vec3{1,1,1});

            break;
        }
    }
    // This is glfw function call is required to display the final image on the screen
    // The front buffer contains the final image that is displayed.
    // The back buffer contains the image that is currently being rendered.
    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}
