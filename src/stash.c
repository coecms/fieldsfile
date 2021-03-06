// uniqueheights FFIN STASH
// Modify file FFIN so that height entries for STASH are unique

#include "fieldsfile.h"
#include <assert.h>
#include <stdlib.h>

int main(int argc, char ** argv){
    assert(argc == 3);

    struct FieldsFile * ff = OpenFieldsFile(argv[1]);

    unsigned int uniqueHeight = 0;
    for (size_t i=0; i< ff->header->field_count; ++i){
        printf("%lld\n",ff->lookup[i].stash_code);
    }

    CloseFieldsFile(ff);
}
