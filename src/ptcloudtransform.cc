#include <glog/logging.h>
#include <gflags/gflags.h>

#include "WuLasFile.hpp"
#include "ptcloudfile.h"

static double kEpsilon = 1e-6;

static inline void pointransform(double* xin, double* xout, double H[16]){
    double xi[4] = {*xin, *(xin+1), *(xin+2), 1};
    double xo[4] = {0};
    xo[0] = xi[0]*H[0]+xi[1]*H[1]+xi[2]*H[2]+xi[3]*H[3];
    xo[1] = xi[0]*H[4]+xi[1]*H[5]+xi[2]*H[6]+xi[3]*H[7];
    xo[2] = xi[0]*H[8]+xi[1]*H[9]+xi[2]*H[10]+xi[3]*H[11];
    xo[3] = xi[0]*H[12]+xi[1]*H[13]+xi[2]*H[14]+xi[3]*H[15];
    if(xo[3]<kEpsilon) xo[3] = kEpsilon;
    *xout++ = xo[0]/xo[3];
    *xout++ = xo[1]/xo[3];
    *xout = xo[2]/xo[3];
}

DEFINE_string(transform_matrix_info, "", "transform matrix[16]");
static bool ValidateString(const char* flagname, const std::string& value) {
    if (!value.empty())
    {
        return true;
    }
    printf("Invalid string for --%s: %s\n", flagname, value.c_str());
    return false;
}

static inline bool ExtractTransformatrix(const char* info, double transform_matrix[16]){
    char strline[512];
    if ( IsFile(info) ){
        FILE* fp = fopen(info,"r");
        fgets(strline,512,fp);
        fclose(fp);
        info = strline;
    }
    if ( sscanf(info,"%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
           transform_matrix+0, transform_matrix+1, transform_matrix+2, transform_matrix+3,
           transform_matrix+4, transform_matrix+5, transform_matrix+6, transform_matrix+7,
           transform_matrix+8, transform_matrix+9, transform_matrix+10, transform_matrix+11,
                transform_matrix+12, transform_matrix+13, transform_matrix+14, transform_matrix+15) < 16 ) return false;
    return true;
}

int main(int argc, char *argv[])
{
    FLAGS_alsologtostderr = true;
    std::string usage("This program transforms point cloud via transform matrix. Usage:\n");
    usage = usage + argv[0] + " <input point cloud file> <output point cloud file> ";
    gflags::SetUsageMessage(usage);
    gflags::SetVersionString("1.0.0");

    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (argc<2)
    {
        LOG(ERROR)<< "No input point cloud file!";
        return 1;
    }

    double transform_matrix[16];
    const char* lpstrptcloudin = argv[1];
    const char* lpstrptcloudout = argc>2? argv[2]:NULL;
    char outputpath[512];
    if(lpstrptcloudout==NULL){
        strcpy(outputpath,lpstrptcloudin);
        const char* ps1 = strrchr(lpstrptcloudin,'.');
        char* ps2 = strrchr(outputpath,'.');
        if(ps1!=NULL){
            sprintf(ps2,"_transform%s",ps1);
        }else strcat(outputpath,"_transform");
        lpstrptcloudout = outputpath;
    }

//    LASHDR lashdr;
//    HANDLE hlasin = _OpenLas(lpstrptcloudin,MODE_RD,&lashdr)
    PtCloudFile *filein,*fileout;
    filein = PtCloudOpen(lpstrptcloudin,PtCloudFile::modeRead);
    VLOG(1)<< "Open point cloud file "<< lpstrptcloudin << " ...";
    if (filein == NULL){
        LOG(ERROR)<< "Fail to OPEN point cloud file !";
        return 1;
    }
    VLOG(1)<< "point num = "<< filein->GetPtNum();

    if(!ExtractTransformatrix(FLAGS_transform_matrix_info.c_str(), transform_matrix)){
        LOG(ERROR)<< "Fail to extract transform matrix from "<< FLAGS_transform_matrix_info ;
        return 1;
    }
    VLOG(1)<<"Transform matrix=\t"<< transform_matrix[0];

    VLOG(1)<< "Create point cloud file "<< lpstrptcloudout << " ...";
    fileout = PtCloudOpen(lpstrptcloudout,PtCloudFile::modeCreate);
    if (fileout == NULL){
        LOG(ERROR)<< "Fail to CREATE point cloud file !";
        return 1;
    }

    VLOG(1)<< "Transform point cloud ...";
    char* ptinfo = new char[filein->GetPtInfoLength()];
    while(!filein->eof()){
        double x[3];
        if( filein->ReadPtInfo(ptinfo) <= 0 ) break;
        if( filein->PtInfo2XYZ(ptinfo, x, x+1, x+2 ) < 3 ) continue;
        pointransform(x,x, transform_matrix);
        filein->ReplaceXYZ(ptinfo,x[0],x[1],x[2]);
        fileout->WritePtInfo(ptinfo);
    }
    delete[] ptinfo;
    VLOG(1)<< "Transformation DONE.";
    filein->Close();
    fileout->Close();

    return 0;
}
