#ifndef _WORKS_H
#define _WORKS_H

#include <android/log.h>
#include <cstring>
#include <vector>

#define LOG_TAG "cpp_works"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace std;

typedef struct rect {
	int rowRun;
	int stRun;
	int enRun;
	int runLable;
}Rect;

/*signed char GetMedianNum(unsigned char *bArray, int iFilterLen){
	int i, j;
	signed char bTemp = 0;
	for (j = 0; j < iFilterLen - 1; j++) {
		for (i = 0; i < iFilterLen - j - 1; i++) {
			if (bArray[i] > bArray[i + 1]) {
	        	bTemp = bArray[i];
	        	bArray[i] = bArray[i + 1];
	        	bArray[i + 1] = bTemp;
	        }
	    }
	}
	if ((iFilterLen & 1) > 0) {
		bTemp = (signed char)bArray[(iFilterLen + 1) >> 1];
	}
	else
	{
	    bTemp = (signed char)((bArray[iFilterLen >> 1] + bArray[(iFilterLen >> 1) + 1]) >> 1);
	}
	return bTemp;
}*/

void filter(signed char *fterp, signed char *yuvp, int width, int height){

	int iFilterW = width / 80;
	int iFilterH = height / 80;

	int iFlength = iFilterW * iFilterH;

	int *hValue = new int[width];
	int i, j, d, fd, md, de, m, s;

	for (i = 0; i < width; i++) {
		hValue[i] = (unsigned char)yuvp[i];
	}

	for (j = 1, d = width; j < iFilterH; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] += (unsigned char)yuvp[d++];
		}
	}

	md = ((iFilterH - 1) / 2) * width + (iFilterW + 1) / 2;
	for (i = 0, m = 0; i < iFilterW; i++) {
		m += hValue[i];
	}
	fterp[md++] = m / iFlength;


	for (fd = 0; i < width; i++) {
		m = m - hValue[fd++] + hValue[i];
		fterp[md++] = m / iFlength;
	}

	de = 0;
	for (j = iFilterH; j < height; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] = hValue[i] - (unsigned char)yuvp[de++] + (unsigned char)yuvp[d++];
		}

		m = 0;
		md = md + iFilterW - 1;
		for (i = 0, m = 0; i < iFilterW; i++) {
			m += hValue[i];
		}
		fterp[md++] = m / iFlength;

		for (fd = 0; i < width; i++) {
			m = m - hValue[fd++] + hValue[i];
			fterp[md++] = m / iFlength;
		}
	}


	for (i = 0; i < width; i++) {
		hValue[i] = (unsigned char)yuvp[d++];
	}

	for (j = 1; j < iFilterH; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] += (unsigned char)yuvp[d++];
		}
	}

	md = width * height + ((iFilterH - 1) / 2) * width + iFilterW - 2 + (iFilterW % 2);
	for (i = 0, m = 0, s = 0; i < 2 * iFilterW;) {
		m += hValue[i++];
		s += hValue[i++];
	}
	fterp[md++] = m / iFlength;
	fterp[md++] = s / iFlength;

	for (fd = 0; i < width;) {
		m = m - hValue[fd++] + hValue[i++];
		fterp[md++] = m / iFlength;
		s = s - hValue[fd++] + hValue[i++];
		fterp[md++] = s / iFlength;
	}

	de = width * height;
	for (j = iFilterH; j < height / 2; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] = hValue[i] - (unsigned char)yuvp[de++] + (unsigned char)yuvp[d++];
		}

		m = 0;
		s = 0;
		md = md + 2 * iFilterW - 2;
		for (i = 0, m = 0; i < 2 * iFilterW;) {
			m += hValue[i++];
			s += hValue[i++];
		}
		fterp[md++] = m / iFlength;
		fterp[md++] = s / iFlength;

		for (fd = 0; i < width;) {
			m = m - hValue[fd++] + hValue[i++];
			fterp[md++] = m / iFlength;
			s = s - hValue[fd++] + hValue[i++];
			fterp[md++] = s / iFlength;
		}
	}
	delete[] hValue;

	/*
	unsigned char *aValue = new unsigned char[iFlength];
	unsigned char *bValue = new unsigned char[iFlength];

	for (fd = 0, m = 0, d = 0; fd < iFlength - iFilterW; fd++, d++) {
		aValue[fd] = yuvp[d];
		m += aValue[fd];
		if ((fd + 1) % iFilterW == 0) {
			d += width - iFilterW;
		}
	}
	for (i = iFlength - iFilterW; i < iFlength; i++) aValue[i] = 0;
	md = ((iFilterH - 1) / 2) * width + (iFilterW + 1) / 2;

	j = iFilterH;
	p = 1;
	for (i = iFilterW; i <= width; i++) {

		for (; (j <= height) && (j > 0); j += p) {

			for (l = 0; l < iFilterW; l++, fd++, d++) {
				m -= aValue[fd];
				aValue[fd] = yuvp[d];
				m += aValue[fd];
			}
			fterp[md] = m / iFlength;
			md += p * width;
			d += p * width - iFilterW;
			if ((p == 1) && (fd == iFlength)) fd = 0;
			if (p == -1) fd = (fd - 2 * iFilterW + iFlength) % iFlength;

		}
		md -= p * width - 1;
		d -= p * width - iFilterW;
		fd = (fd - p * iFilterW + iFlength) % iFlength;
		j -= p;
		if (i == width) break;
		if (p == 1) {
			for (l = iFlength - iFilterW; l >= 0; l -= iFilterW, d -= width, j--) {
				m -= aValue[l];
				aValue[l] = yuvp[d];
				m += aValue[l];
			}
		}
		else {
			for (l = 0; l < iFlength; l += iFilterW, d += width, j++) {
				m -= aValue[l];
				aValue[l] = yuvp[d];
				m += aValue[l];
			}
		}
		fterp[md] = m / iFlength;
		md -= p * width;
		d -= iFilterW - 1;
		p = -p;

	}


	for (fd = 0, m = 0, s = 0, d = frameSize; fd < iFlength - iFilterW; fd++) {
		aValue[fd] = yuvp[d++];
		m += aValue[fd];
		bValue[fd] = yuvp[d++];
		s += bValue[fd];//s += aValue[fd];
		if ((fd + 1) % iFilterW == 0) {
			d += width - 2 * iFilterW;
		}
	}
	for (i = iFlength - iFilterW; i < iFlength; i++) aValue[i] = 0, bValue[i] = 0;
	md = frameSize + ((iFilterH - 1) / 2) * width + iFilterW - 2 + (iFilterW % 2);

	j = iFilterH;
	p = 1;
	for (i = iFilterW; i <= width / 2; i++) {

		for (; (j <= height / 2) && (j > 0); j += p) {

			for (l = 0; l < iFilterW; l++, fd++) {
				m -= aValue[fd];
				aValue[fd] = yuvp[d++];
				m += aValue[fd];
				s -= bValue[fd];
				bValue[fd] = yuvp[d++];
				s += bValue[fd];
			}
			fterp[md] = m / iFlength;
			fterp[md + 1] = s / iFlength;
			md += p * width;
			d += p * width - 2 * iFilterW;
			if ((p == 1) && (fd == iFlength)) fd = 0;
			if (p == -1) fd = (fd - 2 * iFilterW + iFlength) % iFlength;

		}
		md -= p * width - 2;
		d -= p * width - 2 * iFilterW;
		fd = (fd - p * iFilterW + iFlength) % iFlength;
		j -= p;
		if (i == width / 2) break;
		if (p == 1) {
			for (l = iFlength - iFilterW; l >= 0; l -= iFilterW, d -= width, j--) {
				m -= aValue[l];
				aValue[l] = yuvp[d++];
				m += aValue[l];
				s -= bValue[l];
				bValue[l] = yuvp[d--];
				s += bValue[l];
			}
		}
		else {
			for (l = 0; l < iFlength; l += iFilterW, d += width, j++) {
				m -= aValue[l];
				aValue[l] = yuvp[d++];
				m += aValue[l];
				s -= bValue[l];
				bValue[l] = yuvp[d--];
				s += bValue[l];
			}
		}
		fterp[md] = m / iFlength;
		fterp[md + 1] = s / iFlength;
		md -= p * width;
		d -= 2 * iFilterW - 2;
		p = -p;

	}

	delete[] aValue;
	delete[] bValue;*/


}

void close(signed char *binaryp, int width, int height){

	int iCloseW = width / 60;
	int iCloseH = height / 60;

	int iFlength = iCloseW * iCloseH;

	unsigned char *hValue = new unsigned char[width];
	unsigned char *expansion = new unsigned char[width * height];
	int i, j, d, fd, md, de, m;

	for (i = 0; i < width; i++) {
		hValue[i] = binaryp[i];
	}

	for (j = 1, d = width; j < iCloseH; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] += binaryp[d++];
		}
	}

	md = ((iCloseH - 1) / 2) * width + (iCloseW + 1) / 2;
	for (i = 0, m = 0; i < iCloseW; i++) {
		m += hValue[i];
	}
	expansion[md++] = m > 0 ? 1 : 0;

	for (fd = 0; i < width; i++) {
		m = m - hValue[fd++] + hValue[i];
		expansion[md++] = m > 0 ? 1 : 0;
	}

	de = 0;
	for (j = iCloseH; j < height; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] = hValue[i] - binaryp[de++] + binaryp[d++];
		}

		m = 0;
		md = md + iCloseW - 1;
		for (i = 0, m = 0; i < iCloseW; i++) {
			m += hValue[i];
		}
		expansion[md++] = m > 0 ? 1 : 0;

		for (fd = 0; i < width; i++) {
			m = m - hValue[fd++] + hValue[i];
			expansion[md++] = m > 0 ? 1 : 0;
		}
	}


	for (i = 0; i < width; i++) {
		hValue[i] = expansion[i];
	}

	for (j = 1, d = width; j < iCloseH; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] += expansion[d++];
		}
	}

	md = ((iCloseH - 1) / 2) * width + (iCloseW + 1) / 2;
	for (i = 0, m = 0; i < iCloseW; i++) {
		m += hValue[i];
	}
	binaryp[md++] = m == iFlength ? 1 : 0;

	for (fd = 0; i < width; i++) {
		m = m - hValue[fd++] + hValue[i];
		binaryp[md++] = m == iFlength ? 1 : 0;
	}

	de = 0;
	for (j = iCloseH; j < height; j++) {
		for (i = 0; i < width; i++) {
			hValue[i] = hValue[i] - expansion[de++] + expansion[d++];
		}

		m = 0;
		md = md + iCloseW - 1;
		for (i = 0, m = 0; i < iCloseW; i++) {
			m += hValue[i];
		}
		binaryp[md++] = m == iFlength ? 1 : 0;

		for (fd = 0; i < width; i++) {
			m = m - hValue[fd++] + hValue[i];
			binaryp[md++] = m == iFlength ? 1 : 0;
		}
	}

	delete[] expansion;
	delete[] hValue;

}

void bwlabel(signed char *binaryp, int *d0, int width, int height){
	vector<Rect> ver;
	vector<int> size, run;
	vector< vector<int> > lable;
	Rect rect;

	int i, j, d, num, stnum, ennum, prnum, ppnum, temp, q;
	signed char prior, t;

	d = 0;
	num = -1;
	stnum = -1;
	ennum = -1;

	for (j = 0; ennum == -1 && j < height; j++) {
		rect.rowRun = j;
		prior = binaryp[d++];
		if (prior) {
			rect.stRun = 0;
		}
		for (i = 1; i < width; i++) {
			t = binaryp[d++];
			if (t != prior) {
				if (prior) {
					ennum++;
					rect.enRun = i;
					rect.runLable = ++num;
					ver.push_back(rect);
					size.push_back(rect.enRun - rect.stRun);
					lable.push_back(run);
					lable[num].push_back(rect.runLable);
				} else {
					rect.stRun = i;
				}
				prior = t;
			}
		}
		if (t) {
			ennum++;
			rect.enRun = width;
			rect.runLable = ++num;
			ver.push_back(rect);
			size.push_back(rect.enRun - rect.stRun);
			lable.push_back(run);
			lable[num].push_back(rect.runLable);
		}

		if (ennum > -1) {
			stnum = 0;
			prnum = -1;
		}
	}


	for (; j < height; j++) {
		ppnum = prnum;
		prnum = ennum;
		rect.rowRun = j;
		prior = binaryp[d++];
		if (prior) {
			rect.stRun = 0;
			rect.runLable = -1;
		}
		for (i = 1; i < width; i++) {
			 t = binaryp[d++];
			 if (t != prior) {
				 if (prior) {
					 ennum++;
					 rect.enRun = i;
					 if (stnum != -1) {
						 for (; stnum <= prnum ;) {
							 if (rect.enRun < ver[stnum].stRun) {
								 if (rect.runLable == -1) {
									 rect.runLable = ++num;
									 size.push_back(rect.enRun - rect.stRun);
									 lable.push_back(run);
									 lable[num].push_back(rect.runLable);
								 }
								 if (stnum > ppnum + 1) stnum--;
								 break;
							 } else if (rect.stRun > ver[stnum].enRun) {
								 if (stnum == prnum) {
								 	 rect.runLable = ++num;
								 	 size.push_back(rect.enRun - rect.stRun);
								 	 lable.push_back(run);
								 	 lable[num].push_back(rect.runLable);
								 	 break;
								 }
							 	stnum++;
							 } else {
								 temp = ver[stnum].runLable;
								 while (lable[temp][0] >= width * height) {
									 temp = lable[temp][0] - width * height;
								 }
								 ver[stnum].runLable = temp;
								 if (rect.runLable == -1) {
									 rect.runLable = temp;
									 size[temp] += rect.enRun - rect.stRun;
								 } else {
									 if (rect.runLable != temp) {
										 size[rect.runLable] += size[temp];
										 lable[rect.runLable].insert(lable[rect.runLable].end(), lable[temp].begin(), lable[temp].end());
										 lable[temp][0] = width * height + rect.runLable;
										 ver[stnum].runLable = rect.runLable;
									 }
								 }
								 if (stnum == prnum) {
								 	break;
								 }
								 stnum++;
							 }
						 }
					 } else {
						 rect.runLable = ++num;
						 size.push_back(rect.enRun - rect.stRun);
						 lable.push_back(run);
						 lable[num].push_back(rect.runLable);
					 }
					 ver.push_back(rect);

				 } else {
					 rect.stRun = i;
					 rect.runLable = -1;
				 }
				 prior = t;
			 }
		}
		if (t) {
			ennum++;
			rect.enRun = width;
			if (stnum != -1) {
				for (; stnum <= prnum ;) {
					if (rect.stRun > ver[stnum].enRun) {
						if (stnum == prnum) {
							rect.runLable = ++num;
							size.push_back(rect.enRun - rect.stRun);
							lable.push_back(run);
							lable[num].push_back(rect.runLable);
							break;
						}
						stnum++;
					} else {
						temp = ver[stnum].runLable;
						while (lable[temp][0] >= width * height) {
							temp = lable[temp][0] - width * height;
						}
						ver[stnum].runLable = temp;
						if (rect.runLable == -1) {
							rect.runLable = temp;
							size[temp] += rect.enRun - rect.stRun;
						} else {
							if (rect.runLable != temp) {
								size[rect.runLable] += size[temp];
								lable[rect.runLable].insert(lable[rect.runLable].end(), lable[temp].begin(), lable[temp].end());
								lable[temp][0] = width * height + rect.runLable;
								ver[stnum].runLable = rect.runLable;
							}
						}
						stnum++;
					}
				}
			} else {
				rect.runLable = ++num;
		 		size.push_back(rect.enRun - rect.stRun);
			 	lable.push_back(run);
			 	lable[num].push_back(rect.runLable);
		 	}
			ver.push_back(rect);
		}
		if (ennum != prnum) stnum = prnum + 1;
		else stnum = -1;
	}


	int *b = new int[num + 1];
	int la = 0, lap = -1, tt = 0;
	for (i = 0, temp = 0, la = 0; i < lable.size(); i++) {
		if (lable[i][0] < width * height) {
			temp++;
			if (size[i] > la) {
				la = size[i];
				lap = temp;
			}
			for (j = 0; j < lable[i].size(); j++) {
				b[lable[i][j]] = temp;
			}
			//vector<int>(lable[i]).swap(lable[i]);
		}
	}
	//vector<vector<int> >(lable).swap(lable);

	binaryp[0] = lap;
	for (i = 0; i < ver.size(); i++) {
		temp = b[ver[i].runLable];
		if (temp == lap && tt == 0) {
			*d0 = ver[i].rowRun * width + ver[i].stRun;
			tt = 1;
		}
		d = ver[i].rowRun * width;
		for (j = ver[i].stRun; j < ver[i].enRun; j++) {
			binaryp[d + j] = temp;
		}
	}

	//vector<Rect>(ver).swap(ver);
	//vector<int>(run).swap(run);
	//vector<int>(size).swap(size);



	delete[] b;
}

void getbinaryData(signed char *binaryp, int *d0, signed char *fterp, int width, int height){
	LOGI("getbinaryData");

	int frameSize = width * height;
	for (int j = 0, yp = 0; j < height; j++) {
		int uvp = frameSize + (j >> 1) * width;
		unsigned char y = 0, cb = 0, cr = 0;
		for (int i = 0; i < width; i++, yp++) {
			y = fterp[yp];
		    if (y < 0) y = 0;
		    if ((i & 1) == 0) {
		    	cr = fterp[uvp++];
		        cb = fterp[uvp++];
		    }

		    float f = 0, g = 0, h = 0, q = 0;
		    f = 43.070 + 1.427 * cb;
		    g = f - 88.467;
		    h = 208.580 - 0.701 * cb;
		    q = h + 34.261;

		    if ((77.78 <= cb && cb <= 93.88 && h <= cr && cr <= f) ||
		    		(93.88 < cb && cb <= 119.35 && h <= cr && cr <= q) ||
		    		(119.35 < cb && cb <= 135.45 && g <= cr && cr <= q)) {
		    	binaryp[yp] = 1;
		    } else {
		    	binaryp[yp] = 0;
		    }

		}
	}

	close(binaryp, width, height);
	bwlabel(binaryp, d0, width, height);

}

void gethandData(signed char *binaryp, int d0, int width, int height){
	LOGI("gethandData_start");
	if (d0 == -1) {
		LOGI("E");
		return;
	}
	int d, s, t, r, pr, er, f, k, q;
	vector<int> fm;
	int p[8] = {1, -width+1, -width, -width-1, -1, width-1, width, width+1};

	k = -1;
	t = binaryp[0];
	d = d0;
	pr = 0;
	r = 1;
	if (d >= 0 && d < width) r = 0;
	if (!((d + 1) % width)) r = 6;
	s = p[r];

	for (; ; ) {
		//LOGI("for_s");
		while ((binaryp[d + s] != t) && (binaryp[d + s] != 127)) {
			//LOGI("find");
			//if (d < 0 || d >= width * height) LOGI("E");
			r = (r + 7) % 8;
			if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
			if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
			if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
			if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
			if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
			s = p[r];
		}
		f = r - pr;
		if (f > 4) f -= 8;
		if (f < -4) f += 8;
		if (f == -4 || f == 4) f = -4;
		while (f < -2) {
			//LOGI("while_s");
			if (f == -3) {
				binaryp[d] = 126;//126
				if (r & 1) {
					fm[k] -= 2;
					if (fm[k] == -3) {
						fm.pop_back();
						k--;
						d -= p[pr];
						//binaryp[d] = 126;
						r = (r + 1) % 8;
						pr  = (pr + 1) % 8;
					}
					else if (fm[k] == -4) {
						fm.pop_back();
						k--;
						d -= p[pr];
						//binaryp[d] = 126;
						r = (r + 1) % 8;
						pr  = (pr + 2) % 8;
						f = -4;
					}
					else {
						s = p[r];
						d += s;
						r = ((r + 1) % 8) | 1;
						if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
						if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
						if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
						if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
						if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
						pr  = (pr + 6) % 8;
						f = 5;
					}
				}
				else {
					fm[k] -= 1;
					if (fm[k] == -3) {
						fm.pop_back();
						k--;
						d -= p[pr];
						//binaryp[d] = 126;
						r = (r + 2) % 8;
						pr  = (pr + 2) % 8;
					}
					else if (fm[k] == -4) {
						fm.pop_back();
						k--;
						d -= p[pr];
						//binaryp[d] = 126;
						r = (r + 2) % 8;
						pr  = (pr + 3) % 8;
						f = -4;
					}
					else {
						s = p[r];
						d += s;
						r = ((r + 1) % 8) | 1;
						if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
						if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
						if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
						if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
						if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
						pr  = (pr + 7) % 8;
						f = 5;
					}
				}

			}
			if (f == -4) {
				s = p[r];
				binaryp[d] = 126;//
				d += s;
				pr = r;
				r = ((r + 1) % 8) | 1;
				if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
				if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
				if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
				if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
				if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
				s = p[r];
				while ((binaryp[d + s] != t) && (binaryp[d + s] != 127)) {
					r = (r + 7) % 8;
					if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
					if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
					if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
					if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
					if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
					s = p[r];
				}

				er = r - pr;
				if (er > 4) er -= 8;
				if (er < -4) er += 8;
				if (er == -4 || er == 4) er = -4;
				f = er + fm[k] - 4;
				if (f == -4) {
					if (k) {
						//binaryp[d] = 0;//126
						fm.pop_back();
						k--;
					}
					else {
						//binaryp[d] = 0;//126
						fm.pop_back();
						k--;
						d += s;
						pr = r;
						r = ((r + 1) % 8) | 1;
						if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
						if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
						if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
						if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
						if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
						s = p[r];
						f = 5;
					}
				}
				else if (f == -3) {
					fm.pop_back();
					k--;
					pr = (r + 3) % 8;
				}
				else {
					fm[k] = f;
					d += s;
					pr = r;
				 	r = ((r + 1) % 8) | 1;
				 	if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
					if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
					if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
					if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
					if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
				 	s = p[r];
				 	f = 5;
				}
			}
			//LOGI("while_e");
		}
		if (f > -3 && f < 3) {
			binaryp[d] = 127;
			fm.push_back(f);
			k++;
			d += s;
			pr = r;
			r = ((r + 1) % 8) | 1;
			if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
			if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
			if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
			if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
			if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
			s = p[r];
		}
		if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
		if ((d >= 0 && d < width) && (r == 3 || r == 2 || r == 1)) r = 0;
		if (!((d + 1) % width) && (r == 1 || r == 0 || r == 7)) r = 6;
		if ((d >= (width * height - width) && d < (width * height)) && (r == 7 || r == 6 || r == 5)) r = 4;
		if (!(d % width) && (r == 5 || r == 4 || r == 3)) r = 2;
		s = p[r];
		//LOGI("for_e");
		if (d == d0) break;
	}
	//LOGI("getHandData_end");
	if (fm.size()) {
		int u = fm.size();
		int n = u / 25;
		float m_f = 2.7 * n;
		float m_l = -2.5 * n;
		int i, j, pb;
		char num_f = 0, num_l = 0;

		int *a = new int[u];
		a[0] = 0;
		for (i = 0; i < n; i++) a[0] += fm[i];

		for (i = 1, j = n; i < u; i++, j++) {
			a[i] = a[i - 1] - fm[i - 1] + fm[j % u];
		}

		int *b = new int[u];
		b[0] = 0;
		for (i = 0; i < n; i++) b[0] += a[i];
		pb = b[0];
		for (i = 1, j = n; i < u; i++, j++) {
			b[i] = b[i - 1] - a[i - 1] + a[j % u];
			if ((pb >= m_f && b[i] < m_f) || (pb > m_f && b[i] <= m_f)) num_f++;
			else if ((pb <= m_l && b[i] > m_l) || (pb < m_l && b[i] >= m_l)) num_l++;
			pb = b[i];
		}
		binaryp[1] = num_f;
		binaryp[2] = num_l;

		delete[] a;
		delete[] b;
	}
}


#endif
