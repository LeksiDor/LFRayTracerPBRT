#ifndef CUSTOMFILM_H
#define CUSTOMFILM_H

#include "LFRayTracer.h"

#include <vector>

struct RGB
{
    using Real = lfrt::Real;
    Real r = 0;
    Real g = 0;
    Real b = 0;
};


template< typename Spectrum >
class Image
{
public:
    Image() {}
    Image( const int width, const int height ) { Resize(width,height); }

    bool Resize( const int width, const int height )
    {
        if ( width <= 0 || height <= 0 )
            return false;
        data.resize( width * height );
        this->width = width;
        this->height = height;
        return true;
    }

    int Width() const { return width; }
    int Height() const { return height; }

    Spectrum& operator() ( int x, int y ) { return data[x+y*width]; }
    const Spectrum& operator() ( int x, int y ) const { return data[x+y*width]; }

private:
    std::vector<Spectrum> data;
    int width = 0;
    int height = 0;
};


using ImageRGB = Image<RGB>;
using ImageGray = Image<lfrt::Real>;


class CustomFilm : public lfrt::SampleAccumulator
{
public:
    using Int = lfrt::Int;
    using Real = lfrt::Real;

public:
    CustomFilm( const Int width, const Int height );

// Inherited via SampleAccumulator
    virtual bool SetSize(const Int& width, const Int& height) override;
    virtual Int Width() const override { return weighted.Width();}
    virtual Int Height() const override { return weighted.Height(); }
    virtual bool GetRenderBounds(
        Int& startX, Int& startY, Int& endX, Int& endY) const override;
    virtual bool GetSamplingBounds(
        Int& startX, Int& startY, Int& endX, Int& endY) const override;
    virtual lfrt::SampleTile* CreateSampleTile(
        const Int& startX, const Int& startY,
        const Int&   endX, const Int&   endY ) override;
    virtual bool MergeSampleTile( lfrt::SampleTile* tile ) override;
    virtual bool DestroySampleTile( lfrt::SampleTile* tile ) override;
    virtual bool GetColor( const Int& x, const Int& y, Real& r, Real& g, Real& b ) const override;

private:
    ImageRGB weighted;
    ImageGray weights;
    ImageRGB unweighted;
};



class CustomFilmTile : public lfrt::SampleTile
{
public:
    friend class CustomFilm;
    using VEC2 = lfrt::VEC2;
    using Real = lfrt::Real;
    using Int = lfrt::Int;

protected:
    virtual ~CustomFilmTile() = default;

public:

    CustomFilmTile(
        const Int& startX, const Int& startY,
        const Int&   endX, const Int&   endY );

    virtual bool AddSample(const VEC2& raster, const VEC2& secondary,
                         const Real& sampleWeight, const Real& rayWeight,
                         const Real& r, const Real& g, const Real& b,
                         const bool isWeighted = true) override;

private:
    ImageRGB weighted;
    ImageGray weights;
    ImageRGB unweighted;
    Int startX = 0;
    Int startY = 0;
    Int width = 0;
    Int height = 0;
};


#endif // CUSTOMFILM_H