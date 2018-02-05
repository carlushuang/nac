#include <nac.h>
#include "context.h"

/* export api */

NAC_EXPORT nac::nac_context nac::nac_get_context(){
    return nac::context::get_context();
}