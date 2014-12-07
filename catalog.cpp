#include "catalog.h"


RelCatalog::RelCatalog(Status &status) :
	 HeapFile(RELCATNAME, status)
{
// nothing should be needed here
}


const Status RelCatalog::getInfo(const string & relation, RelDesc &record)
{
  if (relation.empty())
    return BADCATPARM;

  Status status;
  Record rec;
  RID rid;

	//Open up a HeapFileScan on our relCatalog file RELCATNAME
	HeapFileScan *scan = new HeapFileScan(RELCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Cannot open HeapFileScan(RELCATNAME) in getInfo\n");
			return status;
	}

	//initiate a startScan for this relation
	status=scan->startScan(0, MAXNAME, STRING, (char*)&relation, EQ);
	if(status!=OK)
	{
			printf("MyError: Cannot initiate startScan in getInfo\n");
			return status;
	}

	//do a scan next to get even one match.
	status=scan->scanNext(rid);
	if(status==FILEEOF)//should only be EOF
		return RELNOTFOUND;
	else if(status!=OK)
	{
			printf("MyError: Something wrong while scanNext() in getInfo\n");
			return status;
	}

	//get record from RID
	status=scan->getRecord(rec);
	if(status!=OK)
	{
			printf("MyError: Problem in getRecord after valid scanNext in getInfo\n");
			return status;
	}

	//memcopy the dbrecord data into the return param Reldesc record
	memcpy(&record, rec.data, rec.length);
	
	delete scan;
	return OK;

}


const Status RelCatalog::addInfo(RelDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;
	Record rec;

	ifs= new InsertFileScan(RELCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Could not open InsertFileScan on relcatalog in addInfo\n");
			return status;
	}

	//create a record to insert in ifs
	rec.data=&record;
	rec.length=sizeof(RelDesc);//FIXME:Check if this is correct.

	//insert the record
	status=ifs->insertRecord(rec, rid);
	if(status!=OK)
	{
			printf("MyError: Could not insertRecord in relcatalog in addInfo\n");
			return status;
	}
	
	delete ifs;
	return OK;
}

const Status RelCatalog::removeInfo(const string & relation)
{
  Status status;
  RID rid;
  HeapFileScan*  scan;

  if (relation.empty()) return BADCATPARM;
	

	//Open up a HeapFileScan on our relCatalog file RELCATNAME
	scan = new HeapFileScan(RELCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Cannot open HeapFileScan(RELCATNAME) in removeInfo\n");
			return status;
	}

	//initiate a startScan for this relation
	status=scan->startScan(0, MAXNAME, STRING, (char*)&relation, EQ);
	if(status!=OK)
	{
			printf("MyError: Cannot initiate startScan in removeInfo\n");
			return status;
	}

	//do a scan next to get even one match.
	status=scan->scanNext(rid);
	if(status==FILEEOF)//should only be EOF
		return RELNOTFOUND;
	else if(status!=OK)
	{
			printf("MyError: Something wrong while scanNext() in removeInfo\n");
			return status;
	}

	//delete the record that was saved in curRec from scanNext
	status=scan->deleteRecord();
	if(status!=OK)
	{
			printf("MyError: Something wrong while deleteRecord() in removeInfo\n");
			return status;
	}
	
	delete scan;
	return OK;
}


RelCatalog::~RelCatalog()
{
// nothing should be needed here
}


AttrCatalog::AttrCatalog(Status &status) :
	 HeapFile(ATTRCATNAME, status)
{
// nothing should be needed here
}


const Status AttrCatalog::getInfo(const string & relation, 
				  const string & attrName,
				  AttrDesc &record)
{

  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  scan;

  if (relation.empty() || attrName.empty()) return BADCATPARM;

	
	//Open up a HeapFileScan on our relCatalog file RELCATNAME
	scan = new HeapFileScan(ATTRCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Cannot open HeapFileScan(ATTRCATNAME) in getInfo\n");
			return status;
	}

	//initiate a startScan for this relation
	status=scan->startScan(0, MAXNAME, STRING, (char*)&relation, EQ);
	if(status!=OK)
	{
			printf("MyError: Cannot initiate startScan in AttrCat::getInfo()\n");
			return status;
	}

	//scan for all records with the relation name
	while(status!=FILEEOF)
	{
			status=scan->scanNext(rid);
			if(status!=OK && status!=FILEEOF)
			{
					printf("MyError: Something wrong while scanNext() in AttrCat::getInfo()\n");
					return status;
			}
			if(status==OK)//rec found
			{	
					//get record from RID
					status=scan->getRecord(rec);
					if(status!=OK)
					{
							printf("MyError: Problem in getRecord after valid scanNext in AttrCat::getInfo()\n");
							return status;
					}

					//memcopy the dbrecord data into the return param AttrDesc record
					memcpy(&record, rec.data, rec.length);
					
					//check if the record attrName matches the given attrName
					if(strcmp(record.attrName, attrName.c_str())==0)
					{
							delete scan;
							return OK;
					}
			}
	}

	//End of file so attr not found
	return ATTRNOTFOUND;

}


const Status AttrCatalog::addInfo(AttrDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;
	Record rec;
	
	ifs= new InsertFileScan(ATTRCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Could not open InsertFileScan on attrcatalog in addInfo\n");
			return status;
	}

	//create a record to insert in ifs
	rec.data=&record;
	rec.length=sizeof(AttrDesc);//FIXME:Check if this is correct.

	//insert the record
	status=ifs->insertRecord(rec, rid);
	if(status!=OK)
	{
			printf("MyError: Could not insertRecord in attrcatalog in addInfo\n");
			return status;
	}
	
	delete ifs;
	return OK;

}


const Status AttrCatalog::removeInfo(const string & relation, 
			       const string & attrName)
{
  Status status;
  Record rec;
  RID rid;
  AttrDesc record;
  HeapFileScan*  scan;

  if (relation.empty() || attrName.empty()) return BADCATPARM;


	//Open up a HeapFileScan on our relCatalog file RELCATNAME
	scan = new HeapFileScan(ATTRCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Cannot open HeapFileScan(ATTRCATNAME) in removeInfo\n");
			return status;
	}

	//initiate a startScan for this relation
	status=scan->startScan(0, MAXNAME, STRING, (char*)&relation, EQ);
	if(status!=OK)
	{
			printf("MyError: Cannot initiate startScan in AttrCat::removeInfo()\n");
			return status;
	}

	//scan for all records with the relation name
	while(status!=FILEEOF)
	{
			status=scan->scanNext(rid);
			if(status!=OK && status!=FILEEOF)
			{
					printf("MyError: Something wrong while scanNext() in AttrCat::removeInfo()\n");
					return status;
			}
			if(status==OK)//rec found
			{	
					//get record from RID
					status=scan->getRecord(rec);
					if(status!=OK)
					{
							printf("MyError: Problem in getRecord after valid scanNext in AttrCat::removeInfo()\n");
							return status;
					}

					//memcopy the dbrecord data into the return param AttrDesc record
					memcpy(&record, rec.data, rec.length);
					
					//check if the record attrName matches the given attrName
					if(strcmp(record.attrName, attrName.c_str())==0)
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
	}

	return OK;//FIXME:What if attribute not found
}


const Status AttrCatalog::getRelInfo(const string & relation, 
				     int &attrCnt,
				     AttrDesc *&attrs)
{
  Status status;
  RID rid;
  Record rec;
	AttrDesc record;
  HeapFileScan*  scan;

  if (relation.empty()) return BADCATPARM;


	//Open up a HeapFileScan on our relCatalog file RELCATNAME
	scan = new HeapFileScan(ATTRCATNAME, status);
	if(status!=OK)
	{
			printf("MyError: Cannot open HeapFileScan(ATTRCATNAME) in AttrCat::getRelInfo\n");
			return status;
	}

	//initiate a startScan for this relation
	status=scan->startScan(0, MAXNAME, STRING, (char*)&relation, EQ);
	if(status!=OK)
	{
			printf("MyError: Cannot initiate startScan in AttrCat::getRelInfo()\n");
			return status;
	}

	attrCnt=0;
	std::vector<AttrDesc> relationMatch;//vector to hold all matches
	//scan for all records with the relation name
	while(status!=FILEEOF)
	{
			status=scan->scanNext(rid);
			if(status!=OK && status!=FILEEOF)
			{
					printf("MyError: Something wrong while scanNext() in getRelInfo\n");
					return status;
			}
			if(status==OK)//rec found
			{	
					//get record from RID
					status=scan->getRecord(rec);
					if(status!=OK)
					{
							printf("MyError: Problem in getRecord after valid scanNext in getRelInfo\n");
							return status;
					}

					//memcopy the dbrecord data into the return param AttrDesc record
					memcpy(&record, rec.data, rec.length);

					relationMatch.push_back(record);//add to vector
					attrCnt++;
			}
	}

	delete scan;

	if(attrCnt>0)
	{
			attrs= new AttrDesc[attrCnt];//might throw exception
			for(int i=0; i<attrCnt; i++)
					attrs[i]=relationMatch[i];
			return OK;
	}
	else
		return ATTRNOTFOUND;

}


AttrCatalog::~AttrCatalog()
{
// nothing should be needed here
}

