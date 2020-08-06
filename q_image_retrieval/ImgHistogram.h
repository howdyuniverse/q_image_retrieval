#ifndef IMGHISTOGRAM_H
#define IMGHISTOGRAM_H

#include "ColorSpace.h"

#include <QImage>

#ifndef mmax
#define mmax(a, b) ( a > b ? a:b )
#endif

#ifndef min
#define min(a, b) ( a < b ? a:b )
#endif


#ifndef tmax
#define tmax(a, b, c) ( mmax(a, mmax(b,c)) )
#endif

#ifndef tmin
#define tmin(a, b, c) ( min(a, min(b,c)) )
#endif

enum COLOR_SPACE {RGB, HSV};

struct HistBin {
    uint	pix_num;
    ushort	comp1;
    ushort	comp2;
    ushort	comp3;
};

class ImgHistogram {
	HistBin * RGB_bins;		ushort rgb_bins_num;
	HistBin * HSV_bins;		ushort hsv_bins_num;
	ColorSpace RGB_qspace;
	ColorSpace HSV_qspace;
    uint colors_num;

    // hmm.. maybe move to private?
    //void initQuantHist(HistBin * hist_bins, ColorSpace & qsteps, ushort qstep1, ushort qstep2, ushort qstep3)
    void initQuantHist(COLOR_SPACE space, ushort qstep1, ushort qstep2, ushort qstep3)
    {
        HistBin	   * hist_bins	= NULL;
        ColorSpace * qsteps		= NULL;
        if (space == RGB) {
            hist_bins	= RGB_bins;
            qsteps		= &RGB_qspace;
        }
        else if (space == HSV) {
            hist_bins	= HSV_bins;
            qsteps		= &HSV_qspace;
        }

        if (hist_bins == NULL) {
            //std::cout << "Error! You must first call init histogram func." << std::endl;
            return;
        }

        ushort h_index = 0; // historgam index
        for (ushort i = 0; i < qstep1; i++)
            for (ushort j = 0; j < qstep2; j++)
                for (ushort k = 0; k < qstep3; k++)
                {
                    hist_bins[h_index].pix_num = 0;
                    hist_bins[h_index].comp1 = qsteps->getValueCh1(i);
                    hist_bins[h_index].comp2 = qsteps->getValueCh2(j);
                    hist_bins[h_index].comp3 = qsteps->getValueCh3(k);
                    h_index++;
                }
    }

    //
    void resetPixNum()
    {
        for (ushort k = 0; k < rgb_bins_num; k++)
                RGB_bins[k].pix_num = 0;

        for (ushort k = 0; k < hsv_bins_num; k++)
                HSV_bins[k].pix_num = 0;
    }
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

    ushort myToSat(float r, float g, float b)
    {
        float max_v = tmax(r, g, b);
        if (max_v == 0)
            return 0;

        float min_v = tmin(r, g, b);

        return ushort((1 - (min_v / max_v)) * 100);
    }

    ushort myToVal(float r, float g, float b)
    {
        return ushort(tmax(r,g,b) / 255 * 100);
    }

public:
    ImgHistogram()
    {
        RGB_bins = NULL;
        HSV_bins = NULL;
        rgb_bins_num = 0;
        hsv_bins_num = 0;
        colors_num = 0;
    }

    ImgHistogram(ushort qr, ushort qg, ushort qb, ushort qh, ushort qs, ushort qv)
    {
        initQHistRGB(qr, qg, qb);
        initQHistHSV(qh, qs, qv);
        colors_num = 0;
    }

    ~ImgHistogram()
    {
        if (RGB_bins != NULL)
            delete [] RGB_bins;
        if (HSV_bins != NULL)
            delete [] HSV_bins;
    }


    void initQHistRGB(ushort qr, ushort qg, ushort qb)
    {
        // сначала инициализируем значения квант. пространства
        RGB_qspace.setQuantStepPerChnl(qr, qg, qb);
        RGB_qspace.quantChnls(256, 256, 256);

        rgb_bins_num = qr * qg * qb;
        RGB_bins = new HistBin[rgb_bins_num];

        // затем на основе этих значений будем получать собственно гистограму картинки
        //initQuantHist(RGB_bins, RGB_qspace, qr, qg, qb);
        initQuantHist(RGB, qr, qg, qb);
    }

    void initQHistHSV(ushort qh, ushort qs, ushort qv)
    {
        qDebug() << "start init qhist HSV";
        // сначала инициализируем значения квант. пространства
        HSV_qspace.setQuantStepPerChnl(qh, qs, qv);
        HSV_qspace.quantChnls(360, 100, 100);

        hsv_bins_num = qh * qs * qv;
        HSV_bins = new HistBin[hsv_bins_num];

        // затем на основе этих значений будем получать собственно гистограму картинки
        //initQuantHist(HSV_bins, HSV_qspace, qh, qs, qv);
        initQuantHist(HSV, qh, qs, qv);
        qDebug() << "end init qhist HSV";
    }

    void getImageHist(char * img_file, COLOR_SPACE space)
    {
        QImage * img = new QImage(img_file);

        if (!img->isNull()) {
            resetPixNum();

            int pixels = img->width() * img->height();
            colors_num = img->width() * img->height();

            if (pixels*(int)sizeof(QRgb) <= img->byteCount())
            {
                QRgb *data = (QRgb *)img->constBits();
                ushort r = 0, g = 0, b = 0;
                if (space == RGB) {
                    for (int i = 0; i < pixels; i++)
                    {
                        r = qRed(data[i]);
                        g = qGreen(data[i]);
                        b = qBlue(data[i]);

                        RGB_qspace.linearQuantComps( r, g, b );

                        for (ushort k = 0; k < rgb_bins_num; k++)
                            if (RGB_bins[k].comp1 == r && RGB_bins[k].comp2 == g && RGB_bins[k].comp3 == b)
                                RGB_bins[k].pix_num++;
                    }
                }
                else {
                    ushort h = 0, s = 0, v = 0;
                    for (int i = 0; i < pixels; i++)
                    {
                        r = qRed(data[i]);
                        g = qGreen(data[i]);
                        b = qBlue(data[i]);

                        h = myToHue((float)r, (float)g, (float)b);
                        s = myToSat((float)r, (float)g, (float)b);
                        v = myToVal((float)r, (float)g, (float)b);

                        HSV_qspace.linearQuantComps( h, s, v );

                        for (ushort k = 0; k < hsv_bins_num; k++)
                            if (HSV_bins[k].comp1 == h && HSV_bins[k].comp2 == s && HSV_bins[k].comp3 == v)
                                HSV_bins[k].pix_num++;
                    }
                }
            }
        }

        delete img;
    }

    double histIntersectionDist(ImgHistogram &comp_hist, COLOR_SPACE space)
    {
        //std::cout << "\nCalc Intersection distance...";
        qDebug() << "Start calc Intersection";
        double minsum = 0;
        if (space == RGB) {
            for (ushort i = 0; i < rgb_bins_num; i++)
                minsum += (double)min( RGB_bins[i].pix_num, comp_hist.RGB_bins[i].pix_num );
        }
        else {
            for (ushort i = 0; i < hsv_bins_num; i++)
                minsum += (double)min( HSV_bins[i].pix_num, comp_hist.HSV_bins[i].pix_num );
        }

        double dist = minsum / (double)min( colors_num, comp_hist.colors_num );
        //std::cout << " [DONE]";

        return dist;
    }

    double histEuclideanDist(ImgHistogram &comp_hist, COLOR_SPACE space)
    {
        //std::cout << "\nCalc Euclidean distance...";
        qDebug() << "Start calc Euclidean";
        double sum = 0;

        if (space == RGB) {
            for (ushort i = 0; i < rgb_bins_num; ++i) {
                short sub = RGB_bins[i].pix_num - comp_hist.RGB_bins[i].pix_num;
                sum += (float) sub * sub;
            }
        }
        else {
            for (ushort i = 0; i < hsv_bins_num; ++i) {
                short sub = HSV_bins[i].pix_num - comp_hist.HSV_bins[i].pix_num;
                sum += (float) sub * sub;
            }
        }

        return sum;
    }

};

#endif
