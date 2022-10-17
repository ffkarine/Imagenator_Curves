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

class Curve {
public:
    GLFWwindow* shapeWindow;
    vector<double> coordinates_X;
    vector<double> coordinates_Y;

    Curve() = default;

    Curve(vector<double> x, vector<double> y)
    {
        generateBSpline(x, y);
    }

    void generateBSpline(vector<double> x, vector<double> y)
    {
        int i = 0;
        double rangeX = 0, rangeY = 0, step = 0, auxX = 0, auxY = 0;

        while ((i + 3) < x.size())
        {
            rangeX = abs(x[i + 2] - x[i + 1]);
            rangeY = abs(y[i + 2] - y[i + 1]);

            if (rangeX > rangeY)
                step = double(1) / rangeX;

            else
                step = double(1) / rangeY;

            cout << "step: " << step << endl;

            for (double t = 0; t <= 1; t += step)
            {
                auxX =  (((-1*pow(t, 3) + 3*pow(t, 2) - 3*t + 1)*x[i] +
                          ( 3*pow(t, 3) - 6*pow(t, 2) + 0*t + 4)*x[i+1] + 
                          (-3*pow(t, 3) + 3*pow(t, 2) + 3*t + 1)*x[i+2] +
                          ( 1*pow(t, 3) + 0*pow(t, 2) + 0*t + 0)*x[i+3])/6);

                auxY =  (((-1*pow(t, 3) + 3*pow(t, 2) - 3*t + 1)*y[i] +
                          ( 3*pow(t, 3) - 6*pow(t, 2) + 0*t + 4)*y[i+1] + 
                          (-3*pow(t, 3) + 3*pow(t, 2) + 3*t + 1)*y[i+2] +
                          ( 1*pow(t, 3) + 0*pow(t, 2) + 0*t + 0)*y[i+3])/6);

                coordinates_X.push_back(auxX / AXIS_TICK_REFERENCE);
                coordinates_Y.push_back(auxY / AXIS_TICK_REFERENCE);
            }

            i++;
        }
    }

    void drawCurve(bool isColored = false)
    {
        glBegin(GL_LINE_STRIP);

        if (isColored)
            glColor3f(75.0 / 255, 152.0 / 255, 240.0 / 255);

        for (int i = 0; i < coordinates_X.size(); i++)
            glVertex2f(coordinates_X[i], coordinates_Y[i]);

        glColor3f(1, 1, 1);

        glEnd();
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

int selectShape(GLFWwindow* window, vector<Curve> curves)
{
    int userInput = 0;

    cout << "\nPerform operation on selected Curve?" << endl;
    cout << "\n1 - Next Curve \n2 - Confirm\n3 - Cancel" << endl;

    for (int i = 0; i < curves.size(); i++)
    {
        userInput = 0;
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawAxis(window);

        for (int j = 0; j < curves.size(); j++)
        {
            if (j == i)
                curves[j].drawCurve(true);

            else
                curves[j].drawCurve();
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

    cout << "\nNo Curve selected. Operation cancelled!" << endl;
    return -1;
}

//Initial prompt for basic option selection
int prompt()
{
    int mainOption = 0;

    cout << "\nChoose your option:" << endl;
    cout << "1 - Draw Curve" << endl;
    cout << "2 - Delete Curve" << endl;
    cout << "3 - Quit";

    while (mainOption < 1 or mainOption > 3)
    {
        cout << "\n: ";
        cin >> mainOption;
    }

    return mainOption;
}

//Prompt for type of input for new Curve
int promptInput()
{
    int inputOption = 0;

    cout << "\nChoose how to input the points:" << endl;
    cout << "1 - Type points" << endl;
    cout << "2 - Click on Screen" << endl;
    cout << "3 - Cancel";

    while (inputOption < 1 or inputOption > 3)
    {
        cout << "\n: ";
        cin >> inputOption;
    }

    return inputOption;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


void refreshWindowToClick(GLFWwindow* window, vector<Curve> curves) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawAxis(window);

    for (int j = 0; j < curves.size(); j++) {
        curves[j].drawCurve();
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

            //xpos /= AXIS_TICK_REFERENCE;
            //ypos /= AXIS_TICK_REFERENCE;

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
    vector<Curve> curves;
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
    glfwSetMouseButtonCallback(window, NULL);

    // Main window loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //////////// DRAWING ZONE ////////////

        drawAxis(window);

        for (int i = 0; i < curves.size(); i++)
            curves[i].drawCurve();

        glfwSwapBuffers(window);
        glfwPollEvents();

        //////////// END OF DRAWING ZONE ////////////

        //Initial prompt for basic option selection
        mainOption = prompt();

        //If adding Curve option was selected
        if (mainOption == 1) {
            //verify current number of curves
            if (curves.size() == MAX_FIGURES) {
                cout << "\nReached max number of figures" << endl;
            }
            else {
                //Prompt for type of input for new Curve
                inputOption = promptInput();

                // Create Curve based on keyboard input points
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

                        // Adding a new point to the Curve
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
                                x_shape.push_back(auxX);
                                y_shape.push_back(auxY);
                            }
                        }
                    }

                    if (x_shape.size() < 3)
                        cout << "\nLess than 3 points entered. NO drawing will be generated!" << endl;
                    
                    else
                    {
                        Curve sh = Curve(x_shape, y_shape);
                        curves.push_back(sh);
                    }

                    x_shape.clear();
                    y_shape.clear();
                    auxInput = 0;
                }

                // Create Curve based on mouse input points
                else if (inputOption == 2) {
                    x_shape.clear();
                    y_shape.clear();

                    glfwPollEvents();
                    
                    enable = true;
                    
                    glfwSetMouseButtonCallback(window, mouse_button_callback);

                    cout << "\nLeft-click to add a new point (at least 3 points to generate a figure).\nRight-click to stop input.\n\n(Repeated points will be discarded!)" << endl;

                    while (enable) 
                    {
                        refreshWindowToClick(window, curves);
                    }

                    if (x_shape.size() >= 3)
                    {
                        Curve sh = Curve(x_shape, y_shape);
                        curves.push_back(sh);
                        x_shape.clear();
                        y_shape.clear();
                    }

                    else
                        cout << "\nLess than 3 points entered. NO drawing will be generated!" << endl;

                    glfwSetMouseButtonCallback(window, NULL);
                }

                else if(inputOption == 3)
                {
                    continue;
                }
            }
        }



        // Deletion option selected
        else if (mainOption == 2)
        {
            selectedShapeId = selectShape(window, curves);

            if (selectedShapeId != -1)
            {
                cout << "\nConfirm deletion of the selected Curve?";
                cout << "\n1 - Yes \n2 - No \n: ";
                cin >> auxInput;

                if (auxInput == 1)
                    curves.erase(curves.begin() + selectedShapeId);
            }
        }

        //Quit Software
        else if (mainOption == 3)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    glfwTerminate();
    return 0;
}