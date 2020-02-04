void SDL_PutPixel32(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32*)p = pixel;
}

Uint32 SDL_GetPixel32(SDL_Surface *surface, int x, int y)
{
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * 4;
    return *(Uint32*)p;
}

void Line(SDL_Surface* surf,int x1,int y1, int x2,int y2,Uint32 couleur)  // Bresenham
{
    int x,y;
    int Dx,Dy;
    int xincr,yincr;
    int erreur;
    int i;

    Dx = abs(x2-x1);
    Dy = abs(y2-y1);
    if(x1<x2)
        xincr = 1;
    else
        xincr = -1;
    if(y1<y2)
        yincr = 1;
    else
        yincr = -1;

    x = x1;
    y = y1;
    if(Dx>Dy)
    {
        erreur = Dx/2;
        for(i=0;i<Dx;i++)
        {
            x += xincr;
            erreur += Dy;
            if(erreur>Dx)
            {
                erreur -= Dx;
                y += yincr;
            }
            SDL_PutPixel32(surf,x, y,couleur);
        }
    }
    else
    {
        erreur = Dy/2;
        for(i=0;i<Dy;i++)
        {
            y += yincr;
            erreur += Dx;
            if(erreur>Dy)
            {
                erreur -= Dy;
                x += xincr;
            }
            SDL_PutPixel32(surf,x, y,couleur);
        }
    }
    SDL_PutPixel32(surf,x1,y1,couleur);
    SDL_PutPixel32(surf,x2,y2,couleur);
}

void HorizLine(SDL_Surface* surf,int x1,int y1, int x2,Uint32 couleur)  // a noter que Line gere ça aussi
{
    int i;
    for(i=x1;i<=x2;i++)
        SDL_PutPixel32(surf,i,y1,couleur);
}

void VertLine(SDL_Surface* surf,int x1,int y1, int y2,Uint32 couleur)  // a noter que Line gere ça aussi
{
    int i;
    for(i=y1;i<=y2;i++)
        SDL_PutPixel32(surf,x1,i,couleur);
}

void Box(SDL_Surface* surf,int x1,int y1, int x2,int y2,Uint32 couleur)
{
    HorizLine(surf,x1,y1,x2,couleur);
    HorizLine(surf,x1,y2,x2,couleur);
    VertLine(surf,x1,y1,y2,couleur);
    VertLine(surf,x2,y1,y2,couleur);
}

void Bar(SDL_Surface* surf,int x1,int y1, int x2,int y2,Uint32 couleur)
{
    int i,j;
    for(i=x1;i<=x2;i++)
        for(j=y1;j<=y2;j++)
            SDL_PutPixel32(surf,i,j,couleur);
}


void Circle(SDL_Surface* surf,int x1,int y1, int radius,Uint32 couleur)
{
    // inspiré de http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/bresenham.html
    int d,x,y,x2m1,max;
    y=radius;
    d= -radius;
    x2m1= -1;
    max = (int)(radius/sqrt(2.0));
    for(x=0;x<=max;x++)
    {
        x2m1 += 2;
        d+= x2m1;
        if (d>=0)
        {
            y--;
            d -= (y<<1);
        }
        SDL_PutPixel32(surf,x1+x,y1+y,couleur);
        SDL_PutPixel32(surf,x1-x,y1+y,couleur);
        SDL_PutPixel32(surf,x1+x,y1-y,couleur);
        SDL_PutPixel32(surf,x1-x,y1-y,couleur);
        SDL_PutPixel32(surf,x1+y,y1+x,couleur);
        SDL_PutPixel32(surf,x1-y,y1+x,couleur);
        SDL_PutPixel32(surf,x1+y,y1-x,couleur);
        SDL_PutPixel32(surf,x1-y,y1-x,couleur);
    }

}

void Disc(SDL_Surface* surf,int x1,int y1, int radius,Uint32 couleur)
{
    int x,y,d,stepd,max;
    y=radius;
    d= -radius;
    stepd= -1;
    max = (int)(radius/sqrt(2.0));
    for(x=0;x<=max;x++)
    {
        stepd += 2;
        d+= stepd;
        if (d>=0)
        {
            y--;
            d -= (y<<1);  /* Must do this AFTER y-- */
        }
        HorizLine(surf,x1-x,y1+y,x1+x,couleur);
        HorizLine(surf,x1-x,y1-y,x1+x,couleur);
        HorizLine(surf,x1-y,y1+x,x1+y,couleur);
        HorizLine(surf,x1-y,y1-x,x1+y,couleur);
    }

}
