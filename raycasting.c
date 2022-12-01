#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "Scene.h"
#include "Quaternion.h"
#include "Camera.h"
#include "Ray.h"
#include "Image.h"

#ifndef M_PI
    #define M_PI (3.14159265358979323846)
#endif
#ifndef M_PIF
    #define M_PIF (3.141592653589793238462643383279502884e+00F)
#endif

void main( int argc, char * argv[] )
{

    Camera camera;
    Image img;
    float position[3] = {0.0, 0.0, 0.0};
    float dir[3] = {0.0, 0.0, 1.0};
    unsigned short res[2] = {2000, 2000};
    float fov[2] = {90, 90};

    Image_set(res[0], res[1], &img);
    Camera_set( position, dir, res, fov, &camera );

    //LIGHTS
    Light * lights;
    lights = (Light *) malloc( sizeof( Light ) * 2 );

    Vec3 light0_position, light0_color;
    Vec3_set(-600.0, 600.0, -600.0, &light0_position);
    Vec3_set(1.0, 1.0, 1.0, &light0_color);

    Light_set( light0_position, light0_color, 0.0002f, &lights[0] );

    Vec3 light1_position, light1_color;
    Vec3_set(0.0, -10.0, -10.0, &light1_position);
    Vec3_set(1.0, 0.0, 0.0, &light1_color);

    Light_set( light1_position, light1_color, 0.0002f, &lights[1] );

    //MATERIALS

    //define some basic colors
    Vec3 grey;
    Vec3_set(0.12, 0.12, 0.12, &grey);

    Material earth, mars, mirror, jade;
    
    //earth
    Image earth_texture;
    Image_import(&earth_texture, "assets/textures/earth.bmp");
    Material_set(&earth, &earth_texture, NULL);

    //mars
    Image mars_texture;
    Image_import(&mars_texture, "assets/textures/mars.bmp");
    Material_set(&mars, &mars_texture, NULL);

    //mirror
    Image mirror_ref;
    Image_import(&mirror_ref, "assets/textures/copper.bmp");
    Material_set(&mirror, NULL, &mirror_ref);
    mirror.default_color = &grey;
    mirror.default_reflection = 0.75f;

    //jade
    Image jade_tex;
    Image_import(&jade_tex, "assets/textures/jade_texture.bmp");
    Material_set(&jade, &jade_tex, &jade_tex);

    //SPHERES
    Sphere * spheres;
    spheres = (Sphere *) malloc( sizeof( Sphere ) * 9);

    Vec3 sphere0_center = { 0, 0, 4 };
    Sphere_set( &sphere0_center, 1.5f, &mirror, &spheres[0] );

    Vec3 sphere1_center = { 0, 5, -3};
    Sphere_set( &sphere1_center, 1, &jade, &spheres[1] );

    Vec3 sphere2_center = { 0, -5, -3};
    Sphere_set( &sphere2_center, 1, &earth, &spheres[2] );

    Vec3 sphere3_center = { 0, 5, 7};
    Sphere_set( &sphere3_center, 1, &mars, &spheres[3] );

    Vec3 sphere4_center = { 0, -5, 7};
    Sphere_set( &sphere4_center, 1, &jade, &spheres[4] );

    Vec3 sphere5_center = { 5, 0, 7};
    Sphere_set( &sphere5_center, 1, &earth, &spheres[5] );

    Vec3 sphere6_center = { -5, 0, 7};
    Sphere_set( &sphere6_center, 1, &jade, &spheres[6] );

    Vec3 sphere7_center = { 5, 0, -3 };
    Sphere_set( &sphere7_center, 1, &jade, &spheres[7] );

    Vec3 sphere8_center = { -5, 0, -3};
    Sphere_set( &sphere8_center, 1, &mars, &spheres[8] );
    
    Scene scene;
    Image skybox;
    Image_import(&skybox, "assets/skybox.bmp");
    Scene_set( &camera, spheres, lights, &skybox, &scene );
    scene.light_count = 1;
    scene.sphere_count = 9;
    
    for (unsigned int t = 0; t < 1; t++)
    {
        clock_t start = clock();

        for (int it = 1; it < scene.sphere_count; it++)
        {
            scene.spheres[it].center->x = cosf( M_PIF/4 + M_PIF * t / 30 + it%4 * M_PIF/2) * 5;
            scene.spheres[it].center->y = sinf( M_PIF/4 + M_PIF * t / 30 + it%4 * M_PIF/2) * 5;
        }

        Scene_render(&scene, &img);

        clock_t end = clock();
        double elapsed = (double) (end - start)/CLOCKS_PER_SEC;
        printf("Ellapsed time (Casting Rays) : %3.4fs\t", elapsed);

        start = clock();

        char filename[100];
        sprintf(filename, "output/image%d.bmp", t);

        Image average;
        Image_set(res[0], res[1], &average);
        Image_average(3, &img, &average);
        Image_export(&average, filename );
        Image_free(&average);

        end = clock();
        elapsed = (double) (end - start)/CLOCKS_PER_SEC;
        printf("Ellapsed time (Exporting image) : %3.4fs\n", elapsed);

    }

    Camera_free( &camera );
    Image_free(&img);

}