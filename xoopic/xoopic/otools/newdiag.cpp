/*
 * newdiag.CPP
 *
 * 1.01 (PG, KC, 9-21-95) After release. First draft.	
 * 1.02	(JohnV 03-08-96) Added initwin(params...) for PC version
 *
 */

#include "newdiag.h"
#include "diagn.h"

//--------------------------------------------------------------------

Diag::Diag(SpatialRegion* _region, int _j1, int _k1, int _j2, int _k2, int _nfft, int _HistMax,
				 int _Ave, int _Comb, ostring _VarName, ostring _x1Label,
				 ostring _x2Label, ostring _x3Label, ostring _title, int _save)
{
	region = _region;
	j1 = MIN(_j1, _j2);
	k1 = MIN(_k1, _k2);
	j2 = MAX(_j1, _j2);
	k2 = MAX(_k1, _k2);
	A1 = region->getX()[j1][k1].e1();
	A2 = region->getX()[j1][k1].e2();
	B1 = region->getX()[j2][k2].e1();
	B2 = region->getX()[j2][k2].e2();

	nfft = _nfft;
	HistMax =  _HistMax;
	Ave = _Ave;
	Comb = _Comb;
	VarName = _VarName;
	x1Label = _x1Label;
	x2Label = _x2Label;
	x3Label = _x3Label;
	title = _title;
	save = _save;
}
/*
Diag::~Diag()
{
}
void Diag::PreChooseVar()
{
}
void Diag::MaintainDiag(Scalar t)
{
}
void Diag::initwin()
{
}
*/




#ifdef HAVE_HDF5
int Diag::dumpH5(dumpHDF5 &dumpObj,int number)
{
  
  Vector2 **X = region->getX();
//  Scalar x,y;
//  int xl,yl;  
  
  hid_t fileId, groupId, datasetId,dataspaceId; //subgroupId ;
  herr_t status;
  hsize_t rank; 
  hsize_t     dimsf[1];   
  hsize_t     dims;
//  int         attrData;
  hid_t  attrdataspaceId,attributeId;
  Scalar       data[4]; 
  hid_t scalarType = dumpObj.getHDF5ScalarType();
  string datasetName;

  stringstream s;

  s <<"Diag" << number <<ends;    
  string groupName = s.str();

  fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  groupId = H5Gcreate(fileId,groupName.c_str(),0);
  
  rank = 1;
  dimsf[0] = 4;
  dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
  datasetName = "dims";  
  datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
		      H5P_DEFAULT);

  data[0] = X[j1][k1].e1();
  data[1] = X[j1][k1].e2();
  data[2] = X[j2][k2].e1();
  data[3] = X[j2][k2].e2();
  
  status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
		   H5P_DEFAULT, data);
  
// dump save flag, history title as attribute
  // name of attribute is title of history, diagnostic
   dims = 1;
   attrdataspaceId = H5Screate_simple(1, &dims, NULL);
   attributeId = H5Acreate(datasetId, title.c_str(),H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
   status = H5Awrite(attributeId,H5T_NATIVE_INT , &save);
   status = H5Aclose(attributeId);
   status = H5Sclose(attrdataspaceId);

  
  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  
  
  H5Gclose(groupId);
  H5Fclose(fileId);

  if (save)
    cerr << "saving diagnostic.\n";
    //    history->dump(DMPFile);



  return(1);

}
#endif

int Diag::Dump(FILE* DMPFile)
{
	Vector2 **X = region->getX();
	Scalar x,y;
	int xl,yl;  
	x = X[j1][k1].e1();
	y = X[j1][k1].e2();
	
	XGWrite(&x, ScalarInt, 1, DMPFile, ScalarChar);
	XGWrite(&y, ScalarInt, 1, DMPFile, ScalarChar);
	x = X[j2][k2].e1();
	y = X[j2][k2].e2();
	
	XGWrite(&x, ScalarInt, 1, DMPFile, ScalarChar);
	XGWrite(&y, ScalarInt, 1, DMPFile, ScalarChar);
	xl = abs(j2-j1);
	yl = abs(k2-k1);
	XGWrite(&xl,4,1,DMPFile,"int");
	XGWrite(&yl,4,1,DMPFile,"int");

	int titleLength = title.length();
	// Write the length of the title
	XGWrite(&titleLength,4,1,DMPFile,"int");
	// Write the title
	char buf[512];
	sprintf(buf, title.c_str());
	XGWrite(buf,1,titleLength+1,DMPFile,"char");

	// Write if there is a history or not 
	XGWrite(&save,4,1,DMPFile,"int");
	if (save)
		history->dump(DMPFile);
	return(TRUE);
}

int Diag::Restore_2_00(FILE* DMPFile, int j1test, int k1test, int j2test, int k2test, ostring titleTest)
{
	if ((j1 == j1test)&&(k1 == k1test)&&(j2 == j2test)&&(k2 == k2test)&&(titleTest == title))
		{
			//Diags is the same get your information from fill
			if (save)
				history->restore_2_00(DMPFile);
			return(TRUE);
		}
	else
		return(FALSE);
}


int Diag::Restore(FILE* DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,
							  int xl,int yl, const char *_title)
{
	if(!strcmp(title,_title)) { // this is the same diagnostic
		history->restore(DMPFile,_A1,_A2,_B1,_B2,xl,yl,A1,A2,B1,B2,j1,k1,j2,k2);
		return TRUE;
	}
		return(FALSE);
}


