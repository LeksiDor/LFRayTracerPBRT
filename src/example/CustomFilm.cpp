#include "CustomFilm.h"

using namespace lfrt;



CustomFilm::CustomFilm( const Int width, const Int height )
{
    SetSize( width, height );
}



bool CustomFilm::SetSize(const Int& width, const Int& height)
{
    if ( width <= 0 || height <= 0 )
        return false;
    weighted.Resize( width, height );
    weights.Resize( width, height );
    unweighted.Resize( width, height );
    return true;
}



bool CustomFilm::GetRenderBounds( Int& startX, Int& startY, Int& endX, Int& endY) const
{
    startX = 0;
    startY = 0;
    endX = Width();
    endY = Height();
    return true;
}


bool CustomFilm::GetSamplingBounds( Int& startX, Int& startY, Int& endX, Int& endY) const
{
    startX = 0;
    startY = 0;
    endX = Width();
    endY = Height();
    return true;
}


SampleTile* CustomFilm::CreateSampleTile(
    const Int& startX, const Int& startY,
    const Int&   endX, const Int&   endY )
{
    return new CustomFilmTile( startX, startY, endX, endY );
}


bool CustomFilm::MergeSampleTile( SampleTile* tile )
{
    CustomFilmTile* filmTile = dynamic_cast<CustomFilmTile*>(tile);
    if ( filmTile == nullptr )
        return false;
    const Int startX = filmTile->startX;
    const Int startY = filmTile->startY;
    for ( Int localX = 0; localX < filmTile->width; ++localX )
    {
        for ( Int localY = 0; localY < filmTile->height; ++localY )
        {
            weighted(startX+localX,startY+localY) = filmTile->weighted(localX,localY);
            weights(startX+localX,startY+localY) = filmTile->weights(localX,localY);
            unweighted(startX+localX,startY+localY) = filmTile->unweighted(localX,localY);
        }
    }
    return true;
}


bool CustomFilm::DestroySampleTile( SampleTile* tile )
{
    CustomFilmTile* filmTile = dynamic_cast<CustomFilmTile*>(tile);
    if ( filmTile == nullptr )
        return false;
    delete filmTile;
    return true;
}


bool CustomFilm::GetColor( const Int& x, const Int& y, Real& r, Real& g, Real& b) const
{
    if ( x < 0 || y < 0 || x >= Width() || y >= Height() )
        return false;
    const RGB& sum = weighted(x,y);
    const Real& w = weights(x,y);
    const RGB& flat = unweighted(x,y);
    r = flat.r;
    g = flat.g;
    b = flat.b;
    if ( w > 0 )
    {
        r += sum.r / w;
        g += sum.g / w;
        b += sum.b / w;
    }
    return true;
}


CustomFilmTile::CustomFilmTile(const Int& startX, const Int& startY,
                               const Int& endX, const Int& endY )
    :startX(startX)
    ,startY(startY)
    ,width(endX-startX)
    ,height(endY-startY)
{
    weighted.Resize(width,height);
    weights.Resize(width,height);
    unweighted.Resize(width,height);
}


bool CustomFilmTile::AddSample(const VEC2& raster, const VEC2& secondary,
                               const Real& sampleWeight, const Real& rayWeight,
                               const Real& r, const Real& g, const Real& b,
                               const bool isWeighted )
{
    const Int x = Int(raster.x) - startX;
    const Int y = Int(raster.y) - startY;
    if ( x < 0 || y < 0 || x >= width || y >= width )
        return false;
    if ( isWeighted )
    {
        const Real w = sampleWeight * rayWeight;
        RGB& rgb = weighted(x,y);
        rgb.r += w*r;
        rgb.g += w*g;
        rgb.b += w*b;
        weights(x,y) = weights(x,y) + w;
    }
    else
    {
        RGB& rgb = unweighted(x,y);
        rgb.r += r;
        rgb.g += g;
        rgb.b += b;
    }
    return true;
}
