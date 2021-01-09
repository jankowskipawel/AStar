#include <iostream>
#include <fstream>
#include <vector>
#include "colorwin.hpp"

using namespace std;
using namespace colorwin;

struct point
{
    int x;
    int y;
};

struct cell
{
    int x;
    int y;
    point parent;
    float f;

	cell(int a, int b, point parentt, float fv)
	{
        x = a;
        y = b;
        parent = parentt;
        f = fv;
	}
};

float CalculateF(int x, int y, int destinationPointX, int destinationPointY)
{
    return sqrt((x - destinationPointX) * (x - destinationPointX) + (y - destinationPointY) * (y - destinationPointY));
}


bool isDest(int x, int y, point dest)
{
    if (x == dest.x && y == dest.y)
    {
        return true;
    }
    return false;
}


bool isValid(int x, int y, vector<vector<int>> grid)
{
    if (x < 0 || y < 0 || x > 19 || y > 19) 
    {
        return false;
    }
    else if(grid[y][x] == 5)
    {
        return false;
    }
	
    return true;
}

cell MinimumCell(vector<cell> list)
{
    cell minimum = list[0];
    for (int i = 0; i < list.size() ; i++)
    {
        if(list[i].f <= minimum.f)
        {
            minimum = list[i];
        }
    }
    return minimum;
}

bool isInVector(vector<cell> list, int x, int y)
{

	for (int i = 0; i < list.size(); i++)
    {
        if(list[i].x == x && list[i].y == y)
        {
            return true;
        }
    }
    return false;
}

int FindIndex(vector<cell> list, int x, int y)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (list[i].x == x && list[i].y == y)
        {
            return i;
        }
    }
    return -1;
}

vector<cell> RemoveFromVector(vector<cell> list, int index)
{
    vector<cell> result;
    for (int i = 0; i < list.size(); i++)
    {
        if (i != index)
        {
            result.push_back(list[i]);
        }
    }
    return result;
}

vector<vector<int>> MakePathTwo(vector<vector<int>> grid, vector<cell> cells, int startX, int startY)
{
    int indexX = cells.back().parent.x;
    int indexY = cells.back().parent.y;
    grid[cells.back().y][cells.back().x] = 3;
    while (indexX != startX || indexY != startY)
    {
        int parentIndex = FindIndex(cells, indexX, indexY);
        grid[indexY][indexX] = 3;
        indexX = cells[parentIndex].parent.x;
        indexY = cells[parentIndex].parent.y;
        grid[indexY][indexX] = 3;
    }
    grid[startY][startX] = 3;
    return grid;
}

void ShowGrid(vector<vector<int>> grd)
{
    for (int i = 0; i < grd.size(); i++)
    {
        for (int j = 0; j < grd[i].size(); j++)
        {
            if (grd[i][j] == 5)
            {
                cout << color(red) << grd[i][j];
            }
            else if (grd[i][j] == 3)
            {
                cout << color(green) << grd[i][j];
            }
            else
            {
                cout << grd[i][j];
            }
            cout << " ";
        }
        cout << "\n";
    }
}

vector<cell> AStar(int startX, int startY, int destX, int destY, vector<vector<int>> grid)
{
    //KOLEJNOSC gora dol lewa prawa
    vector<cell> closed;
    vector<cell> open;
    open.push_back(cell(startX, startY, { startX,startY }, 0));
    int currentX = startX;
    int currentY = startY;
    int steps = 1;
    while (!isDest(currentX, currentY, { destX,destY }) && !open.empty())
    {
        cell currentCell = MinimumCell(open);
        currentX = currentCell.x;
        currentY = currentCell.y;
        closed.push_back(currentCell);
        int indexToRemove = FindIndex(open, currentCell.x, currentCell.y);
        open = RemoveFromVector(open, indexToRemove);
        vector<point> neighbours;
        if (isValid(currentX, currentY + 1, grid))
        {
            neighbours.push_back({ currentX, currentY + 1 });
        }
        if (isValid(currentX, currentY - 1, grid))
        {
            neighbours.push_back({ currentX, currentY - 1 });
        }
        if (isValid(currentX - 1, currentY, grid))
        {
            neighbours.push_back({ currentX - 1, currentY });
        }
        if (isValid(currentX + 1, currentY, grid))
        {
            neighbours.push_back({ currentX + 1, currentY });
        }
        for (int i = 0; i < neighbours.size(); i++)
        {
            if (!isInVector(closed, neighbours[i].x, neighbours[i].y))
            {
                if (!isInVector(open, neighbours[i].x, neighbours[i].y))
                {
                    open.push_back(cell(neighbours[i].x, neighbours[i].y, { currentX,currentY }, (steps + CalculateF(neighbours[i].x, neighbours[i].y, destX, destY))));
                }
                else
                {
                    int index = FindIndex(open, neighbours[i].x, neighbours[i].y);
                    float newF = steps + CalculateF(neighbours[i].x, neighbours[i].y, destX, destY);
                    if (open[index].f > newF)
                    {
                        open[index].f = newF;
                        open[index].parent = { currentX,currentY };
                    }
                }
            }
        }
        steps += 1;
        if (open.empty() && !isInVector(closed, destX, destY))
		{
            return open;
		}
    }
	
	return closed;
}

int main() 
{
    cout << "Wczytane dane z pliku:\n\n";

    ifstream file;
    file.open("grid.txt");
    vector<vector<int>> grid(20, vector<int>(20, 0));
    int startX = -1;
    int startY = -1;
    int destX = -1;
    int destY = -1;

    //czytanie z pliku i wstawianie do vectora
    while (!file.eof())
    {
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 20; j++)
            {
                file >> grid[i][j];
            }
        }
    }
    file.close();

    ShowGrid(grid);
    
    cout << color(yellow) << "\nWspolrzedne liczymy od lewego dolnego rogu (od 0 do 19). ";
    while (!isValid(startX, startY, grid))
    {
        cout << "\nWpisz wspolrzedne X i Y punktu startowego oddzielone spacja (np. 1 2): ";
        cin >> startX >> startY;
        startY = abs(startY - 19);
    }
    cout << "Wspolrzedne punktu startowego to (" << startX << ", " << abs(startY-19) << ")\n";
    while (!isValid(destX, destY, grid))
    {
        cout << "\nWpisz wspolrzedne X i Y celu oddzielone spacja (np. 3 4): ";
        cin >> destX >> destY;
        destY = abs(destY - 19);
    }
    cout << "Wspolrzedne celu to (" << destX << ", " << abs(destY - 19) << ")\n";
	
    vector<cell> x = AStar(startX, startY, destX, destY, grid);
    if (x.size() == 0)
    {
        cout << color(red) << "\n\nNie udalo sie dotrzec do celu\n\n";
    }
    else
    {
        ShowGrid(MakePathTwo(grid, x, startX, startY));
    }
    //cout << "\Wpisz cos i nacisnij enter zeby zakonczyc: ";
    //cin >> destX;
    return 0;
}

