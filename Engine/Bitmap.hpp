// Really basic bitmapping class.
#pragma once
#include <stdlib.h>
#include <math.h>

struct basicCoord 
{
	int x;
	int y;
};

class Bitmap
{
	const int w, h;
	unsigned char* pixels;
	const size_t size;
	
public:
	Bitmap(int _w, int _h)
		: w(_w), h(_h), size(w * h)
	{
		pixels = (unsigned char*)malloc(size);
	}

	Bitmap(const Bitmap* other, int x0, int y0, int _w, int _h)
		: Bitmap(_w, _h)
	{
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				pixels[(y * h) + x] = other->pixels[((y0 + y) * w) + (x0 + x)];
			}
		}
	}

	~Bitmap()
	{
		free(pixels);
	}

	static void Blit(Bitmap* to, Bitmap* from, int toX, int toY, int fromX, int fromY, int w, int h);

	void Clear(unsigned char toPixel = 0)
	{
		for (int i = 0; i < size; i++)
		{
			pixels[i] = toPixel;
		}
	}

	unsigned char* operator[](basicCoord c)
	{
		int x = c.x;
		int y = c.y;

		return GetAt(x, y);
	}

	unsigned char* operator[](int idx)
	{
		return GetAt(idx);
	}

	const int GetW() 
	{
		return w;
	}

	const int GetH()
	{
		return h;
	}

	unsigned char* Get() const {
		return pixels;
	}

	unsigned char* GetAt(int idx)
	{
		if (idx < 0 || idx >= size)
		{
			return nullptr;
		}
		return &pixels[idx];
	}

	unsigned char* GetAt(int x, int y)
	{
		if (x < 0 || x >= w)
		{
			return nullptr;
		}
		else if (y < 0 || y >= h)
		{
			return nullptr;
		}
		return &pixels[(y * w) + x];
	}

	friend class SubBitmap;
};

class SubBitmap
{
	int w, h, size;
	unsigned char** rows;

public:
	SubBitmap(Bitmap* parent, int x0, int y0, int _w, int _h)
		: w(_w), h(_h), size(_w * _h)
	{
		rows = new unsigned char*[h];
		for (int y = 0; y < h; y++)
		{
			rows[y] = &parent->pixels[((y0 + y) * parent->w) + x0];
		}
	}

	SubBitmap() : w(0), h(0), size(0), rows(nullptr) {}

	SubBitmap(const SubBitmap& other)
		: w(other.w), h(other.h), size(other.size)
	{
		rows = new unsigned char*[h];
		for (int y = 0; y < h; y++)
		{
			rows[y] = other.rows[y];
		}
	}

	~SubBitmap()
	{
		if (size > 0)
		{
			delete[] rows;
		}
	}

	unsigned char* GetAtIdx(int idx)
	{
		if (idx < 0 || idx >= size)
		{
			return nullptr;
		}

		return &rows[(int)floor(idx / w)][idx % w];
	}

	unsigned char* operator[](int idx)
	{
		GetAtIdx(idx);
	}

	unsigned char* GetAtCoord(const basicCoord& c)
	{
		const int x = c.x;
		const int y = c.y;

		return &rows[y][x];
	}

	unsigned char* operator[](const basicCoord& c)
	{
		return GetAtCoord(c);
	}

	SubBitmap& operator=(const SubBitmap& other)
	{
		this->w = other.w;
		this->h = other.h;
		this->size = other.size;

		if (this->rows != nullptr)
		{
			delete[] this->rows;
		}
		this->rows = new unsigned char*[h];
		for (int y = 0; y < h; y++)
		{
			this->rows[y] = other.rows[y];
		}
		return *this;
	}

	void Clear(int toColor = 0)
	{
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				rows[y][x] = toColor;
			}
		}
	}

	static void Blit(Bitmap* to, SubBitmap& from, int toX, int toY, int fromX, int fromY, int w, int h);

	static void Blit(SubBitmap& to, Bitmap* from, int toX, int toY, int fromX, int fromY, int w, int h);
};
