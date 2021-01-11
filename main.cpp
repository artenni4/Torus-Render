/*

Made with help of One Lone Coder Pixel Game Engine by Artenni
Github: https://github.com/OneLoneCoder/olcPixelGameEngine/
Author's Github: https://github.com/artenni4

If you have troubles with fps then try to increase THETA_SPACING and PHI_SPACING vaules
It will decrease accuracy but perfomance will be better

*/


#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <cmath>
#include <cstring>


constexpr unsigned int WINDOW_WIDTH = 100;
constexpr unsigned int WINDOW_HEIGHT = 100;

constexpr float PI = 3.1415926;
constexpr float THETA_SPACING = 0.02f;
constexpr float PHI_SPACING = 0.01f;
constexpr float R1 = 1.f;
constexpr float R2 = 2.f;
constexpr float K2 = 5.f;
constexpr float K1 = WINDOW_WIDTH * K2 * 3 / (8 * (R1 + R2));

class TorusRender : public olc::PixelGameEngine {
public:
	TorusRender()
	{
		std::cout << "Made with One Lone Coder Pixel Game Engine" << std::endl;
		std::cout << "With help of article https://www.a1k0n.net/2011/07/20/donut-math.html" << std::endl;
		std::cout << "By Artenni" << std::endl;

		sAppName = "Torus Render";

		//Initialize variables
		A = B = 0.f;
		Ak = 1.f;
		Bk = 0.6f;
	}
	~TorusRender() override
	{

	}

private:
	char screenBuffer[WINDOW_WIDTH][WINDOW_HEIGHT]; // buffer for out put on screen
	char zBuffer[WINDOW_WIDTH][WINDOW_HEIGHT]; // z buffer
	float A, B; // torus rotation in radians
	float Ak, Bk; // speed of rotation

private:
	void RenderFrame(float fElapsedTime)
	{
		//reset buffers from previous call
		memset(screenBuffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT);
		memset(zBuffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT);

		// A - rotate torus around x axis by A radians
		// B - rotate torus around z axis by B radians
		// One full rotation around x is 2 seconds and around z is 3 seconds (fElapsedTime is in seconds)
		A += Ak * PI * fElapsedTime;
		B += Bk * PI * fElapsedTime;

		// Precompile sines and cosines
		float cosA = cos(A), sinA = sin(A);
		float cosB = cos(B), sinB = sin(B); 

		for (float theta = 0.f; theta < 2 * PI; theta += THETA_SPACING)
		{
			//Precompute sines and cosines
			float sintheta = sin(theta), costheta = cos(theta);

			for (float phi = 0.f; phi < 2 * PI; phi += PHI_SPACING)
			{
				//Precompute sines and cosines of phi
				float sinphi = sin(phi), cosphi = cos(phi);

				//circle coordinates before 3d
				float circlex = R2 + R1 * costheta;
				float circley = R1 * sintheta;

				//final point
				float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
				float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
				float z = K2 + cosA * circlex * sinphi + circley * sinA;
				float rz = 1 / z; //reversed z

				// x and y projections on the screen
				int xp = (int)(WINDOW_WIDTH / 2 + K1 * rz * x);
				int yp = (int)(WINDOW_HEIGHT / 2 + K1 * rz * y);

				//calculate luminance
				float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);
				// L is from -sqrt(2) tp +sqrt(2)
				// The bigger value is more light is on the point

				if (L > 0 && rz > zBuffer[xp][yp])
				{
					zBuffer[xp][yp] = rz;
					//convert luminance from 0 to sqrt(2) into from 0 to 255 (253 actually)
					// sqrt(2) = 1.41; 1.41 * 180 = 253.8; (int)253.8 = 253
					screenBuffer[xp][yp] = L * 180;
				}
			}
		}

	}

public:
	// Set torus' rotating speed in seconds for full rotate (2pi radians)
	void SetRotatingSpeed(float xAxis, float zAxis)
	{
		Ak = 2 / xAxis;
		Bk = 2 / zAxis;
	}

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//Fill screenBuffer
		RenderFrame(fElapsedTime);

		//Display on screen
		for (int x = 0; x < WINDOW_WIDTH; x++)
		{
			for (int y = 0; y < WINDOW_HEIGHT; y++)
			{
				char l = screenBuffer[x][y];
				Draw(x, WINDOW_HEIGHT - 1 - y, olc::Pixel(l, l, l)); // set start of the coordinates from left bottom
				// And you can not judge me for this, I do not want to fuck with inverting y in my RenderFrame function.
			}
		}

		return true;
	}
};

int main(int argc, char* argv[])
{
	TorusRender render;
	render.SetRotatingSpeed(4, 6);
	if (render.Construct(WINDOW_WIDTH, WINDOW_HEIGHT, 6, 6))
		render.Start();

	return 0;
}