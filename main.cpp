#include <SFML/Graphics.hpp>
#include <sstream>

typedef std::vector<std::pair<int,int> > TArea;

class TMap
{
public:
    TMap(int width,int height) {m_data.resize(height*width);Width = width;Height=height;};
    ~TMap(){};
    int* operator[](int i) {return &m_data[i*Width];};
    void Set(int value){m_data.clear(); m_data.resize(Width*Height,value);}
    int Width,Height;
private:
    std::vector<int> m_data;
};

class KeyAddition // Дополнение для обработки клавиатуры
{

    public:
        KeyAddition(sf::Keyboard::Key newKey)
        {
            key = newKey;
        }
        bool isKeyPressure(sf::Event event)
        {
            if (key == event.key.code)
                switch (event.type)
                {
                case sf::Event::KeyReleased:
                    isPressure = false;
                    return false;
                case sf::Event::KeyPressed:
                    if (isPressure)
                    {
                        return false;
                    }
                    else
                    {
                        isPressure = true;
                        return true;
                    }

                default:
                    break;
                }
            return false;
        }
    private:
        sf::Keyboard::Key key;
        bool isPressure = false;
};


TMap initialiseMap(int width, int height, int chanceToStartAlive);//function initialisation, returns a 2d array, takes a 2d array
TMap simStep(TMap& oldMap); //"game of life" style simulation
int countAliveNeighbours(TMap& map, int x, int y); //checks how many of the squares around a certain square are walls
void printMap(TMap, sf::RenderTexture&);
void drawLine(TMap& map,int x0,int  y0,int  x1,int  y1,int filling);
void drawHelp(sf::RenderWindow&);

sf::Time fixedDelay = sf::milliseconds(200.0);

int main()
{

    //allows you to change size of map
    int mapXSize = 200;
    int mapYSize = 200;
    int simSteps = 4;
    int chanceToStartAlive = 41; //percentage of being a wall tile at start
    bool isPaused = false;
    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;
    bool isHelpVisible = false;
    int prevX=0,prevY=0;
    KeyAddition SpaceKey(sf::Keyboard::Key::Space);

    srand(time(NULL));
    TMap map = initialiseMap(mapXSize,mapYSize,chanceToStartAlive); //setting up the map grid

    // Create the main window
    sf::RenderWindow app(sf::VideoMode(800, 600), "Cellular automata SCR", sf::Style::Fullscreen);
    //sf::RenderWindow app(sf::VideoMode(800, 600), "SFML window");
    app.setFramerateLimit(100);

    // Load a sprite to display
    sf::Vector2u size = app.getSize();
    sf::RenderTexture texture;
    texture.create(size.x,size.y);
    /*if (!texture.loadFromFile("cb.bmp"))
        return EXIT_FAILURE;*/
    //sf::Sprite sprite(texture);

    sf::Clock clock;
    sf::Time elapsedTime = sf::milliseconds(0);

    sf::Time previousTime = sf::milliseconds(0);




	// Start the game loop
    while (app.isOpen())
    {

        size = app.getSize();
        float scaleX = static_cast<float>(size.x)/static_cast<float>(mapXSize);
        float scaleY = static_cast<float>(size.x)/static_cast<float>(mapXSize);

        // Process events
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                app.close();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                sf::Vector2i localPosition = sf::Mouse::getPosition(app);
                int mapX = localPosition.x/scaleX;
                int mapY = localPosition.y/scaleY;
                if (isLeftMousePressed)
                    drawLine(map,prevX,prevY,mapX,mapY,1);
                else
                    drawLine(map,mapX,mapY,mapX,mapY,1);
                //map[mapY][mapX] = 1;
                isLeftMousePressed = true;
                prevX = mapX;
                prevY = mapY;
            }
            else isLeftMousePressed = false;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                sf::Vector2i localPosition = sf::Mouse::getPosition(app);
                int mapX = localPosition.x/scaleX;
                int mapY = localPosition.y/scaleY;
                if (isRightMousePressed)
                    drawLine(map,prevX,prevY,mapX,mapY,0);
                else
                    drawLine(map,mapX,mapY,mapX,mapY,0);
                //drawLine(map,prevX,prevY,mapX,mapY,0);
                //map[mapY][mapX] = 0;
                isRightMousePressed = true;
                prevX = mapX;
                prevY = mapY;
            }
            else isRightMousePressed = false;

            if (SpaceKey.isKeyPressure(event))
            //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                isPaused = !isPaused;
            }
            if (event.type == sf::Event::KeyPressed)
            {
                //app.close();
                if (event.key.code == sf::Keyboard::F1) isHelpVisible = !isHelpVisible;
                else if (event.key.code == sf::Keyboard::R)  map = initialiseMap(mapXSize,mapYSize,chanceToStartAlive);
                else if (event.key.code == sf::Keyboard::C)  map.Set(0);
                else if (event.key.code == sf::Keyboard::Add)  fixedDelay+=sf::milliseconds(50);
                else if (event.key.code == sf::Keyboard::Subtract)
                    {if (fixedDelay>sf::milliseconds(0))
                    fixedDelay-=sf::milliseconds(50);}
                else app.close();

            }
        }

        // Clear screen
        app.clear();

        // Draw the sprite
        printMap(map,texture);
        sf::Sprite sprite(texture.getTexture());


        sprite.setScale(scaleX,scaleY);
        sf::View mGUIView = sf::View(sf::FloatRect(0.f, 0.f, size.x, size.y));
        app.setView(mGUIView);

        app.draw(sprite);
        if (isPaused)
        {
            sf::Font fontPaused;
            fontPaused.loadFromFile("arial.ttf");
            sf::Text textPaused("PAUSED",fontPaused);
            textPaused.setFillColor(sf::Color::White);
            sf::FloatRect bounds = textPaused.getLocalBounds();
            textPaused.setPosition((size.x-bounds.width)/2,(size.y-bounds.height)/2);
            app.draw(textPaused);
        }

        if (isHelpVisible) drawHelp(app);

        // Update the window
        app.display();

        sf::Time currentTime = clock.getElapsedTime();
        elapsedTime  = currentTime-previousTime;
        //previousTime = currentTime;


        if (elapsedTime > fixedDelay && !isPaused)
        {
            map = simStep(map);
            previousTime = currentTime;
        }

    }

    return EXIT_SUCCESS;
}


TMap initialiseMap(int width, int height,int chanceToStartAlive) //returns a 2d array of bools
{
    TMap map(width,height); //create map to return

    for (int h=0; h<height; h++)
    {
        for (int w=0; w<width; w++) //initialising values here
        {
            if (rand()%100+1 < chanceToStartAlive) //it has a chance of being a wall or not
            {
                map[h][w] = 1;
            }
            else
            {
                map[h][w] = 0;
            }
        }
    }
    //return our new map
    return map;
}

TMap simStep(TMap& oldMap) //this is the magic "game of life" thingy, google it
{

    int width = oldMap.Width;
    int height = oldMap.Height;
    TMap newMap(width,height);

    //int birthLimit = 4;
    int birthLimit = 3;
    //int deathLimit = 3;
    int deathLimit = 2;

    //iterates through every tile in the map and decides if needs to be born, die, or remain unchanged
    for (int h=0; h<height; h++)
    {
        for (int w=0; w<width; w++)
        {
            int newVal = countAliveNeighbours(oldMap,w,h);
            if (oldMap[h][w])
            {
                //if (newVal<deathLimit)
                if (!(newVal>=deathLimit && newVal<=birthLimit))
                {
                    //if (rand()%10==0) newMap[h][w] = 0;
                    newMap[h][w] = 0;
                }
                else
                {
                    //if (rand()%100!=0) newMap[h][w] = 1;
                    newMap[h][w] = 1;
                }
            }
            else
            {
                //if (newVal>birthLimit)
                if (newVal==birthLimit)
                {
                    newMap[h][w] = 1;
                }
                else
                {
                    newMap[h][w] = 0;
                }
            }
            //some randomness
            if (newMap[h][w]==0 && rand()%1000==0)
            {
                newMap[h][w] = 1;
            }
        }
    }
    return newMap;
}
//counts the number of "alive" cells around the target cell, given a current map and a target x and y coord
int countAliveNeighbours(TMap& map, int x, int y)
{
    int count = 0;
    for (int i=-1; i<2; i++)
    {
        for (int j=-1; j<2; j++)
        {
            int neighbour_x = x+i;
            int neighbour_y = y+j;

            if (i==0&&j==0)
            {
                //do nothing as its our target cell
            }
            //if out of bounds, obviously don't check it, but add one anyway as it counts as an alive neighbour
            else if (neighbour_x<0||neighbour_y<0||neighbour_x>=map.Width||neighbour_y>=map.Height)
            {
                //count +=1;
            }
            else if (map[neighbour_y][neighbour_x]==1)
            {
                count +=1;
            }
        }
    }
    return count;
}

void printMap(TMap map,sf::RenderTexture& texture)
{
    texture.clear(sf::Color::Black);
    sf::Vector2u size = texture.getSize();
    std::vector<sf::Vertex> vertices;
    for (int h=0; h<map.Height; h++)
    {
        for (int w=0; w<map.Width; w++)
        {
            if(map[h][w] == 1)
            {
                vertices.push_back(sf::Vertex(sf::Vector2f(  w,   h), sf::Color::Green));
            }
        }
    }
    //vertices.push_back(sf::Vertex(sf::Vector2f( 10,   10), sf::Color::Red));

    /*sf::View mapView(sf::FloatRect(0, 0, size.x, size.y));
    mapView.setViewport(sf::FloatRect(0, 0, 1, 1));
    texture.setView(mapView);*/

    texture.draw(static_cast<const sf::Vertex *>(&vertices[0]),vertices.size(),sf::PrimitiveType::Points);
    texture.display();
}

void drawLine(TMap& map,int x0,int  y0,int  x1,int  y1,int filling)
{
    if (abs(y1 - y0) < abs(x1 - x0))
    {
        if (x0>x1)
        {
            std::swap(x0,x1);
            std::swap(y0,y1);
        }
        auto dx = x1 - x0;
        auto dy = y1 - y0;
        auto D = 2*dy - dx;
        auto y = y0;

        for (int x=x0;x<=x1;x++)
        {
            map[y][x] = filling;
            if (D>0)
            {
                y++;
                D-=2*dx;
            }
            D+=2*dy;
        }
    }
    else
    {
        if (y0>y1)
        {
            std::swap(x0,x1);
            std::swap(y0,y1);
        }
        auto dx = x1 - x0;
        auto dy = y1 - y0;
        auto D = 2*dx - dy;
        auto x = x0;

        for (int y=y0;y<=y1;y++)
        {
            map[y][x] = filling;
            if (D>0)
            {
                x++;
                D-=2*dy;
            }
            D+=2*dx;
        }
    }

}

void drawHelp(sf::RenderWindow& app)
{
    std::vector<std::string> helpText;
    helpText.push_back("Left mouse button - set alive cell");
    helpText.push_back("Right mouse button - kill cell");
    helpText.push_back("Space - pause simulation");
    helpText.push_back("R - restart simulation, with random");
    helpText.push_back("C - kill all");
    std::stringstream ss;
    ss<<"+ - increase delay (delay = "<<fixedDelay.asMilliseconds()<<"ms)";
    helpText.push_back(ss.str());
    ss.str("");
    ss<<"- - decrease delay (delay = "<<fixedDelay.asMilliseconds()<<"ms)";
    helpText.push_back(ss.str());

    helpText.push_back("Alt+F4 - quit");

    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text textTest("Test",font);
    sf::FloatRect bounds = textTest.getLocalBounds();

    for (int i=0;i<helpText.size();i++)
    {
        sf::Text curString(helpText[i],font);
        curString.setPosition(0,(bounds.height+10)*i);
        curString.setFillColor(sf::Color::Black);
        curString.setOutlineColor(sf::Color::Green);
        curString.setOutlineThickness(3);
        app.draw(curString);
    }
}

