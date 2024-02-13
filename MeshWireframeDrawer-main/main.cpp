#include <vector>
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

void triangle(Vec2i points[3]) {
    Vec2i bbox[2] = find_bounding_box(points)

}



void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
    if (t0.y == t1.y & t0.y == t2.y) { return ;}

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
            image.set(i, y, red);
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
            image.set(i, y, red);
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
        model = new Model("cc.obj");
    }
    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0, 0, -1);

  for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec2i screen_coords[3];
            Vec3f world_coords[3];

            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);

            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;

            line(x0, y0, x1, y1, image, white);
        }
    }

/*    Vec2i t0[3] = { Vec2i(10,70),Vec2i(50,160) ,Vec2i(70,80) };
    Vec2i t1[3] = { Vec2i(180,50),Vec2i(150,1) ,Vec2i(70,180) };
    Vec2i t2[3] = { Vec2i(180,150),Vec2i(120,160) ,Vec2i(130,180) };

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, green);
    triangle(t2[0], t2[1], t2[2], image, blue);
*/
    image.flip_vertically(); // we want to have the origin at the left bottom corner of the image

    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

