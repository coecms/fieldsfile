/**
 * \file    fieldsfile.h
 * \author  Scott Wales (scott.wales@unimelb.edu.au)
 * \brief   Interface for working with UM output files
 * 
 * UM output files are stored as an array of big-endian 64 bit values. The
 * array is structured into several sections - a header of 256 values, sections
 * for constants, a lookup table for data values and the data values
 * themselves. The full format is described in UM technical paper F3.
 *
 * This interface provides functions to open, close and write an output file.
 * The file must be accessable in random-access mode. The object returned by
 * the open function contains the header and lookup table. Some entries have
 * descriptive names, others use placeholders since not all entries are defined
 * by the format.
 *
 * This is only a simple interface for getting subsets of the data, it may be
 * expanded as there is need for it.
 *
 * Copyright 2013 ARC Centre of Excellence for Climate System Science
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */ 

#ifndef FIELDSFILE_H
#define FIELDSFILE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

struct FFHeader;
struct FFLookup;

/** @defgroup fieldsfile
 *  @{
 */

/** 
 * @brief The UM file object
 *
 * header and lookup are initialised by the open function. They may be
 * memory-mapped, do not write to them unless you want that to appear in the
 * file itself.
 */
struct FieldsFile {
    FILE * stream;
    struct FFHeader * header;
    struct FFLookup * lookup;
};

/** 
 * @brief Open a new file given a filename
 * 
 * The named file will be opened in random-access read-write mode. If a read
 * error occurs the function will call perror() and exit(-1).
 */
struct FieldsFile * OpenFieldsFile(const char * filename);

/**
 * @brief Write a file to disk
 *
 * Data in the header & lookup tables will be written to disk. This doesn't
 * alter the data tables, they should be operated on separately.
 */
void WriteFieldsFile(struct FieldsFile * ff);

/**
 * @brief Read a single 2D field from the fields file
 *
 * Data array will be resized as needed
 */
void ReadFieldsFileData(double ** data,
                        struct FieldsFile * ff,
                        int field);

/**
 * @brief Close the file, flushing & freeing buffers
 *
 * May implicitly call write. After calling this the header and lookup pointers
 * will be freed.
 */
void CloseFieldsFile(struct FieldsFile * ff);


/// Missing data constant
static const int64_t IMDI = -32768;

/**
 * @brief Date container
 *
 * Multiple dates are stored in the file with this format. Convenience
 * functions may be added to convert this to a ctime date structure at some
 * point.
 */
struct FFDate {
    int64_t year;
    int64_t month;
    int64_t day;
    int64_t hour;
    int64_t minute;
    int64_t second;
};


double FFDateToUnixTime(const struct FFDate date);

/**
 * @brief Header structure
 *
 * Many entries are not used in the UM output format, they are given names u%d,
 * where %d is the offset as given by UM document F3 (i.e. 1-based). Other
 * entries are used in the format but have not yet had names entered (patches
 * appreciated).
 */
struct FFHeader {
    int64_t version;
    int64_t u2;
    int64_t u3;
    int64_t u4;
    int64_t u5;
    int64_t u6;
    int64_t u7;
    int64_t u8;
    int64_t u9;
    int64_t u10;
    int64_t u11;
    int64_t u12;
    int64_t u13;
    int64_t u14;
    int64_t u15;
    int64_t u16;
    int64_t u17;
    int64_t u18;
    int64_t u19;
    int64_t u20;
    struct FFDate initial_time;
    int64_t u27;
    struct FFDate valid_time;
    int64_t u34;
    struct FFDate generated_time;
    int64_t u41;
    int64_t u42;
    int64_t u43;
    int64_t u44;
    int64_t u45;
    int64_t u46;
    int64_t u47;
    int64_t u48;
    int64_t u49;
    int64_t u50[90];
    int64_t u140;
    int64_t u141;
    int64_t u142;
    int64_t u143;
    int64_t u144;
    int64_t u145;
    int64_t u146;
    int64_t u147;
    int64_t u148;
    int64_t u149;
    int64_t lookup_start;
    int64_t lookup_size;
    int64_t field_count;
    int64_t u153;
    int64_t u154;
    int64_t u155;
    int64_t u156;
    int64_t u157;
    int64_t u158;
    int64_t u159;
    int64_t data_start;
    int64_t u161;
    int64_t u162;
    int64_t u163;
    int64_t u164;
    int64_t u165;
    int64_t u166;
    int64_t u167;
    int64_t u168;
    int64_t u169;
    int64_t u170[87];
};

/**
 * @brief Format of a single lookup table entry
 *
 * As per the header there are lots of unused values in the lookup table.
 */
struct FFLookup {
    struct FFDate valid_time;
    struct FFDate data_time;
    int64_t u13;
    int64_t u14;
    int64_t data_length;
    int64_t u16;
    int64_t u17;
    int64_t rows;
    int64_t columns;
    int64_t u20;
    int64_t u21;
    int64_t u22;
    int64_t u23;
    int64_t u24;
    int64_t u25;
    int64_t u26;
    int64_t u27;
    int64_t u28;
    int64_t file_start;
    int64_t record_count;
    int64_t u31;
    int64_t u32;
    int64_t u33;
    int64_t u34;
    int64_t u35;
    int64_t u36;
    int64_t u37;
    int64_t u38;
    int64_t u39;
    int64_t u40;
    int64_t u41;
    int64_t stash_code;
    int64_t pseudo_dimension;
    int64_t u44;
    int64_t u45;
    double u46;
    double u47;
    double u48;
    double u49;
    double u50;
    double u51;
    double heightlevel;
    double u53;
    double u54;
    double u55;
    double pole_latitude;
    double pole_longitude;
    double u58;
    double origin_latitude;
    double latitude_interval;
    double origin_longitude;
    double longitude_interval;
    double missing_data;
    double mks_scale;
};

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif
