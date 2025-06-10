#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

static bool allowMove = false;
//colors for background and snake
Color purple = {161, 150, 255, 255};
Color darkPurple = {87, 49, 117, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque){
    for (unsigned int i = 0; i < deque.size(); i++){
        if (Vector2Equals(deque[i], element)){
            return true;
        }

    }
    return false;
}

//initializes and controls how fast the snake moves
bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

//class where snake body is made
class Snake{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw(){
        for (unsigned int i = 0; i < body.size(); i++){
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};   
            DrawRectangleRounded(segment, 0.5, 6, darkPurple);
        }
    }

    //updates snake body
    void Update(){
        body.push_front(Vector2Add(body[0], direction));
        if(addSegment == true){
            addSegment = false;
        }
        else {
            body.pop_back();
        }

    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }

};


//class where game target is made
class Food{

public:
    Vector2 position;
    Texture2D texture;


    //loads image from folder
    Food(deque<Vector2> snakeBody){
        Image image = LoadImage("Assets/candy.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);

    }

    ~Food(){
        UnloadTexture(texture);
    }

    //draw the image on the screen
    void Draw(){
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);

    }
    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    //generates random position so that food is in diff spot everytime
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)){
        position = GenerateRandomCell();
        }
        return position;
    }
};

class Game{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("Assets/eat.mp3");
        wallSound = LoadSound("Assets/wallCrash.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }


    void Draw(){
        food.Draw();
        snake.Draw();
    }

    void Update(){
        if (running){
        snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithEdges();
        CheckCollisionWithTail();
        }
    }

    //chekcs if snake eats, then changes position of target
    void CheckCollisionWithFood(){
        if(Vector2Equals(snake.body[0], food.position)){
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }
    
    //checking if snake collides with the walls
    void CheckCollisionWithEdges(){
        if (snake.body[0].x == cellCount || snake.body[0].x == -1){
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1){
            GameOver();
        }

    }

    void GameOver(){
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

     void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }

};

//main function that handles running the game
int main()
{
    cout << "Starting the game..." << endl;
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
    SetTargetFPS(60);

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (eventTriggered(0.2))
        {
            allowMove = true;
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && allowMove)
        {
            game.snake.direction = {0, -1};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && allowMove)
        {
            game.snake.direction = {0, 1};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && allowMove)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && allowMove)
        {
            game.snake.direction = {1, 0};
            game.running = true;
            allowMove = false;
        }

        //Starts clean background everytime game is run
        ClearBackground(purple);
         DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkPurple);
        DrawText("Retro Snake", offset - 5, 20, 40, darkPurple);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkPurple);
        game.Draw();


        EndDrawing();

    }

    
    CloseWindow();
    return 0;


}