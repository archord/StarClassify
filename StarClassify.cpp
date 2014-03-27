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

PartitionClassify::PartitionClassify(float errBox, float minZoneLen, float searchRds) {
  errRadius = errBox;
  minZoneLength = minZoneLen;
  searchRadius = searchRds;
}

/**
 * 
 * @param zoneIdx
 * @param objStar
 */
void PartitionClassify::searchSimilarStar(long zoneIdx, CMStar *objStar) {

  float error = errRadius;
  CMStar *matchStar = objStar->match;
  CMStar *nextStar = zoneArray[zoneIdx].star;
  CMStar *topStar = zoneArray[zoneIdx].star;
  CMStar *tmpStar = NULL;

  while (NULL != matchStar && NULL != matchStar->match) {
    matchStar = matchStar->match;
  }

  /**
   * 如果区域中的第一个星就匹配，则在去掉这个星后，区域的头指针需要指向下一个星
   * 在区域中去掉一颗星后，区域的星的总数要减1
   */
  while (nextStar) {
    float distance = getLineDistance(nextStar, objStar);
    if (distance < error) {
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

      tmpStar->next = NULL;
      tmpStar->match = NULL;

      zoneArray[zoneIdx].starNum--;
      starCount++;
      if (NULL == matchStar) {
	objStar->match = tmpStar;
	matchStar = tmpStar;
      } else {
	matchStar->match = tmpStar;
	matchStar = matchStar->match;
      }
      matchStar->error = distance;
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
  int tNum = 0;

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
      tNum += zoneArray[idx].starNum;
    }
  }

#ifdef PRINT_CM_DETAIL
  printf("Classified stars: %d\n", starClassify);
  printf("match total stars: %d\n", starCount);
  printf("match total stars2: %d\n", tNum);
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

  while (fgets(line, MaxStringLength, fp) != NULL) {
    if (2 == sscanf(line, "%*s%*s%*s%*s%f%f", &pixx, &pixy)) {
      nextStar = (CMStarClassify *) malloc(sizeof (CMStarClassify));
      nextStar->id = starNum;
      nextStar->pixx = pixx;
      nextStar->pixy = pixy;
      nextStar->next = NULL;
      nextStar->match = NULL;
      nextStar->line = (char*) malloc(strlen(line) + 1);
      strcpy(nextStar->line, line);
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
  printf("read %d stars from %s\n", starNum - 1, fName);
  return starList;
}

void CrossMatchClassify::match(char *infile, char *outfile, float errorBox) {

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;

  refStarList = readStarFile(infile, refNum);
  zones = new PartitionClassify(errorBox, minZoneLen, searchRds);
  zones->partitonStarField(refStarList, refNum);
  rstStar = zones->match();
  writeResult(outfile);
  freeRstList(rstStar);
  zones->freeZoneArray();

#ifdef PRINT_CM_DETAIL
  printf("match done!\n");
#endif
}

void CrossMatchClassify::writeResult(char *outfName) {

  FILE *fp = fopen(outfName, "w");

  long clfStarNum = 0;
  long totalStar = 0;
  CMStarClassify *tStar = (CMStarClassify*) rstStar;
  while (NULL != tStar) {
    clfStarNum++;
    fprintf(fp, "#Star %d\n", clfStarNum);
    fprintf(fp, "%s", tStar->line);
    long starMchNum = 1;
    CMStarClassify *tStarMch = (CMStarClassify *) tStar->match;
    while (NULL != tStarMch) {
      fprintf(fp, "%s", tStarMch->line);
      starMchNum++;
      tStarMch = (CMStarClassify *) tStarMch->match;
    }
    totalStar += starMchNum;
    fprintf(fp, "#total matched %d\n\n", starMchNum);
    tStar = (CMStarClassify *) tStar->next;
  }
  fclose(fp);

  printf("write %d classes, total %d stars to %s\n", clfStarNum, totalStar, outfName);
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

