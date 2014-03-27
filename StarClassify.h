/* 
 * File:   StarClassify.h
 * Author: xy
 *
 * Created on March 11, 2014, 9:57 AM
 */

#include <string>
#include "CrossMatch.h"
#include "cmhead.h"

#ifndef STARCLASSIFY_H
#define	STARCLASSIFY_H

class CMStarClassify : public CMStar {
public:
  char *line;
};

class PartitionClassify : public Partition {
public:
  long starCount = 0;
public:
  PartitionClassify();
  PartitionClassify(float errBox, float minZoneLen, float searchRds);
  void searchSimilarStar(long zoneIdx, CMStar *objStar);
  CMStar *match();
};

class CrossMatchClassify : public CrossMatch {
private:
  CMStar *rstStar;
  PartitionClassify *zones;
public:
  CMStar *readStarFile(char *fName, int &starNum);
  void match(char *infile, char *outfile, float errorBox);
  void writeResult(char *outfName);
  void freeMchList(CMStar *starList);
  void freeRstList(CMStar *starList);
};

class StarClassify {
public:
  StarClassify();
  StarClassify(const StarClassify& orig);
  virtual ~StarClassify();

private:

};

#endif	/* STARCLASSIFY_H */

