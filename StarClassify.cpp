/* 
 * File:   StarClassify.cpp
 * Author: xy
 * 
 * Created on March 11, 2014, 9:57 AM
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "StarClassify.h"

using namespace std;

/**
 * 
 * @param zoneIdx
 * @param objStar
 */
void PartitionClassify::searchSimilarStar(long zoneIdx, CMStar *objStar) {

  float error = errRadius;
  CMStar *matchStar = objStar->match;
  CMStar *nextStar = zoneArray[zoneIdx].star;
  CMStar *topStar = nextStar;

  while (NULL != matchStar && NULL != matchStar->match) {
    matchStar = matchStar->match;
  }

  /**
   * 如果区域中的第一个星就匹配，则在去掉这个星后，区域的头指针需要指向下一个星
   */
  while (nextStar) {
    float distance = getLineDistance(nextStar, objStar);
    if (distance < error) {
      if (zoneArray[zoneIdx].star == nextStar) {
        zoneArray[zoneIdx].star = nextStar->next;
      }
      if (NULL == matchStar) {
        objStar->match = nextStar;
        matchStar = nextStar;
      } else {
        matchStar->next = nextStar;
        matchStar = nextStar;
      }
      matchStar->error = distance;
    }
    nextStar = nextStar->next;
  }

  return matchStar;
}

CMStar *PartitionClassify::match(float errorBox) {

  CMStar *headStar = NULL;
  CMStar *curStar = NULL;

  for (int idx = 0; idx < totalZone; idx++) {
    if (zoneArray[idx].starNum > 0) {
      //headStar指向结果列表的第一颗星
      if (NULL == headStar) {
        headStar = zoneArray[idx].star;
      }
      //curStar指向当前待匹配的星
      if (NULL == curStar) {
        curStar = zoneArray[idx].star;
      }else{
        curStar->next = zoneArray[idx].star;
        curStar = curStar->next;
      }
      //去掉当前这颗星
      zoneArray[idx].star = curStar->next;

      while (NULL != curStar) {
        long sZoneNum = 0;
        long *searchZonesIdx = getStarSearchZone(curStar, sZoneNum);

        float minError = errRadius;
        CMStar *minPoint = NULL;

        int i;
        for (i = 0; i < sZoneNum; i++) {
          searchSimilarStar(searchZonesIdx[i], curStar);
        }
        free(searchZonesIdx);
        curStar = curStar->next;
      }
    }
  }

  return headStar;

}

CMStar *CrossMatchClassify::readStarFile(char *fName, int &starNum) {

  FILE *fp = fopen(fName, "r");
  if (fp == NULL) {
    return NULL;
  }

  starNum = 0;
  CMStarClassify *starList = NULL;
  CMStarClassify *tStar = NULL;
  CMStarClassify *nextStar = NULL;
  char line[MaxStringLength];

  while (fgets(line, MaxStringLength, fp) != NULL) {
    if (3 == sscanf(line, "%f%f%f", &nextStar->pixx, &nextStar->pixy, &nextStar->mag)) {
      nextStar = (CMStarClassify *) malloc(sizeof (CMStarClassify));
      nextStar->id = starNum;
      nextStar->next = NULL;
      nextStar->match = NULL;
      nextStar->line = line;
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

#ifdef PRINT_CM_DETAIL
  printf("%s\t%d stars\n", fName, starNum);
#endif
  return starList;

}

void CrossMatchClassify::match(char *fName, float errorBox) {

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;

  refStarList = readStarFile(fName, refNum);

  zones = new Partition(errorBox, minZoneLen, searchRds);
  zones->partitonStarField(refStarList, refNum);

#ifdef PRINT_CM_DETAIL
  printf("partition match done!\n");
#endif
}

void CrossMatchClassify::writeResult(char *outfName, float errorBox) {

}

StarClassify::StarClassify() {
}

StarClassify::StarClassify(const StarClassify& orig) {
}

StarClassify::~StarClassify() {
}

