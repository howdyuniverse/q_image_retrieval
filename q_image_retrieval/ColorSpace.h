#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <cmath>

typedef unsigned char	byte;
typedef unsigned short	ushort;
typedef unsigned int	uint;

class ColorSpace {
	ushort * channel_1;
	ushort * channel_2;
	ushort * channel_3;
	ushort pix_ch1;
	ushort pix_ch2;
	ushort pix_ch3;

public:
    ColorSpace()
    {
        channel_1 = NULL;
        channel_2 = NULL;
        channel_3 = NULL;
        pix_ch1 = 0;
        pix_ch2 = 0;
        pix_ch3 = 0;
    }
    ColorSpace(ushort ch1, ushort ch2, ushort ch3)
    {
        setQuantStepPerChnl(ch1, ch2, ch3);
    }
    ~ColorSpace()
    {
        if (channel_1 == NULL)
            return;
        delete [] channel_1;
        delete [] channel_2;
        delete [] channel_3;
    }

    void setQuantStepPerChnl(ushort ch1, ushort ch2, ushort ch3)
    {
        // устанавливаем шаги квантования
        pix_ch1 = ch1;
        pix_ch2 = ch2;
        pix_ch3 = ch3;
        channel_1 = new ushort [pix_ch1];
        channel_2 = new ushort [pix_ch2];
        channel_3 = new ushort [pix_ch3];
        memset(channel_1, 0, sizeof(ushort) * pix_ch1);
        memset(channel_2, 0, sizeof(ushort) * pix_ch2);
        memset(channel_3, 0, sizeof(ushort) * pix_ch3);
    }
    void quantChnls(ushort old_clr1, ushort old_clr2, ushort old_clr3)
    {
        // распределяем значения по шагу квантования в старом цветовом пространстве
        if (channel_1 == NULL) {
            //std::cout << "Error! You must first set channel values. [setQuantStepPerChnl]" << std::endl;
            return;
        }
        for (ushort i = 0, j = 1; i < pix_ch1; ++i, ++j)
            channel_1[i] = (ushort) j * (old_clr1 / pix_ch1) - 1;

        for (ushort i = 0, j = 1; i < pix_ch2; ++i, ++j)
            channel_2[i] = (ushort) j * (old_clr2 / pix_ch2) - 1;

        for (ushort i = 0, j = 1; i < pix_ch3; ++i, ++j)
            channel_3[i] = (ushort) j * (old_clr3 / pix_ch3) - 1;
    }

    // get channel values
    ushort getValueCh1(ushort index)
    {
        if (index < 0 || index >= pix_ch1) {
            //std::cout << "Error! Out of range. Return: 0\n";
            return 0;
        }
        return channel_1[index];
    }

    ushort getValueCh2(ushort index)
    {
        if (index < 0 || index >= pix_ch2) {
            //std::cout << "Error! Out of range. Return: 0\n";
            return 0;
        }
        return channel_2[index];
    }

    ushort getValueCh3(ushort index)
    {
        if (index < 0 || index >= pix_ch3) {
            //std::cout << "Error! Out of range. Return: 0\n";
            return 0;
        }
        return channel_3[index];
    }

    void linearQuantComps(ushort &ch1, ushort &ch2, ushort &ch3)
    {
        ushort min = abs( static_cast<double>(channel_1[0] - ch1) );
        ushort min_index = 0;
        ushort temp_min = 0;

        for (ushort i = 1; i < pix_ch1; i++)
        {
            temp_min = abs( static_cast<double>(channel_1[i] - ch1) );
            if (temp_min < min) {
                min = temp_min;
                min_index = i;
            }
        }
        ch1 = channel_1[min_index];

        // channel 2
        min = abs( static_cast<double>(channel_2[0] - ch2) );
        min_index = temp_min = 0;

        for (ushort i = 1; i < pix_ch2; i++)
        {
            temp_min = abs( static_cast<double>(channel_2[i] - ch2) );
            if (temp_min < min) {
                min = temp_min;
                min_index = i;
            }
        }
        ch2 = channel_2[min_index];

        // channel 3
        min = abs( static_cast<double>(channel_3[0] - ch3) );
        min_index = temp_min = 0;

        for (ushort i = 1; i < pix_ch3; i++)
        {
            temp_min = abs( static_cast<double>(channel_3[i] - ch3) );
            if (temp_min < min) {
                min = temp_min;
                min_index = i;
            }
        }
        ch3 = channel_3[min_index];
    }
};

#endif
