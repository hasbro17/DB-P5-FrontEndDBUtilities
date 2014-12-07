#include <sys/types.h>
#include <functional>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "error.h"
#include "utility.h"
#include "catalog.h"

// define if debug output wanted


//
// Retrieves and prints information from the catalogs about the for
// the user. If no relation is given (relation.empty() is true), then
// it lists all the relations in the database, along with the width in
// bytes of the relation, the number of attributes in the relation,
// and the number of attributes that are indexed.  If a relation is
// given, then it lists all of the attributes of the relation, as well
// as its type, length, and offset, whether it's indexed or not, and
// its index number.
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::help(const string & relation)
{
  Status status;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;

  if (relation.empty()) return UT_Print(RELCATNAME);

	//get the relation name and num attributes from relcatalog
	status=getInfo(relation, rd);
	if(status==RELNOTFOUND)
			return status;
	else if(status!=OK)
	{
			printf("MyError: something wrong with getRelInfo in help()\n");
			return status;
	}
	//get all the attributes for that relation in attrs
	status=attrCat->getRelInfo(relation, attrCnt, attrs);
	if(status==ATTRNOTFOUND)
			return status;
	else if(status!=OK)
	{
			printf("MyError: something wrong with getRelInfo in help()\n");
			return status;
	}

	printf("Relation name: %s (%d attributes)\n",relation.c_str(), attrCnt);
	printf("  Attribute name   Off   T   Len\n");
	printf("---------------- ----- --- -----\n");
	for(int i=0; i<attrCnt; i++)
	{
			printf("%16s %5d ",attrs[i].attrName, attrs[i].attrOffset);
				
			switch(attrs[i].attrType) 
			{
				case INTEGER:
					printf("%3c ", 'i');
					break;
				case FLOAT:
					printf("%3c ", 'f');
					break;
				case STRING:
					printf("%3c ", 's');
					break;
			}

			printf("%5d\n",attrs[i].attrLen);
	}
  return OK;
}
