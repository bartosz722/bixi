#ifndef UTILGLDRAW_H_
#define UTILGLDRAW_H_

#include <glm/glm.hpp>

void drawCircle(double centerX, double centerY, double radius, int parts);
void drawFilledCircle(double centerX, double centerY, double centerZ, double radius, int parts);

void drawGlutSphere(double centerX, double centerY, double centerZ,
                    double radius, int slices, int stacks, bool solid);

void drawGluSphere(double centerX, double centerY, double centerZ,
                   double radius, int slices, int stacks, bool enableTexturing);

void drawGluCylinder(double base, double top, double height,
                     int slices, int stacks, bool enableTexturing);

#endif
