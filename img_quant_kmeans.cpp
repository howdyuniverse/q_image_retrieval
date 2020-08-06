#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <jconfig.h>
#include <jinclude.h>
#include <jpeglib.h>

using namespace std;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;

struct Color {
	byte comp1;
	byte comp2;
	byte comp3;
};

class Clusters {
	Color *centers;
	word *pix_nums;
	word k_num;
public:
	Clusters();
	Clusters(word k) {
		k_num = k;
		centers = new Color[k_num];
		pix_nums = new word[k_num];
		memset(pix_nums, 0, sizeof(word) * k_num);
	}
	~Clusters()
	{
		delete []centers;
		delete []pix_nums;
	}
	void setKNum(word k)
	{
		k_num = k;
	}
	bool setCenter(word cluster_index, Color new_color, word pix_num)
	{
		// 0 - если установить новый цвет не удалось
		if (cluster_index < 0 || cluster_index >= k_num)
			return 0;
		pix_nums[cluster_index] = pix_num;
		centers[cluster_index].comp1 = new_color.comp1;
		centers[cluster_index].comp2 = new_color.comp2;
		centers[cluster_index].comp3 = new_color.comp3;
		return 1;
	}
	void initRandCenters(int a, int b)
	{
		cout << "DEBUG> Init Random Centers:" << endl;
		srand((unsigned)time(NULL));
		for (word i = 0; i < k_num; i++) {
			centers[i].comp1 = a + rand() % (b-a);
			centers[i].comp2 = a + rand() % (b-a);
			centers[i].comp3 = a + rand() % (b-a);
		}
		// Debug
		printClustCenters();
	}
	word getClusterNum(Color pixel)
	{
		// Определяем самый близкий кластер к данному пикселю
		word best_k = 0;
		double dist = 0;
		for (word i = 0; i < k_num; i++) {
			dist = getEDistance(centers[i], pixel);
			if ( dist < getEDistance(centers[best_k], pixel) )
				best_k = i;
		}
		return best_k;
	}
	double getEDistance(Color a, Color b)
	{
		// Евклидово расстояние между 3 компонентами цвета
		double dist = 0;
		double d1 = static_cast<double>(a.comp1 - b.comp1);
		double d2 = static_cast<double>(a.comp2 - b.comp2);
		double d3 = static_cast<double>(a.comp3 - b.comp3);
		dist = sqrt( pow(d1, 2) + pow(d2, 2) + pow(d3, 2) );
		return dist;
	}
	void printClustCenters()
	{
		cout << "DEBUG> Print Clusters Center:" << endl;
		for (int i = 0; i < k_num; i++) {
			cout << '[' << (int)centers[i].comp1 << ',';
			cout << (int)centers[i].comp2 << ',';
			cout << (int)centers[i].comp3 << "] " << pix_nums[i] << endl;
		}
	}
};

void getImgKmeansHist(char * img_file)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	FILE *f = fopen(img_file, "rb");
	jpeg_stdio_src(&cinfo, f);
	jpeg_read_header(&cinfo, TRUE);

	int w, h;
	int n;
	w = cinfo.image_width;
	h = cinfo.image_height;
	n = cinfo.num_components;
	cout << "Dimension:\t" << w << 'x' << h << endl;
	cout << "Channels:\t" << n << endl;

	int stride = w * n;
	byte *pBuf, *q;

	pBuf = new byte[stride * h];
	memset(pBuf, 0, stride* h);

	jpeg_start_decompress(&cinfo);
	q = pBuf;
	// заносим все компоненты пикселей в буфер
	// используя указатель q
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&(q),1);
		q += stride;
	}

	word * index_table = new word[w * h];
	memset(index_table, 0, w * h);
	word k = 8;
	Clusters clusters(k);
	clusters.initRandCenters(0, 255);

	for (int i = 0, j = 0; i < stride * h; i+=n, j++) {
		Color temp_color;
		temp_color.comp1 = pBuf[i];
		temp_color.comp2 = pBuf[i+1];
		temp_color.comp3 = pBuf[i+2];
		index_table[j] = clusters.getClusterNum(temp_color);
	}

	int moves	  = 1,	iter	 = 0;
	int moves_lim = 100,	iter_lim = 100;

	while ( (moves != 0) || ( moves < moves_lim && iter > iter_lim ) )
	{
		moves = 0;
		// проходимся по кластерам пересчитывая центры
		for (word clust = 0; clust < k; clust++) {
			//cout << "DEBUG> cluster iter" << endl;
			unsigned int pixel_num = 0; // число пикселей входящих в кластер
			unsigned int sum_comp1 = 0; // считаем сумму каждого компонента
			unsigned int sum_comp2 = 0; // для вычисления сред.
			unsigned int sum_comp3 = 0;

			// проходимся по всем цветам и смотрим по таблице индексов
			// какому кластеру пренадлежит цвет
			for (int comp = 0, j = 0; comp < stride * h; comp+=n, j++) {
				if ( clust == index_table [j] ) {
					pixel_num++;
					sum_comp1 += pBuf [comp];
					sum_comp2 += pBuf [comp+1];
					sum_comp3 += pBuf [comp+2];
				}
			}
			// пересчитываем центер кластера
			if (pixel_num != 0) {
				Color avr_color;
				avr_color.comp1 = word (sum_comp1 / pixel_num);
				avr_color.comp2 = word (sum_comp2 / pixel_num);
				avr_color.comp3 = word (sum_comp3 / pixel_num);
				clusters.setCenter(clust, avr_color, pixel_num);
			}
		}
		// ищем новый центр для кластера
		Color tcolor;
		for (int comp = 0, j = 0; comp < stride * h; comp+=n, j++) {
			tcolor.comp1 = pBuf [comp];
			tcolor.comp2 = pBuf [comp+1];
			tcolor.comp3 = pBuf [comp+2];
			word new_clust_index = clusters.getClusterNum(tcolor);

			if (new_clust_index != index_table[j]) {
				moves++;
				index_table[j] = new_clust_index;
			}
		}
		iter++;
	}
	clusters.printClustCenters();
	
	//while(true) {}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(f);
	delete [] pBuf;
}

int main()
{
	getImgKmeansHist("D:\\img\\image_orig\\760.jpg");
	return 0;
}
