
# op c registry file
file(GLOB tmp op_c_*.cc)

set(_nac_SRC ${_nac_SRC} ${tmp})
set(_nac_SRC ${_nac_SRC} PARENT_SCOPE)

