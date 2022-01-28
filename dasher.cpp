#include "raylib.h"
#include <iostream> 
#include <string> 

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
} ;

void DrawEntity(Texture2D texture, AnimData data);
bool isOnGound(AnimData data, int windowHeight);
AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame);

void UpdateBg(Texture2D texture, float& xPos, const float& dT, int vel);



int main(){

    int windowDimensions[2];

    windowDimensions[0] = 512;
    windowDimensions[1] = 380;


    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");

    // Nebula
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    // AnimData for nebula

    const int sizeOfNebulae{3};
    AnimData nubulae[sizeOfNebulae]{};

    for(int i=0;i<sizeOfNebulae;i++){
        nubulae[i].rec.x = 0.0;
        nubulae[i].rec.y = 0.0;
        nubulae[i].rec.width = nebula.width/8;
        nubulae[i].rec.height = nebula.height/8;
        nubulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nubulae[i].frame = 0;
        nubulae[i].runningTime = 0.0;
        nubulae[i].updateTime = 0.0;
        nubulae[i].pos.x = windowDimensions[0] + (i*500);
    }


    float finishLine{nubulae[sizeOfNebulae-1].pos.x + 100};

    // Nebula X velocity (pixels/sec)
    int nebVel{-300};

    // Scarfy
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;

    scarfyData.rec.width =  scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = (1.0f / 10.0f);
    scarfyData.runningTime = 0.0f;

    int velocity{0};
    // //* acceleration due to gravity (pixels/frame)/frame
    // acceleration due to gravity (pixels/s)/s
    const int gravity{800};

    // pixel per sec
    const int jumpVel{-600};
    bool isInAir{false};    

    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    float bgX{};
    float mdX{};
    float frX{};
    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        // delta time
        const float dT{GetFrameTime()}; 

        BeginDrawing();
        ClearBackground(WHITE);


        UpdateBg(background,bgX,dT, 20);
        UpdateBg(midground,mdX,dT, 40);
        UpdateBg(foreground,frX,dT, 80);

        DrawText(std::to_string(GetFPS()).c_str(),0,0,20,GREEN);
        DrawText(std::to_string(finishLine).c_str(),0,30,20,RED);


        // ground check
        if(isOnGound(scarfyData, windowDimensions[1])){
            // rect on the ground
            velocity = 0;
            isInAir = false;
        }else{
            // apply gravity
            velocity += gravity * dT;
            isInAir = true;
        }

        // check for jump
        if(IsKeyPressed(KEY_SPACE) && !isInAir){
            velocity+=jumpVel;
        }

        // update nebula position
        for(int i=0;i<sizeOfNebulae;i++){
            nubulae[i].pos.x += nebVel * dT;      
        }

        // update finish line
        finishLine += nebVel * dT;


        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        //printf("Frame: %d\n", frame);
        // update running time

        if(!isInAir){
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        for(int i = 0; i < sizeOfNebulae;i++){
            nubulae[i] =  updateAnimData(nubulae[i], dT, 7);
        }

  
        for(AnimData nebula : nubulae){
            float pad{20};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };

            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            }; 

            if(CheckCollisionRecs(nebRec, scarfyRec)){
                collision = true;
            }           

        }

      
        if(!collision){
        // Draw Nebula
            for(int i=0;i<sizeOfNebulae;i++){
                DrawEntity(nebula, nubulae[i]);
            }              
            DrawEntity(scarfy, scarfyData);
        }
        else{
            DrawText("Game Over!",windowDimensions[0]/4,windowDimensions[1]/2,50,RED);
        }

        if(scarfyData.pos.x >= finishLine && !collision){
            DrawText("Finished!",windowDimensions[0]/4,windowDimensions[1]/2,50,GREEN);
        }

        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);

    UnloadTexture(midground);
    UnloadTexture(foreground);
    UnloadTexture(background);

    CloseWindow();
    return 0;
}


void DrawEntity(Texture2D texture, AnimData data){
    DrawTextureRec(texture, data.rec,  data.pos, WHITE);
}

bool isOnGound(AnimData data, int windowHeight){
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame){
    
    // update running time
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime){
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame){
            data.frame = 0;
        }
    }

    return data;
}


void UpdateBg(Texture2D texture, float& xPos, const float& dT, int vel){


       xPos -= vel * dT;
        if(xPos <= -texture.width*2){
            xPos = 0;
        }                
        
        // draw bg
        Vector2 _1Pos{xPos, 0.0};
        DrawTextureEx(texture, _1Pos, 0.0, 2.0, WHITE);
        Vector2 _2Pos{xPos + texture.width*2, 0.0};
        DrawTextureEx(texture, _2Pos, 0.0, 2.0, WHITE);


}


