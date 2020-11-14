#include "Bitmap.hpp"
#include <Windows.h>
#include <stdio.h>

void Bitmap::Blit(Bitmap* to, Bitmap* from, int toX, int toY, int fromX, int fromY, int w, int h)
{
	// TODO: assert if (to->bitwidth != from->bitwidth)
	if (toX >= to->w || toY >= to->h || fromX >= from->w || fromY >= from->h)
	{
		return;
	}

	for (int y0 = 0; y0 < h; y0++)
	{
		for (int x0 = 0; x0 < w; x0++)
		{
			int fromXReal = fromX + x0;
			int fromYReal = fromY + y0;
			int fromPixel = (fromYReal * from->w) + fromXReal;
			if (fromPixel < 0 || fromPixel >= from->h * from->w) {
				continue;
			}

			int toXReal = toX + x0;
			int toYReal = toY + y0;
			int toPixel = (toYReal * to->w) + toXReal;
			if (toPixel < 0 || toPixel >= to->h * to->w) {
				continue;
			}

			to->pixels[toPixel] = from->pixels[fromPixel];
		}
	}
}

void SubBitmap::Blit(Bitmap* to, SubBitmap& from, int toX, int toY, int fromX, int fromY, int w, int h)
{
	for (int y0 = toY; y0 < toY + h && y0 < to->h; y0++)
	{
		if (y0 < 0)
		{
			continue;
		}

		int y1 = fromY + (y0 - toY);
		if (y1 < 0)
		{
			continue;
		}
		if (y1 >= from.h)
		{
			break;
		}

		for (int x0 = toX; x0 < toX + w && x0 < to->w; x0++)
		{
			if (x0 < 0)
			{
				continue;
			}

			int x1 = fromX + (x0 - toX);
			if (x1 < 0)
			{
				continue;
			}
			if (x1 >= from.w)
			{
				break;
			}

			basicCoord c;
			c.x = x1;
			c.y = y1;
			*to->GetAt(x0, y0) = from.rows[y1][x1];
		}
	}
}

void SubBitmap::Blit(SubBitmap& to, Bitmap* from, int toX, int toY, int fromX, int fromY, int w, int h)
{
	for (int y0 = toY; y0 < toY + h && y0 < to.h; y0++)
	{
		if (y0 < 0)
		{
			continue;
		}

		int y1 = fromY + (y0 - toY);
		if (y1 < 0)
		{
			continue;
		}
		if (y1 >= from->h)
		{
			break;
		}

		for (int x0 = toX; x0 < toX + w && x0 < to.w; x0++)
		{
			if (x0 < 0)
			{
				continue;
			}

			int x1 = fromX + (x0 - toX);
			if (x1 < 0)
			{
				continue;
			}
			if (x1 >= from->w)
			{
				break;
			}

			to.rows[y0][x0] = from->pixels[(y1 * from->w) + x1];
		}
	}
}