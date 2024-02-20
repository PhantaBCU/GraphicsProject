#include <vector>
#include <iostream>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0  , 0  , 255);
const TGAColor green = TGAColor(0  , 255, 0  , 255);
const TGAColor blue  = TGAColor(0  , 0  , 255, 255);

Model *model = NULL;
const int width  = 800;
const int height = 800;

// Write the line method here
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor colour) {
    //Determine whether step should be x or y
    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    //Determine whether drawing left to right or right to left
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++) {
        float t = static_cast<float>(x - x0) / static_cast<float>(x1 - x0);
        float y = y0 * (1 - t) + (y1 * t);
        if (steep) {
            image.set(y, x, colour);
        }
        else {
            image.set(x, y, colour);
        }
    }
}

void line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor colour) {
    //Determine whether step should be x or y
    bool steep = false;
    if (abs(t0.x - t1.x) < abs(t0.y - t1.y)) {
        std::swap(t0.x, t0.y);
        std::swap(t1.x, t1.y);
        steep = true;
    }
    //Determine whether drawing left to right or right to left
    if (t0.x > t1.x) {
        std::swap(t0.x, t1.x);
        std::swap(t0.y, t1.y);
    }

    for (int x = t0.x; x <= t1.x; x++) {
        float t = static_cast<float>(x - t0.x) / static_cast<float>(t1.x - t0.x);
        float y = t0.y * (1 - t) + (t1.y * t);
        if (steep) {
            image.set(y, x, colour);
        }
        else {
            image.set(x, y, colour);
        }
    }
}

bool computeBarycentricPoint(Vec2i vertices[3], Vec2i point)
{
    Vec2i v0 = vertices[1] - vertices[0];
    Vec2i v1 = vertices[2] - vertices[0];
    Vec2i v2 = point - vertices[0];

    float d00 = (v0.x * v0.x + v0.y * v0.y);
    float d01 = (v0.x * v1.x + v0.y * v1.y);
    float d11 = (v1.x * v1.x + v1.y * v1.y);
    float d20 = (v2.x * v0.x + v2.y * v0.y);
    float d21 = (v2.x * v1.x + v2.y * v1.y);

    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;

    float u = 1.0f - v - w;

    if (u >= 0.0f && v >= 0.0f && w >= 0.0f) {
        return true;
    }
    else {
        return false;
    }
}

void triangle(Vec2i vertices[3], TGAImage& image, TGAColor color) {

    int minX, minY, maxX, maxY;

    minX = minY = INT_MAX;
    maxX = maxY = INT_MIN;

    computeBoundingBox(minX, minY, maxX, maxY, vertices);

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (computeBarycentricPoint(vertices, Vec2i(x, y))) {
                image.set(x, y, color);
            }
        }
    }

}

void computeBoundingBox(int& minX, int& minY, int& maxX, int& maxY, Vec2i vertices[]) {
    for (int vert = 0; vert < 3; vert++) {
        if (minX > vertices[vert].x) {
            minX = vertices[vert].x;
        }
        if (maxX < vertices[vert].x) {
            maxX = vertices[vert].x;
        }
        if (minY > vertices[vert].y) {
            minY = vertices[vert].y;
        }
        if (maxY < vertices[vert].y) {
            maxY = vertices[vert].y;
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
    if (t0.y == t1.y && t0.y == t2.y) { return; }

    if (t0.y > t1.y) { std::swap(t0, t1); }
    if (t0.y > t2.y) { std::swap(t0, t2); }
    if (t1.y > t2.y) { std::swap(t1, t2); }

    int total_height = t2.y - t0.y;

    for (int y = t0.y; y <= t1.y; y++) {
        int segment_height = t1.y - t0.y + 1;
        float alpha = (static_cast<float>(y) - t0.y) / total_height;
        float beta = (static_cast<float>(y) - t0.y) / segment_height;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;
        if (A.x > B.x) { std::swap(A,B); }
        for (int i = A.x; i <= B.x; i++) {
            image.set(i, y, color);
        }
    }

    for (int y = t1.y; y <= t2.y; y++) {
        int segment_height = t2.y - t1.y + 1;
        float alpha = (static_cast<float>(y) - t0.y) / total_height;
        float beta = (static_cast<float>(y) - t1.y) / segment_height;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t1 + (t2 - t1) * beta;

        if (A.x > B.x) { std::swap(A, B); }
        for (int i = A.x; i <= B.x; i++) {
            image.set(i, y, color);
        }
    }
    
    //line(t0, t1, image, white);
    //line(t1, t2, image, blue);
    //line(t2, t0, image, green);
}


int main(int argc, char** argv) {
    
    if (argc == 2) {
        model = new Model(argv[1]);
    }
    else{
        model = new Model("assets/cc.obj");
    }
    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0, 0, -1);

    int allowedIntensityCount = 0;
    int failedIntensityCount = 0;

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i(
                (v.x + 1.0f) * width / 2.0f,
                (v.y + 1.0f) * height / 2.0f
            );
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0) {
            triangle(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }
    Vec2i tempTriangle[] = {Vec2i(200, 100),Vec2i(100, 100),Vec2i(150, 200)};
    Vec2i tempTriangle2[] = { Vec2i(150, 200),Vec2i(200, 100),Vec2i(400, 200) };


    triangle(tempTriangle, image, red);
    triangle(tempTriangle2, image, red);


    image.flip_vertically(); // we want to have the origin at the left bottom corner of the image

    image.write_tga_file("output/output.tga");
    delete model;
    return 0;
}

