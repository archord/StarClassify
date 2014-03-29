/* 
 * File:   StarClassify.cpp
 * Author: xy
 * 
 * Created on March 11, 2014, 9:57 AM
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include "StarClassify.h"

using namespace std;

int CMStarClassify::getNum() {
  char b[10];
  char *numHed = strrchr(line, '_') + 1;
  char *numEnd = strstr(numHed, ".fit");
  strncpy(b, numHed, numEnd - numHed);
  b[numEnd - numHed] = '\0';
  number = atoi(b);
}

bool CMStarClassify::isSame(CMStarClassify *star) {
  if (star->number == number) {
    return true;
  } else {
    return false;
  }
}

void PartitionClassify::swapValue(CMStarClassify *star1, CMStarClassify *star2) {
  CMStarClassify *tStar = (CMStarClassify *) malloc(sizeof (CMStarClassify));
  memcpy(tStar, star1, sizeof (CMStarClassify));
  memcpy(star1, star2, sizeof (CMStarClassify));
  memcpy(star2, tStar, sizeof (CMStarClassify));
  CMStar *tMch = star1->match;
  star1->match = star2->match;
  star2->match = tMch;

  free(tStar);
}

PartitionClassify::PartitionClassify(float errBox, float minZoneLen, float searchRds) {
  errRadius = errBox;
  minZoneLength = minZoneLen;
  searchRadius = searchRds;
}

bool PartitionClassify::hasSame(CMStarClassify *mchStar, CMStarClassify *objStar) {

  CMStarClassify *nextStar = mchStar;
  while (nextStar) {
    if (objStar->isSame(nextStar)) {
      return true;
    }
    nextStar = (CMStarClassify *) nextStar->match;
  }
  return false;
}

void PartitionClassify::addMchSort(CMStarClassify *headStar, CMStarClassify *tStar) {

  headStar->avgMag += tStar->mag;
  headStar->avgX += tStar->pixx;
  headStar->avgY += tStar->pixy;
  headStar->total++;
  tStar->avgMag = headStar->avgMag;
  tStar->avgX = headStar->avgX;
  tStar->avgY = headStar->avgY;
  tStar->total = headStar->total;

  tStar->match = NULL;
  if (headStar->number > tStar->number) {
    swapValue(headStar, tStar);
    tStar->match = headStar->match;
    headStar->match = tStar;
  } else {
    if (NULL == headStar->match) {
      headStar->match = tStar;
    } else {
      CMStarClassify *hStar = (CMStarClassify *) headStar->match;
      CMStarClassify *curStar = hStar;
      if (curStar->number > tStar->number) {
	tStar->match = curStar;
	headStar->match = tStar;
      } else {
	while (NULL != curStar && curStar->number < tStar->number) {
	  hStar = curStar;
	  curStar = (CMStarClassify *) curStar->match;
	}
	tStar->match = hStar->match;
	hStar->match = tStar;
      }
    }
  }
}

/**
 * 
 * @param zoneIdx
 * @param objStar
 */
void PartitionClassify::searchSimilarStar(long zoneIdx, CMStar *objStar) {

  float error = errRadius;
  //CMStar *matchStar = objStar->match;
  CMStar *nextStar = zoneArray[zoneIdx].star;
  CMStar *topStar = zoneArray[zoneIdx].star;
  CMStar *tmpStar = NULL;

  /*
  while (NULL != matchStar && NULL != matchStar->match) {
    matchStar = matchStar->match;
  }
   */

  /**
   * 如果区域中的第一个星就匹配，则在去掉这个星后，区域的头指针需要指向下一个星
   * 在区域中去掉一颗星后，区域的星的总数要减1
   */
  while (nextStar) {
    float distance = getLineDistance(nextStar, objStar);
    if (distance < error) {
      nextStar->error = distance;
#ifdef PRINT_CM_DETAILa
      printf("mtch star id: %d\n", nextStar->id);
#endif
      tmpStar = nextStar;
      if (zoneArray[zoneIdx].star == nextStar) {
	zoneArray[zoneIdx].star = nextStar->next;
	topStar = zoneArray[zoneIdx].star;
	nextStar = zoneArray[zoneIdx].star;
      } else {
	topStar->next = nextStar->next;
	nextStar = nextStar->next;
      }

      if (hasSame((CMStarClassify *) objStar, (CMStarClassify *) tmpStar)) {
	rptStar++;
	zoneArray[zoneIdx].starNum--;
	free(tmpStar);
      } else {
	tmpStar->next = NULL;
	tmpStar->match = NULL;
	zoneArray[zoneIdx].starNum--;
	starCount++;
	addMchSort((CMStarClassify *) objStar, (CMStarClassify *) tmpStar);
	/**
	if (NULL == matchStar) {
	  objStar->match = tmpStar;
	  matchStar = tmpStar;
	} else {
	  matchStar->match = tmpStar;
	  matchStar = matchStar->match;
	}
	matchStar->error = distance;
	 */
      }
    } else {
      topStar = nextStar;
      nextStar = nextStar->next;
    }

  }
}

CMStar *PartitionClassify::match() {

  CMStar *headStar = NULL;
  CMStar *curStar = NULL;
  int starClassify = 0;

  for (int idx = 0; idx < totalZone; idx++) {
    if (zoneArray[idx].starNum > 0) {
#ifdef PRINT_CM_DETAILa
      printf("**************\n");
      printf("zone id: %d\n", idx);
      printf("zone star: %d\n", zoneArray[idx].starNum);
#endif
      while (NULL != zoneArray[idx].star) {
	//headStar指向结果列表的第一颗星
	//curStar指向当前待匹配的星
	if (NULL == headStar) {
	  headStar = zoneArray[idx].star;
	  curStar = headStar;
	} else {
	  curStar->next = zoneArray[idx].star;
	  curStar = curStar->next;
	}
#ifdef PRINT_CM_DETAILa
	printf("obj star id: %d\n", curStar->id);
#endif
	//去掉当前这颗星
	zoneArray[idx].star = curStar->next;
	curStar->next = NULL;
	curStar->match = NULL;
	//分区中星的总数减一
	zoneArray[idx].starNum--;
	starCount++;
	//分类星加一
	starClassify++;
	//寻找分类星的匹配星
	long sZoneNum = 0;
	long *searchZonesIdx = getStarSearchZone(curStar, sZoneNum);
	for (int i = 0; i < sZoneNum; i++) {
	  searchSimilarStar(searchZonesIdx[i], curStar);
	}
	free(searchZonesIdx);
      }
    }
  }

#ifdef PRINT_CM_DETAIL
  printf("stars: %d\n", starClassify);
  printf("total records: %d\n", starCount);
  printf("total repeat records: %d\n", rptStar);
#endif

  return headStar;
}

CMStar *CrossMatchClassify::readStarFile(char *fName, int &starNum) {

  FILE *fp = fopen(fName, "r");
  if (fp == NULL) {
    return NULL;
  }

  starNum = 1;
  CMStarClassify *starList = NULL;
  CMStarClassify *tStar = NULL;
  CMStarClassify *nextStar = NULL;
  char line[MaxStringLength];
  float pixx = 0.0;
  float pixy = 0.0;
  float mag = 0.0;

  while (fgets(line, MaxStringLength, fp) != NULL) {
    if (3 == sscanf(line, "%*s%*s%*s%*s%f%f%*s%*s%*s%*s%*s%*s%f", &pixx, &pixy, &mag)) {
      nextStar = (CMStarClassify *) malloc(sizeof (CMStarClassify));
      nextStar->id = starNum;
      nextStar->pixx = pixx;
      nextStar->pixy = pixy;
      nextStar->mag = mag;
      nextStar->avgMag = mag;
      nextStar->avgX = pixx;
      nextStar->avgY = pixy;
      nextStar->total = 1;
      nextStar->next = NULL;
      nextStar->match = NULL;
      nextStar->line = (char*) malloc(strlen(line) + 1);
      strcpy(nextStar->line, line);
      nextStar->getNum();
      if (NULL == starList) {
	starList = nextStar;
	tStar = nextStar;
      } else {
	tStar->next = nextStar;
	tStar = nextStar;
      }
      starNum++;
    }
  }
  printf("read %d records from %s\n", starNum - 1, fName);
  return starList;
}

void CrossMatchClassify::match(char *infile, char *outfile, float errorBox) {

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;

  refStarList = readStarFile(infile, refNum);
  zones = new PartitionClassify(errorBox, minZoneLen, searchRds);
  zones->partitonStarField(refStarList, refNum);
  rstStar = zones->match();
  writeResult1(outfile);
  writeResult2(outfile);
  writeResult3(outfile);
  writeXYMag(outfile);
  freeRstList(rstStar);
  zones->freeZoneArray();

#ifdef PRINT_CM_DETAIL
  printf("match done!\n");
#endif
}

void CrossMatchClassify::writeXYMag(char *outfName) {

  char rstfile[256];
  sprintf(rstfile, "%s.xymag", outfName);

  FILE *fp = fopen(rstfile, "w");

  long clfStarNum = 0;
  CMStarClassify *tStar = (CMStarClassify*) rstStar;
  while (NULL != tStar) {
    if (tStar->total >= 3) {
      clfStarNum++;
      fprintf(fp, "#%f %f %f\n", tStar->avgX / tStar->total, tStar->avgY / tStar->total, tStar->avgMag / tStar->total);
    }
    tStar = (CMStarClassify *) tStar->next;
  }
  fclose(fp);

  printf("write %d stars to %s\n", clfStarNum, rstfile);
  printf("total repeat records: %d\n", zones->rptStar);
}

void CrossMatchClassify::writeResult1(char *outfName) {

  char rstfile[256];
  sprintf(rstfile, "%s.1star", outfName);

  FILE *fp = fopen(rstfile, "w");

  long clfStarNum = 0;
  long totalStar = 0;
  CMStarClassify *tStar = (CMStarClassify*) rstStar;
  while (NULL != tStar) {
    if (tStar->total == 1) {
      clfStarNum++;
      //fprintf(fp, "#Star %d\n", clfStarNum);
      //fprintf(fp, "#%f %f %f\n", tStar->avgX / tStar->total, tStar->avgY / tStar->total, tStar->avgMag / tStar->total);
      fprintf(fp, "%s", tStar->line);
      CMStarClassify *tStarMch = (CMStarClassify *) tStar->match;
      while (NULL != tStarMch) {
	fprintf(fp, "%s", tStarMch->line);
	tStarMch = (CMStarClassify *) tStarMch->match;
      }
      totalStar += tStar->total;
      //fprintf(fp, "#total matched %d\n\n", tStar->total);
    }
    tStar = (CMStarClassify *) tStar->next;
  }
  fclose(fp);

  printf("write %d stars, total %d records to %s\n", clfStarNum, totalStar, rstfile);
  //printf("total repeat records: %d\n", zones->rptStar);
}

void CrossMatchClassify::writeResult2(char *outfName) {

  char rstfile[256];
  sprintf(rstfile, "%s.2star", outfName);

  FILE *fp = fopen(rstfile, "w");

  long clfStarNum = 0;
  long totalStar = 0;
  CMStarClassify *tStar = (CMStarClassify*) rstStar;
  while (NULL != tStar) {
    if (tStar->total == 2) {
      clfStarNum++;
      fprintf(fp, "#Star %d\n", clfStarNum);
      fprintf(fp, "#%f %f %f\n", tStar->avgX / tStar->total, tStar->avgY / tStar->total, tStar->avgMag / tStar->total);
      fprintf(fp, "%s", tStar->line);
      CMStarClassify *tStarMch = (CMStarClassify *) tStar->match;
      while (NULL != tStarMch) {
	fprintf(fp, "%s", tStarMch->line);
	tStarMch = (CMStarClassify *) tStarMch->match;
      }
      totalStar += tStar->total;
      //fprintf(fp, "#total matched %d\n\n", tStar->total);
    }
    tStar = (CMStarClassify *) tStar->next;
  }
  fclose(fp);

  printf("write %d stars, total %d records to %s\n", clfStarNum, totalStar, rstfile);
  //printf("total repeat records: %d\n", zones->rptStar);
}

void CrossMatchClassify::writeResult3(char *outfName) {

  char rstfile[256];
  sprintf(rstfile, "%s.3star", outfName);

  FILE *fp = fopen(rstfile, "w");

  long clfStarNum = 0;
  long totalStar = 0;
  CMStarClassify *tStar = (CMStarClassify*) rstStar;
  while (NULL != tStar) {
    if (tStar->total >= 3) {
      clfStarNum++;
      fprintf(fp, "#Star %d\n", clfStarNum);
      fprintf(fp, "#%f %f %f\n", tStar->avgX / tStar->total, tStar->avgY / tStar->total, tStar->avgMag / tStar->total);
      fprintf(fp, "%s", tStar->line);
      CMStarClassify *tStarMch = (CMStarClassify *) tStar->match;
      while (NULL != tStarMch) {
	fprintf(fp, "%s", tStarMch->line);
	tStarMch = (CMStarClassify *) tStarMch->match;
      }
      totalStar += tStar->total;
      fprintf(fp, "#total matched %d\n\n", tStar->total);
    }
    tStar = (CMStarClassify *) tStar->next;
  }
  fclose(fp);

  printf("write %d stars, total %d records to %s\n", clfStarNum, totalStar, rstfile);
  //printf("total repeat records: %d\n", zones->rptStar);
}

void CrossMatchClassify::freeRstList(CMStar *starList) {

  CMStar *tStar = starList;
  while (NULL != tStar) {
    freeMchList(tStar->match);
    tStar = tStar->next;
  }
  freeStarList(starList);
}

void CrossMatchClassify::freeMchList(CMStar *starList) {

  if (NULL != starList) {
    CMStar *tStar = starList->match;
    while (tStar) {
      starList->match = tStar->match;
      free(((CMStarClassify *) tStar)->line);
      free(tStar);
      tStar = starList->match;
    }
    free(starList);
  }
}

StarClassify::StarClassify() {
}

StarClassify::StarClassify(const StarClassify& orig) {
}

StarClassify::~StarClassify() {
}

