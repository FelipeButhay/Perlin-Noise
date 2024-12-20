#include "raylib.h"
#include <cmath>
#include <iostream>
#include <ctime>
#include <vector>

#define BACKGORUND (Color){40,40,40,255}

Color gradient(float n){ // -1<= n <=1
    if (n < 0) return (Color){(unsigned char)((1+n)*255.0f), (unsigned char)((1+n)*255.0f), 255, 255};
    if (n == 0) return WHITE;
    if (n > 0) return (Color){255, (unsigned char)((1-n)*255.0f), (unsigned char)((1-n)*255.0f), 255};

    return GREEN;
} 

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

class NoiseLayer{
    public:
        int nx, ny;
        std::vector<std::vector<Vector2>> map;
        std::vector<std::vector<float>> mapRd;

    NoiseLayer(int nx, int ny){
        this->nx = nx;
        this->ny = ny;

        map.resize(nx+1, std::vector<Vector2>(ny+1));
        mapRd.resize(nx+1, std::vector<float>(ny+1));

        for(int i = 0; i<(nx+1); i++){
            for (int j = 0; j<(ny+1); j++){
                float angle = 2.0f * PI * (float)std::rand() / RAND_MAX;
                mapRd[i][j] = angle;
                map[i][j] = (Vector2){(float)cos(mapRd[i][j]), (float)sin(mapRd[i][j])};
            }
        }
    }

    float noise(float pointx, float pointy){ // ambos deben estar entre 0 y nx o ny respectivamente
        float dotProducts[2][2];
        float pointxDP = pointx - (int)pointx;
        float pointyDP = pointy - (int)pointy;
        dotProducts[0][0] = (map[(int)pointx  ][(int)pointy  ].x * (pointxDP    )) + (map[(int)pointx  ][(int)pointy  ].y * (pointyDP    ));
        dotProducts[1][0] = (map[(int)pointx+1][(int)pointy  ].x * (pointxDP - 1)) + (map[(int)pointx+1][(int)pointy  ].y * (pointyDP    ));
        dotProducts[0][1] = (map[(int)pointx  ][(int)pointy+1].x * (pointxDP    )) + (map[(int)pointx  ][(int)pointy+1].y * (pointyDP - 1));
        dotProducts[1][1] = (map[(int)pointx+1][(int)pointy+1].x * (pointxDP - 1)) + (map[(int)pointx+1][(int)pointy+1].y * (pointyDP - 1));

        float u = fade(pointxDP);
        float v = fade(pointyDP);

        float upInter = (1 - u)*dotProducts[0][0] + (u)*dotProducts[1][0];
        float dwInter = (1 - u)*dotProducts[0][1] + (u)*dotProducts[1][1];
        float fiInter = (1 - v)*upInter           + (v)*dwInter;

        return fiInter;
    } 

    void rotateVectors(){
        for(int i = 0; i<(nx+1); i++){
            for (int j = 0; j<(ny+1); j++){
                mapRd[i][j] += (((float)std::rand() / RAND_MAX))*0.1f;
                map[i][j] = (Vector2){(float)cos(mapRd[i][j]), (float)sin(mapRd[i][j])};
            }
        }
    }
};

class NoiseWindow{
    public:
        int sx = 800, sy = 800;
        int nx = 40, ny = 40;
        NoiseLayer L1;
        NoiseLayer L2;
        NoiseLayer L3;

    float noiseLayering(int px, int py){
        return L1.noise(px/80.0f, py/80.0f)/1.0f + L2.noise(px/40.0f, py/40.0f)/2.0f + L3.noise(px/20.0f, py/20.0f)/4.0f;
    }
    
    public:
    NoiseWindow(): L1(10, 10), L2(20, 20), L3(40, 40){
        this->sx = 800;
        this->sy = 800;

        InitWindow(sx, sy, "NOISE");
        SetTargetFPS(60);
    }

    void draw(){
        for(int i = 0; i<sx; i++){
            for (int j = 0; j<sy; j++){
                DrawPixel(i, j, gradient(noiseLayering(i, j))); //gradient(noise(i/10.0f, j/10.0f)));
            }
        }
    }

    void rotateEachLayerVectors(){
        L1.rotateVectors();
        L2.rotateVectors();
        L3.rotateVectors();
    }
};

int main() {
    NoiseWindow noisew;

    while (!WindowShouldClose()) {
        noisew.rotateEachLayerVectors();

        BeginDrawing();
            noisew.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
