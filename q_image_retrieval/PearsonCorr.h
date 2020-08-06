#ifndef PEARSONCORR_H
#define PEARSONCORR_H

#include <QImage>

ushort qsteps[10] = {0, 39, 60, 120, 180, 240, 300, -1, -2, -3};

struct SpectHistBin {
    uint	pix_num;
    ushort	color;
};

class CorrSpectrum {
    SpectHistBin hist_bins[10];

    ushort myToHue(float r, float g, float b)
    {
        float max_v = tmax(r, g, b);
        float min_v = tmin(r, g, b);
        float d = max_v - min_v;

        float hue = 0;

        if (max_v == min_v)
            hue = 0;

        else if (max_v == r && g >= b)
            hue = 60 * ((g-b) /d) + 0;

        else if (max_v == r && g < b)
            hue = 60 * ((g-b) /d) + 360;

        else if (max_v == g)
            hue = 60 * ((b-r) /d) + 120;

        else if (max_v == b)
            hue = 60 * ((r-g) /d) + 240;

        return ushort(hue);
    }
    void initQuantHist()
    {
        for (int i = 0; i < 10; ++i) {
            hist_bins[i].color = qsteps[i];
            hist_bins[i].pix_num = 0;
        }
    }
    void addColorToHist(ushort hue)
    {
        ushort min = abs(qsteps[0] - hue);
        ushort min_index = 0;
        ushort temp_min = 0;

        for (byte j = 1; j < 7; j++) {
            temp_min = abs(qsteps[j] - hue);
            if (temp_min < min) {
                min = temp_min;
                min_index = j;
            }
        }
        hist_bins[min_index].pix_num++;
    }

    void resetPixNum()
    {
        for (ushort k = 0; k < 10; k++)
            hist_bins[k].pix_num = 0;
    }

public:
    CorrSpectrum()
    {
        initQuantHist();
    }

    void getImageHist(char * img_file)
    {
        QImage * img = new QImage(img_file);

        if (!img->isNull()) {
            resetPixNum();
            int pixels = img->width() * img->height();
            //
            if (pixels*(int)sizeof(QRgb) <= img->byteCount())
            {
                QRgb *data = (QRgb *)img->constBits();
                ushort r = 0, g = 0, b = 0;
                ushort hue = 0;

                for (int i = 0; i < pixels; i++) {
                    r = qRed(data[i]);
                    g = qGreen(data[i]);
                    b = qBlue(data[i]);

                    // check black
                    if (r <= 30 && g <= 30 && b <= 30)
                        hist_bins[9].pix_num++;
                    // check gray
                    else if ( (r <= 225 && r >= 125) && (g <= 225 && g >= 125) && (b <= 225 && b >= 125) )
                        hist_bins[8].pix_num++;
                    // check white
                    else if ( (r <= 255 && r >= 225) && (g <= 255 && g >= 225) && (b <= 255 && b >= 225) )
                        hist_bins[7].pix_num++;
                    else {
                        // if color is not neutral convert it to hue
                        hue = myToHue(r,g,b);

                        addColorToHist( hue );
                    }
                 }
              }

         }

        delete img;
    }
    double calcCorrelation(CorrSpectrum img2)
    {
        const int N = 10;
        int i = 0;
        double sumx = 0;
        double sumy = 0;
        for (i = 0; i < N; ++i) {
            sumx += hist_bins[i].pix_num;
            sumy += img2.hist_bins[i].pix_num;
        }

        double sumxSq = 0;
        double sumySq = 0;
        for (i = 0; i < N; ++i) {
            sumxSq += pow(hist_bins[i].pix_num, 2.0);
            sumySq += pow(img2.hist_bins[i].pix_num, 2.0);
        }

        double pSum = 0;
        for (i = 0; i < N; ++i)
            pSum += hist_bins[i].pix_num * img2.hist_bins[i].pix_num;

        // calc coeff.
        double num = pSum - (sumx*sumy/N);
        double den = sqrt((sumxSq - pow(sumx,2.0)/N) * (sumySq - pow(sumy, 2.0)/N));

        if (den == 0) return 0;

        return num/den;
    }
};

#endif // PEARSONCORR_H
