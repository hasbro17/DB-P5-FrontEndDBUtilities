#include "catalog.h"

//
// Destroys a relation. It performs the following steps:
//
// 	removes the catalog entry for the relation
// 	destroys the heap file containing the tuples in the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::destroyRel(const string & relation)
{
  Status status;

  if (relation.empty() || 
      relation == string(RELCATNAME) || 
      relation == string(ATTRCATNAME))
    return BADCATPARM;

	//remove all info for this relation from relcat, should only be one entry
	status=relCat->removeInfo(relation);
	if(status==RELNOTFOUND)
			return status;
	if(status!=OK)
	{
			printf("MyError: RelCat::removeInfo() returned error in destroyRel\n");
			return status;
	}

	//remove all info for this relation from attrcat
	status=attrCat->dropRelation(relation);

	//destroy the heapfile for the realtion
	status=destroyHeapFile(relation);
	if(status!=OK)
	{
			printf("MyError: destroyHeapFile() returned error in destroyRel\n");
	}

	return status;

}


//
// Drops a relation. It performs the following steps:
//
// 	removes the catalog entries for the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status AttrCatalog::dropRelation(const string & relation)
{
	RID rid;
  Status status;
  AttrDesc *attrs;
  int attrCnt, i;

  if (relation.empty()) return BADCATPARM;
	
	//Open up a HeapFileScan on our relCatalog file RELCATNAME
	HeapFileScan *scan = new HeapFileScan(ATTRCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Cannot open HeapFileScan(ATTRCATNAME) in removeInfo\n");
			return status;
	}

	//initiate a startScan for this relation
	status=scan->startScan(0, MAXNAME, STRING, relation.data(), EQ);
	if(status!=OK)
	{
			printf("MyError: Cannot initiate startScan in dropRelation\n");
			return status;
	}

	//do a scan next to get every match until end of file.//Alternative method to get count for attrCnt from RelCat::getInfo() and use a for loop
	//but this should work as well
	while(status!=FILEEOF)
	{
			status=scan->scanNext(rid);
			if(status!=OK && status!=FILEEOF)
			{
					printf("MyError: Something wrong while scanNext() in dropRelation\n");
					return status;
			}
			if(status==OK)//rec found
			{
					//delete the record that was saved in curRec from scanNext
					status=scan->deleteRecord();
					if(status!=OK)
					{
							printf("MyError: Something wrong while deleteRecord() in removeInfo\n");
							return status;
					}
			}
	}

	scan->endScan();
	delete scan;
	return OK;
}


