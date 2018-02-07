#include "ptcloudfile.h"

#include <stdio.h>
#include <string.h>

PtCloudFile* PtCloudOpen(const char* lpstrfilepath, int mode){
    const char* lpstrext = strrchr(lpstrfilepath,'.');
    if( !lpstrext ) return NULL;
    char strext[128]; strcpy(strext,lpstrext+1);
    if(!strcmp(strext,"txt")){
        PtCloudTxt* ptcloud = new PtCloudTxt;
        if( ptcloud->Open(lpstrfilepath,mode) ) return (PtCloudFile*)ptcloud;
        delete ptcloud;
        return NULL;
    }
    return NULL;
}

#define PTCLOUDTXT_POINTER(a) ((FILE*)a)
PtCloudTxt::PtCloudTxt(){
    ptinfolength_ = 2048;
}
bool PtCloudTxt::Open(const char* lpstrfilepath, int mode){
    if ( mode == modeCreate || mode == modeWrite){
        hfile_ = (unsigned long int)fopen(lpstrfilepath,"w");
    }else if( mode == modeRead ){
        hfile_ = (unsigned long int)fopen(lpstrfilepath,"r");
    }else if( mode ==  modeReadWrite){
        hfile_ = (unsigned long int)fopen(lpstrfilepath,"wr");
    }else return false;
    if( hfile_ == 0) {
        return false;
    }
    strcpy(filepath_,lpstrfilepath);
    return true;
}
void PtCloudTxt::Close() { if(hfile_!=0) fclose(PTCLOUDTXT_POINTER(hfile_)); }
bool PtCloudTxt::IsLoaded() const { return hfile_==0?false:true; }
int PtCloudTxt::GetPtNum() { return 0; }
int  PtCloudTxt::GetPtInfoLength() {return ptinfolength_;}

bool PtCloudTxt::eof() { return feof(PTCLOUDTXT_POINTER(hfile_)); };
int  PtCloudTxt::SeekPt(int idx) { return idx; };
int  PtCloudTxt::ReadPtInfo(char* info) { return fgets(info,ptinfolength_,PTCLOUDTXT_POINTER(hfile_))?1:0; };
int  PtCloudTxt::PtInfo2XYZ(const char* info, double* x, double* y, double* z){
    return sscanf(info,"%lf%lf%lf",x,y,z);
};
int  PtCloudTxt::ReplaceXYZ(char* info, double x, double y, double z){
    char strTmp[2048];
    sscanf(info,"%*lf%*lf%*lf%[^A-Z]",strTmp);
    return sprintf(info,"%lf %lf %lf %s",x,y,z,strTmp);
};
int  PtCloudTxt::WritePtInfo(const char* info){
    return fputs(info,PTCLOUDTXT_POINTER(hfile_));
}
