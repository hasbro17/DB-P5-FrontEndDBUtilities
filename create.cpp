#include "catalog.h"


const Status RelCatalog::createRel(const string & relation, 
				   const int attrCnt,
				   const attrInfo attrList[])
{
  Status status;
  RelDesc rd;
  AttrDesc ad;

  if (relation.empty() || attrCnt < 1)
    return BADCATPARM;

  if (relation.length() >= sizeof rd.relName)
    return NAMETOOLONG;

	//check if realtion does not already exist inside of RelCatalog
	status=relCat->getInfo(relation, rd);
	if(status==OK)//should be no record found
		return RELEXISTS;
	else if(status!=RELNOTFOUND)//some other problem, this should be RELNOTFOUND
	 {
		 printf("MyError: RelCat::getInfo() returned error in createRel\n");
		 return status;
	 }

	//Add this new relation as a tuple to the relcatalog relation
	//as a RelDesc tuple
	strcpy(rd.relName, relation.c_str());//set the relName
	rd.attrCnt=attrCnt;
	//add this to the relcatalog
	status=relCat->addInfo(rd);
	if(status!=OK)
	{
		printf("MyError: RelCat::addInfo() returned error in createRel\n");
		return status;
	}

	//Add all the new attributes to the attrcatalog
	//as AttrDesc tuples
	int offset=0;
	for(int i=0; i<attrCnt; i++)
	{
		//initialize AttrDesc tuple
		strcpy(ad.relName, attrList[i].relName);
		strcpy(ad.attrName, attrList[i].attrName);
		ad.attrOffset=offset;
		ad.attrType=attrList[i].attrType;
		ad.attrLen=attrList[i].attrLen;
		//add it to the attrcatalog
		status=attrCat->addInfo(ad);
		if(status!=OK)
		{
			printf("MyError: AttrCat::addInfo() returned error in createRel\n");
			return status;
		}
		offset+=ad.attrLen;//Offset for next attribute is increased by the len of previous attribute

	}

	//Create Heapfile for the actual relation itself
	status = createHeapFile(relation);
	if(status!=OK)
	{
		printf("MyError: createHeapFile() failed in createRel\n");
		return status;
	}

	return OK;

}

