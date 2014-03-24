/* 
 * File:   StarClassify.h
 * Author: xy
 *
 * Created on March 11, 2014, 9:57 AM
 */

#include <string>
#include "CrossMatch.h"
#include "cmhead.h"

using std::string;

#ifndef STARCLASSIFY_H
#define	STARCLASSIFY_H

class CMStarClassify : public CMStar {
public:
  string line;

};

class PartitionClassify : public Partition {
public:
  void searchSimilarStar(long zoneIdx, CMStar *objStar);
  CMStar *match(float errorBox);
};

class CrossMatchClassify : public CrossMatch {
public:
  CMStar *readStarFile(char *fName, int &starNum);
  void match(char *fName, float errorBox);
  void writeResult(char *outfName, float errorBox);
};

class StarClassify {
public:
  StarClassify();
  StarClassify(const StarClassify& orig);
  virtual ~StarClassify();

private:

};

#endif	/* STARCLASSIFY_H */

