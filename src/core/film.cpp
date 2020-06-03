
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


// core/film.cpp*
#include "film.h"
#include "paramset.h"
#include "imageio.h"
#include "stats.h"

namespace pbrt {

STAT_MEMORY_COUNTER("Memory/Film pixels", filmPixelMemory);

// Film Method Definitions

Film::Film() {}

Film::~Film() {}

bool Film::SetSize(const int &width, const int &height) { return false; }


bool Film::GetRenderBounds( int &startX, int &startY, int &endX, int &endY ) const
{
    startX = croppedPixelBounds.pMin.x;
    startY = croppedPixelBounds.pMin.y;
    endX   = croppedPixelBounds.pMax.x;
    endY   = croppedPixelBounds.pMax.y;
    return true;
}


bool Film::GetSamplingBounds( int &startX, int &startY, int &endX, int &endY ) const
{
    startX = std::floor(croppedPixelBounds.pMin.x) + 0.5 - filter->radius.x;
    startY = std::floor(croppedPixelBounds.pMin.y) + 0.5 - filter->radius.y;
    endX   = std::ceil( croppedPixelBounds.pMax.x) - 0.5 + filter->radius.x;
    endY   = std::ceil( croppedPixelBounds.pMax.y) - 0.5 + filter->radius.y;
    return true;
}


lfrt::SampleTile *Film::CreateSampleTile(
    const int &startX, const int &startY, const int &sizeX, const int &sizeY )
{
    // Bound image pixels that samples in _sampleBounds_ contribute to
    Vector2f halfPixel = Vector2f( 0.5f, 0.5f );
    const Bounds2i sampleBounds( Point2i(startX,startY), Point2i(startX+sizeX,startY+sizeY) );
    Bounds2f floatBounds = (Bounds2f)sampleBounds;
    Point2i p0 = (Point2i)Ceil(floatBounds.pMin - halfPixel - filter->radius);
    Point2i p1 = (Point2i)Floor(floatBounds.pMax - halfPixel + filter->radius) + Point2i(1, 1);
    Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), croppedPixelBounds);

    FilmTile* tile = new FilmTile( tilePixelBounds, filter->radius, filterTable,
                     filterTableWidth, maxSampleLuminance);
    tiles.insert(tile);

    return tile;
}


bool Film::MergeSampleTile( lfrt::SampleTile *tile )
{
    // Original PBRT-v3 function:
    // void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile)
    ProfilePhase p(Prof::MergeFilmTile);
    FilmTile *filmTile = dynamic_cast<FilmTile*>(tile);
    if ( filmTile == nullptr )
        return false;
    VLOG(1) << "Merging film tile " << filmTile->pixelBounds;
    std::lock_guard<std::mutex> lock(mutex);
    for ( Point2i pixel : filmTile->GetPixelBounds() ) {
        // Merge _pixel_ into _Film::pixels_
        const FilmTilePixel &tilePixel = filmTile->GetPixel(pixel);
        Pixel &mergePixel = GetPixel(pixel);
        Float xyz[3];
        tilePixel.contribSum.ToXYZ(xyz);
        for (int i = 0; i < 3; ++i) mergePixel.xyz[i] += xyz[i];
        mergePixel.filterWeightSum += tilePixel.filterWeightSum;
        Float xyzSplat[3];
        tilePixel.splats.ToXYZ(xyzSplat);
        for (int i = 0; i < 3; ++i) mergePixel.splatXYZ[i].Add( xyzSplat[i] );
    }
    return false;
}


bool Film::DestroySampleTile( lfrt::SampleTile *tile )
{
    FilmTile* filmTile = dynamic_cast<FilmTile*>(tile);
    if ( filmTile == nullptr )
        return false;
    if ( tiles.find(filmTile) == tiles.end() )
        return false;
    tiles.erase( filmTile );
    delete filmTile;
    return true;
}


bool Film::GetColor( const int &x, const int &y, Float &r, Float &g, Float &b ) const
{
    const Point2i coord = Point2i(x,y);
    if ( !InsideExclusive(coord, croppedPixelBounds) )
        return false;
    const int width = croppedPixelBounds.pMax.x - croppedPixelBounds.pMin.x;
    const int offset = (coord.x - croppedPixelBounds.pMin.x) +
                       (coord.y - croppedPixelBounds.pMin.y) * width;

    const auto& pixel = pixels[offset];

    Float rgb[3];
    XYZToRGB( pixel.xyz, rgb );

    Float filterWeightSum = pixel.filterWeightSum;
    if (filterWeightSum != 0)
    {
        const Float invWt = (Float)1 / filterWeightSum;
        rgb[0] = std::max( (Float)0, rgb[0] * invWt );
        rgb[1] = std::max( (Float)0, rgb[1] * invWt );
        rgb[2] = std::max( (Float)0, rgb[2] * invWt );
    }

    // Add splat value at pixel
    const Float splatScale = (Float)1;
    Float splatRGB[3];
    Float splatXYZ[3] = { pixel.splatXYZ[0], pixel.splatXYZ[1], pixel.splatXYZ[2] };
    XYZToRGB( splatXYZ, splatRGB );
    rgb[0] += splatScale * splatRGB[0];
    rgb[1] += splatScale * splatRGB[1];
    rgb[2] += splatScale * splatRGB[2];

    r = scale * rgb[0];
    g = scale * rgb[1];
    b = scale * rgb[2];

    return true;
}


bool Film::Initialize( const ParamSet &params, std::unique_ptr<Filter> filt )
{
    int xres = params.FindOneInt("xresolution", 1280);
    int yres = params.FindOneInt("yresolution", 720);
    if (PbrtOptions.quickRender) xres = std::max(1, xres / 4);
    if (PbrtOptions.quickRender) yres = std::max(1, yres / 4);
    Bounds2f cropWindow;
    int cwi;
    const Float *cr = params.FindFloat("cropwindow", &cwi);
    if (cr && cwi == 4) {
        cropWindow.pMin.x = Clamp(std::min(cr[0], cr[1]), 0.f, 1.f);
        cropWindow.pMax.x = Clamp(std::max(cr[0], cr[1]), 0.f, 1.f);
        cropWindow.pMin.y = Clamp(std::min(cr[2], cr[3]), 0.f, 1.f);
        cropWindow.pMax.y = Clamp(std::max(cr[2], cr[3]), 0.f, 1.f);
    }
    else if (cr)
    {
        Error("%d values supplied for \"cropwindow\". Expected 4.", cwi);
        return false;
    }
    else
        cropWindow = Bounds2f(Point2f(Clamp(PbrtOptions.cropWindow[0][0], 0, 1),
                                Clamp(PbrtOptions.cropWindow[1][0], 0, 1)),
                        Point2f(Clamp(PbrtOptions.cropWindow[0][1], 0, 1),
                                Clamp(PbrtOptions.cropWindow[1][1], 0, 1)));

    scale = params.FindOneFloat("scale", 1.);
    diagonal = params.FindOneFloat("diagonal", 35.);
    maxSampleLuminance = params.FindOneFloat( "maxsampleluminance", Infinity );
    fullResolution = Point2i(xres, yres);
    filter = std::move(filt);

    // Compute film image bounds
    croppedPixelBounds =
        Bounds2i(Point2i(std::ceil(fullResolution.x * cropWindow.pMin.x),
                         std::ceil(fullResolution.y * cropWindow.pMin.y)),
                 Point2i(std::ceil(fullResolution.x * cropWindow.pMax.x),
                         std::ceil(fullResolution.y * cropWindow.pMax.y)));
    LOG(INFO) << "Created film with full resolution " << fullResolution
              << ". Crop window of " << cropWindow << " -> croppedPixelBounds "
              << croppedPixelBounds;

    // Allocate film image storage
    pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.Area()]);
    filmPixelMemory += croppedPixelBounds.Area() * sizeof(Pixel);

    // Precompute filter weight table
    int offset = 0;
    for (int y = 0; y < filterTableWidth; ++y) {
        for (int x = 0; x < filterTableWidth; ++x, ++offset) {
            Point2f p;
            p.x = (x + 0.5f) * filter->radius.x / filterTableWidth;
            p.y = (y + 0.5f) * filter->radius.y / filterTableWidth;
            filterTable[offset] = filter->Evaluate(p);
        }
    }

    return true;
}

//Bounds2i Film::GetSampleBounds() const {
//    Bounds2f floatBounds(Floor(Point2f(croppedPixelBounds.pMin) +
//                               Vector2f(0.5f, 0.5f) - filter->radius),
//                         Ceil(Point2f(croppedPixelBounds.pMax) -
//                              Vector2f(0.5f, 0.5f) + filter->radius));
//    return (Bounds2i)floatBounds;
//}

Bounds2f Film::GetPhysicalExtent() const {
    Float aspect = (Float)fullResolution.y / (Float)fullResolution.x;
    Float x = std::sqrt(diagonal * diagonal / (1 + aspect * aspect));
    Float y = aspect * x;
    return Bounds2f(Point2f(-x / 2, -y / 2), Point2f(x / 2, y / 2));
}

//std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i &sampleBounds) {
//    // Bound image pixels that samples in _sampleBounds_ contribute to
//    Vector2f halfPixel = Vector2f(0.5f, 0.5f);
//    Bounds2f floatBounds = (Bounds2f)sampleBounds;
//    Point2i p0 = (Point2i)Ceil(floatBounds.pMin - halfPixel - filter->radius);
//    Point2i p1 = (Point2i)Floor(floatBounds.pMax - halfPixel + filter->radius) +
//                 Point2i(1, 1);
//    Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), croppedPixelBounds);
//    return std::unique_ptr<FilmTile>(new FilmTile(
//        tilePixelBounds, filter->radius, filterTable, filterTableWidth,
//        maxSampleLuminance));
//}

void Film::Clear() {
    for (Point2i p : croppedPixelBounds) {
        Pixel &pixel = GetPixel(p);
        for (int c = 0; c < 3; ++c)
            pixel.splatXYZ[c] = pixel.xyz[c] = 0;
        pixel.filterWeightSum = 0;
    }
}

//void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile) {
//    ProfilePhase p(Prof::MergeFilmTile);
//    VLOG(1) << "Merging film tile " << tile->pixelBounds;
//    std::lock_guard<std::mutex> lock(mutex);
//    for (Point2i pixel : tile->GetPixelBounds()) {
//        // Merge _pixel_ into _Film::pixels_
//        const FilmTilePixel &tilePixel = tile->GetPixel(pixel);
//        Pixel &mergePixel = GetPixel(pixel);
//        Float xyz[3];
//        tilePixel.contribSum.ToXYZ(xyz);
//        for (int i = 0; i < 3; ++i) mergePixel.xyz[i] += xyz[i];
//        mergePixel.filterWeightSum += tilePixel.filterWeightSum;
//    }
//}

void Film::SetImage(const Spectrum *img) const {
    int nPixels = croppedPixelBounds.Area();
    for (int i = 0; i < nPixels; ++i) {
        Pixel &p = pixels[i];
        img[i].ToXYZ(p.xyz);
        p.filterWeightSum = 1;
        p.splatXYZ[0] = p.splatXYZ[1] = p.splatXYZ[2] = 0;
    }
}

//void Film::AddSplat(const Point2f &p, Spectrum v) {
//    ProfilePhase pp(Prof::SplatFilm);
//
//    if (v.HasNaNs()) {
//        LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with NaN values "
//                                   "at (%f, %f)", p.x, p.y);
//        return;
//    } else if (v.y() < 0.) {
//        LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with negative "
//                                   "luminance %f at (%f, %f)", v.y(), p.x, p.y);
//        return;
//    } else if (std::isinf(v.y())) {
//        LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with infinite "
//                                   "luminance at (%f, %f)", p.x, p.y);
//        return;
//    }
//
//    Point2i pi = Point2i(Floor(p));
//    if (!InsideExclusive(pi, croppedPixelBounds)) return;
//    if (v.y() > maxSampleLuminance)
//        v *= maxSampleLuminance / v.y();
//    Float xyz[3];
//    v.ToXYZ(xyz);
//    Pixel &pixel = GetPixel(pi);
//    for (int i = 0; i < 3; ++i) pixel.splatXYZ[i].Add(xyz[i]);
//}


FilmTile::~FilmTile() {}

bool FilmTile::AddSample(const lfrt::VEC2 &raster, const lfrt::VEC2 &secondary,
                         const Float &sampleWeight, const Float &rayWeight,
                         const Float &r, const Float &g, const Float &b,
                         const bool isWeighted )
{
    const Float rgb[3] = { r, g, b };
    Spectrum color = Spectrum::FromRGB( rgb );
    if ( color.y() > maxSampleLuminance )
            color *= maxSampleLuminance / color.y();
    const Point2f pFilm( raster.x, raster.y );
    if ( isWeighted )
    {
        // Original PBRT-v3 function:
        // void AddSample(const Point2f &pFilm, Spectrum L, Float sampleWeight = 1.)
        ProfilePhase _( Prof::AddFilmSample );

        // Compute sample's raster bounds
        Point2f pFilmDiscrete = pFilm - Vector2f(0.5f, 0.5f);
        Point2i p0 = (Point2i)Ceil(pFilmDiscrete - filterRadius);
        Point2i p1 = (Point2i)Floor(pFilmDiscrete + filterRadius) + Point2i(1, 1);
        p0 = Max(p0, pixelBounds.pMin);
        p1 = Min(p1, pixelBounds.pMax);

        // Loop over filter support and add sample to pixel arrays

        // Precompute $x$ and $y$ filter table offsets
        int *ifx = ALLOCA(int, p1.x - p0.x);
        for (int x = p0.x; x < p1.x; ++x) {
            Float fx = std::abs( (x - pFilmDiscrete.x) * invFilterRadius.x * filterTableSize );
            ifx[x - p0.x] = std::min( (int)std::floor(fx), filterTableSize-1 );
        }
        int *ify = ALLOCA(int, p1.y - p0.y);
        for (int y = p0.y; y < p1.y; ++y) {
            Float fy = std::abs( (y - pFilmDiscrete.y) * invFilterRadius.y * filterTableSize );
            ify[y - p0.y] = std::min( (int)std::floor(fy), filterTableSize-1 );
        }
        for (int y = p0.y; y < p1.y; ++y) {
            for (int x = p0.x; x < p1.x; ++x) {
                // Evaluate filter value at $(x,y)$ pixel
                int offset = ify[y - p0.y] * filterTableSize + ifx[x - p0.x];
                Float filterWeight = filterTable[offset];

                // Update pixel values with filtered sample contribution
                FilmTilePixel &pixel = GetPixel(Point2i(x, y));
                pixel.contribSum += color * sampleWeight * filterWeight;
                pixel.filterWeightSum += filterWeight;
            }
        }
        return true;
    }
    else
    {
        // Original PBRT-v3 function:
        // void Film::AddSplat(const Point2f &p, Spectrum v)
        ProfilePhase pp( Prof::SplatFilm );
        if ( color.HasNaNs() ) {
            LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with NaN values "
                                       "at (%f, %f)", pFilm.x, pFilm.y);
            return false;
        } else if ( color.y() < 0. ) {
            LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with negative "
                                       "luminance %f at (%f, %f)", color.y(), pFilm.x, pFilm.y);
            return false;
        } else if ( std::isinf(color.y()) ) {
            LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with infinite "
                                       "luminance at (%f, %f)", pFilm.x, pFilm.y);
            return false;
        }
        //Point2i pi = Point2i(Floor(p));
        Point2i pi = Point2i( Floor(pFilm) );
        //if (!InsideExclusive(pi, croppedPixelBounds)) return;
        pi = Min( Max( pi, pixelBounds.pMin ), pixelBounds.pMax );
        Float xyz[3];
        color.ToXYZ(xyz);
        FilmTilePixel &pixel = GetPixel(pi);
        //for (int i = 0; i < 3; ++i) pixel.splatXYZ[i].Add(xyz[i]);
        //pixel.contribSum += color * sampleWeight;
        //pixel.filterWeightSum += 1.0;
        pixel.splats += color;
        return true;
    }

    return false;
}

}  // namespace pbrt
