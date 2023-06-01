#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <QFont>


using namespace std;

const int INF = INT_MAX;

// Function to find the minimum edge cost
int findMinCost(int node, vector<vector<int>>& graph, vector<bool>& visited) {
    int minCost = INF;
    int minIndex = -1;
    int n = graph.size();

    for (int i = 0; i < n; i++) {
        if (!visited[i] && graph[node][i] < minCost) {
            minCost = graph[node][i];
            minIndex = i;
        }
    }

    return minIndex;
}

// Function to calculate the lower bound cost using the minimum spanning tree
int calculateLowerBound(vector<vector<int>>& graph, vector<bool>& visited) {
    int n = graph.size();
    int lowerBound = 0;

    // Find the minimum cost edge from each unvisited node
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            int minCost = findMinCost(i, graph, visited);
            if (minCost != -1) {
                lowerBound += graph[i][minCost];
            }
        }
    }

    return lowerBound;
}

// Recursive function for branch and bound algorithm
void tspBranchAndBound(int node, int cost, int level, vector<vector<int>>& graph, vector<bool>& visited, vector<int>& path, int& minCost, vector<int>& minPath) {
    int n = graph.size();

    // Mark the current node as visited and add it to the path
    visited[node] = true;
    path[level] = node;

    // If all nodes have been visited
    if (level == n - 1) {
        // Check if the current path has a lower cost
        if (graph[node][path[0]] != 0 && cost + graph[node][path[0]] < minCost) {
            minCost = cost + graph[node][path[0]];
            minPath = path;
        }
    }
    else {
        // Calculate the lower bound cost for the current node
        int lowerBound = calculateLowerBound(graph, visited);

        // If the lower bound cost is higher than the current minimum cost, prune the branch
        if (cost + lowerBound >= minCost) {
            visited[node] = false;
            return;
        }

        // Explore all unvisited neighboring nodes
        for (int i = 0; i < n; i++) {
            if (!visited[i] && graph[node][i] != 0) {
                tspBranchAndBound(i, cost + graph[node][i], level + 1, graph, visited, path, minCost, minPath);
            }
        }
    }

    // Mark the current node as unvisited
    visited[node] = false;
}

// Function to solve the Traveling Salesman Problem using branch and bound
void solveTSP(vector<vector<int>>& graph, int startNode, QGraphicsScene* scene, QLabel* resultLabel) {
    QPen shortestPathPen(Qt::red, 2); // Красный цвет и толщина линии 2
    int n = graph.size();
    vector<bool> visited(n, false);
    vector<int> path(n);
    int minCost = INF;
    vector<int> minPath;

    visited[startNode] = true;
    path[0] = startNode;

    tspBranchAndBound(startNode, 0, 0, graph, visited, path, minCost, minPath);

    // Update the result label with the minimum cost and path
    QString result = "Длина пути: " + QString::number(minCost) + "\n";
    result += "Кратчайший путь: ";
    for (int i = 0; i < n; i++) {
        result += QString::number(minPath[i] + 1) + " ";
    }
    result += QString::number(startNode + 1) + "\n";
    resultLabel->setText(result);

    // Clear the previous items in the scene
    scene->clear();

    // Draw the graph
    int radius = 200; // Decreased the radius for larger nodes
    int centerX = 400; // Decreased the center X position
    int centerY = 400; // Decreased the center Y position
    double angle = 2 * M_PI / n;

    for (int i = 0; i < n; i++) {
        int x = centerX + radius * cos(i * angle);
        int y = centerY + radius * sin(i * angle);

        QGraphicsEllipseItem* ellipse = scene->addEllipse(x - 10, y - 10, 20, 20);
        QGraphicsTextItem* label = scene->addText(QString::number(i + 1), QFont("Arial", 12));

        label->setPos(x - label->boundingRect().width() / 2, y - label->boundingRect().height() / 2);

        for (int j = i + 1; j < n; j++) {
            if (graph[i][j] != 0) {
                int x2 = centerX + radius * cos(j * angle);
                int y2 = centerY + radius * sin(j * angle);

                QGraphicsLineItem* line = scene->addLine(x, y, x2, y2);
                int weight = graph[i][j]; // Получение веса ребра
                int weightX = (x + x2) / 2; // Вычисление координаты X для текста веса
                int weightY = (y + y2) / 2; // Вычисление координаты Y для текста веса

                QGraphicsTextItem* weightLabel = scene->addText(QString::number(weight));
                weightLabel->setPos(weightX, weightY);


                line->setPen(QPen(Qt::black, 2));
            }
        }
    }
    for (int i = 0; i < n - 1; i++) {
        int node1 = minPath[i];
        int node2 = minPath[i + 1];

        int x1 = centerX + radius * cos(node1 * angle);
        int y1 = centerY + radius * sin(node1 * angle);
        int x2 = centerX + radius * cos(node2 * angle);
        int y2 = centerY + radius * sin(node2 * angle);
        int weight = graph[node1][node2]; // Получение веса ребра
        int weightX = (x1 + x2) / 2; // Вычисление координаты X для текста веса
        int weightY = (y1 + y2) / 2; // Вычисление координаты Y для текста веса

        QGraphicsTextItem* weightLabel = scene->addText(QString::number(weight));
        weightLabel->setPos(weightX, weightY);


        QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, shortestPathPen);
    }

    int start = minPath[0];
    int end = minPath[n - 1];

    int startX = centerX + radius * cos(start * angle);
    int startY = centerY + radius * sin(start * angle);
    int endX = centerX + radius * cos(end * angle);
    int endY = centerY + radius * sin(end * angle);

    QGraphicsLineItem* line = scene->addLine(endX, endY, startX, startY, shortestPathPen);
}



int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create the main window
    QWidget window;
    window.setWindowTitle("Коммивояжер");

    // Create the result label
    QLabel* resultLabel = new QLabel;
    QFont font;
    font.setPointSize(24);  // Установите желаемый размер шрифта
    resultLabel->setFont(font);


    // Create the input widgets
    QLabel* labelVertices = new QLabel("Введите количество вершин:");
    QLineEdit* lineEditVertices = new QLineEdit;
    QLabel* labelEdges = new QLabel("Введите количество рёбер:");
    QLineEdit* lineEditEdges = new QLineEdit;
    QPushButton* buttonSubmit = new QPushButton("Решить");
    QGraphicsView* graphView = new QGraphicsView;
    QGraphicsScene* graphScene = new QGraphicsScene;

    // Create the layout
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(labelVertices, 0, 0);
    layout->addWidget(lineEditVertices, 0, 1);
    layout->addWidget(labelEdges, 1, 0);
    layout->addWidget(lineEditEdges, 1, 1);
    layout->addWidget(buttonSubmit, 2, 0, 1, 2);
    layout->addWidget(graphView, 3, 0, 1, 2);
    layout->addWidget(resultLabel, 4, 0, 1, 2); // Moved the result label to the bottom
    window.setLayout(layout);

    graphView->setScene(graphScene);

    // Connect the button click signal to the slot
    QObject::connect(buttonSubmit, &QPushButton::clicked, [&]() {
        int n = lineEditVertices->text().toInt();
        int numEdges = lineEditEdges->text().toInt();

        // Create the graph
        vector<vector<int>> graph(n, vector<int>(n, 0));

        // Create the input dialog for edge weights
        for (int i = 0; i < numEdges; i++) {
            QString dialogText = "Введите 2 вершины и вес между ними (например 1 3 5)";
            QString inputText = QInputDialog::getText(nullptr, "Ребро " + QString::number(i + 1), dialogText);
            QStringList inputList = inputText.split(' ');

            if (inputList.size() == 3) {
                int from = inputList[0].toInt() - 1;
                int to = inputList[1].toInt() - 1;
                int weight = inputList[2].toInt();

                graph[from][to] = weight;
                graph[to][from] = weight;  // Assuming bidirectional (undirected) graph
            }
            else {
                QMessageBox::warning(nullptr, "Ошибка", "Неверный формат ввода.");
                return;
            }
        }

        bool ok;
        int startNode = QInputDialog::getInt(nullptr, "Исходная вершина", "Введите исходную вершину:", 1, 1, n, 1, &ok);

        if (ok) {
            solveTSP(graph, startNode - 1, graphScene, resultLabel);
            graphView->fitInView(graphScene->sceneRect(), Qt::KeepAspectRatio);
        }
    });

    // Show the main window
    window.show();

    // Run the event loop
    return app.exec();
}
