
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CORE_FILM_H
#define PBRT_CORE_FILM_H

// core/film.h*
#include "pbrt.h"
#include "geometry.h"
#include "spectrum.h"
#include "filter.h"
#include "stats.h"
#include "parallel.h"


#include "LFRayTracer.h"

#include <set>


namespace pbrt {

// FilmTilePixel Declarations
struct FilmTilePixel {
    Spectrum contribSum = 0.f;
    Float filterWeightSum = 0.f;
    Spectrum splats = 0.f;
};

// Film Declarations
class Film : public lfrt::SampleAccumulator
{
public:
    Film();

    virtual ~Film();

    virtual bool SetSize( const int& width, const int& height ) override;
    virtual int Width()  const override { return fullResolution.x; };
    virtual int Height() const override { return fullResolution.y; };
    virtual bool GetRenderBounds( int& startX, int& startY, int& endX, int& endY ) const override;
    virtual bool GetSamplingBounds( int& startX, int& startY, int& endX, int& endY ) const override;
    virtual lfrt::SampleTile* CreateSampleTile(
        const int& startX, const int& startY,
        const int& sizeX,  const int& sizeY ) override;
    virtual bool MergeSampleTile( lfrt::SampleTile* tile ) override;
    virtual bool DestroySampleTile( lfrt::SampleTile* tile ) override;
    virtual bool GetColor(
        const int& x, const int& y,
        Float& r, Float& g, Float& b ) const override;

    bool Initialize( const ParamSet &params, std::unique_ptr<Filter> filter,
        const int width, const int height );

    Bounds2f GetPhysicalExtent() const;
    void SetImage(const Spectrum *img) const;
    void Clear();
    
    const Float& Diagonal() const { return diagonal; }

    // Film Public Data
    std::unique_ptr<Filter> filter;
    Bounds2i croppedPixelBounds;
    
private:
    // Film Private Data
    struct Pixel {
        Pixel() { xyz[0] = xyz[1] = xyz[2] = filterWeightSum = 0; }
        Float xyz[3];
        Float filterWeightSum;
        AtomicFloat splatXYZ[3];
        Float pad;
    };

    Point2i fullResolution;
    Float diagonal;

    std::unique_ptr<Pixel[]> pixels;
    static PBRT_CONSTEXPR int filterTableWidth = 16;
    Float filterTable[filterTableWidth * filterTableWidth];
    std::mutex mutex;
    Float scale;
    Float maxSampleLuminance;

    std::set<FilmTile*> tiles;
    
    // Film Private Methods
    Pixel &GetPixel(const Point2i &p) {
        CHECK(InsideExclusive(p, croppedPixelBounds));
        int width = croppedPixelBounds.pMax.x - croppedPixelBounds.pMin.x;
        int offset = (p.x - croppedPixelBounds.pMin.x) +
                     (p.y - croppedPixelBounds.pMin.y) * width;
        return pixels[offset];
    }
};


class FilmTile : public lfrt::SampleTile
{
protected:
     FilmTile(const Bounds2i &pixelBounds, const Vector2f &filterRadius,
             const Float *filterTable, int filterTableSize,
             Float maxSampleLuminance)
        : pixelBounds(pixelBounds),
          filterRadius(filterRadius),
          invFilterRadius(1 / filterRadius.x, 1 / filterRadius.y),
          filterTable(filterTable),
          filterTableSize(filterTableSize),
          maxSampleLuminance(maxSampleLuminance) {
        pixels = std::vector<FilmTilePixel>(std::max(0,
        pixelBounds.Area()));
    }

    virtual ~FilmTile();

public:

    virtual bool AddSample(
		const lfrt::VEC2& raster,
		const lfrt::VEC2& secondary,
		const Float& sampleWeight,
		const Float& rayWeight,
		const Float& r, const Float& g, const Float& b,
        const bool isWeighted = true
	) override;


    FilmTilePixel &GetPixel(const Point2i &p) {
        CHECK(InsideExclusive(p, pixelBounds));
        int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
        int offset =
            (p.x - pixelBounds.pMin.x) + (p.y - pixelBounds.pMin.y) * width;
        return pixels[offset];
    }
    const FilmTilePixel &GetPixel(const Point2i &p) const {
        CHECK(InsideExclusive(p, pixelBounds));
        int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
        int offset =
            (p.x - pixelBounds.pMin.x) + (p.y - pixelBounds.pMin.y) * width;
        return pixels[offset];
    }
    Bounds2i GetPixelBounds() const { return pixelBounds; }

  private:
    // FilmTile Private Data
    const Bounds2i pixelBounds;
    const Vector2f filterRadius, invFilterRadius;
    const Float *filterTable;
    const int filterTableSize;
    std::vector<FilmTilePixel> pixels;
    const Float maxSampleLuminance;
    friend class Film;
};


}  // namespace pbrt

#endif  // PBRT_CORE_FILM_H
