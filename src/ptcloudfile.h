#ifndef PTCLOUDFILE_H
#define PTCLOUDFILE_H

class PtCloudFile{
public:
    PtCloudFile(){ hfile_ = 0; filepath_[0] =0; }
    enum OPENFLAGS { modeRead= 0x0000,modeWrite=0x0001,modeReadWrite=0x0002,modeCreate=0x1000 };
    virtual bool Open(const char* lpstrfilepath, int mode) = 0;
    virtual bool IsLoaded() const = 0;
    virtual void Close() = 0;

    virtual int  GetPtNum() = 0;
    virtual int  GetPtInfoLength()=0;

    virtual bool eof() = 0;
    virtual int  SeekPt(int idx) = 0;
    virtual int  ReadPtInfo(char* info) = 0;
    virtual int  PtInfo2XYZ(const char* info, double* x, double* y, double* z)=0;
    virtual int  ReplaceXYZ(char* info, double x, double y, double z)=0;
    virtual int  WritePtInfo(const char* info)=0;

    const char* filepath() const { return filepath_; }
protected:
    unsigned long int hfile_;
    char     filepath_[512];
};

PtCloudFile* PtCloudOpen(const char* lpstrfilepath, int mode);

class PtCloudTxt : public PtCloudFile{
public:
    PtCloudTxt();
    virtual ~PtCloudTxt(){
        PtCloudTxt::Close();
    }
    static const char* Tag() { return "txt"; }
    virtual bool Open(const char* lpstrfilepath, int mode);
    virtual void Close();
    virtual bool IsLoaded() const ;
    virtual int GetPtNum();
    virtual int  GetPtInfoLength();

    virtual bool eof();
    virtual int  SeekPt(int idx);
    virtual int  ReadPtInfo(char* info);
    virtual int  PtInfo2XYZ(const char* info, double* x, double* y, double* z);
    virtual int  ReplaceXYZ(char* info, double x, double y, double z);
    virtual int  WritePtInfo(const char* info);
private:
    int      ptinfolength_;
};

#endif
