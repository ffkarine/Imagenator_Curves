#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <glut.h>

#define CANVAS_SIZE 10
#define AXIS_TICK_REFERENCE (CANVAS_SIZE + 1)
#define MAX_FIGURES 255

using namespace std;

vector<double> x_shape;
vector<double> y_shape;
bool enable = false;

class Shape {
public:
    GLFWwindow* shapeWindow;
    double origin_X, origin_Y, center_X, center_Y, edgeLenght;
    vector<double> coordinates_X;
    vector<double> coordinates_Y;

    Shape() = default;

    Shape(vector<double> x, vector<double> y)
    {
        for (int i = 0; i < x.size(); i++)
        {
            coordinates_X.push_back(x[i]);
            coordinates_Y.push_back(y[i]);
        }
    }

    void drawShape(bool isColored = false)
    {
        glBegin(GL_LINE_LOOP);

        if (isColored)
            glColor3f(75.0 / 255, 152.0 / 255, 240.0 / 255);

        for (int i = 0; i < coordinates_X.size(); i++)
            glVertex2f(coordinates_X[i], coordinates_Y[i]);

        glColor3f(1, 1, 1);

        glEnd();
    }

    void setCenter(double newCenter_X, double newCenter_Y)
    {
        center_X = newCenter_X;
        center_Y = newCenter_Y;
    }

    void setOrigin(double newOrigin_X, double newOrigin_Y)
    {
        origin_X = newOrigin_X;
        origin_Y = newOrigin_Y;
    }

    virtual void calculateVertices() {};

    void translateShape(double increment_X, double increment_Y)
    {
        setCenter(center_X + increment_X, center_Y + increment_Y);
        setOrigin(origin_X + increment_X, origin_Y + increment_Y);

        for (int i = 0; i < coordinates_X.size(); i++)
        {
            coordinates_X[i] = coordinates_X[i] + increment_X;
            coordinates_Y[i] = coordinates_Y[i] + increment_Y;
        }
    }

    void rotateShape(double rotAngleDeg)
    {
        double auxCenter_X = center_X, auxCenter_Y = center_Y;
        double auxX, auxY;

        double sinA = sin(rotAngleDeg * M_PI / 180);
        double cosA = cos(rotAngleDeg * M_PI / 180);

        this->translateShape(-center_X, -center_Y);

        double newCenter_X = (((center_X)*cosA) - ((center_Y)*sinA));
        double newCenter_Y = (((center_X)*sinA) + ((center_Y)*cosA));

        double newOrigin_X = (((origin_X)*cosA) - ((origin_Y)*sinA));
        double newOrigin_Y = (((origin_X)*sinA) + ((origin_Y)*cosA));

        for (int i = 0; i < coordinates_X.size(); i++)
        {
            auxX = coordinates_X[i];
            auxY = coordinates_Y[i];

            coordinates_X[i] = ((auxX * cosA) - (auxY * sinA));
            coordinates_Y[i] = ((auxX * sinA) + (auxY * cosA));
        }

        setCenter(newCenter_X, newCenter_Y);
        setOrigin(newOrigin_X, newOrigin_Y);

        this->translateShape(auxCenter_X, auxCenter_Y);
    }

    void scaleShape(double scalingFactor)
    {
        for (int i = 0; i < coordinates_X.size(); i++)
        {
            coordinates_X[i] = coordinates_X[i] * scalingFactor;
            coordinates_Y[i] = coordinates_Y[i] * scalingFactor;
        }

        setCenter(center_X * scalingFactor, center_Y * scalingFactor);
        setOrigin(origin_X * scalingFactor, origin_Y * scalingFactor);
    }

    void shearShape(char axis, double shearingFactor)
    {
        if (axis == 'X' or axis == 'x')
        {
            for (int i = 0; i < coordinates_X.size(); i++)
            {
                coordinates_X[i] = coordinates_X[i] + coordinates_Y[i] * shearingFactor;
            }

            setCenter(center_X + center_Y * shearingFactor, center_Y);
            setOrigin(origin_X + origin_Y * shearingFactor, origin_Y);
        }

        else if (axis == 'Y' or axis == 'y')
        {
            for (int i = 0; i < coordinates_X.size(); i++)
            {
                coordinates_Y[i] = coordinates_Y[i] + coordinates_X[i] * shearingFactor;
            }

            setCenter(center_X, center_Y + center_X * shearingFactor);
            setOrigin(origin_X, origin_Y + origin_X * shearingFactor);
        }
    }

    void mirrorShape(char axis)
    {
        if (axis == 'X' or axis == 'x')
        {
            for (int i = 0; i < coordinates_X.size(); i++)
            {
                coordinates_Y[i] = coordinates_Y[i] * -1;
            }

            setCenter(center_X, center_Y * -1);
            setOrigin(origin_X, origin_Y * -1);
        }

        else if (axis == 'Y' or axis == 'y')
        {
            for (int i = 0; i < coordinates_X.size(); i++)
            {
                coordinates_X[i] = coordinates_X[i] * -1;
            }

            setCenter(center_X * -1, center_Y);
            setOrigin(origin_X * -1, origin_Y);
        }
    }
};

class Square : public Shape {
public:
    // Recebe os parâmetros de entrada e calcula o ponto (v�rtice) de origem (ponto inferior esquerdo) do quadrado
    Square(double center_Xc, double center_Yc, double edgeLenghtc, GLFWwindow* window)
    {
        center_X = center_Xc;
        center_Y = center_Yc;
        edgeLenght = edgeLenghtc;
        shapeWindow = window;

        origin_X = center_X - edgeLenght / 2;
        origin_Y = center_Y - edgeLenght / 2;

        calculateVertices();
    }

    void calculateVertices() override
    {
        coordinates_X.push_back(origin_X);
        coordinates_Y.push_back(origin_Y);

        coordinates_X.push_back(origin_X + edgeLenght);
        coordinates_Y.push_back(origin_Y);

        coordinates_X.push_back(origin_X + edgeLenght);
        coordinates_Y.push_back(origin_Y + edgeLenght);

        coordinates_X.push_back(origin_X);
        coordinates_Y.push_back(origin_Y + edgeLenght);
    }
};

class Triangle : public Shape
{
public:
    // Recebe os par�metros de entrada e calcula o ponto (v�rtice) de origem (ponto inferior esquerdo) do tri�ngulo
    Triangle(double center_Xc, double center_Yc, double edgeLenghtc, GLFWwindow* window)
    {
        center_X = center_Xc;
        center_Y = center_Yc;
        edgeLenght = edgeLenghtc;
        shapeWindow = window;

        origin_X = center_X - edgeLenght / 2;
        origin_Y = center_Y - edgeLenght / 3;

        calculateVertices();
    }

    void calculateVertices() override
    {
        double triangleHeight = sqrt(3) * edgeLenght / 2;

        coordinates_X.push_back(origin_X);
        coordinates_Y.push_back(origin_Y);

        coordinates_X.push_back(origin_X + edgeLenght);
        coordinates_Y.push_back(origin_Y);

        coordinates_X.push_back(origin_X + edgeLenght / 2);
        coordinates_Y.push_back(origin_Y + triangleHeight);
    }
};

class Hexagon : public Shape
{
public:
    // Recebe os par�metros de entrada e calcula o ponto (v�rtice) de origem (ponto inferior esquerdo) do hex�gono
    Hexagon(double center_Xc, double center_Yc, double edgeLenghtc, GLFWwindow* window)
    {
        center_X = center_Xc;
        center_Y = center_Yc;
        edgeLenght = edgeLenghtc;

        origin_X = center_X - edgeLenght / 2;
        origin_Y = center_Y - edgeLenght * sqrt(3) / 2;

        calculateVertices();
    }

    void calculateVertices() override
    {
        double halfEdgeLenght = edgeLenght / 2;
        double edgeHeight = sqrt(3) * halfEdgeLenght;

        coordinates_X.push_back(origin_X);
        coordinates_Y.push_back(origin_Y);

        coordinates_X.push_back(origin_X + edgeLenght);
        coordinates_Y.push_back(origin_Y);

        coordinates_X.push_back(origin_X + edgeLenght + halfEdgeLenght);
        coordinates_Y.push_back(origin_Y + edgeHeight);

        coordinates_X.push_back(origin_X + edgeLenght);
        coordinates_Y.push_back(origin_Y + edgeHeight + edgeHeight);

        coordinates_X.push_back(origin_X);
        coordinates_Y.push_back(origin_Y + edgeHeight + edgeHeight);

        coordinates_X.push_back(origin_X - halfEdgeLenght);
        coordinates_Y.push_back(origin_Y + edgeHeight);
    }
};

void drawAxis(GLFWwindow* window)
{
    // width; height
    int* w = (int*)malloc(sizeof(int)), * h = (int*)malloc(sizeof(int));
    int tickSize = 10;

    glfwGetWindowSize(window, w, h);

    if (w == NULL or h == NULL)
        return;

    glBegin(GL_LINES);

    //Eixo X principal
    glVertex2f(-1, 0);
    glVertex2f(1, 0);

    //Eixo Y principal
    glVertex2f(0, -1);
    glVertex2f(0, 1);

    //Ticks dos eixos
    for (int i = -10; i <= 10; i++)
    {
        // Ticks eixo X
        glVertex2f((double(i) / AXIS_TICK_REFERENCE), double(-tickSize) / (*h));
        glVertex2f((double(i) / AXIS_TICK_REFERENCE), double(tickSize) / (*h));

        // Ticks eixo Y
        glVertex2f(double(-tickSize) / (*w), (double(i) / AXIS_TICK_REFERENCE));
        glVertex2f(double(tickSize) / (*w), (double(i) / AXIS_TICK_REFERENCE));
    }

    glEnd();

    free(w);
    free(h);
}

int selectShape(GLFWwindow* window, vector<Shape> shapes)
{
    int userInput = 0;

    cout << "\nPerform operation on selected shape?" << endl;
    cout << "\n1 - Next shape \n2 - Confirm\n3 - Cancel" << endl;

    for (int i = 0; i < shapes.size(); i++)
    {
        userInput = 0;
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawAxis(window);

        for (int j = 0; j < shapes.size(); j++)
        {
            if (j == i)
                shapes[j].drawShape(true);

            else
                shapes[j].drawShape();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (userInput < 1 or userInput > 3)
        {
            cout << ": ";
            cin >> userInput;
        }

        if (userInput == 2)
            return i;

        else if (userInput == 3)
            return -1;
    }

    cout << "\nNo shape selected. Operation cancelled!" << endl;
    return -1;
}

//Initial prompt for basic option selection
int prompt()
{
    int mainOption = 0;

    cout << "\nChoose your option:" << endl;
    cout << "1 - Draw Figure" << endl;
    cout << "2 - Transform Figure" << endl;
    cout << "3 - Delete Figure" << endl;
    cout << "4 - Quit";

    while (mainOption < 1 or mainOption > 4)
    {
        cout << "\n: ";
        cin >> mainOption;
    }

    return mainOption;
}

//Prompt for type of input for new shape
int promptInput()
{
    int inputOption = 0;

    cout << "\nChoose how to input the points:" << endl;
    cout << "1 - Type points" << endl;
    cout << "2 - Click on Screen" << endl;
    cout << "3 - Type center and size" << endl;
    cout << "4 - Cancel";

    while (inputOption < 1 or inputOption > 4)
    {
        cout << "\n: ";
        cin >> inputOption;
    }

    return inputOption;
}

int transformationPrompt()
{
    int transformationOption = 0;

    cout << "\nChoose a transformation to perform:" << endl;
    cout << "1 - Translation" << endl;
    cout << "2 - Rotation" << endl;
    cout << "3 - Scaling" << endl;
    cout << "4 - Mirroring" << endl;
    cout << "5 - Shearing" << endl;
    cout << "6 - Cancel";

    while (transformationOption < 1 or transformationOption > 6)
    {
        cout << "\n: ";
        cin >> transformationOption;
    }

    return transformationOption;
}

void refreshWindowToClick(GLFWwindow* window, vector<Shape> shapes) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawAxis(window);

    for (int j = 0; j < shapes.size(); j++) {
        shapes[j].drawShape();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (enable)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            bool isPointRepeated = false;

            //getting cursor position
            glfwGetCursorPos(window, &xpos, &ypos);

            // Normalizing input
            xpos = ( (xpos - 400) * AXIS_TICK_REFERENCE / 400);
            ypos = ( (400 - ypos) * AXIS_TICK_REFERENCE / 400);

            // Limiting x input from -10 to 10
            if (xpos > 10)
                xpos = 10;
            else if (xpos < -10)
                xpos = -10;

            // Limiting y input from -10 to 10
            if (ypos > 10)
                ypos = 10;
            else if (ypos < -10)
                ypos = -10;

            xpos /= AXIS_TICK_REFERENCE;
            ypos /= AXIS_TICK_REFERENCE;

            for (int i = 0; i < x_shape.size(); i++)
            {
                if (x_shape[i] == xpos and y_shape[i] == ypos)
                {
                    isPointRepeated = true;
                }
            }

            if (!isPointRepeated)
            {
                x_shape.push_back(xpos);
                y_shape.push_back(ypos);
            }
        }

        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            enable = false;
    }
}

int main(void) {
    GLFWwindow* window;
    vector<Shape> shapes;
    int selectedShapeId = -1, inputOption = 0, mainOption = 0, auxInput = 0, transformationInput = 0, pointsInput = 0, control;
    double auxX, auxY, auxTransform;
    bool isPointRepeated = false;

    // Initializing GLFW
    if (!glfwInit())
        return -1;

    // Create window 
    window = glfwCreateWindow(800, 800, "Imagenator", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Main window loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //////////// DRAWING ZONE ////////////

        drawAxis(window);

        for (int i = 0; i < shapes.size(); i++)
            shapes[i].drawShape();

        glfwSwapBuffers(window);
        glfwPollEvents();

        //////////// END OF DRAWING ZONE ////////////

        //Initial prompt for basic option selection
        mainOption = prompt();

        //If adding shape option was selected
        if (mainOption == 1) {
            //verify current number of shapes
            if (shapes.size() == MAX_FIGURES) {
                cout << "\nReached max number of figures" << endl;
            }
            else {
                //Prompt for type of input for new shape
                inputOption = promptInput();

                // Create shape based on keyboard input points
                if (inputOption == 1)
                {
                    while (auxInput != 2)
                    {
                        isPointRepeated = false;

                        // Invalid value to enter on the input point 'while';
                        auxX = -AXIS_TICK_REFERENCE;
                        auxY = -AXIS_TICK_REFERENCE;

                        cout << "\n1 - New point" << endl;
                        cout << "2 - Stop Drawing (If less than 3 points were entered no drawing will be generated)." << endl;
                        cin >> auxInput;

                        // Adding a new point to the shape
                        if (auxInput == 1)
                        {
                            while (((auxX > CANVAS_SIZE) or (auxX < -CANVAS_SIZE)) or ((auxY > CANVAS_SIZE) or (auxY < -CANVAS_SIZE))) {
                                cout << "\nCoordinates must be in the interval of -" << CANVAS_SIZE << " and " << CANVAS_SIZE << "." << endl;
                                cout << "X : ";
                                cin >> auxX;

                                cout << "Y : ";
                                cin >> auxY;
                            }

                            for (int i = 0; i < x_shape.size(); i++)
                            {
                                if (x_shape[i] == auxX and y_shape[i] == auxY)
                                {
                                    cout << "\nThis point was already entered!" << endl;
                                    isPointRepeated = true;
                                }
                            }

                            if (!isPointRepeated)
                            {
                                x_shape.push_back(auxX / AXIS_TICK_REFERENCE);
                                y_shape.push_back(auxY / AXIS_TICK_REFERENCE);
                            }
                        }
                    }

                    if (x_shape.size() < 3)
                        cout << "\nLess than 3 points entered. NO drawing will be generated!" << endl;

                    // Set center and origin of the shape and create it
                    else
                    {
                        // Invalid value to enter on the input point 'while';
                        auxX = -AXIS_TICK_REFERENCE;
                        auxY = -AXIS_TICK_REFERENCE;

                        cout << "\nType center point." << endl;

                        while (((auxX > CANVAS_SIZE) or (auxX < -CANVAS_SIZE)) or ((auxY > CANVAS_SIZE) or (auxY < -CANVAS_SIZE))) {
                            cout << "\nCoordinates must be in the interval of -" << CANVAS_SIZE << " and " << CANVAS_SIZE << "." << endl;
                            cout << "X : ";
                            cin >> auxX;

                            cout << "Y : ";
                            cin >> auxY;
                        }

                        Shape sh = Shape(x_shape, y_shape);

                        sh.setCenter(auxX / AXIS_TICK_REFERENCE, auxY / AXIS_TICK_REFERENCE);
                        sh.setOrigin(x_shape[0], y_shape[0]);

                        shapes.push_back(sh);
                    }

                    x_shape.clear();
                    y_shape.clear();
                    auxInput = 0;
                }

                // Create shape based on mouse input points
                else if (inputOption == 2) {
                    x_shape.clear();
                    y_shape.clear();
                    enable = true;

                    cout << "\nLeft-click to add a new point (at least 3 points to generate a figure).\nRight-click to stop input.\n\n(Repeated points will be discarded!)" << endl;

                    while (enable) 
                    {
                        refreshWindowToClick(window, shapes);
                    }

                    if (x_shape.size() > 2)
                    {
                        Shape sh = Shape(x_shape, y_shape);
                        shapes.push_back(sh);
                        x_shape.clear();
                        y_shape.clear();
                    }

                    else
                        cout << "\nLess than 3 points entered. NO drawing will be generated!" << endl;
                }

                //If option of typing center coordinates and size was selected
                else if (inputOption == 3) {
                    int shapeOption = 0;
                    double centerX, centerY, size;

                    while (shapeOption < 1 or shapeOption > 3)
                    {
                        cout << "\nChoose Figure Type:" << endl;
                        cout << "1 - Square" << endl;
                        cout << "2 - Triangle" << endl;
                        cout << "3 - Hexagon" << endl;
                        cin >> shapeOption;
                    }

                    cout << "\nType center (X and Y):" << endl;
                    cin >> centerX >> centerY;

                    cout << "\nType size:" << endl;
                    cin >> size;

                    //Limit coordinates and size to be inside the screen
                    while (((centerX > CANVAS_SIZE) or (centerX < -CANVAS_SIZE)) or ((centerY > CANVAS_SIZE) or (centerY < -CANVAS_SIZE)) or (size > CANVAS_SIZE)) {
                        cout << "\nCoordinates and size must be in the interval of -" << CANVAS_SIZE << " and " << CANVAS_SIZE << ", type X and Y again:" << endl;
                        cin >> centerX >> centerY;

                        cout << "\nType size:" << endl;
                        cin >> size;
                    }

                    centerX = centerX / AXIS_TICK_REFERENCE;
                    centerY = centerY / AXIS_TICK_REFERENCE;
                    size = size / AXIS_TICK_REFERENCE;

                    //Call drawing methods for each option
                    if (shapeOption == 1) {
                        Square s = Square(centerX, centerY, size, window);
                        shapes.push_back(s);
                    }
                    else if (shapeOption == 2) {
                        Triangle t = Triangle(centerX, centerY, size, window);
                        shapes.push_back(t);
                    }
                    else if (shapeOption == 3) {
                        Hexagon h = Hexagon(centerX, centerY, size, window);
                        shapes.push_back(h);
                    }
                }

                else if(inputOption == 4)
                {
                    continue;
                }
            }
        }

        //Transformation option selected
        else if (mainOption == 2)
        {
            auxX = -30;
            auxY = -30;
            transformationInput = transformationPrompt();

            if (transformationInput == 6)
            {
                continue;
            }

            selectedShapeId = selectShape(window, shapes);

            if (selectedShapeId != -1)
            {
                // Invalid value to enter on the input point 'while';
                auxX = -AXIS_TICK_REFERENCE;
                auxY = -AXIS_TICK_REFERENCE;

                switch (transformationInput)
                {
                // Translation Transformation
                case 1:
                    cout << "\nType the destination point (relative to the center of the figure)" << endl;

                    while (((auxX > CANVAS_SIZE) or (auxX < -CANVAS_SIZE)) or ((auxY > CANVAS_SIZE) or (auxY < -CANVAS_SIZE))) {
                        cout << "\nCoordinates must be in the interval of -" << CANVAS_SIZE << " and " << CANVAS_SIZE << "." << endl;
                        cout << "X : ";
                        cin >> auxX;

                        cout << "\nY : ";
                        cin >> auxY;
                    }

                    shapes[selectedShapeId].translateShape((auxX / AXIS_TICK_REFERENCE - shapes[selectedShapeId].center_X), (auxY / AXIS_TICK_REFERENCE - shapes[selectedShapeId].center_Y));
                    break;

                // Rotation Transformation
                case 2:
                    cout << "\nType the angle (in degrees) to rotate the shape:" << endl;
                    cout << ": ";
                    cin >> auxTransform;

                    shapes[selectedShapeId].rotateShape(auxTransform);
                    break;

                // Scaling Transformation
                case 3:
                    auxTransform = -AXIS_TICK_REFERENCE;

                    cout << "\nType the scaling factor (positive numbers only):";

                    while (auxTransform < 0)
                    {
                        cout << "\n: ";
                        cin >> auxTransform;
                    }

                    shapes[selectedShapeId].scaleShape(auxTransform);
                    break;

                    // Mirroring Transformation
                case 4:
                    auxInput = 0;

                    cout << "\nType the axis to perform the mirroring:" << endl;
                    cout << "\n1 - X";
                    cout << "\n2 - Y";
                    cout << "\n3 - XY";

                    while (auxInput < 1 or auxInput > 3)
                    {
                        cout << "\n: ";
                        cin >> auxInput;
                    }

                    if (auxInput == 1)
                        shapes[selectedShapeId].mirrorShape('x');

                    else if (auxInput == 2)
                        shapes[selectedShapeId].mirrorShape('y');

                    else if (auxInput == 3)
                    {
                        shapes[selectedShapeId].mirrorShape('x');
                        shapes[selectedShapeId].mirrorShape('y');
                    }
                    break;

                    // Shearing Transformation
                case 5:
                    auxInput = 0;
                    auxTransform = -200;

                    cout << "\nType the axis to perform the shearing:" << endl;
                    cout << "\n1 - X";
                    cout << "\n2 - Y";
                    cout << "\n3 - XY (the same shearing factor will be applied for both axes)";

                    while (auxInput < 1 or auxInput > 3)
                    {
                        cout << "\n: ";
                        cin >> auxInput;
                    }

                    cout << "\nType the shearing value (-100 - 100):" << endl;

                    while (auxTransform < -100 or auxTransform > 100)
                    {
                        cout << "\n: ";
                        cin >> auxTransform;
                    }

                    if (auxInput == 1)
                        shapes[selectedShapeId].shearShape('x', auxTransform / (10 * AXIS_TICK_REFERENCE));

                    else if (auxInput == 2)
                        shapes[selectedShapeId].shearShape('y', auxTransform / (10 * AXIS_TICK_REFERENCE));

                    else if (auxInput == 3)
                    {
                        shapes[selectedShapeId].shearShape('y', auxTransform / (10 * AXIS_TICK_REFERENCE));
                        shapes[selectedShapeId].shearShape('x', auxTransform / (10 * AXIS_TICK_REFERENCE));
                    }
                    break;

                default:
                    break;
                }
            }
        }



        // Deletion option selected
        else if (mainOption == 3)
        {
            selectedShapeId = selectShape(window, shapes);

            if (selectedShapeId != -1)
            {
                cout << "\nConfirm deletion of the selected shape?";
                cout << "\n1 - Yes \n2 - No \n: ";
                cin >> auxInput;

                if (auxInput == 1)
                    shapes.erase(shapes.begin() + selectedShapeId);
            }
        }

        //Quit Software
        else if (mainOption == 4)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    glfwTerminate();
    return 0;
}