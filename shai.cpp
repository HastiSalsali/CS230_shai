// Yaish, Shai CS230 Section 11091
// Lab 2 - Bresenham's Line Algorithm
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include "windows.h"
using namespace std;

/// Pseudo code for Bresenham's Line Algorithm
// (Prompt user for two pairs of int numbers in decimal)
// (Verify that they are within the range of the bitmap (0-256)
// (Calculate the line path using integer arithmetic only)

// The following defines the size of the square image in pixels.
#define IMAGE_SIZE 256

// Function prototypes
void bresenLine(unsigned char bits[][IMAGE_SIZE], int, int, int, int);

int main(int argc, char* argv[])
{
	// Bit map file and information headers.
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	// Variables for the line
	int x0, y0, x1, y1;

	char colorTable[1024]; // Color table

	// The following defines the array which holds the image.  
	unsigned char bits[IMAGE_SIZE][IMAGE_SIZE] = { 0 }; // Array for Bitmap (bresenham)
	char grayScale[IMAGE_SIZE][IMAGE_SIZE];
	int i, j;

	// Program info
	cout << "Pierce College CS230 Spring 2025 Lab Assignment 2 - YAISH, SHAI\n";

	//// User Input starts here (two pairs of coordinates)
	cout << "Enter two pairs of point coordinates in the range of 0 to " << IMAGE_SIZE - 1 << endl;
	cin >> x0 >> y0 >> x1 >> y1;

	// Input validation for bitmap range (shorter version)
	while (x0 < 0 || x0 > IMAGE_SIZE - 1
		|| x1 < 0 || x1 > IMAGE_SIZE - 1
		|| y0 < 0 || y0 > IMAGE_SIZE - 1
		|| y1 < 0 || y1 > IMAGE_SIZE - 1) {

		if (x0 < 0 || x0 > IMAGE_SIZE - 1) {
			printf("Value %d out of range, ending.\n", x0);
		}
		if (x1 < 0 || x1 > IMAGE_SIZE - 1) {
			printf("Value %d out of range, ending.\n", x1);
		}
		if (y0 < 0 || y0 > IMAGE_SIZE - 1) {
			printf("Value %d out of range, ending.\n", y0);
		}
		if (y1 < 0 || y1 > IMAGE_SIZE - 1) {
			printf("Value %d out of range, ending.\n", y1);
		}

		return -1;
	}

	cout << "User input is valid. Starting line drawing...\n";

	// Initialize the bit map file header with static values.
	bmfh.bfType = 0x4d42;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih) + sizeof(colorTable);
	bmfh.bfSize = bmfh.bfOffBits + sizeof(bits);

	// Initialize the bit map information header with static values.
	bmih.biSize = 40;
	bmih.biWidth = IMAGE_SIZE;
	bmih.biHeight = IMAGE_SIZE;
	bmih.biPlanes = 1;
	bmih.biBitCount = 8;
	bmih.biCompression = 0;
	bmih.biSizeImage = IMAGE_SIZE * IMAGE_SIZE;
	bmih.biXPelsPerMeter = 2835;  // magic number (adjustable)
	bmih.biYPelsPerMeter = 2835;
	bmih.biClrUsed = 256;
	bmih.biClrImportant = 0;


	// Build color table.
	for (i = 0; i < 256; i++) {
		j = i * 4;

		// Set the line color to red
		if (i == 255) {
			colorTable[j] = 255; // Red
			colorTable[j + 1] = 0; // Green
			colorTable[j + 2] = 0; // Blue
			colorTable[j + 3] = 0; // Alpha
		}
		else {
			// Adds colors together to create gray scale.
			colorTable[j] = colorTable[j + 1] = colorTable[j + 2] = i;
			colorTable[j + 3] = 0;
		}
	}

	// Build gray scale array of bits in image, 
	for (i = 0; i < IMAGE_SIZE; i++) {
		for (j = 0; j < IMAGE_SIZE; j++) {
			grayScale[i][j] = j; // displaying black to white left to right
		}
	}

	// Combine the line with the grayscale gradient
	for (i = 0; i < IMAGE_SIZE; i++) {
		for (j = 0; j < IMAGE_SIZE; j++) {
			if (bits[i][j] != 255) {
				bits[i][j] = grayScale[i][j]; // use the grayscale gradient
			}
		}
	}

	// Function call to DRAW THE LINE
	bresenLine(bits, x0, y0, x1, y1);

	// Debug: Check if any pixels were drawn
	//cout << "Checking the bitmap after drawing...\n";
	//cout << "Pixel at (x0, y0): " << (int)bits[x0][y0] << "\n";
	//cout << "Pixel at (x1, y1): " << (int)bits[x1][y1] << "\n";

	// Define and open the output file. 
	ofstream bmpOut("foo.bmp", ios::out + ios::binary);
	if (!bmpOut) {
		cout << "...could not open file, ending.";
	}

	// Write out the bit map.  
	bmpOut.write((char*)&bmfh, sizeof(bmfh));
	bmpOut.write((char*)&bmih, sizeof(bmih));
	bmpOut.write(colorTable, sizeof(colorTable));
	bmpOut.write((char*)bits, sizeof(bits));
	bmpOut.close();

	bmpOut.flush();  // Ensure the buffer is written

	// Now let's look at our creation.
	system("mspaint.exe foo.bmp");

	// Done.
	return 0;
}



// Bresenham's Line Algorithm (integer arithmetic only)
void bresenLine(unsigned char bits[IMAGE_SIZE][IMAGE_SIZE], int x0, int y0, int x1, int y1)
{

	// Check if the input values are within the valid range
	if (x0 < 0 || x0 >= IMAGE_SIZE || x1 < 0 || x1 >= IMAGE_SIZE ||
		y0 < 0 || y0 >= IMAGE_SIZE || y1 < 0 || y1 >= IMAGE_SIZE)
	{
		return;
	}

	// dx = change of x, dy = change of y
	// sx = direction of x, sy = direction of y
	// err = error; error from the previous point

	int dxx = x1 - x0;
	int dy = y1 - y0;

	int sx = (x1 - x0 > 0) ? 1 : -1; // Step direction for x
	int sy = (y1 - y0 > 0) ? 1 : -1; // Step direction for y
	int err = dxx - dy;
	int e2;

	unsigned char* flatBits = &bits[0][0]; // Get base address of the 2D array

	__asm {
		push eax;
		push ebx;
		push ecx;
		push edx;
		push esi;
		push edi;

		// Initialize registers
		mov ecx, x0
		mov edx, y0
		mov eax, err
		mov ebx, flatBits
		mov esi, sx
		mov edi, sy

		// Loop until end of line
		loop_start :
		// Check if points are the same (stop the loop)
		cmp ecx, x1
			jne not_same
			cmp edx, y1
			jne not_same
			jmp done

			not_same :

		// Calculate pixel index
		mov eax, ecx
			imul eax, IMAGE_SIZE
			add eax, edx
			mov edi, IMAGE_SIZE
			mul edi, edi
			cmp eax, edi
			jge skip_write
			mov byte ptr[ebx + eax], 255

			skip_write:
		// Update error term
		mov eax, err
			shl eax, 1
			mov e2, eax // Store error in e2
			sub eax, dy
			cmp eax, dxx
			jl skip_x_update
			add eax, dxx
			add edx, sy

			skip_x_update :
		// Update x
		mov eax, e2 // Load error from e2
			//cmp eax, -dy
			mov edi, dy
			neg edi
			cmp eax, edi
			jl skip_y_update
			//sub err, dy
			mov edi, err
			sub edi, dy
			mov err, edi
			add ecx, sx

			skip_y_update :
		mov eax, e2 // load error from e2
			cmp eax, dxx
			jl skip_err_update
			//add err, dxx
			mov edi, err
			add edi, dxx
			mov err, edi
			add edx, sy

			skip_err_update :
		mov eax, err
			mov err, eax
			jmp loop_start

			done :
		pop edi;
		pop esi;
		pop edx;
		pop ecx;
		pop ebx;
		pop eax;
	}
	cout << "Finished drawing line from (" << x0 << ", " << y0 << ") to (" << x1 << ", " << y1 << ")\n";

}
/*	// Check if the points are the same
	while (true)
	{
		int e2 = err * 2; // Temp variable to store error (Crucial for the algorithm!)

		// Checks if the points (x0, y0) are within the range
		if (x0 >= 0 && x0 < IMAGE_SIZE && y0 >= 0 && y0 < IMAGE_SIZE) {
			bits[x0][y0] = 255;
		}

		// Check if the points are the same (stop the loop)
		if (x0 == x1 && y0 == y1) break;

		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dxx) {
			err += dxx;
			y0 += sy;
		}
	}
*/