#include "fieldsfile.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define be64read(ptr,count,offset,stream) \
    be64read_(ptr,sizeof(*(ptr)),count,offset,stream)
void be64read_(void * ptr, size_t size, size_t count,
               size_t offset, FILE * stream){
    int err = fseek(stream,(offset-1)*sizeof(int64_t),SEEK_SET);
    assert(err == 0);
    size_t nread = fread(ptr,size,count,stream);
    if (nread != count){
        perror("be64read failed:");
        exit(-1); 
    }
    for (size_t i=0;i<size*count/sizeof(int64_t);++i){
        ((int64_t*)ptr)[i] = _bswap64(((int64_t*)ptr)[i]);
    }
}
#define be64write(ptr,count,offset,stream) \
    be64write_(ptr,sizeof(*(ptr)),count,offset,stream)
void be64write_(void * ptr, size_t size, size_t count,
               size_t offset, FILE * stream){
    for (size_t i=0;i<size*count/sizeof(int64_t);++i){
        ((int64_t*)ptr)[i] = _bswap64(((int64_t*)ptr)[i]);
    }
    fseek(stream,(offset-1)*sizeof(int64_t),SEEK_SET);
    size_t nwrite = fwrite(ptr,size,count,stream);
    if (nwrite != count){
        perror("be64write failed:");
        exit(-1); 
    }
    for (size_t i=0;i<size*count/sizeof(int64_t);++i){
        ((int64_t*)ptr)[i] = _bswap64(((int64_t*)ptr)[i]);
    }
}

struct FieldsFile * OpenFieldsFile(const char * filename){
    struct FieldsFile * this = malloc(sizeof(*this));
    char * errmsg = NULL;
    asprintf(&errmsg,"OpenFieldsFile(%s)",filename);
    this->stream = fopen(filename,"r+");
    if (!this->stream) {
        perror(errmsg);
        exit(-1);
    }

    assert(sizeof(*(this->header))/sizeof(int64_t) == 256);

    size_t offset = 1;
    this->header = malloc(sizeof(*(this->header)));
    be64read(this->header,1,offset,this->stream);
    assert(this->header->version == 20 ||
           this->header->version == IMDI);

    assert((sizeof(*(this->lookup))/sizeof(int64_t) == this->header->lookup_size) &&
           "Observation files are not supported");

    offset = this->header->lookup_start;
    this->lookup = malloc(this->header->field_count * sizeof(*(this->lookup)));
    be64read(this->lookup,this->header->field_count,offset,this->stream);

    free(errmsg);
    return this;
}
void WriteFieldsFile(struct FieldsFile * this){
    size_t offset = 1;
    be64write(this->header,1,offset,this->stream);

    offset = this->header->lookup_start;
    be64write(this->lookup,this->header->field_count,offset,this->stream);
}
void CloseFieldsFile(struct FieldsFile * ff){
    if (ff){
        fclose(ff->stream);
        free(ff->header);
        free(ff->lookup);
    }
    free(ff);
}
double FFDateToUnixTime(const struct FFDate date){
    struct tm tm = {
        .tm_sec = date.second,
        .tm_min = date.minute,
        .tm_hour = date.hour,
        .tm_mday = date.day,
        .tm_mon = date.month - 1,
        .tm_year = date.year - 1900,
        .tm_isdst = 0,
    };
    // Ensure time is UTC
    char * tz = getenv("TZ");
    setenv("TZ","",1);
    tzset();
    time_t time = mktime(&tm);
    if (tz) setenv("TZ",tz,1);
    else unsetenv("TZ");
    tzset();
    return time;
}

void ReadFieldsFileData(double ** data,
                        struct FieldsFile * this,
                        int i){
    size_t count = this->lookup[i].rows*this->lookup[i].columns;

    *data = realloc(*data,count*sizeof(**data));
    be64read(*data,count,this->lookup[i].file_start,this->stream);
}
